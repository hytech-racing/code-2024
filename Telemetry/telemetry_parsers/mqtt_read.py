
from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct
import numpy as np
from cobs import cobs

MQTT_SERVER = "3.134.2.166"
MQTT_PORT   = 1883
MQTT_TOPIC  = 'HT07/CAN'
client = mqtt.Client()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))

    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    byte_obj = msg.payload
    bytearray_obj = bytearray(byte_obj)
    split_list = bytearray_obj.split(b'\x00')
    for byte_msg in split_list:
        if len(byte_msg) > 1:
            print(byte_msg)
            decoded = cobs.decode(byte_msg)
            msg_id = decoded[0]
            msg_len = decoded[4]
            msg_data = decoded[5:5+8]
            print(decoded)
            #while len(msg_data) < 8:
            #    msg_data += b'\x00'
            print(len(msg_data))
            msg_data = np.frombuffer(msg_data, dtype=np.uint64)[0]
            msg_data = (np.uint64(np.uint64(msg_data) << np.uint16(8*(8-msg_len))) >> np.uint16(8*(8-msg_len)))
            print(msg_id)
            l = MESSAGE_DICT[int(msg_id)][0](msg_data)
            upload_parsed_data(l)

def upload_parsed_data(data_list):
    for i in data_list:
        directory = i[3]
        for j in range(len(directory)):
            topic = directory[j].replace(".", "/")
            payload = i[0].flatten()[j]
            #print(payload)
            #payload = struct.pack('<f', payload)
            payload = str(payload)
            publish_data(topic, payload)

def publish_data(topic, payload):
    client.publish(topic, payload)

def publish_data(topic, payload):
    client.publish(topic, payload)

def initialize_mqtt():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()




initialize_mqtt()