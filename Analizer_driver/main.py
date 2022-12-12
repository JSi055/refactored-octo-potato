import asyncio
import queue
import random
import sys
import termios
import threading

import dash
from dash import dcc
from dash import html
from dash.dependencies import Input, Output
import plotly.graph_objects as go
import serial
import time

import commands

# array of [pulseid, data[x][y]]
pulse_results_queue = queue.Queue(10)

# arrays for saving data points
traces_data = {}

def update_traces():
    global traces_data

    # take a bunch of data from the queue
    while not pulse_results_queue.empty():
        pulse_result = pulse_results_queue.get(block=False)
        traces_data[pulse_result[0]](pulse_result[1])

    traces = []
    for id in traces_data:
        traces.append(go.Scatter(
            name = "A" + str(id),
            x = traces_data[id][0],
            y = traces_data[id][1]
        ))

    return traces

def update_figure():
    plot = go.Figure(
        data=update_traces(), layout=dict(
            xaxis_title="time",
            yaxis_title="Voltage",
            yaxis_range=[1.0, 4.0]
            # yaxis=dict(domain=[0.0, 0.5])
        )
    )
    return plot


# GUI layout/design
app = dash.Dash(__name__, update_title=None)
app.layout = html.Div(
    children = [
        html.H1('Pulse Tests'),
        dcc.Graph(id='live-graph', figure=update_figure()),
        dcc.Interval(id='update_graph', interval=20000)
        #html.Button(id='textout', children='text')
    ]
)


# update part where data is read and the plot and data arrays are updated
@app.callback(Output('live-graph', 'figure'),
              [Input('update_graph', 'n_intervals')])
def update(i_input):
    return update_figure()


async def run_auto_test(com: serial.Serial):
    while com.is_open():
        await asyncio.sleep(30) # test every 30s
        await commands.trigger_test(com, 4000, 500, 2000, 10.0)

async def run_user_input(com: serial.Serial):
    while com.is_open():
        command = input("> ").split()
        if command[0] == "help":
            print("lol, nope\n")
        elif command[0] == "l":
            print("set load to ")
            commands.set_load(float(command[1]))

async def main():

    port = '/dev/ttyUSB0'
    baud = 115200
    if len(sys.argv) > 0:
        port = sys.argv[0]

    #############################
    # magical mystery code from https://stackoverflow.com/questions/15460865/disable-dtr-in-pyserial-from-code
    # somehow makes it so DTR does *not* get pulled low when opening the connection
    #f = open(port)
    #attrs = termios.tcgetattr(f)
    #attrs[2] = attrs[2] & ~termios.HUPCL
    #termios.tcsetattr(f, termios.TCSAFLUSH, attrs)
    #f.close()
    #############################

    com = serial.Serial()
    com.port = port
    com.baudrate = baud
    com.timeout = 60 # TODO: make this longer
    com.dtr = True
    com.open()

    retry_count = 0
    print("getting status...")
    while 1==1:
        try:
            async with asyncio.timeout(10):
                print(await commands.get_status(com))
            break
        except TimeoutError:
            if retry_count >= 5:
                print("Failed to get status.")
                exit(1)
            print("Timeout. Retrying...")

    io_threads = asyncio.gather(
        asyncio.to_thread(commands.run_process_rx(com)),
        asyncio.to_thread(run_auto_test(com)),
        asyncio.to_thread(run_user_input(com))
    )

    # init UI
    app.run_server()
    await io_threads

if __name__ == '__main__':
    asyncio.run(main())
