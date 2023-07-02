
from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct
import numpy as np
from cobs import cobs
import serial
import time
import threading

def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))

    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    byte_obj = msg.payload
    bytearray_obj = bytearray(byte_obj)
    split_list = bytearray_obj.split(b'\x00')
    for byte_msg in split_list:
        if len(byte_msg) > 1:
            #print(byte_msg)
            decoded = cobs.decode(byte_msg)
            msg_id = decoded[0]
            msg_len = decoded[4]
            msg_data = decoded[5:5+8]
            #print(decoded)
            #while len(msg_data) < 8:
            #    msg_data += b'\x00'
            #print(len(msg_data))
            msg_data = np.frombuffer(msg_data, dtype=np.uint64)[0]
            msg_data = (np.uint64(np.uint64(msg_data) << np.uint16(8*(8-msg_len))) >> np.uint16(8*(8-msg_len)))
            #print(msg_id)
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
            print(topic, end= ": ")
            print(payload)
            publish_data(topic, payload)
            #publish_data("HT07_PARSED/"+topic, payload)

def publish_data(topic, payload):
    client.publish(topic, payload)

def initialize_mqtt():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_start()

def print_messages_recieved():
    global messages_received, previous_time, bytes_received, transmissions_received
    while loop:
        current_time = time.time()
        elapsed_time = current_time - previous_time
        if elapsed_time >= 1.0:
            print(f"Messages recieved: {messages_received}, Bytes : {bytes_received}, Transmissions: {transmissions_received}")
            previous_time += 1.0
            messages_received = 0
            bytes_received = 0
            transmissions_received = 0
    

def read_serial_until_double_null(serial_port):
    global messages_received, bytes_received, transmissions_received
    data = b""
    while True:
        # Read a single byte from the serial port
        byte = serial_port.read(1)
        #print("hell3")
        # Append the byte to the data
        data += byte
        bytes_received += 1
        #print(byte,end="")
        #messages_received += 1
        
        # Check if two consecutive \x00 characters are encountered
        if data[-2:] == b"\x00\x00":
            #print("",end="\n")
            transmissions_received += 1
            break
        #if data[-1:] == b"\x00":
        #    print("",end="\n")
        #    break

    
    # Split the data on the character \x00
    #print(data)
    split_data = data.split(b"\x00")
    
    for byte_msg in split_data:
        print(len(byte_msg), end=" ")
        if len(byte_msg) == 16:
            decoded = cobs.decode(byte_msg)
            msg_id = decoded[0]
            print(hex(msg_id), end="")
            msg_len = decoded[4]
            msg_data = decoded[5:5+8]
            #print(decoded)
            #while len(msg_data) < 8:
            #    msg_data += b'\x00'
            #print(len(msg_data))
            msg_data = np.frombuffer(msg_data, dtype=np.uint64)[0]
            msg_data = (np.uint64(np.uint64(msg_data) << np.uint16(8*(8-msg_len))) >> np.uint16(8*(8-msg_len)))
            #print(msg_id)
            l = MESSAGE_DICT[int(msg_id)][0](msg_data)
            upload_parsed_data(l)
            messages_received += 1
        print("")

    return split_data

MQTT_PORT   = 1883
MQTT_TOPIC  = 'HT07/CAN'
client = mqtt.Client()

print("AWS IP: 3.134.2.166")
MQTT_SERVER = input("Input MQTT server (default: localhost): ") or "localhost"

# Specify the serial port and other settings
PORT = input("Input the serial port: ")
BAUDRATE = 115200

# Open the serial port
initialize_mqtt()
serial_port = serial.Serial(PORT, BAUDRATE)

previous_time = time.time()
messages_received = 0
bytes_received = 0
transmissions_received = 0

loop = True
thread = threading.Thread(target=print_messages_recieved)
thread.daemon = True
thread.start()

try:
    while True:
        # Read from the serial port until double null is encountered
        #print("hell0")
        result = read_serial_until_double_null(serial_port)

        # Print the split data
        #print(result)
        

        
except KeyboardInterrupt:
    print("Ctrl+C detected. Exiting...")

loop = False
thread.join()

# Close the serial port
serial_port.close()