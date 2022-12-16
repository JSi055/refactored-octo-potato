import asyncio
import queue
import random
import sys
import termios
import threading
import serial.tools.miniterm

import dash
from dash import dcc
from dash import html
from dash.dependencies import Input, Output
import plotly.graph_objects as go
import serial
import time

import commands
import database

loop: asyncio.events

# array of [pulseid, data[x][y]]
pulse_results_queue = queue.Queue(10)

# arrays for saving data points
traces_data = {}


def update_pulse_traces():
    global traces_data

    all_pulse_data = database.get_pulse_tests()

    # take a bunch of data from the queue
    #while not pulse_results_queue.empty():
    #    pulse_result = pulse_results_queue.get(block=False)
    #    traces_data[pulse_result[0]](pulse_result[1])

    traces = []
    for id in all_pulse_data:
        traces.append(go.Scatter(
            name = "P" + str(id),
            x = all_pulse_data[id][0],
            y = all_pulse_data[id][1]
        ))

    return traces


def update_figure():
    plot = go.Figure(
        data=update_pulse_traces(), layout=dict(
            xaxis_title="time",
            yaxis_title="Voltage",
            yaxis_range=[-1.0, 8.0]
            # yaxis=dict(domain=[0.0, 0.5])
        )
    )
    return plot


# GUI layout/design
app = dash.Dash(__name__, update_title=None)


# update part where data is read and the plot and data arrays are updated
@app.callback(Output('live-graph', 'figure'),
              [Input('update_graph', 'n_intervals')])
def update(i_input):
    return update_figure()


async def run_auto_test(com: serial.Serial):
    await asyncio.sleep(8)  # test every 30s
    print("pulse test...")
    await commands.trigger_test(com, 50000, 5000, 50000, 10.0)
    print("complete.")
    while com.is_open:
        await asyncio.sleep(8) # test every 30s
        await commands.get_ping(com)
        print("pulse test...")
        await commands.trigger_test(com, 50000, 5000, 50000, 10.0)
        print("complete.")


async def run_user_input(com: serial.Serial):
    reader = asyncio.StreamReader(loop=asyncio.get_running_loop())
    protocol = asyncio.StreamReaderProtocol(reader)
    await loop.connect_read_pipe(lambda: protocol, sys.stdin)

    while com.is_open:
        command = (await reader.readline()).decode("UTF-8").split()
        if len(command) < 1:
            continue
        print(command)
        if command[0] == "help":
            print("lol, nope")
        elif command[0] == "load":
            await commands.set_load(com, float(command[1]))
            print("set load to " + command[1])
        elif command[0] == 'status':
            print("status string: " + await commands.get_status(com))
        elif command[0] == 'test':
            print("running test...")
            await commands.trigger_test(com, 4000, 500, 2000, 10.0)
            print("done")
        elif command[0] == 'exit':
            print("closing connections")
            exit() #TODO: close database
        else:
            print("unknown command")


async def main():
    global loop
    loop = asyncio.get_running_loop()
    commands.set_loop(loop)

    port = '/dev/ttyUSB0'
    baud = 115200
    if len(sys.argv) >= 2:
        port = sys.argv[1]

    #############################
    # magical mystery code from https://stackoverflow.com/questions/15460865/disable-dtr-in-pyserial-from-code
    # somehow makes it so DTR does *not* get pulled low when opening the connection
    #f = open(port)
    #attrs = termios.tcgetattr(f)
    #attrs[2] = attrs[2] & ~termios.HUPCL
    #termios.tcsetattr(f, termios.TCSAFLUSH, attrs)
    #f.close()
    #############################

    com = serial.serial_for_url(
                port,
                baud,
                parity='N',
                rtscts=False,
                xonxoff=False,
                do_not_open=True)

    #com = serial.Serial()
    #com.port = port
    #com.baudrate = baud
    #com.timeout = 60 # TODO: make this longer
    #com.dtr = True
    com.open()

    database.loadDatabase("test.db")

    io_tasks = asyncio.gather(
        asyncio.create_task(asyncio.to_thread(commands.run_process_rx, com)),
        asyncio.create_task(run_auto_test(com)),
        asyncio.create_task(run_user_input(com))
    )

    retry_count = 0
    print("getting status...")
    while True:
        try:
            #async with asyncio.timeout(10):
            status = await commands.get_status(com)
            print(status)
            break
        except TimeoutError:
            if retry_count >= 5:
                print("Failed to get status.")
                exit(1)
            print("Timeout. Retrying...")

    # init UI
    app.layout = html.Div(
        children=[
            html.H1('Pulse Tests'),
            dcc.Graph(id='live-graph', figure=update_figure()),
            dcc.Interval(id='update_graph', interval=10000)
            # html.Button(id='textout', children='text')
        ]
    )

    webapp_thread = threading.Thread(target=app.run_server)
    webapp_thread.start()
    await io_tasks
    webapp_thread.join()


if __name__ == '__main__':
    asyncio.run(main())
