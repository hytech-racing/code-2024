import pandas as pd
import numpy as np

def parse_BMS_balancing_status(ordering, id, data):
    msg_id = 0xDE
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_BMS_coulomb_counts(ordering, id, data):
    msg_id = 0xE2
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_BMS_detailed_temperatures(ordering, id, data):
    msg_id = 0xDA
    mask = id==msg_id
    data = data[mask]
    
    # (Size (bits), Position (bits), lambda func, name, units)
    vectors = [
        (4, -1, lambda x: x, "id", ""),
        (4, -1, lambda x: x, "group", ""),
        (16, -1, lambda x: x/100, "temps", ""),
        (16, -1, lambda x: x/100, "temps", ""),
        (16, -1, lambda x: x/100, "temps", "")
    ]
    offsets = np.zeros((len(vectors),),dtype=np.uint16)
    masks = np.zeros((len(vectors),),dtype=np.uint64)
    for i in range(len(vectors)):
        mask[i] = np.uint64((1 << vectors[i][0]) - 1)
        if vectors[i][1] == -1 and i > 0:
            offsets[i] = offsets[i-1]
        elif vectors[i][1] > -1:
            offsets[i] = vectors[i][1]
        else:
            offsets[i] = 0
    
    
    """
    out = np.zeros((data.size, 5))
    out[:,0] = data & 0xF
    out[:,1] = (data & (0xF<<4))>>4
    out[:,2] = (data & (0xFFFF<<8))>>8
    out[:,3] = (data & (0xFFFF<<24))>>24
    out[:,4] = (data & (0xFFFF<<40))>>40
    """
    #df = pd.DataFrame(arr, columns='cols')
    return out

def parse_BMS_detailed_voltages(ordering, id, data):
    msg_id = 0xD8
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_BMS_onboard_detailed_temperatures(ordering, id, data):
    msg_id = 0xD6
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_BMS_onboard_temperatures(ordering, id, data):
    msg_id = 0xD5
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_BMS_status(ordering, id, data):
    msg_id = 0xDB
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_BMS_temperatures(ordering, id, data):
    msg_id = 0xD9
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_BMS_voltages(ordering, id, data):
    msg_id = 0xD7
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_CCU_status(ordering, id, data):
    msg_id = 0xDD
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_Charger_configure(ordering, id, data):
    msg_id = 0x1806E0F4
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_Charger_data(ordering, id, data):
    msg_id = 0x18FF50E0
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_Dashboard_status(ordering, id, data):
    msg_id = 0xEB
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MCU_GPS_readings(ordering, id, data):
    msg_id = 0xE7
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MCU_pedal_readings(ordering, id, data):
    msg_id = 0xC4
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MCU_status(ordering, id, data):
    msg_id = 0xC3
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MCU_analog_readings(ordering, id, data):
    msg_id = 0xCC
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MCU_load_cells(ordering, id, data):
    msg_id = 0xC5
    mask = id==msg_id
    data = data[mask]
    
    arr = np.zeros((data.size, 4))
    arr[:,0] = (data & 0xFFFF)
    arr[:,1] = (data & 0xFFFF<<16)>>48
    arr[:,2] = (data & 0xFFFF<<32)>>48
    arr[:,3] = (data & 0xFFFF<<48)>>48

    #df = pd.DataFrame(arr, columns='cols')
    return arr

def parse_MC1_energy(ordering, id, data):
    msg_id = 0xA8
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC2_energy(ordering, id, data):
    msg_id = 0xA9
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC3_energy(ordering, id, data):
    msg_id = 0xAA
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC4_energy(ordering, id, data):
    msg_id = 0xAB
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC1_setpoints_command(ordering, id, data):
    msg_id = 0xB0
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC2_setpoints_command(ordering, id, data):
    msg_id = 0xB1
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC3_setpoints_command(ordering, id, data):
    msg_id = 0xB2
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC4_setpoints_command(ordering, id, data):
    msg_id = 0xB3
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC1_status(ordering, id, data):
    msg_id = 0xA0
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC2_status(ordering, id, data):
    msg_id = 0xA1
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC3_status(ordering, id, data):
    msg_id = 0xA2
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC4_status(ordering, id, data):
    msg_id = 0xA3
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC1_temps(ordering, id, data):
    msg_id = 0xA4
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC2_temps(ordering, id, data):
    msg_id = 0xA5
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC3_temps(ordering, id, data):
    msg_id = 0xA6
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_MC4_temps(ordering, id, data):
    msg_id = 0xA7
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_SAB_readings_front(ordering, id, data):
    msg_id = 0x92
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_SAB_readings_rear(ordering, id, data):
    msg_id = 0x93
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_SAB_readings_gps(ordering, id, data):
    msg_id = 0xEE
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_EM_status(ordering, id, data):
    msg_id = 0x100
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_EM_measurement(ordering, id, data):
    msg_id = 0x400
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_IMU_accelerometer(ordering, id, data):
    msg_id = 0x90
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 

def parse_IMU_gryoscope(ordering, id, data):
    msg_id = 0x91
    mask = id==msg_id
    data = data[mask]
    


    #df = pd.DataFrame(arr, columns='cols')
    return 
