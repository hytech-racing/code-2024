# MQTT_SERVER = "hytech-virtual-machine"
# MQTT_PORT   = 1883
# MQTT_TOPIC  = 'hytech_car/telemetry'

import paho.mqtt.client as mqtt 
from random import randrange, uniform
import numpy as np
import time

mqttBroker ="hytech-virtual-machine" 

client = mqtt.Client("Temperature_Inside")
client.connect(mqttBroker)
cnt = 0


while True:
    randNumber = np.round(np.sin(2*np.pi*(cnt/100)),2) #uniform(20.0, 21.0)
    cnt = (cnt+1)%100
    print(randNumber)
    client.publish("TEMPERATURE", randNumber)
    print("Just published " + str(randNumber) + " to topic TEMPERATURE")
    time.sleep(.1)