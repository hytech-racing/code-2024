import pandas as pd
import numpy as np
from parser_helpers import get_offsets_masks, parse_to_np

MESSAGE_DICT = {}

# (Size (bits), Position (bits), signed, lambda func, name, units)

def parse_BMS_balancing_status(id, data):
    msg_id = 0xDE
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xDE] = parse_BMS_balancing_status 

def parse_BMS_coulomb_counts(id, data):
    msg_id = 0xE2
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xE2] = parse_BMS_coulomb_counts 

def parse_BMS_detailed_temperatures(id, data):
    msg_id = 0xDA
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (4 , -1, False, lambda x: x    , "id"    , ""),
        (4 , -1, False, lambda x: x    , "group" , ""),
        (16, -1, False, lambda x: x/100, "temps" , ""),
        (16, -1, False, lambda x: x/100, "temps" , ""),
        (16, -1, False, lambda x: x/100, "temps" , "")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return out
MESSAGE_DICT[0xDA] = parse_BMS_detailed_temperatures

def parse_BMS_detailed_voltages(id, data):
    msg_id = 0xD8
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xD8] = parse_BMS_detailed_voltages 

def parse_BMS_onboard_detailed_temperatures(id, data):
    msg_id = 0xD6
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xD6] = parse_BMS_onboard_detailed_temperatures 

def parse_BMS_onboard_temperatures(id, data):
    msg_id = 0xD5
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xD5] = parse_BMS_onboard_temperatures 

def parse_BMS_status(id, data):
    msg_id = 0xDB
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xDB] = parse_BMS_status 

def parse_BMS_temperatures(id, data):
    msg_id = 0xD9
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xD9] = parse_BMS_temperatures 

def parse_BMS_voltages(id, data):
    msg_id = 0xD7
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (16, -1, False, lambda x: x/10000 , "average voltage" , "V"),
        (16, -1, False, lambda x: x/10000 , "low voltage"     , "V"),
        (16, -1, False, lambda x: x/10000 , "high voltage"    , "V"),
        (16, -1, False, lambda x: x/100   , "total voltage"   , "V")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xD7] = parse_BMS_voltages 

def parse_CCU_status(id, data):
    msg_id = 0xDD
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xDD] = parse_CCU_status 

def parse_Charger_configure(id, data):
    msg_id = 0x1806E0F4
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x1806E0F4] = parse_Charger_configure 

def parse_Charger_data(id, data):
    msg_id = 0x18FF50E0
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x18FF50E0] = parse_Charger_data 

def parse_Dashboard_status(id, data):
    msg_id = 0xEB
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xEB] = parse_Dashboard_status 

def parse_MCU_GPS_readings(id, data):
    msg_id = 0xE7
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xE7] = parse_MCU_GPS_readings 

def parse_MCU_pedal_readings(id, data):
    msg_id = 0xC4
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xC4] = parse_MCU_pedal_readings 

def parse_MCU_status(id, data):
    msg_id = 0xC3
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xC3] = parse_MCU_status 

def parse_MCU_analog_readings(id, data):
    msg_id = 0xCC
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xCC] = parse_MCU_analog_readings 

def parse_MCU_load_cells(id, data):
    msg_id = 0xC5
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (16, -1, False, lambda x: x , "" , "lb"),
        (16, -1, False, lambda x: x , "" , "lb"),
        (16, -1, False, lambda x: x , "" , "lb"),
        (16, -1, False, lambda x: x , "" , "lb")
    ]
    '''arr = np.zeros((data.size, 4))
    arr[:,0] = (data & 0xFFFF)
    arr[:,1] = (data & 0xFFFF<<16)>>48
    arr[:,2] = (data & 0xFFFF<<32)>>48
    arr[:,3] = (data & 0xFFFF<<48)>>48'''

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return arr
MESSAGE_DICT[0xC5] = parse_MCU_load_cells

def parse_MC1_energy(id, data):
    msg_id = 0xA8
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", ""),
        (1, -1, False, lambda x: x, "error", ""),
        (1, -1, False, lambda x: x, "warning", ""),
        (1, -1, False, lambda x: x, "quit_dc_on", ""),
        (1, -1, False, lambda x: x, "dc_on", ""),
        (1, -1, False, lambda x: x, "quit_inverter_on", ""),
        (1, -1, False, lambda x: x, "inverter_on", ""),
        (1, -1, False, lambda x: x, "derating_on", ""),
        (16, 16, True, lambda x: x, "speed", "rpm"),
        (16, -1, True, lambda x: x/10, "torque_current", "% mn"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA8] = parse_MC1_energy 

def parse_MC2_energy(id, data):
    msg_id = 0xA9
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", ""),
        (1, -1, False, lambda x: x, "error", ""),
        (1, -1, False, lambda x: x, "warning", ""),
        (1, -1, False, lambda x: x, "quit_dc_on", ""),
        (1, -1, False, lambda x: x, "dc_on", ""),
        (1, -1, False, lambda x: x, "quit_inverter_on", ""),
        (1, -1, False, lambda x: x, "inverter_on", ""),
        (1, -1, False, lambda x: x, "derating_on", ""),
        (16, 16, True, lambda x: x, "speed", "rpm"),
        (16, -1, True, lambda x: x/10, "torque_current", "% mn"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA9] = parse_MC2_energy 

def parse_MC3_energy(id, data):
    msg_id = 0xAA
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", ""),
        (1, -1, False, lambda x: x, "error", ""),
        (1, -1, False, lambda x: x, "warning", ""),
        (1, -1, False, lambda x: x, "quit_dc_on", ""),
        (1, -1, False, lambda x: x, "dc_on", ""),
        (1, -1, False, lambda x: x, "quit_inverter_on", ""),
        (1, -1, False, lambda x: x, "inverter_on", ""),
        (1, -1, False, lambda x: x, "derating_on", ""),
        (16, 16, True, lambda x: x, "speed", "rpm"),
        (16, -1, True, lambda x: x/10, "torque_current", "% mn"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xAA] = parse_MC3_energy 

def parse_MC4_energy(id, data):
    msg_id = 0xAB
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", ""),
        (1, -1, False, lambda x: x, "error", ""),
        (1, -1, False, lambda x: x, "warning", ""),
        (1, -1, False, lambda x: x, "quit_dc_on", ""),
        (1, -1, False, lambda x: x, "dc_on", ""),
        (1, -1, False, lambda x: x, "quit_inverter_on", ""),
        (1, -1, False, lambda x: x, "inverter_on", ""),
        (1, -1, False, lambda x: x, "derating_on", ""),
        (16, 16, True, lambda x: x, "speed", "rpm"),
        (16, -1, True, lambda x: x/10, "torque_current", "% mn"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xAB] = parse_MC4_energy 

def parse_MC1_setpoints_command(id, data):
    msg_id = 0xB0
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xB0] = parse_MC1_setpoints_command 

def parse_MC2_setpoints_command(id, data):
    msg_id = 0xB1
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xB1] = parse_MC2_setpoints_command 

def parse_MC3_setpoints_command(id, data):
    msg_id = 0xB2
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xB2] = parse_MC3_setpoints_command 

def parse_MC4_setpoints_command(id, data):
    msg_id = 0xB3
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xB3] = parse_MC4_setpoints_command 

def parse_MC1_status(id, data):
    msg_id = 0xA0
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA0] = parse_MC1_status 

def parse_MC2_status(id, data):
    msg_id = 0xA1
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA1] = parse_MC2_status 

def parse_MC3_status(id, data):
    msg_id = 0xA2
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA2] = parse_MC3_status 

def parse_MC4_status(id, data):
    msg_id = 0xA3
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA3] = parse_MC4_status 

def parse_MC1_temps(id, data):
    msg_id = 0xA4
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C"),
        (16, -1, False, lambda x: x, "diagnostic_number", ""),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA4] = parse_MC1_temps 

def parse_MC2_temps(id, data):
    msg_id = 0xA5
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C"),
        (16, -1, False, lambda x: x, "diagnostic_number", ""),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA5] = parse_MC2_temps 

def parse_MC3_temps(id, data):
    msg_id = 0xA6
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C"),
        (16, -1, False, lambda x: x, "diagnostic_number", ""),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA6] = parse_MC3_temps 

def parse_MC4_temps(id, data):
    msg_id = 0xA7
    mask = id==msg_id
    data = data[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C"),
        (16, -1, False, lambda x: x, "diagnostic_number", ""),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C")
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xA7] = parse_MC4_temps 

def parse_SAB_readings_front(id, data):
    msg_id = 0x92
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x92] = parse_SAB_readings_front 

def parse_SAB_readings_rear(id, data):
    msg_id = 0x93
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x93] = parse_SAB_readings_rear 

def parse_SAB_readings_gps(id, data):
    msg_id = 0xEE
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0xEE] = parse_SAB_readings_gps 

def parse_EM_status(id, data):
    msg_id = 0x100
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x100] = parse_EM_status 

def parse_EM_measurement(id, data):
    msg_id = 0x400
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x400] = parse_EM_measurement 

def parse_IMU_accelerometer(id, data):
    msg_id = 0x90
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x90] = parse_IMU_accelerometer 

def parse_IMU_gryoscope(id, data):
    msg_id = 0x91
    mask = id==msg_id
    data = data[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    out = parse_to_np(data, vectors, bitoffsets, bitmasks)

    #df = pd.DataFrame(arr, columns='cols')
    return
MESSAGE_DICT[0x91] = parse_IMU_gryoscope 
