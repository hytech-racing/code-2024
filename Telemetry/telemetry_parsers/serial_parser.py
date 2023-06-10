
from parsers import MESSAGE_DICT
import paho.mqtt.client as mqtt
import struct
import numpy as np
from cobs import cobs
import serial

MQTT_SERVER = "localhost"
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
            print(topic)
            print(payload)
            publish_data(topic, payload)
            #publish_data("HT07_PARSED/"+topic, payload)

def publish_data(topic, payload):
    print(client.publish(topic, payload))

def initialize_mqtt():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()


def read_serial_until_double_null(serial_port):
    data = b""
    while True:
        # Read a single byte from the serial port
        byte = serial_port.read(1)
        
        # Append the byte to the data
        data += byte
        
        # Check if two consecutive \x00 characters are encountered
        if data[-2:] == b"\x00\x00":
            break
    
    # Split the data on the character \x00
    split_data = data.split(b"\x00")
    
    for byte_msg in split_data:
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

    return split_data

# Specify the serial port and other settings
port = "COM5"
baudrate = 115200

# Open the serial port
serial_port = serial.Serial(port, baudrate)

try:
    while True:
        # Read from the serial port until double null is encountered
        result = read_serial_until_double_null(serial_port)

        # Print the split data
        #print(result)
        
        # Add a delay or perform other tasks
        
except KeyboardInterrupt:
    print("Ctrl+C detected. Exiting...")

# Close the serial port
serial_port.close()