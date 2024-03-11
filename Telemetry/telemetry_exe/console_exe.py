'''
@Author: Sophia Smith, Bo Han Zhu
@Date: 4/23/2022
@Description: HyTech live telemetry console
'''


import PySimpleGUI as sg, sys
import time
import threading
import serial
from os import path
from enum import Enum
from datetime import datetime
import paho.mqtt.client as mqtt
import itertools
import binascii
import struct
import sys
import argparse

__file__ = sys.path[0]
sys.path.insert(1, "../telemetry_parsers")
from parser_api import parse_message
sys.path.insert(1, '../telemetry_aws')
from db import unpack

parser = argparse.ArgumentParser(description="Telemetry console")
parser.add_argument('--mode', '-m', action='store', default='0', required=False, help="Mode for parser to run in")
parser.add_argument('--font', '-f', action='store', default='Courier New', required=False, help="Font to use")
parser.add_argument('--title_size', '-ts', action='store', default='12', required=False, help="Title font")
parser.add_argument('--body_size', '-bs', action='store', default='8', required=False, help='Body font size')

args = parser.parse_args()


# Connection type definitions
class ConnectionType(Enum):
    SERVER = 0
    ESP32 = 1
    TEST_CSV = 2
    UNKNOWN = 3

# AWS/MQTT Connection Definitions
MQTT_SERVER = "ec2-3-134-2-166.us-east-2.compute.amazonaws.com"
MQTT_PORT   = 1883
MQTT_TOPIC  = 'hytech_car/telemetry'

# ESP32 Connection Defintions
COM_PORT = 'COM4' # Needs to be set to the right COM Port with the ESP32
BAUD_RATE = 115200

# Set this to whatever the script is running
# @TODO: Make a screen at the beginning to let the user choose the type

CONNECTION = {'0': ConnectionType.SERVER.value,
              '1': ConnectionType.ESP32.value,
              '2': ConnectionType.TEST_CSV.value,
              '3': ConnectionType.UNKNOWN.value}[args.mode]

DICT = {
    "DASHBOARD": {
        "SSOK_ABOVE_THRESHOLD": " ",
        "SHUTDOWN_H_ABOVE_THRESHOLD": " "
    },
    "BATTERY_MANAGEMENT_SYSTEM": {
        "BMS_VOLTAGE_AVERAGE": " ",
        "BMS_VOLTAGE_LOW": " ",
        "BMS_VOLTAGE_HIGH": " ",
        "BMS_VOLTAGE_TOTAL": " ",
        "BMS_AVERAGE_TEMPERATURE": " ",
        "BMS_LOW_TEMPERATURE": " ",
        "BMS_HIGH_TEMPERATURE": " ",
        "BMS_CURRENT": " ",
        "BMS_TOTAL_CHARGE": " ",
        "BMS_TOTAL_DISCHARGE": " ",
        "BMS_STATE": " ",
        "BMS_ERROR_FLAGS": " ",
    },
    "ENERGY_METER": {
        "VOLTAGE": " ",
        "CURRENT": " ",
        "OVERPOWER": " ",
        "OVERVOLTAGE": " ",
        "LOGGING": " ",
        "VOLTAGE_GAIN": " ",
        "CURRENT_GAIN": " ",
    },
    "IMU": {
        "LAT_ACCEL": " ",
        "LONG_ACCEL": " ",
        "VERT_ACCEL": " ",
        "YAW": " ",
        "PITCH": " ",
        "ROLL": " "
    },
    "RMS_INVERTER" : {
        "OUTPUT_POWER": " ",
        "RMS_UPTIME": " ",
        "INVERTER_ENABLE_STATE": " ",
        "MOTOR_SPEED": " ",
        "MOTOR_ANGLE": " ",
        "ELEC_OUTPUT_FREQ": " ",
        "COMMANDED_TORQUE": " ",
        "TORQUE_FEEDBACK": " ",
        "DC_BUS_VOLTAGE": " ",
        "OUTPUT_VOLTAGE": " ",
        "PHASE_AB_VOLTAGE": " ",
        "PHASE_BC_VOLTAGE": " ",
        "DC_BUS_CURRENT": " ",
        "PHASE_A_CURRENT": " ",
        "PHASE_B_CURRENT": " ",
        "PHASE_C_CURRENT": " ",
        "MOTOR_TEMPERATURE": " ",
        "GATE_DRIVER_BOARD_TEMPERATURE": " ",
        "MODULE_A_TEMPERATURE": " ",
        "MODULE_B_TEMPERATURE": " ",
        "MODULE_C_TEMPERATURE": " ",
        "TORQUE_SHUDDER": " ",
        "INVERTER_STATE": " ",
        "VSM_STATE": " ",
        "INVERTER_ACTIVE_DISCHARGE_STATE": " ",
        "INVERTER_COMMAND_MODE": " ",
        "DIRECTION_COMMAND": " ",
        "POST_FAULT_LO": " ",
        "POST_FAULT_HI": " ",
        "RUN_FAULT_LO": " ",
        "RUN_FAULT_HI": " ",
    },
    "MAIN_ECU": {
        "GLV_BATTERY_VOLTAGE": " ",
        "ECU_CURRENT": " ",
        "COOLING_CURRENT": " ",
        "TEMPERATURE": " ",
        "IMD_OK_HIGH": " ",
        "BMS_OK_HIGH": " ",
        "BSPD_OK_HIGH": " ",
        "SOFTWARE_OK_HIGH": " ",
        "INVERTER_POWERED": " ",
        "TORQUE_MODE": " ",
        "MAX_TORQUE": " ",
        "REQUESTED_TORQUE": " ",
        "ACCELERATOR_PEDAL_1": " ",
        "ACCELERATOR_PEDAL_2": " ",
        "NO_ACCEL_IMPLAUSIBILITY": " ",
        "BRAKE_TRANSDUCER_1": " ",
        "BRAKE_TRANSDUCER_2": " ",
        "BRAKE_PEDAL_ACTIVE": " ",
        "NO_BRAKE_IMPLAUSIBILITY": " ",
        "LAUNCH_CTRL_ACTIVE": " ",
    },
    "SENSOR_ACQUISITION_BOARD": {
        "COOLING_LOOP_FLUID_TEMP": " ",
        "AMB_AIR_TEMP": " ",
        "FL_SUSP_LIN_POT": " ",
        "FR_SUSP_LIN_POT": " ",
        "BL_SUSP_LIN_POT": " ",
        "BR_SUSP_LIN_POT": " ",
    },
    "WHEEL_SPEED_SENSORS": {
        "RPM_BACK_LEFT": " ",
        "RPM_BACK_RIGHT": " ",
        "RPM_FRONT_LEFT": " ",
        "RPM_FRONT_RIGHT": " "
    },
    "LOAD_CELLS": {
        "FL_LOAD_CELL": " ",
        "FR_LOAD_CELL": " ",
        "RL_LOAD_CELL": " ",
        "RR_LOAD_CELL": " "
    }
}

# Variables to keep track of inverter current and power for inverter power calculation
inverter_voltage = 0.0
inverter_current = 0.0
inverter_power = 0.0
ALPHA = 0.95 # for filtering

'''
@brief: Helper function to search for keys in a nested dictionary
@reference: https://stackoverflow.com/questions/49662970/how-can-i-search-for-specific-keys-in-this-nested-dictionary-in-python
@param[in]: k - the key to search
@param[in]: d - the (nested) dictionary
@param[out]: true if found, false if not
'''
def recursive_lookup(k, d):
    if k in d: return d[k]
    for v in d.values():
        if isinstance(v, dict):
            a = recursive_lookup(k, v)
            if a is not None: return a
    return None

'''
@brief: Helper function to calculate inverter power if inverter voltage or current is updated
@param[in]: name - name of the parsed label
@param[in]: data - the parsed data; if used, it will be a floating-type value
@param[in]: window - the PySimpleGUI window object
'''
def handle_inverter_power(name, data, window):
    # Specially handle Inverter output power since it is not a CAN label and needs calculation
    if name == "DC_BUS_CURRENT" or name == "DC_BUS_VOLTAGE":
        if name == "DC_BUS_VOLTAGE":
            global inverter_voltage
            inverter_voltage = data
        else:
            global inverter_current
            inverter_current = data
        
        # Power = voltage * current
        # Apply filtering constant so changes are not so volatile
        global inverter_power
        inverter_power = round(ALPHA * inverter_power + (1.0 - ALPHA) * inverter_current * inverter_voltage, 2)

        window.write_event_value("-Update Data-", ["OUTPUT_POWER", "OUTPUT POWER: " + str(inverter_power) + " W"])

'''
@brief: Thread to read raw CSV line, parse it, and send event to GUI if match 
        Sends event to close GUI upon CSV read completion
        Requires a raw data CSV in the current directory with the name raw_data.csv
@param[in]: window - the PySimpleGUI window object
'''
def read_from_csv_thread(window):
    infile = open("raw_data.csv", "r")
    line_count =  1 # bypass first header line
    raw_data_lines = infile.readlines()
    window.write_event_value("-Test Connection Success-", "good job!")

    while line_count < len(raw_data_lines):
        raw_id = raw_data_lines[line_count].split(",")[1]
        length = raw_data_lines[line_count].split(",")[2]
        raw_message = raw_data_lines[line_count].split(",")[3]
        raw_message = raw_message[:(int(length) * 2)] # Strip trailing end of line/file characters that may cause bad parsing
        raw_message = raw_message.zfill(16) # Sometimes messages come truncated if 0s on the left. Append 0s so field-width is 16.
        table = parse_message(raw_id, raw_message)
        
        if table != "INVALID_ID" and table != "UNPARSEABLE":
            for i in range(len(table[1])):
                name = table[1][i].upper()
                data = table[2][i]
                units = table[3][i]
                if name == "MCU_STATE":
                    window.write_event_value("-MCU State Change-", [data.replace("_", " ")])
                elif recursive_lookup(name, DICT):
                    window.write_event_value("-Update Data-", [name, name.replace("_", " ") + ": " + str(data) + " " + units])
                    handle_inverter_power(name, data, window)

        line_count += 1

    window.write_event_value("-Read CSV Done-", "No data for you left")

'''
@brief: Thread to connect to ESP32 via PySerial.
        Parses incoming messages and packages them as an event to the GUI if match.
@param[in]: window - the PySimpleGUI window object
'''
def handle_esp32_thread(window):
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    window.write_event_value("-Connection Success-", "good job!")

    while True:
        line =  ser.readline()
        if line:
            string = line.decode()
            vals = string.split(",")

            id =  hex(int(vals[0]))[2:]
            raw = hex(int(vals[1]))[2:]

            table = parse_message(id, raw)
            if table != "INVALID_ID" and table != "UNPARSEABLE":
                for i in range(len(table[1])):
                    name = table[1][i].upper()
                    data = table[2][i]
                    units = table[3][i]
                    if name == "MCU_STATE":
                        window.write_event_value("-MCU State Change-", [data.replace("_", " ")])
                    elif recursive_lookup(name, DICT):
                        window.write_event_value("-Update Data-", [name, name.replace("_", " ") + ": " + str(data) + " " + units])
                        handle_inverter_power(name, data, window)


'''
@brief: Thread to connect to MQTT broker on AWS EC2 instance.
        Parses incoming messages and packages them as an event to the GUI if match.
@param[in]: window - the PySimpleGUI window object
'''
def mqtt_connection_thread(window):
    ##############################
    # Paho MQTT Callback Functions
    ##############################

    '''
    @brief: Callback function to handle MQTT messages. Runs them through the parser if valid and sends update event to GUI.
    @param[in]: client - unused
    @param[in]: userdata - unused
    @param[in]: msg - the raw hexlified MQTT message
    '''
    def mqtt_message(client, userdata, msg):
        frame = msg.payload[msg.payload.find(b',') + 1:-1]
        frame = binascii.hexlify(frame)
        data = unpack("".join(chr(c) for c in frame))

        if data != -1:
            id = format(data[0], 'x').upper()
            size = data[4]
            # Catch when leadings zeros got stripped by struct.unpack and account for that
            subtracts = 0
            if data[5] == 0x0: # data[5] is first index of message payload
                subtracts += 2
            elif data[5] <= 0xF:
                subtracts += 1   
            raw = format(struct.unpack(">1Q", data[5:13])[0], 'x')[:size*2 - subtracts].zfill(16)   

            table = parse_message(id, raw)
            if table != "INVALID_ID" and table != "UNPARSEABLE":
                for i in range(len(table[1])):
                    name = table[1][i].upper()
                    data = table[2][i]
                    units = table[3][i]
                    if name == "MCU_STATE":
                        window.write_event_value("-MCU State Change-", [data.replace("_", " ")])
                    elif recursive_lookup(name, DICT):
                        window.write_event_value("-Update Data-", [name, name.replace("_", " ") + ": " + str(data) + " " + units])
                        handle_inverter_power(name, data, window)

    '''
    @brief: Callback function for MQTT connection success. Sends an connection succes event to the GUI.
    @param[in]: client - the MQTT client object
    @param[in]: userdata - unused
    @param[in]: flags - unused
    @param[in]: rc - unused
    '''
    def mqtt_connect(client, userdata, flags, rc):
        window.write_event_value("-Connection Success-", "good job!")
        client.subscribe(MQTT_TOPIC)

    ################################
    # Paho MQTT Setup and Connection
    ################################
    client = mqtt.Client()
    client.on_connect = mqtt_connect
    client.on_message = mqtt_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()

'''
@brief: Helper function to get multi-columns rows for detailed messages
@return: dictionary, an list of combined elements 
'''
def get_bms_detailed_messages():
    ic_list = ['IC_' + '{0:02d}'.format(x) for x in range(12)]
    cell_list_odd = ['CELL_' + str(x) for x in range(9)]
    cell_list_even = ['CELL_' + str(x) for x in range(12)]
    ic_cells = []
    
    for i in range(12):
        # Even ICs have 12 cells
        if i % 2 == 0:
            ic_cells += [ic_list[i] + '_' + cell for cell in cell_list_even]
        # Odd ICs have 9 cells
        else:
            ic_cells += [ic_list[i] + '_' + cell for cell in cell_list_odd]
    
    result = dict.fromkeys(ic_cells, ' ')
    dictionary = {"BATTERY_MANAGEMENT_SYSTEM_DETAILED_VOLTAGES": result}


    ic_list2 = ['IC_' + '{0:02d}'.format(x) for x in range(12)]
    temperature_list = ['THERM_' + str(x) for x in range(4)]
    ic_temperature = [ic + '_' + therm for ic in ic_list2 for therm in temperature_list]
    ic_temperature = ic_temperature + ["IC_0_HUMIDITY", "IC_2_HUMIDITY", "IC_4_HUMIDITY", "IC_6_HUMIDITY", "IC_8_HUMIDITY", "IC_10_HUMIDITY"]
    ic_temperature = ic_temperature + ["IC_1_TEMPERATURE", "IC_3_TEMPERATURE", "IC_5_TEMPERATURE", "IC_7_TEMPERATURE", "IC_9_TEMPERATURE", "IC_11_TEMPERATURE"]
    result2 = dict.fromkeys(ic_temperature, ' ')
    dictionary2 = {"BATTERY_MANAGEMENT_SYSTEM_DETAILED_TEMPERATURES": result2}
    return dictionary, dictionary2




'''
@brief: The main function to spawn the PySimpleGUI and handle events
'''
def main():
    sg.change_look_and_feel("Black")
    title_font = (args.font, int(args.title_size))
    text_font = (args.font, int(args.body_size))
    

    # Subtitle text declarations
    inverter = [[sg.Text("RMS INVERTER", pad=(0,2), font=title_font, text_color="light blue")]]
    dashboard = [[sg.Text("DASHBOARD", pad=(0,2), font=title_font, text_color="light blue")]]
    bms = [[sg.Text("BMS OVERVIEW", pad=(0,2), font=title_font, text_color="light blue")]]
    main_ecu = [[sg.Text("MAIN ECU", pad=(0,2), font=title_font, text_color="light blue")]]
    wheel_speed_sensors = [[sg.Text("WHEEL SPEED SENSORS", pad=(0,2), font=title_font, text_color="light blue")]]
    load_cells = [[sg.Text("LOAD CELLS", pad=(0,2), font=title_font, text_color="light blue")]]
    sab = [[sg.Text("SENSOR ACQUISITION BOARD", pad=(0,2), font=title_font, text_color="light blue")]]
    imu = [[sg.Text("IMU", pad=(0,2), font=title_font, text_color="light blue")]]
    em = [[sg.Text("ENERGY METER", pad=(0,2), font=title_font, text_color="light blue")]]
    bms_detailed_voltages = [[sg.Text("BMS DETAILED VOLTAGES", size=(33,1), pad=(0,2), font=title_font, text_color="light blue")]]
    bms_detailed_temps = [[sg.Text("BMS DETAILED TEMPERATURES", pad=(0,2), font=title_font, text_color="light blue")]]

    bms_voltages = [[]]
    bms_temperatures = [[]]
    
    DICT1, DICT2 = get_bms_detailed_messages()
    DICT.update(DICT1)
    DICT.update(DICT2)
    row_count_temperatures = 0
    row_count_voltages = 0
    
    # Data text arrangements and manipulations
    for label, value in DICT["RMS_INVERTER"].items():
        inverter.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(40,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["BATTERY_MANAGEMENT_SYSTEM"].items():
        bms.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["MAIN_ECU"].items():
        main_ecu.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["DASHBOARD"].items():
        dashboard.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["WHEEL_SPEED_SENSORS"].items():
        wheel_speed_sensors.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["LOAD_CELLS"].items():
        load_cells.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["SENSOR_ACQUISITION_BOARD"].items():
        sab.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["IMU"].items():
        imu.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["ENERGY_METER"].items():
        em.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(35,1), pad=(0,0), font=text_font, key=label)])
    for label, value in DICT["BATTERY_MANAGEMENT_SYSTEM_DETAILED_VOLTAGES"].items():
        # if row_count_voltages % 9 == 8 and int(label[3]) == 3 or int(label[3]) == 9:
            # No padding for ICs 3 and 7 last cells since text will take care of it in the next column over
        #    bms_voltages.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(23,1), pad=(0,0), font=text_font, key=label)])
        #    row_count_voltages = 0
        if row_count_voltages % 9 == 8 and int(label[3:5]) % 2 == 1:
            bms_voltages.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(23,1), pad=((0,0),(0,10)), font=text_font, key=label)])
            row_count_voltages = 0
        elif row_count_voltages % 12 == 11 and int(label[3:5]) % 2 == 0:
            bms_voltages.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(23,1), pad=((0,0),(0,10)), font=text_font, key=label)])
            row_count_voltages = 0
        else:            
            bms_voltages.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(23,1), pad=(0,0), font=text_font, key=label)])
            row_count_voltages = row_count_voltages + 1
    for label, value in DICT["BATTERY_MANAGEMENT_SYSTEM_DETAILED_TEMPERATURES"].items():
        if row_count_temperatures >= 48:
            bms_temperatures.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(25,1), pad=(0,0), font=text_font, key=label)])
        elif row_count_temperatures % 4 == 3:
            bms_temperatures.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(23,1), pad=((0,0),(0,10)), font=text_font, key=label)])
        else:
            bms_temperatures.append([sg.Text(label.replace("_", " ") + ": " + value, justification="left", size=(23,1), pad=(0, 0),font=text_font, key=label)])
        row_count_temperatures = row_count_temperatures + 1

    # We ran out of room so ICs 3 and 7 will be on column with BMS detailed temps
    left_voltages_first_column = bms_voltages[:34]
    right_voltages_first_column = bms_voltages[64:97]
    left_voltages_second_column = bms_voltages[34:64]
    right_voltages_second_column = bms_voltages[97:]
    
    first_half_therm = bms_temperatures[:25]
    second_half_therm = bms_temperatures[25:49]
    therm_humidities = bms_temperatures[49:55]
    therm_temperatures = bms_temperatures[55:]

    voltages = [[sg.Column(left_voltages_first_column, pad=(0,0), vertical_alignment='t'), sg.Column(right_voltages_first_column, pad=(0,0), vertical_alignment='t')]]
    voltages_second_column = [[sg.Column(left_voltages_second_column, pad=(0,0), vertical_alignment='t'), sg.Column(right_voltages_second_column, pad=(0,0), vertical_alignment='t')]]
    temperatures = [[sg.Column(first_half_therm + therm_humidities, pad=(0,0), vertical_alignment='t'), sg.Column(second_half_therm + therm_temperatures, pad=(0,0), vertical_alignment='t')]]

    # Header texts and columns
    connection_text = [[sg.Text("CONSOLE STATUS: NOT CONNECTED", justification="left", pad=((5,0),12), text_color='red', font=title_font, key="-Connection Text-")]]
    divider_text_1 = [[sg.Text(" | ", pad=(5,12), font=title_font)]]
    vehicle_status_text = [[sg.Text("VEHICLE STATUS: NOT RECEIVED", justification="left", pad=((0,0),12), font=title_font, key="-Vehicle Status Text-")]]
    divider_text_2 = [[sg.Text(" | ", pad=(5,12), font=title_font)]]
    last_update_text = [[sg.Text("LAST UPDATE: NOT RECEIVED", justification="left", pad=((0,5),12), font=title_font, key="-Last Update Text-")]]

    status_header_column1 = sg.Column(connection_text, pad=(0,0), vertical_alignment='t')
    status_header_column2 = sg.Column(divider_text_1, pad=(0,0), vertical_alignment='t')
    status_header_column3 = sg.Column(vehicle_status_text, pad=(0,0), vertical_alignment='t')
    status_header_column4 = sg.Column(divider_text_2, pad=(0,0), vertical_alignment='t')
    status_header_column5 = sg.Column(last_update_text, pad=(0,0), vertical_alignment='t')

    # Data colummns
    column1 = sg.Column(dashboard + [[sg.Text(" ", size=(35,1), pad=(0,0), font=text_font)]] + bms + [[sg.Text(" ", size=(35,1), pad=(0,0), font=text_font)]] + em + [[sg.Text(" ", size=(35,1), pad=(0,0), font=text_font)]] + imu, vertical_alignment='t')
    column2 = sg.Column(main_ecu + [[sg.Text(" ", size=(35,1), pad=(0,0), font=text_font)]] + sab + [[sg.Text(" ", size=(35,1), pad=(0,0), font=text_font)]] + wheel_speed_sensors + [[sg.Text(" ", size=(35,1), pad=(0,0), font=text_font)]] + load_cells, vertical_alignment='t')
    column3 = sg.Column(inverter, vertical_alignment='t')
    column4 = sg.Column(bms_detailed_voltages + voltages, vertical_alignment='t')
    column5 = sg.Column(voltages_second_column + [[sg.Text(" ", size=(35,1), pad=(0,0), font=text_font)]], vertical_alignment='t')
    column6 = sg.Column(bms_detailed_temps + temperatures, vertical_alignment='t')

    # Finalize layout
    layout = [[status_header_column1, status_header_column2, status_header_column3, status_header_column4, status_header_column5], [column1, column2, column3, column4, column5, column6]]

    window = sg.Window("HyTech Racing Live Telemetry Console", resizable=True).Layout(layout).Finalize()
    window.Maximize()

    # Choose messaging thread based on connection type
    if CONNECTION == ConnectionType.SERVER.value:
        thread = threading.Thread(target=mqtt_connection_thread, args=[window], daemon=True)
    elif CONNECTION == ConnectionType.ESP32.value:
        thread = threading.Thread(target=handle_esp32_thread, args=[window], daemon=True)
    elif CONNECTION == ConnectionType.TEST_CSV.value:
        thread = threading.Thread(target=read_from_csv_thread, args=[window], daemon=True)
    else:
        sys.exit("Invalid connection source selection. Terminating script")

    thread.start()

    # Event Loop
    while True:
        event, values = window.read()

        if event in (sg.WIN_CLOSED, "Quit"):
            break
        elif event == "-Read CSV Done-":
            thread.join(timeout=0)
            break
        elif event == "-Test Connection Success-":
            window["-Connection Text-"].update("CONSOLE STATUS: TESTING", text_color="yellow")
        elif event == "-Connection Success-":
            window["-Connection Text-"].update("CONSOLE STATUS: CONNECTED", text_color="green")
        elif event == "-MCU State Change-":
            received_status = values["-MCU State Change-"][0]
            status_color = ""

            if received_status == "STARTUP": status_color = "cyan"
            elif received_status == "TRACTIVE SYSTEM NOT ACTIVE": status_color = "light grey"
            elif received_status == "TRACTIVE SYSTEM ACTIVE": status_color = "orange"
            elif received_status == "ENABLING INVERTER": status_color = "yellow"
            elif received_status == "WAITING READY TO DRIVE SOUND": status_color = "green yellow"
            elif received_status == "READY TO DRIVE": status_color = "green"
            elif received_status == "UNRECOGNIZED STATE": status_color = "red"
            else: status_color = "red" # Should not get here since parser will output UNRECOGNIZED STATE if invalid; here just as a failsafe

            window["-Vehicle Status Text-"].update("VEHICLE STATUS: " + received_status, text_color=status_color)
            window.refresh()
        elif event == "-Update Data-":
            window[values["-Update Data-"][0]].update(values["-Update Data-"][1])
            window["-Last Update Text-"].update("LAST UPDATE: " + datetime.now().strftime('%H:%M:%S.%f')[:-5])
            window.refresh()

    window.close()

############################
# Entry point to application
############################
main()
