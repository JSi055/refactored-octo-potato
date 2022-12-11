
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

analogs = [0,1,2,3]
input_q = queue.Queue(200)

# arrays for saving data points
traces_data = {}

currentx = 0


def update_traces():
    global traces_data, currentx

    # take a bunch of data from the queue
    while not input_q.empty():
        column = input_q.get(block=False)
        for i, point in enumerate(column):
            id = analogs[i]
            if id not in traces_data:
                traces_data[id] = [[],[]]
            X = traces_data[id][0]
            Y = traces_data[id][1]

            X.append(currentx)
            Y.append(point)

            if len(Y) > 1000:
                del (Y[0])
                del (X[0])

        currentx = currentx + 1

    traces = []
    for id in traces_data:
        traces.append(go.Scatter(
            name = "A" + str(id),
            x = traces_data[id][0],
            y = traces_data[id][1]
        ))


    return traces

def update_figure():
    global X, Y, currentx, input_q

    plot = go.Figure(
        data=update_traces(), layout=dict(
            xaxis_title="time",
            yaxis_title="Voltage",
            yaxis_range=[-1.0, 6.0]
            # yaxis=dict(domain=[0.0, 0.5])
        )
    )
    return plot


# GUI layout/design
app = dash.Dash(__name__, update_title=None)
app.layout = html.Div(
    children = [
        html.H1('Logic Scope Thingy '),
        dcc.Graph(id='live-graph', figure=update_figure()),
        dcc.Interval(id='update_graph', interval=200)
        #html.Button(id='textout', children='text')
    ]
)


def read_input(c: serial.Serial):
    while c.isOpen():
        command = c.read()
        if len(command) != 1:
            raise TimeoutError("Serial timed out waiting for packet")

        if command == b's':

            values = []
            for i in range(len(analogs)):
                v_bytes = c.read(2)
                if len(v_bytes) != 2:
                    raise TimeoutError("Serial timed out while reading packet " + command)
                values.append((int.from_bytes(v_bytes, "big") / 1024.0) * 5.0)
            input_q.put(values)

        time.sleep(0.0001) # 100us - just in case com.read() is not blocking right? idk

        r = random.randint(0,40)
        if r == 0:
            c.write(b"o\x01")
        if r == 1:
            c.write(b"o\x00")


# update part where data is read and the plot and data arrays are updated
@app.callback(Output('live-graph', 'figure'),
              [Input('update_graph', 'n_intervals')])
def update(i_input):
    return update_figure()


if __name__ == '__main__':

    port = '/dev/ttyUSB0'
    baud = 115200
    if len(sys.argv) > 0:
        port = sys.argv[1]

    #############################
    # magical mystery code from https://stackoverflow.com/questions/15460865/disable-dtr-in-pyserial-from-code
    # somehow makes it so DTR does *not* get pulled low when opening the connection
    f = open(port)
    attrs = termios.tcgetattr(f)
    attrs[2] = attrs[2] & ~termios.HUPCL
    termios.tcsetattr(f, termios.TCSAFLUSH, attrs)
    f.close()
    #############################

    com = serial.Serial()
    com.port = port
    com.baudrate = baud
    com.timeout = 60 # TODO: make this longer
    com.dtr = True
    com.open()

    # start getting data
    com.write(b"d" + bytes([9]))              # stream analogs
    com.write(b"a" + bytes([bits(analogs)]))  # stream analogs
    com.write(b"m" + bytes([0x01]))           # enable output on PIN2

    io_thread = threading.Thread(target=read_input, args=(com,))
    io_thread.start()

    # init UI
    app.run_server()
