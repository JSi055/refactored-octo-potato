import asyncio

import serial
import sqlite3
import main

import calibration as cal
import database

futures_complete_event = asyncio.Event()
futures_lock = asyncio.Lock()

last_future_id = 0
future_ids = set()
futures = {}

#FIXME: we might want a signle thread for write operations so races dont happen

##### TODO: Ugly global state. Put this in a class as it is state associated with that connection.
cal_curve_voltage = cal.CalCurve()
cal_curve_voltage.points = [cal.Point(0, 0.0), cal.Point(2500, 4.410)]
cal_curve_current = cal.CalCurve()
cal_curve_current.points = [cal.Point(0, 0.0), cal.Point(255, 14.286)] # FIXME: this is calculated

loop: asyncio.events
##### End ugly global state

# TX and RX
CMD_STATUS = b's' # print a status string
CMD_PING = b'p'   # ping the device
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

def set_loop(l: asyncio.events):
    global loop
    loop = l


async def get_status(com: serial.Serial) -> str:
    stuff = await create_command_future(ord(CMD_STATUS))

    await asyncio.to_thread(com.write, CMD_STATUS) # send the status command
    return await stuff[0]


async def get_ping(com: serial.Serial):
    stuff = await create_command_future(ord(CMD_PING))

    await asyncio.to_thread(com.write, CMD_PING) # send the status command
    return await stuff[0]


def _trigger_test(com: serial.Serial, dur_us: int, pre_dur_us: int, post_dur_us: int, current: float):
    database.putStartTest(dur_us, pre_dur_us, post_dur_us, current)
    com.write(b"T"
              + int.to_bytes(dur_us, 4, "big")
              + int.to_bytes(pre_dur_us, 4, "big")
              + int.to_bytes(post_dur_us, 4, "big")
              + int.to_bytes(cal_curve_current.y_to_x(current), 2, "big"))


async def trigger_test(com: serial.Serial, dur_us: int, pre_dur_us: int, post_dur_us: int, current: float):
    stuff = await create_command_future(ord(CMD_TEST))
    await asyncio.to_thread(_trigger_test, com, dur_us, pre_dur_us, post_dur_us, current)
    return await stuff[0]


async def set_load(com: serial.Serial, current: float):
    def io_stuff():
        com.write(b"l" + cal_curve_current.y_to_x(current).to_bytes(2, "big"))
        com.flush()
    await asyncio.to_thread(io_stuff)


def handle_status(com: serial.Serial):
    status = com.readline().decode("UTF-8") # read until a new-line char
    safe_complete_cmd_future(ord(CMD_STATUS), status)


def handle_ping(com: serial.Serial):
    safe_complete_cmd_future(ord(CMD_PING), None)


def handle_error(com: serial.Serial):
    print("DEVICE ERROR: " + str(com.readline())) # read until a new-line char


def handle_voltage_stream(com: serial.Serial):
    raw_value = int.from_bytes(com.read(2), "big")
    volt_value = cal_curve_voltage.x_to_y(raw_value)
    database.putVoltage(volt_value)


def handle_test(com: serial.Serial):
    database.putTestComplete()
    safe_complete_cmd_future(ord(CMD_TEST), None)
    #TODO: update the traces in the graph!


def handle_test_stream(com: serial.Serial):
    raw_value = int.from_bytes(com.read(2), "big")
    volt_value = cal_curve_voltage.x_to_y(raw_value)
    database.putTestVoltage(volt_value)


def safe_complete_cmd_future(cmd_id: int, data: any):
    global loop
    asyncio.run_coroutine_threadsafe(complete_command_future(cmd_id, data), loop)


async def complete_command_future(cmd_id: int, data: any):
    global futures
    global futures_lock

    cmd_futures = {}
    await futures_lock.acquire()
    if cmd_id in futures:
        cmd_futures = futures[cmd_id]
    futures_lock.release()

    for future in cmd_futures.values():
        future.set_result(data) # complete the future outside the lock as it will use it


async def create_command_future(cmd_id: int) -> (asyncio.Future, int):
    global last_future_id
    global futures
    global futures_lock
    global futures_complete_event
    future = asyncio.Future()

    while True:
        await futures_lock.acquire()
        if len(future_ids) < 256:
            while last_future_id in future_ids:
                last_future_id = (last_future_id + 1) % 256

            fid = last_future_id
            future_ids.add(fid)

            async def completed(f: asyncio.Future):
                data = await f
                await futures_lock.acquire()
                if len(futures[cmd_id]) <= 1:
                    futures.pop(cmd_id)
                else:
                    futures[cmd_id].pop(fid)
                futures_lock.release()
                futures_complete_event.set()
                return data

            if cmd_id not in futures:
                futures[cmd_id] = {}
            futures[cmd_id][fid] = future

            # we are returning so release the lock NOW
            futures_lock.release()

            return completed(future), fid

        futures_lock.release()
        # the queue is full. wait for another command to complete its result
        await futures_complete_event.wait()
        futures_complete_event.clear()

unknown_cmd_ids = {}


def run_process_rx(com: serial.Serial):
    while com.is_open:
        cmd_id = com.read()

        if cmd_id == CMD_STATUS:
            handle_status(com)
        elif cmd_id == CMD_PING:
            handle_ping(com)
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
                char = "unknown"
                try:
                    char = cmd_id.decode("UTF-8")
                except UnicodeDecodeError:
                    nothing = 0
                print("Got bad command id '0x" + cmd_id.hex() + "' (" + char + "), ignoring in the future\n")
            unknown_cmd_ids[cmd_id] = 1

