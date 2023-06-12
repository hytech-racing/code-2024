from dash import Dash, html, dcc
from dash.dependencies import Output, Input
import dash_bootstrap_components as dbc
from plotly import graph_objects as go
from plotly.subplots import make_subplots

import paho.mqtt.client as mqtt
import struct
import numpy as np
import time
import re

#MQTT_SERVER = "3.134.2.166"
MQTT_SERVER = "localhost"
MQTT_PORT   = 1883
MQTT_TOPIC_LIST  = ['BMS/#', "MOTOR_CONTROLLER/#", "TPMS/#", "MCU/load_cells/#"]
client = mqtt.Client()

datadict = {}

a = 0
root = {}

#import pickle
#with open("data0076.pkl", "rb") as file:
#    root = pickle.load(file)
#highdate = np.datetime64("2023-06-01T18:13:50.7")
#lowdate = highdate - np.timedelta64(5, 'm')
#
#root2 = {}
#with open("data0100.pkl", "rb") as file:
#    root2 = pickle.load(file)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))
    for topic in MQTT_TOPIC_LIST:
        client.subscribe(topic)

#def on_message(client, userdata, msg):
#    if msg.topic in datadict.keys():
#        datadict[msg.topic] = np.append(datadict[msg.topic], [[time.time(), struct.unpack("<f",msg.payload)[0]]], axis = 0)
#    else:
#        datadict[msg.topic] = np.array([[time.time(), struct.unpack("<f",msg.payload)[0]]])

def on_message(client, userdata, msg):
    ref, key = index_from_key(root, msg.topic)
    if key in ref.keys():
        ref[key] = np.append(ref[key], [[time.time(), float(msg.payload.decode("utf-8"))]], axis = 0)
    else:
        ref[key] = np.array([[time.time(), float(msg.payload.decode("utf-8"))]])

def index_from_key(root, keystring):
    keystring = keystring.replace("HT07_PARSED/","")
    keys = re.split("[.]|[\/]|[\\]|[\[]", keystring)
    cwd = root
    for k in keys[:-1]:
        if k not in cwd.keys():
            cwd[k] = {}
        cwd = cwd[k]
    return cwd, keys[-1]

def initialize_mqtt():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_start()

app = Dash(__name__, external_stylesheets=[dbc.themes.BOOTSTRAP, dbc.icons.BOOTSTRAP])

#app.css.config.serve_locally = True
app.scripts.config.serve_locally = True

app.layout = dbc.Container([
#app.layout = html.Div([
    html.Div(id="hidden-div", style={"display":"none"}),
    html.H1("HyTech Racing Live Telemetry", className="display-3"),
    html.Hr(className="my-2"),
    #dbc.Button("Clear Data", id = 'data_clear_button', color="primary", className="me-1"),
    dcc.Graph(
        id='minimum_voltages_graph'),
    dcc.Graph(
        id='motor_temps_graph'),
    dcc.Graph(
        id='tyre_temps_graph'),
    dcc.Graph(
        id='inverter_temps_graph'),
    dcc.Graph(
        id='igbt_temps_graph'),
    dcc.Graph(
        id='maximum_temp_graph'),
    dcc.Graph(
        id='load_cells_graph'),
    dcc.Graph(
        id='tyre_pressures_graph'),
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
        Output('motor_temps_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def motor_temps_graph(n):

    #keys = get_keys_matching_regex(datadict, "BMS\/detailed_temperatures\/ic_\d*\/cell_\d*")
    #data = create_scatter_plots(keys)
    
    fldata = root["MOTOR_CONTROLLER"]["mc_fl"]["motor_temp"][-1000:,:]
    frdata = root["MOTOR_CONTROLLER"]["mc_fr"]["motor_temp"][-1000:,:]
    rldata = root["MOTOR_CONTROLLER"]["mc_rl"]["motor_temp"][-1000:,:]
    rrdata = root["MOTOR_CONTROLLER"]["mc_rr"]["motor_temp"][-1000:,:]

    data = [
        go.Scatter(
        x = (fldata[:,0]*1000).astype('datetime64[ms]'),
        y = fldata[:,1],
        name = "Front Left"
    ),
        go.Scatter(
        x = (frdata[:,0]*1000).astype('datetime64[ms]'),
        y = frdata[:,1],
        name = "Front Right"
    ),
        go.Scatter(
        x = (rldata[:,0]*1000).astype('datetime64[ms]'),
        y = rldata[:,1],
        name = "Rear Left"
    ),
        go.Scatter(
        x = (rrdata[:,0]*1000).astype('datetime64[ms]'),
        y = rrdata[:,1],
        name = "Rear Right"
    )]
    
    return {'data': data,
            'layout': go.Layout(
        title = dict(text="Motor Temperature"),
        legend = dict(x = 0, y = 1, bgcolor = 'rgba(255,255,255,.7)'),
        yaxis_title = "Temps (C)"
        )
                    
        }
    '''
    return {'data': [data],
            'layout' : go.Layout(xaxis=dict(
                    range=[min(X),max(X)]),yaxis = 
                    dict(range = [min(l),max(l)]),
                    )}
    '''

@app.callback(
        Output('minimum_voltages_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def minimum_voltages_graph(n):
    #keys = get_keys_matching_regex(datadict, "BMS\/detailed_voltage\/ic_\d*\/cell_\d*")
    #data = create_scatter_plots(keys)
    d = root["BMS"]["voltages"]["low_voltage"][-1000:,:]
    data = [go.Scatter(
        x = (d[:,0]*1000).astype('datetime64[ms]'),
        y = d[:,1],
    )]


    return {'data': data,
            'layout': go.Layout(
        title = dict(text="Minimum Cell Voltage"),
        yaxis_title = "Voltage (V)"
        )}
    '''
    return {'data': [data],
            'layout' : go.Layout(xaxis=dict(
                    range=[min(X),max(X)]),yaxis = 
                    dict(range = [min(l),max(l)]),
                    )}
    '''

@app.callback(
        Output('tyre_temps_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def tyre_temps_graph(n):
    def get_data(data, corner, times):
        out = np.zeros((times,16))
        for i in range(16):
            out[:,i] = data["TPMS"][corner]["temp_"+str(i+1)][-1*times:,1]
        return out
    points = 16
    fig = make_subplots(rows=2, cols=2, subplot_titles=("Front Left","Front Right", "Rear Left", "Rear Right"))
    
    fig.add_trace(row = 1, col = 1,
                  trace = go.Heatmap(
                    z = get_data(root, "LF", points)[::-1,::-1],
                    x = [str(i) for i in np.arange(16,0,-1)],
                    y = ["T-"+str(i) for i in np.arange(points,0,-1)],
                    coloraxis = "coloraxis",
                  )
                  )
    
    fig.add_trace(row = 1, col = 2,
                  trace = go.Heatmap(
                    z = get_data(root, "RF", points)[::-1,:],
                    x = [str(i) for i in np.arange(1,17)],
                    y = ["T-"+str(i) for i in np.arange(points,0,-1)],
                    coloraxis = "coloraxis",
                  )
                  )
    
    fig.add_trace(row = 2, col = 1,
                  trace = go.Heatmap(
                    z = get_data(root, "LR", points)[::-1,::-1],
                    x = [str(i) for i in np.arange(16,0,-1)],
                    y = ["T-"+str(i) for i in np.arange(points,0,-1)],
                    coloraxis = "coloraxis",
                  )
                  )
    
    fig.add_trace(row = 2, col = 2,
                  trace = go.Heatmap(
                    z = get_data(root, "RR", points)[::-1,:],
                    x = [str(i) for i in np.arange(1,17)],
                    y = ["T-"+str(i) for i in np.arange(points,0,-1)],
                    coloraxis = "coloraxis",
                  )
                  )
    
    fig.update_yaxes(title_text="Time", row=1, col=1)
    fig.update_yaxes(title_text="Time", row=1, col=2)
    fig.update_yaxes(title_text="Time", row=2, col=1)
    fig.update_yaxes(title_text="Time", row=2, col=2)
    
    
    fig.update_layout(height=800, width=1000, title_text="Tire Temps")
    fig.update_layout(coloraxis=dict(colorscale = 'RdBu_r'), showlegend=False)
    fig.update_coloraxes(colorbar=dict(title='Temps (C)'))

    return fig

@app.callback(
        Output('igbt_temps_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def igbt_temps_graph(n):

    #keys = get_keys_matching_regex(datadict, "BMS\/detailed_temperatures\/ic_\d*\/cell_\d*")
    #data = create_scatter_plots(keys)
    
    fldata = root["MOTOR_CONTROLLER"]["mc_fl"]["igbt_temp"][-1000:,:]
    frdata = root["MOTOR_CONTROLLER"]["mc_fr"]["igbt_temp"][-1000:,:]
    rldata = root["MOTOR_CONTROLLER"]["mc_rl"]["igbt_temp"][-1000:,:]
    rrdata = root["MOTOR_CONTROLLER"]["mc_rr"]["igbt_temp"][-1000:,:]

    data = [
        go.Scatter(
        x = (fldata[:,0]*1000).astype('datetime64[ms]'),
        y = fldata[:,1],
        name = "Front Left"
    ),
        go.Scatter(
        x = (frdata[:,0]*1000).astype('datetime64[ms]'),
        y = frdata[:,1],
        name = "Front Right"
    ),
        go.Scatter(
        x = (rldata[:,0]*1000).astype('datetime64[ms]'),
        y = rldata[:,1],
        name = "Rear Left"
    ),
        go.Scatter(
        x = (rrdata[:,0]*1000).astype('datetime64[ms]'),
        y = rrdata[:,1],
        name = "Rear Right"
    )]
    
    return {'data': data,
            'layout': go.Layout(
        title = dict(text="IGBT Temperature"),
        legend = dict(x = 0, y = 1, bgcolor = 'rgba(255,255,255,.7)'),
        yaxis_title = "Temps (C)"
        )
                    
        }
    '''
    return {'data': [data],
            'layout' : go.Layout(xaxis=dict(
                    range=[min(X),max(X)]),yaxis = 
                    dict(range = [min(l),max(l)]),
                    )}
    '''

@app.callback(
        Output('inverter_temps_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def inverter_temps_graph(n):

    #keys = get_keys_matching_regex(datadict, "BMS\/detailed_temperatures\/ic_\d*\/cell_\d*")
    #data = create_scatter_plots(keys)
    
    fldata = root["MOTOR_CONTROLLER"]["mc_fl"]["inverter_temp"][-1000:,:]
    frdata = root["MOTOR_CONTROLLER"]["mc_fr"]["inverter_temp"][-1000:,:]
    rldata = root["MOTOR_CONTROLLER"]["mc_rl"]["inverter_temp"][-1000:,:]
    rrdata = root["MOTOR_CONTROLLER"]["mc_rr"]["inverter_temp"][-1000:,:]

    data = [
        go.Scatter(
        x = (fldata[:,0]*1000).astype('datetime64[ms]'),
        y = fldata[:,1],
        name = "Front Left"
    ),
        go.Scatter(
        x = (frdata[:,0]*1000).astype('datetime64[ms]'),
        y = frdata[:,1],
        name = "Front Right"
    ),
        go.Scatter(
        x = (rldata[:,0]*1000).astype('datetime64[ms]'),
        y = rldata[:,1],
        name = "Rear Left"
    ),
        go.Scatter(
        x = (rrdata[:,0]*1000).astype('datetime64[ms]'),
        y = rrdata[:,1],
        name = "Rear Right"
    )]
    
    return {'data': data,
            'layout': go.Layout(
        title = dict(text="Inverter Temperature"),
        legend = dict(x = 0, y = 1, bgcolor = 'rgba(255,255,255,.7)'),
        yaxis_title = "Temps (C)"
        )
                    
        }
    '''
    return {'data': [data],
            'layout' : go.Layout(xaxis=dict(
                    range=[min(X),max(X)]),yaxis = 
                    dict(range = [min(l),max(l)]),
                    )}
    '''

@app.callback(
        Output('maximum_temp_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def maximum_temp_graph(n):
    #keys = get_keys_matching_regex(datadict, "BMS\/detailed_voltage\/ic_\d*\/cell_\d*")
    #data = create_scatter_plots(keys)
    d = root["BMS"]["temperatures"]["high_temperature"][-1000:,:]
    data = [go.Scatter(
        x = (d[:,0]*1000).astype('datetime64[ms]'),
        y = d[:,1],
    )]


    return {'data': data,
            'layout': go.Layout(
        title = dict(text="Maximum Cell Temperature"),
        yaxis_title = "Temp (C)"
        )}
    '''
    return {'data': [data],
            'layout' : go.Layout(xaxis=dict(
                    range=[min(X),max(X)]),yaxis = 
                    dict(range = [min(l),max(l)]),
                    )}
    '''


@app.callback(
        Output('load_cells_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def load_cells_graph(n):

    #keys = get_keys_matching_regex(datadict, "BMS\/detailed_temperatures\/ic_\d*\/cell_\d*")
    #data = create_scatter_plots(keys)
    
    fldata = root["MCU"]["load_cells"]["FL"][-100:,:]
    frdata = root["MCU"]["load_cells"]["FR"][-100:,:]
    rldata = root["MCU"]["load_cells"]["RL"][-100:,:]
    rrdata = root["MCU"]["load_cells"]["RR"][-100:,:]

    data = [
        go.Scatter(
        x = (fldata[:,0]*1000).astype('datetime64[ms]'),
        y = fldata[:,1],
        name = "Front Left"
    ),
        go.Scatter(
        x = (frdata[:,0]*1000).astype('datetime64[ms]'),
        y = frdata[:,1],
        name = "Front Right"
    ),
        go.Scatter(
        x = (rldata[:,0]*1000).astype('datetime64[ms]'),
        y = rldata[:,1],
        name = "Rear Left"
    ),
        go.Scatter(
        x = (rrdata[:,0]*1000).astype('datetime64[ms]'),
        y = rrdata[:,1],
        name = "Rear Right"
    )]
    
    return {'data': data,
            'layout': go.Layout(
        title = dict(text="Load Cells"),
        legend = dict(x = 0, y = 1, bgcolor = 'rgba(255,255,255,.7)'),
        yaxis_title = "Force (lb)"
        )
                    
        }

@app.callback(
        Output('tyre_pressures_graph', 'figure'),
        Input('interval-component', 'n_intervals')
    )
def tyre_pressures_graph(n):

    #keys = get_keys_matching_regex(datadict, "BMS\/detailed_temperatures\/ic_\d*\/cell_\d*")
    #data = create_scatter_plots(keys)
    
    fldata = root["TPMS"]["LF"]["guage_pressure"][-10:,:]
    frdata = root["TPMS"]["RF"]["guage_pressure"][-10:,:]
    rldata = root["TPMS"]["LR"]["guage_pressure"][-10:,:]
    rrdata = root["TPMS"]["RR"]["guage_pressure"][-10:,:]

    data = [
        go.Scatter(
        x = (fldata[:,0]*1000).astype('datetime64[ms]'),
        y = fldata[:,1]*14.50377,
        name = "Front Left"
    ),
        go.Scatter(
        x = (frdata[:,0]*1000).astype('datetime64[ms]'),
        y = frdata[:,1]*14.50377,
        name = "Front Right"
    ),
        go.Scatter(
        x = (rldata[:,0]*1000).astype('datetime64[ms]'),
        y = rldata[:,1]*14.50377,
        name = "Rear Left"
    ),
        go.Scatter(
        x = (rrdata[:,0]*1000).astype('datetime64[ms]'),
        y = rrdata[:,1]*14.50377,
        name = "Rear Right"
    )]
    
    return {'data': data,
            'layout': go.Layout(
        title = dict(text="Tire Guage Pressure"),
        legend = dict(x = 0, y = 1, bgcolor = 'rgba(255,255,255,.7)'),
        yaxis_title = "Tire Pressure (psi)"
        )
                    
        }

def get_keys_matching_regex(dictionary, regex):
    pattern = re.compile(regex)
    matching_keys = [key for key in dictionary.keys() if pattern.match(key)]
    return matching_keys

def create_scatter_plots(topics):
    graphs = []
    for t in topics:
        graphs.append(
            go.Scatter(
                x = (datadict[t][:,0]*1000).astype('datetime64[ms]'),
                y = datadict[t][:,1],
                name = t,
                mode= 'lines'
            )
        )
    return graphs

if __name__ == '__main__':
    initialize_mqtt()
    app.run_server(host= '0.0.0.0',debug=True)