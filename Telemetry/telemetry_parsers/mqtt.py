from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct

MQTT_SERVER = ""
MQTT_PORT   = 1883
MQTT_TOPIC  = '+/can_message'
client = mqtt.Client()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))

    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

def initialize_mqtt():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()

def upload_parsed_data(list):
    for i in list:
        directory = i[3]
        for j in len(directory):
            topic = directory[j].replace(".", "/")
            payload = i[0][j]
            payload = struct.pack('<f', payload)

            publish_data(topic, payload)

def publish_data(topic, payload):
    client.publish(topic, payload)