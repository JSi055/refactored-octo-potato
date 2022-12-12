import asyncio

import serial
import sqlite3

import calibration as cal
import database

futures_complete_event = asyncio.Event()
futures_lock = asyncio.Lock()

last_future_id = 0
futures = {}

##### TODO: Ugly global state. Put this in a class as it is state associated with that connection.
cal_curve_voltage = cal.CalCurve()
cal_curve_voltage.points = [cal.Point(0, 0.0), cal.Point(2500, 4.410)]
cal_curve_current = cal.CalCurve()
cal_curve_current.points = [cal.Point(0, 0.0), cal.Point(255, 14.286)] # FIXME: this is calculated
##### End ugly global state

# TX and RX
CMD_STATUS = b's' # print a status string
CMD_TEST = b'T'   # trigger a test pulse

# 0x01 : enable stream voltage
# 0x02 : manual current crashout
# 0x80 : automatic mode
CMD_FLAGS = b'f'    # enable/disable the voltage stream, maybe other flags also
CMD_DUMP_CAL = b'D' # dump the calibration data so it can be hard coded

# RX
CMD_VOLTAGE_STREAM = b'v' # voltage stream packets (raw, uncalibrated)
CMD_TEST_STREAM = b't'    # voltage test stream packets (raw, uncalibrated)
CMD_ERROR = b'e'          # error string ending in \n

# TX
CMD_CALIBRATE = b'c' # calibrate voltage, current, or % (v, c, %) at points (1 through 4).
                     # with decimal values X.X
CMD_SET_LOAD = b'l'  # set the load in micro amps


def get_status(com: serial.Serial) -> asyncio.Future:
    stuff = create_command_future(ord(CMD_STATUS))
    com.write(CMD_STATUS) # send the status command
    return stuff[0]


def trigger_test(com: serial.Serial, dur_us: int, pre_dur_us: int, post_dur_us: int, current: float) -> asyncio.Future:
    stuff = create_command_future(ord(CMD_STATUS))
    database.putStartTest(dur_us, pre_dur_us, post_dur_us, current);
    com.write(b"T"
              + int.to_bytes(dur_us, 4, "big")
              + int.to_bytes(pre_dur_us, 4, "big")
              + int.to_bytes(post_dur_us, 4, "big")
              + int.to_bytes(cal_curve_current.y_to_x(current), 2, "big"))
    return stuff[0]


def set_load(com: serial.Serial, current: float):
    com.write(b"l" + cal_curve_current.y_to_x(current).to_bytes(2, "big"))


def handle_status(com: serial.Serial):
    status = com.readline() # read until a new-line char
    complete_command_future(ord(CMD_STATUS), status)


def handle_error(com: serial.Serial):
    print("DEVICE ERROR: " + str(com.readline())) # read until a new-line char


def handle_voltage_stream(com: serial.Serial):
    raw_value = int.from_bytes(com.read(2), "big")
    volt_value = cal_curve_voltage.x_to_y(raw_value)
    database.putVoltage(volt_value)


def handle_test(com: serial.Serial):
    database.putTestComplete()
    #TODO: update the traces in the graph!


def handle_test_stream(com: serial.Serial):
    raw_value = int.from_bytes(com.read(2), "big")
    volt_value = cal_curve_voltage.x_to_y(raw_value)
    database.putTestVoltage(volt_value)


def complete_command_future(cmd_id: int, data: any):
    global futures
    global futures_lock

    cmd_futures = {}
    futures_lock.acquire()
    if cmd_id in futures:
        cmd_futures = futures[cmd_id].copy()
    futures_lock.release()

    for future in cmd_futures:
        future.set_result(data)  # complete the future outside the lock as it will use it

def create_command_future(cmd_id: int) -> (asyncio.Future, int):
    global last_future_id
    global futures
    global futures_lock
    global futures_complete_event
    future = asyncio.Future()

    while 1==1:

        futures_lock.acquire()
        if len(futures) < 256:
            while last_future_id in futures:
                last_future_id = (last_future_id + 1) % 256

            fid = last_future_id

            def completed(f: asyncio.Future):
                futures_lock.acquire()
                if futures[cmd_id].len <= 1:
                    futures.pop(cmd_id)
                else:
                    futures[cmd_id].pop(fid)
                futures_lock.release()
                futures_complete_event.set()

            future.add_done_callback(completed)
            if cmd_id not in futures:
                futures[cmd_id] = {}
            futures[cmd_id][fid] = future

            futures_lock.release()
            return future, fid

        futures_lock.release()
        # the queue is full. wait for another command to complete its result
        futures_complete_event.wait()
        futures_complete_event.clear()

unknown_cmd_ids = {}


async def run_process_rx(com: serial.Serial):
    while com.is_open():
        cmd_id = com.read()

        if cmd_id == CMD_STATUS:
            handle_status(com)
        elif cmd_id == CMD_ERROR:
            handle_error(com)
        elif cmd_id == CMD_VOLTAGE_STREAM:
            handle_voltage_stream(com)
        elif cmd_id == CMD_TEST_STREAM:
            handle_test_stream(com)
        elif cmd_id == CMD_TEST:
            handle_test(com)
        else:
            # unknown command
            if cmd_id not in unknown_cmd_ids:
                print("Got bad command id '" + cmd_id + "', ignoring in the future\n")
            unknown_cmd_ids[cmd_id] = 1

