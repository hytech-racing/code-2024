# MQTT_SERVER = "hytech-virtual-machine"
# MQTT_PORT   = 1883
# MQTT_TOPIC  = 'hytech_car/telemetry'

import paho.mqtt.client as mqtt 
from random import randrange, uniform
import numpy as np
import time

mqttBroker = "test.mosquitto.org"    #"hytech-virtual-machine" 

client = mqtt.Client("I dont think it really matters")
client.connect(mqttBroker)
cnt = 0


while True:
    randNumber = np.round(np.sin(2*np.pi*(cnt/100)),2) #uniform(20.0, 21.0)
    cnt = (cnt+1)%100
    print(randNumber)
    client.publish("BMS/detailed_voltage/ic_00", randNumber)
    client.publish("BMS/detailed_voltage/ic_02", randNumber + 1)
    client.publish("BMS/detailed_voltage/ic_04", randNumber + 2)
    client.publish("BMS/detailed_voltage/ic_06", randNumber + 3)
    client.publish("BMS/detailed_voltage/ic_08", randNumber - 1)
    client.publish("BMS/detailed_voltage/ic_10", randNumber - 2)
    print("Just published " + str(randNumber) + " to topic")
    time.sleep(.1)