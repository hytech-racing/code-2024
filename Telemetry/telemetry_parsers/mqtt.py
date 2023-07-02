from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct
import numpy as np

MQTT_SERVER = "3.134.2.166"
MQTT_PORT   = 1883
MQTT_TOPIC  = 'HT07/CAN'
client = mqtt.Client()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))

    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    #print(msg.topic + " " + str(msg.payload))
    message = msg.payload.decode('ascii').rstrip('\x00')
    msg_list = str(message).split("\n")
    for i in range(len(msg_list)-1):
        msg_split = msg_list[i].split(",")
        l = MESSAGE_DICT[int(msg_split[1], base=16)][0](np.uint64(int(msg_split[3], base=16)).byteswap()>>np.uint16(8*(8-int(msg_split[2]))))
        #print(l)
        upload_parsed_data(l)


def initialize_mqtt():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()

def upload_parsed_data(data_list):
    for i in data_list:
        directory = i[3]
        for j in range(len(directory)):
            topic = directory[j].replace(".", "/")
            payload = i[0].flatten()[j]
            #print(payload)
            payload = struct.pack('<f', payload)

            publish_data(topic, payload)

def publish_data(topic, payload):
    client.publish(topic, payload)

initialize_mqtt()