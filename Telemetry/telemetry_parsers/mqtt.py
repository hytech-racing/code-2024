from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt

MQTT_SERVER = ""
MQTT_PORT   = 1883
MQTT_TOPIC  = 'hytech_car/telemetry'

def on_connect(client, userdata, flags, rc, topic):
    print("Connected with result code: " + str(rc))

    client.subscribe(topic)

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

def initialize_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()

def upload_parsed_data(list):
    for i in list:
        directory = i[3]
        for i in len(directory):
            topic = directory[i].replace(".", "/")
            publish_data(topic)

def publish_data(topic, client):
    