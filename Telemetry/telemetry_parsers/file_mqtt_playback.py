
from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct
import numpy as np
from cobs import cobs
import serial
import time
import vector_parser_api as vp
import threading
import pandas as pd




def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))

def upload_parsed_data(data_list):
    for i in data_list:
        directory = i[3]
        for j in range(len(directory)):
            topic = directory[j].replace(".", "/")
            payload = i[0].flatten()[j]
            #print(payload)
            #payload = struct.pack('<f', payload)
            payload = str(payload)
            #print(topic)
            #print(payload)
            publish_data(topic, payload)
            #publish_data("HT07_PARSED/"+topic, payload)

def publish_data(topic, payload):
    client.publish(topic, payload)

def initialize_mqtt():
    client.on_connect = on_connect
    #client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_start()

def csv_proc(file):
    df = vp.read_csv(file)
    df["time"] = df["time"]-df["time"][0]
    return df

def csv_step(df, t):
    global loop
    index = np.where(df["time"] >= (t*1000))[0][0]
    start_t = time.time()
    while loop:
        l = MESSAGE_DICT[int(df["msg.id"][index])][0](df["data"][index])
        upload_parsed_data(l)
        if index+1 >= df["time"].size:
            break;
        else:
            sleeptime = (df["time"][index+1] - df["time"][index])
            if sleeptime < -950:
                sleeptime = 1
            time.sleep((sleeptime)*.001)
        index += 1
    print(f"Finished in {time.time()-start_t} seconds")
    
#MQTT_SERVER = "localhost"
MQTT_PORT   = 1883
client = mqtt.Client()
loop = True

print("AWS IP: 3.134.2.166")
MQTT_SERVER = input("Input MQTT server (default: localhost): ") or "localhost"

initialize_mqtt()
time.sleep(.1)

filename = input(r"Input a filename (.csv or .pkl): ").replace("\"","")
df = pd.DataFrame()
if filename.lower().endswith(('.csv')):
    start_t = time.time()
    df = csv_proc(filename)
    print(f"Parsed in {time.time() - start_t} seconds")
elif filename.lower().endswith(('.pkl')):
    df = pd.read_pickle(filename)

while True:
    loop = True
    max_t = df["time"][df["time"].size-1]/1000
    print(f"File length: {max_t} seconds")
    start_time = float(input("Input the time to start at: "))
    thread = threading.Thread(target=csv_step, args=(df, start_time))
    thread.daemon = True
    print("Press Enter to stop the thread...")
    thread.start()
    input("")
    loop = False
    thread.join()
