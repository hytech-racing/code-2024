
from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct
import numpy as np
import time
from cobs import cobs
from random import randint
import pandas as pd
import csv



#MQTT_SERVER = "3.134.2.166"
MQTT_SERVER = "localhost"
MQTT_PORT   = 1883
MQTT_TOPIC  = 'HT08/CAN'
client = mqtt.Client()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))

    client.subscribe(MQTT_TOPIC)


#publish_data("HT07_PARSED/"+topic, payload)

def publish_data(topic, payload):
    (client.publish(topic, payload))

def initialize_mqtt():
    client.on_connect = on_connect
    #client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    #client.loop_forever()




initialize_mqtt()
time.sleep(0.1)

data = []
with open('test_data.csv', mode ='r') as file:    
       csvFile = csv.DictReader(file)
       for lines in csvFile:
            lineList = (list(lines.values()))
            data.append([lineList[0], lineList[1]])


index = 0
while True:
    name = input('Press Enter ')
    
    if (index == 70):
         break
    index+=1
    publish_data("MCU/GPS/longitude", data[index][0])
    publish_data("MCU/GPS/latitude", data[index][1])
    print(index)
    '''
    data =  randint(0, 20)
    publish_data("MCU/GPS/msl",data)
    print(data)
    time.sleep(1)
    '''
    

    