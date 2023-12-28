
from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct
import numpy as np
import time
from cobs import cobs
from random import randint

MQTT_SERVER = "3.134.2.166"
MQTT_PORT   = 1883
MQTT_TOPIC  = 'HT07/CAN'
client = mqtt.Client()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))

    client.subscribe(MQTT_TOPIC)


#publish_data("HT07_PARSED/"+topic, payload)

def publish_data(topic, payload):
    print(client.publish(topic, payload))

def initialize_mqtt():
    client.on_connect = on_connect
    #client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    #client.loop_forever()




initialize_mqtt()
time.sleep(0.1)
while True:
    publish_data("HT07_PARSED/BMS/detailed_temps/ic", randint(0, 20))