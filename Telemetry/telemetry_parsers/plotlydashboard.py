from dash import Dash, html, dcc
from dash.dependencies import Output, Input
import dash_bootstrap_components as dbc
from plotly import graph_objects as go

import paho.mqtt.client as mqtt
import struct
import numpy as np
import time
import re

MQTT_SERVER = "3.134.2.166"
MQTT_PORT   = 1883
MQTT_TOPIC_LIST  = ['BMS/detailed_voltage/#']
client = mqtt.Client()

datadict = {}

a = 0

def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))
    for topic in MQTT_TOPIC_LIST:
        client.subscribe(topic)

def on_message(client, userdata, msg):
    if msg.topic in datadict.keys():
        datadict[msg.topic] = np.append(datadict[msg.topic], [[time.time(), struct.unpack("<f",msg.payload)[0]]], axis = 0)
    else:
        datadict[msg.topic] = np.array([[time.time(), struct.unpack("<f",msg.payload)[0]]])

def initialize_mqtt():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_start()

app = Dash(__name__, external_stylesheets=[dbc.themes.BOOTSTRAP, dbc.icons.BOOTSTRAP])

app.layout = dbc.Container([
    html.Div(id="hidden-div", style={"display":"none"}),
    html.H1("HyTech Racing Live Telemetry", className="display-3"),
    html.Hr(className="my-2"),
    #dbc.Button("Clear Data", id = 'data_clear_button', color="primary", className="me-1"),
    dcc.Graph(
        id='detailed_voltages_graph'),
    dcc.Graph(
        id='detailed_temps_graph'),
    dcc.Interval(
            id='interval-component',
            interval=1000, # in milliseconds
            n_intervals=0
        )
])

#@app.callback(
#        Input("data_clear_button", "n_clicks")
#)
def clear_data(n):
    datadict = {}

@app.callback(
        Output('detailed_temps_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def detailed_temps_graph(n):

    keys = get_keys_matching_regex(datadict, "BMS\/detailed_temperatures\/ic_\d*\/cell_\d*")
    data = create_scatter_plots(keys)
    return {'data': data,
            'layout': {
            'title': 'Detailed Temps'
        }}
    '''
    return {'data': [data],
            'layout' : go.Layout(xaxis=dict(
                    range=[min(X),max(X)]),yaxis = 
                    dict(range = [min(l),max(l)]),
                    )}
    '''

@app.callback(
        Output('detailed_voltages_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def update_detailed_voltages_graph_live(n):
    #global a
    keys = get_keys_matching_regex(datadict, "BMS\/detailed_voltage\/ic_\d*\/cell_\d*")
    data = create_scatter_plots(keys)
    #a = a+1
    #print(a)
    
    return {'data': data,
            'layout': {
            'title': 'Detailed Voltages'
        }}
    '''
    return {'data': [data],
            'layout' : go.Layout(xaxis=dict(
                    range=[min(X),max(X)]),yaxis = 
                    dict(range = [min(l),max(l)]),
                    )}
    '''

def get_keys_matching_regex(dictionary, regex):
    pattern = re.compile(regex)
    matching_keys = [key for key in dictionary.keys() if pattern.match(key)]
    return matching_keys

def create_scatter_plots(topics):
    graphs = []
    for t in topics:
        graphs.append(
            go.Scatter(
                x = datadict[t][:,0].astype('datetime64[s]'),
                y = datadict[t][:,1],
                name = t,
                mode= 'lines'
            )
        )
    return graphs

if __name__ == '__main__':
    initialize_mqtt()
    app.run_server(debug=True)