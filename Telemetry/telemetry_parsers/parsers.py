import pandas as pd
import numpy as np
from parser_helpers import get_offsets_masks, parse_to_np

NOMINAL_TORQUE = 9.8
GRAVITY = 9.80665

MESSAGE_DICT = {}

# (Size (bits), Position (bits), signed, lambda func, name, units)

def parse_BMS_balancing_status(data, id = None, time=None):
    msg_id = 0xDE
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xDE] = (parse_BMS_balancing_status, "BMS_balancing_status") 

def parse_BMS_coulomb_counts(data, id = None, time=None):
    msg_id = 0xE2
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xE2] = (parse_BMS_coulomb_counts, "BMS_coulomb_counts") 

#done
def parse_BMS_detailed_temperatures(data, id = None, time=None):
    msg_id = 0xDA
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (4 , -1, False, lambda x: x    , "ic"    , "", "BMS.detailed_temps.ic"),
        (4 , -1, False, lambda x: x    , "group" , "", "BMS.detailed_temps.group"),
        (16, -1,  True, lambda x: x/100, "temps" , "C", "BMS.detailed_temps.temps[0]"),
        (16, -1,  True, lambda x: x/100, "temps" , "C", "BMS.detailed_temps.temps[1]"),
        (16, -1,  True, lambda x: x/100, "temps" , "C", "BMS.detailed_temps.temps[2]"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    root_dir = "BMS.detailed_temps.ic_{ic:02}.temp_{cell}"

    if time is not None:
        ic_group_list = np.unique(arr[:,(1, 2)], axis = 0)
        cells_list = [arr[:,(0,3,4,5)][(arr[:,(1,2)]==ic_group).all(axis=1)] for ic_group in ic_group_list]
    else:
        ic_group_list = np.unique(arr[:,(0, 1)], axis = 0)
        cells_list = [arr[:,(2,3,4)][(arr[:,(0,1)]==ic_group).all(axis=1)] for ic_group in ic_group_list]
    dirs_list = [
        [root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+1)), \
         root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+2)), \
         root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+3))]
                  for ic_group in ic_group_list]

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vectors[i+2][4] for i in range(3)]
    units = [vectors[i+2][5] for i in range(3)]
    return [(cells_list[ic_group_index], cols, units, dirs_list[ic_group_index]) for ic_group_index in range(len(ic_group_list))]
MESSAGE_DICT[0xDA] = (parse_BMS_detailed_temperatures, "BMS_detailed_temperatures")

#done
def parse_BMS_detailed_voltages(data, id = None, time=None):
    msg_id = 0xD8
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (4 , -1, False, lambda x: x    , "ic"    , "", "BMS.detailed_voltage.ic"),
        (4 , -1, False, lambda x: x    , "group" , "", "BMS.detailed_voltage.group"),
        (16, -1, False, lambda x: x/10000, "voltage" , "V", "BMS.detailed_voltage.voltage[0]"),
        (16, -1, False, lambda x: x/10000, "voltage" , "V", "BMS.detailed_voltage.voltage[1]"),
        (16, -1, False, lambda x: x/10000, "voltage" , "V", "BMS.detailed_voltage.voltage[2]"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    root_dir = "BMS.detailed_voltage.ic_{ic:02}.cell_{cell:02}"
    ic_dir = ".ic_{ic}"
    cell_dir = ".cell_{cell}"

    if time is not None:
        ic_group_list = np.unique(arr[:,(1, 2)], axis = 0)
        cells_list = [arr[:,(0,3,4,5)][(arr[:,(1,2)]==ic_group).all(axis=1)] for ic_group in ic_group_list]
    else:
        ic_group_list = np.unique(arr[:,(0, 1)], axis = 0)
        cells_list = [arr[:,(2,3,4)][(arr[:,(0,1)]==ic_group).all(axis=1)] for ic_group in ic_group_list]

    dirs_list = [
        [root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+1)), \
         root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+2)), \
         root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+3))]
                  for ic_group in ic_group_list]
    

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vectors[i+2][4] for i in range(3)]
    units = [vectors[i+2][5] for i in range(3)]
    return [(cells_list[ic_group_index], cols, units, dirs_list[ic_group_index]) for ic_group_index in range(len(ic_group_list))]
MESSAGE_DICT[0xD8] = (parse_BMS_detailed_voltages, "BMS_detailed_voltages") 

def parse_BMS_onboard_detailed_temperatures(data, id = None, time=None):
    msg_id = 0xD6
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xD6] = (parse_BMS_onboard_detailed_temperatures, "BMS_onboard_detailed_temperatures") 

#done
def parse_BMS_onboard_temperatures(data, id = None, time=None):
    msg_id = 0xD5
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/100, "average_temperature", "C", "BMS.onboard_temperatures.average_temperature"),
        (16, -1, True, lambda x: x/100, "low_temperature", "C", "BMS.onboard_temperatures.low_temperature"),
        (16, -1, True, lambda x: x/100, "high_temperature", "C", "BMS.onboard_temperatures.high_temperature"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xD5] = (parse_BMS_onboard_temperatures, "BMS_onboard_temperatures") 

#done
def parse_BMS_status(data, id = None, time=None):
    msg_id = 0xDB
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (8, -1, False, lambda x: x, "state", "", "BMS.status.state"),
        (1, -1, False, lambda x: x, "overvoltage", "", "BMS.status.overvoltage"),
        (1, -1, False, lambda x: x, "undervoltage", "", "BMS.status.undervoltage"),
        (1, -1, False, lambda x: x, "total_voltage_high", "", "BMS.status.total_voltage_high"),
        (1, -1, False, lambda x: x, "discharge_overcurrent", "", "BMS.status.discharge_overcurrent"),
        (1, -1, False, lambda x: x, "charge_overcurrent", "", "BMS.status.charge_overcurrent"),
        (1, -1, False, lambda x: x, "discharge_overtemp", "", "BMS.status.discharge_overtemp"),
        (1, -1, False, lambda x: x, "charge_overtemp", "", "BMS.status.charge_overtemp"),
        (1, -1, False, lambda x: x, "undertemp", "", "BMS.status.undertemp"),
        (1, -1, False, lambda x: x, "onboard_overtemp", "", "BMS.status.onboard_overtemp"),
        (16, 24, True, lambda x: x/100, "current", "A", "BMS.status.current"),
        (1, -1, False, lambda x: x, "shutdown_g_above_threshold", "", "BMS.status.shutdown_g_above_threshold"),
        (1, -1, False, lambda x: x, "shutdown_h_above_threshold", "", "BMS.status.shutdown_h_above_threshold"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xDB] = (parse_BMS_status, "BMS_status") 

#done
def parse_BMS_temperatures(data, id = None, time=None):
    msg_id = 0xD9
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/100, "average_temperature", "C", "BMS.temperatures.average_temperature"),
        (16, -1, True, lambda x: x/100, "low_temperature", "C", "BMS.temperatures.low_temperature"),
        (16, -1, True, lambda x: x/100, "high_temperature", "C", "BMS.temperatures.high_temperature"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xD9] = (parse_BMS_temperatures, "BMS_temperatures") 

#done
def parse_BMS_voltages(data, id = None, time=None):
    msg_id = 0xD7
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x/10000 , "average_voltage" , "V", "BMS.voltages.average_voltage"),
        (16, -1, False, lambda x: x/10000 , "low_voltage"     , "V", "BMS.voltages.low_voltage"),
        (16, -1, False, lambda x: x/10000 , "high_voltage"    , "V", "BMS.voltages.high_voltage"),
        (16, -1, False, lambda x: x/100   , "total_voltage"   , "V", "BMS.voltages.total_voltage"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xD7] = (parse_BMS_voltages, "BMS_voltages") 

def parse_CCU_status(data, id = None, time=None):
    msg_id = 0xDD
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xDD] = (parse_CCU_status, "CCU_status") 

def parse_Charger_configure(data, id = None, time=None):
    msg_id = 0x1806E0F4
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x1806E0F4] = (parse_Charger_configure, "Charger_configure") 

def parse_Charger_data(data, id = None, time=None):
    msg_id = 0x18FF50E0
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x18FF50E0] = (parse_Charger_data, "Charger_data") 

#done
def parse_Dashboard_status(data, id = None, time=None):
    msg_id = 0xEB
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1, -1, False, lambda x: x, "start_btn", "", "DASHBOARD.start_btn"),
        (1, -1, False, lambda x: x, "buzzer_active", "", "DASHBOARD.buzzer_active"),
        (1, -1, False, lambda x: x, "ssok_above_threshold", "", "DASHBOARD.ssok_above_threshold"),
        (1, -1, False, lambda x: x, "shutdown_h_above_threshold", "", "DASHBOARD.shutdown_h_above_threshold"),
        (1,  8, False, lambda x: x, "mark_btn", "", "DASHBOARD.mark_btn"),
        (1, -1, False, lambda x: x, "mode_btn", "", "DASHBOARD.mode_btn"),
        (1, -1, False, lambda x: x, "mc_cycle_btn", "", "DASHBOARD.mc_cycle_btn"),
        (1, -1, False, lambda x: x, "launch_ctrl_btn", "", "DASHBOARD.launch_ctrl_btn"),
        (1, -1, False, lambda x: x, "torque_mode_btn", "", "DASHBOARD.torque_mode_btn"),
        (1, -1, False, lambda x: x, "led_dimmer_btn", "", "DASHBOARD.led_dimmer_btn"),
        (8, 16, False, lambda x: x, "dial_state", "", "DASHBOARD.dial_state"),
        (2, -1, False, lambda x: x, "ams_led", "", "DASHBOARD.ams_led"),
        (2, -1, False, lambda x: x, "imd_led", "", "DASHBOARD.imd_led"),
        (2, -1, False, lambda x: x, "mode_led", "", "DASHBOARD.mode_led"),
        (2, -1, False, lambda x: x, "mc_error_led", "", "DASHBOARD.mc_error_led"),
        (2, -1, False, lambda x: x, "start_led", "", "DASHBOARD.start_led"),
        (2, -1, False, lambda x: x, "inertia_led", "", "DASHBOARD.inertia_led"),
        (2, -1, False, lambda x: x, "mech_brake_led", "", "DASHBOARD.mech_brake_led"),
        (2, -1, False, lambda x: x, "gen_purp_led", "", "DASHBOARD.gen_purp_led"),
        (2, -1, False, lambda x: x, "bots_led", "", "DASHBOARD.bots_led"),
        (2, -1, False, lambda x: x, "cockpit_brb_led", "", "DASHBOARD.cockpit_brb_led"),
        (2, -1, False, lambda x: x, "crit_charge_led", "", "DASHBOARD.crit_charge_led"),
        (2, -1, False, lambda x: x, "glv_led", "", "DASHBOARD.glv_led"),
        (2, -1, False, lambda x: x, "launch_control_led", "", "DASHBOARD.launch_control_led"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xEB] = (parse_Dashboard_status, "Dashboard_status") 

def parse_MCU_GPS_readings(data, id = None, time=None):
    msg_id = 0xE7
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xE7] = (parse_MCU_GPS_readings, "MCU_GPS_readings") 

def parse_GPS_lat_long(data, id = None, time=None):
    msg_id = 0xED
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (32, -1, True, lambda x: x/10000000.0 , "lat" , "deg", "MCU.GPS.latitude"),
        (32, -1, True, lambda x: x/10000000.0 , "lon" , "deg", "MCU.GPS.longitude"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xED] = (parse_GPS_lat_long, "GPS_lat_long") 

def parse_GPS_other(data, id = None, time=None):
    msg_id = 0xEE
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (32, -1, False, lambda x: x/1000.0 , "msl" , "m", "MCU.GPS.msl"),
        (32, -1, False, lambda x: x/1000.0 , "acc" , "m", "MCU.GPS.accuracy"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xEE] = (parse_GPS_other, "GPS_other") 

#done
def parse_MCU_pedal_readings(data, id = None, time=None):
    msg_id = 0xC4
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x*.001*NOMINAL_TORQUE, "accelerator_pedal_1", "", "MCU.pedals.accelerator_pedal_1"),
        (16, -1, False, lambda x: x*.001*NOMINAL_TORQUE, "accelerator_pedal_2", "", "MCU.pedals.accelerator_pedal_2"),
        (16, -1, False, lambda x: x*.001*NOMINAL_TORQUE, "brake_pedal_1", "", "MCU.pedals.brake_pedal_1"),
        (16, -1, False, lambda x: x*.001*NOMINAL_TORQUE, "brake_pedal_2", "", "MCU.pedals.brake_pedal_2"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xC4] = (parse_MCU_pedal_readings, "MCU_pedal_readings") 

#done
def parse_MCU_status(data, id = None, time=None):
    msg_id = 0xC3
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        ( 1, -1, False, lambda x: x, "imd_ok_high"                    , "", "MCU.status.shutdown.imd_ok_high"),
        ( 1, -1, False, lambda x: x, "shutdown_b_above_threshold"     , "", "MCU.status.shutdown.shutdown_b_above_threshold"),
        ( 1, -1, False, lambda x: x, "bms_ok_high"                    , "", "MCU.status.shutdown.bms_ok_high"),
        ( 1, -1, False, lambda x: x, "shutdown_c_above_threshold"     , "", "MCU.status.shutdown.shutdown_c_above_threshold"),
        ( 1, -1, False, lambda x: x, "bspd_ok_high"                   , "", "MCU.status.shutdown.bspd_ok_high"),
        ( 1, -1, False, lambda x: x, "shutdown_d_above_threshold"     , "", "MCU.status.shutdown.shutdown_d_above_threshold"),
        ( 1, -1, False, lambda x: x, "software_ok_high"               , "", "MCU.status.shutdown.software_ok_high"),
        ( 1, -1, False, lambda x: x, "shutdown_e_above_threshold"     , "", "MCU.status.shutdown.shutdown_e_above_threshold"),
        ( 1, -1, False, lambda x: x, "mech_brake_active"              , "", "MCU.status.pedals.mech_brake_active"),
        ( 1, 10, False, lambda x: x, "no_accel_implausability"        , "", "MCU.status.pedals.no_accel_implausability"),
        ( 1, -1, False, lambda x: x, "no_brake_implausability"        , "", "MCU.status.pedals.no_brake_implausability"),
        ( 1, -1, False, lambda x: x, "brake_pedal_active"             , "", "MCU.status.pedals.brake_pedal_active"),
        ( 1, -1, False, lambda x: x, "bspd_current_high"              , "", "MCU.status.pedals.bspd_current_high"),
        ( 1, -1, False, lambda x: x, "bspd_brake_high"                , "", "MCU.status.pedals.bspd_brake_high"),
        ( 1, -1, False, lambda x: x, "no_accel_brake_implausability"  , "", "MCU.status.pedals.no_accel_brake_implausability"),
        ( 3, -1, False, lambda x: x, "mcu_state"                      , "", "MCU.status.ecu.mcu_state"),
        ( 1, -1, False, lambda x: x, "inverter_powered"               , "", "MCU.status.ecu.inverter_powered"),
        ( 1, -1, False, lambda x: x, "energy_meter_present"           , "", "MCU.status.ecu.energy_meter_present"),
        ( 1, -1, False, lambda x: x, "activate_buzzer"                , "", "MCU.status.ecu.activate_buzzer"),
        ( 1, -1, False, lambda x: x, "software_is_ok"                 , "", "MCU.status.ecu.software_is_ok"),
        ( 1, -1, False, lambda x: x, "launch_ctrl_active"             , "", "MCU.status.ecu.launch_ctrl_active"),
        ( 2, -1, False, lambda x: x, "pack_charge_critical"           , "", "MCU.status.ecu.pack_charge_critical"),
        ( 8, 32, False, lambda x: x, "max_torque"                    ,"Nm", "MCU.status.max_torque"),
        ( 8, -1, False, lambda x: x, "torque_mode"                    , "", "MCU.status.torque_mode"),
        (16, -1, False, lambda x: x/100, "distance_travelled"         ,"M", "MCU.status.distance_travelled"),
        
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xC3] = (parse_MCU_status, "MCU_status") 

#TODO
def parse_MCU_analog_readings(data, id = None, time=None):
    msg_id = 0xCC
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True , lambda x: x*(180/(2**13)) , "steering_1"          , "" , "MCU.analog.steering_1"),
        (16, -1, False, lambda x: x               , "steering_2"          , "" , "MCU.analog.steering_2"),
        (16, -1, True , lambda x: x/100           , "temperature"         , "C", "MCU.analog.temperature"),
        (16, -1, False, lambda x: x/2500          , "glv_battery_voltage" , "V", "MCU.analog.glv_battery_voltage"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xCC] = (parse_MCU_analog_readings, "MCU_analog_readings") 

#done
def parse_MCU_load_cells(data, id = None, time=None):
    msg_id = 0xC5
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x , "FL_load_cell" , "lb", "MCU.load_cells.FL"),
        (16, -1, False, lambda x: x , "FR_load_cell" , "lb", "MCU.load_cells.FR"),
        (16, -1, False, lambda x: x , "RL_load_cell" , "lb", "MCU.load_cells.RL"),
        (16, -1, False, lambda x: x , "RR_load_cell" , "lb", "MCU.load_cells.RR"),
    ]
    '''arr = np.zeros((data.size, 4))
    arr[:,0] = (data & 0xFFFF)
    arr[:,1] = (data & 0xFFFF<<16)>>48
    arr[:,2] = (data & 0xFFFF<<32)>>48
    arr[:,3] = (data & 0xFFFF<<48)>>48'''

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xC5] = (parse_MCU_load_cells, "MCU_load_cells")

#done
def parse_MCU_front_pots(data, id = None, time=None):
    msg_id = 0xC6
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x * 0.059312 + 3.0 , "pot1" , "lb", "MCU.pots.pot1"),
        (16, -1, False, lambda x: x * 0.059312 + 3.0 , "pot2" , "lb", "MCU.pots.pot2"),
        (16, -1, False, lambda x: x * 0.059312 + 3.0 , "pot3" , "lb", "MCU.pots.pot3"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xC6] = (parse_MCU_front_pots, "MCU_front_potentiometers")

#done
def parse_MCU_rear_pots(data, id = None, time=None):
    msg_id = 0xC7
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x * 0.059312 + 3.0 , "pot4" , "lb", "MCU.pots.pot4"),
        (16, -1, False, lambda x: x * 0.059312 + 3.0 , "pot5" , "lb", "MCU.pots.pot5"),
        (16, -1, False, lambda x: x * 0.059312 + 3.0 , "pot6" , "lb", "MCU.pots.pot6"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xC7] = (parse_MCU_rear_pots, "MCU_rear_potentiometers")

#done
def parse_MC1_energy(data, id = None, time=None):
    msg_id = 0xA8
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x, "dc_bus_voltage", "V", "MOTOR_CONTROLLER.mc_fl.dc_bus_voltage"),
        (32, -1, False, lambda x: x, "actual_power", "W", "MOTOR_CONTROLLER.mc_fl.actual_power"),
        (16, -1,  True, lambda x: x*(NOMINAL_TORQUE/1000), "feedback_torque", "Nm", "MOTOR_CONTROLLER.mc_fl.feedback_torque"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA8] = (parse_MC1_energy, "MC1_energy") 

#done
def parse_MC2_energy(data, id = None, time=None):
    msg_id = 0xA9
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x, "dc_bus_voltage", "V", "MOTOR_CONTROLLER.mc_fr.dc_bus_voltage"),
        (32, -1, False, lambda x: x, "actual_power", "W", "MOTOR_CONTROLLER.mc_fr.actual_power"),
        (16, -1,  True, lambda x: x*(NOMINAL_TORQUE/1000), "feedback_torque", "Nm", "MOTOR_CONTROLLER.mc_fr.feedback_torque"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA9] = (parse_MC2_energy, "MC2_energy") 

#done
def parse_MC3_energy(data, id = None, time=None):
    msg_id = 0xAA
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x, "dc_bus_voltage", "V", "MOTOR_CONTROLLER.mc_rl.dc_bus_voltage"),
        (32, -1, False, lambda x: x, "actual_power", "W", "MOTOR_CONTROLLER.mc_rl.actual_power"),
        (16, -1,  True, lambda x: x*(NOMINAL_TORQUE/1000), "feedback_torque", "Nm", "MOTOR_CONTROLLER.mc_rl.feedback_torque"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xAA] = (parse_MC3_energy, "MC3_energy") 

#done
def parse_MC4_energy(data, id = None, time=None):
    msg_id = 0xAB
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, False, lambda x: x, "dc_bus_voltage", "V", "MOTOR_CONTROLLER.mc_rr.dc_bus_voltage"),
        (32, -1, False, lambda x: x, "actual_power", "W", "MOTOR_CONTROLLER.mc_rr.actual_power"),
        (16, -1,  True, lambda x: x*(NOMINAL_TORQUE/1000), "feedback_torque", "Nm", "MOTOR_CONTROLLER.mc_rr.feedback_torque"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xAB] = (parse_MC4_energy, "MC4_energy") 

#done
def parse_MC1_setpoints_command(data, id = None, time=None):
    msg_id = 0xB0
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1, 8, False, lambda x: x, "inverter_enable", "", "MOTOR_CONTROLLER.mc_fl.inverter_enable"),
        (1, -1, False, lambda x: x, "hv_enable", "", "MOTOR_CONTROLLER.mc_fl.hv_enable"),
        (1, -1, False, lambda x: x, "driver_enable", "", "MOTOR_CONTROLLER.mc_fl.driver_enable"),
        (1, -1, False, lambda x: x, "remove_error", "", "MOTOR_CONTROLLER.mc_fl.remove_error"),
        (16, 16, True, lambda x: x, "speed_setpoint", "rpm", "MOTOR_CONTROLLER.mc_fl.speed_setpoint"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "pos_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_fl.pos_torque_limit"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "neg_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_fl.neg_torque_limit"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xB0] = (parse_MC1_setpoints_command, "MC1_setpoints_command") 

#done
def parse_MC2_setpoints_command(data, id = None, time=None):
    msg_id = 0xB1
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1, 8, False, lambda x: x, "inverter_enable", "", "MOTOR_CONTROLLER.mc_fr.inverter_enable"),
        (1, -1, False, lambda x: x, "hv_enable", "", "MOTOR_CONTROLLER.mc_fr.hv_enable"),
        (1, -1, False, lambda x: x, "driver_enable", "", "MOTOR_CONTROLLER.mc_fr.driver_enable"),
        (1, -1, False, lambda x: x, "remove_error", "", "MOTOR_CONTROLLER.mc_fr.remove_error"),
        (16, 16, True, lambda x: x, "speed_setpoint", "rpm", "MOTOR_CONTROLLER.mc_fr.speed_setpoint"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "pos_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_fr.pos_torque_limit"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "neg_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_fr.neg_torque_limit"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xB1] = (parse_MC2_setpoints_command, "MC2_setpoints_command") 

#done
def parse_MC3_setpoints_command(data, id = None, time=None):
    msg_id = 0xB2
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1, 8, False, lambda x: x, "inverter_enable", "", "MOTOR_CONTROLLER.mc_rl.inverter_enable"),
        (1, -1, False, lambda x: x, "hv_enable", "", "MOTOR_CONTROLLER.mc_rl.hv_enable"),
        (1, -1, False, lambda x: x, "driver_enable", "", "MOTOR_CONTROLLER.mc_rl.driver_enable"),
        (1, -1, False, lambda x: x, "remove_error", "", "MOTOR_CONTROLLER.mc_rl.remove_error"),
        (16, 16, True, lambda x: x, "speed_setpoint", "rpm", "MOTOR_CONTROLLER.mc_rl.speed_setpoint"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "pos_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_rl.pos_torque_limit"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "neg_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_rl.neg_torque_limit"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xB2] = (parse_MC3_setpoints_command, "MC3_setpoints_command") 

#done
def parse_MC4_setpoints_command(data, id = None, time=None):
    msg_id = 0xB3
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1, 8, False, lambda x: x, "inverter_enable", "", "MOTOR_CONTROLLER.mc_rr.inverter_enable"),
        (1, -1, False, lambda x: x, "hv_enable", "", "MOTOR_CONTROLLER.mc_rr.hv_enable"),
        (1, -1, False, lambda x: x, "driver_enable", "", "MOTOR_CONTROLLER.mc_rr.driver_enable"),
        (1, -1, False, lambda x: x, "remove_error", "", "MOTOR_CONTROLLER.mc_rr.remove_error"),
        (16, 16, True, lambda x: x, "speed_setpoint", "rpm", "MOTOR_CONTROLLER.mc_rr.speed_setpoint"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "pos_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_rr.pos_torque_limit"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "neg_torque_limit", "Nm", "MOTOR_CONTROLLER.mc_rr.neg_torque_limit"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xB3] = (parse_MC4_setpoints_command, "MC4_setpoints_command") 

#done
def parse_MC1_status(data, id = None, time=None):
    msg_id = 0xA0
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", "", "MOTOR_CONTROLLER.mc_fl.system_ready"),
        (1, -1, False, lambda x: x, "error", "", "MOTOR_CONTROLLER.mc_fl.error"),
        (1, -1, False, lambda x: x, "warning", "", "MOTOR_CONTROLLER.mc_fl.warning"),
        (1, -1, False, lambda x: x, "quit_dc_on", "", "MOTOR_CONTROLLER.mc_fl.quit_dc_on"),
        (1, -1, False, lambda x: x, "dc_on", "", "MOTOR_CONTROLLER.mc_fl.dc_on"),
        (1, -1, False, lambda x: x, "quit_inverter_on", "", "MOTOR_CONTROLLER.mc_fl.quit_inverter_on"),
        (1, -1, False, lambda x: x, "inverter_on", "", "MOTOR_CONTROLLER.mc_fl.inverter_on"),
        (1, -1, False, lambda x: x, "derating_on", "", "MOTOR_CONTROLLER.mc_fl.derating_on"),
        (16, 16, True, lambda x: x, "speed", "rpm", "MOTOR_CONTROLLER.mc_fl.speed"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "torque_current", "Nm", "MOTOR_CONTROLLER.mc_fl.torque_current"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A", "MOTOR_CONTROLLER.mc_fl.magnetizing_current"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA0] = (parse_MC1_status, "MC1_status") 

#done
def parse_MC2_status(data, id = None, time=None):
    msg_id = 0xA1
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", "", "MOTOR_CONTROLLER.mc_fr.system_ready"),
        (1, -1, False, lambda x: x, "error", "", "MOTOR_CONTROLLER.mc_fr.error"),
        (1, -1, False, lambda x: x, "warning", "", "MOTOR_CONTROLLER.mc_fr.warning"),
        (1, -1, False, lambda x: x, "quit_dc_on", "", "MOTOR_CONTROLLER.mc_fr.quit_dc_on"),
        (1, -1, False, lambda x: x, "dc_on", "", "MOTOR_CONTROLLER.mc_fr.dc_on"),
        (1, -1, False, lambda x: x, "quit_inverter_on", "", "MOTOR_CONTROLLER.mc_fr.quit_inverter_on"),
        (1, -1, False, lambda x: x, "inverter_on", "", "MOTOR_CONTROLLER.mc_fr.inverter_on"),
        (1, -1, False, lambda x: x, "derating_on", "", "MOTOR_CONTROLLER.mc_fr.derating_on"),
        (16, 16, True, lambda x: x, "speed", "rpm", "MOTOR_CONTROLLER.mc_fr.speed"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "torque_current", "Nm", "MOTOR_CONTROLLER.mc_fr.torque_current"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A", "MOTOR_CONTROLLER.mc_fr.magnetizing_current"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA1] = (parse_MC2_status, "MC2_status") 

#done
def parse_MC3_status(data, id = None, time=None):
    msg_id = 0xA2
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", "", "MOTOR_CONTROLLER.mc_rl.system_ready"),
        (1, -1, False, lambda x: x, "error", "", "MOTOR_CONTROLLER.mc_rl.error"),
        (1, -1, False, lambda x: x, "warning", "", "MOTOR_CONTROLLER.mc_rl.warning"),
        (1, -1, False, lambda x: x, "quit_dc_on", "", "MOTOR_CONTROLLER.mc_rl.quit_dc_on"),
        (1, -1, False, lambda x: x, "dc_on", "", "MOTOR_CONTROLLER.mc_rl.dc_on"),
        (1, -1, False, lambda x: x, "quit_inverter_on", "", "MOTOR_CONTROLLER.mc_rl.quit_inverter_on"),
        (1, -1, False, lambda x: x, "inverter_on", "", "MOTOR_CONTROLLER.mc_rl.inverter_on"),
        (1, -1, False, lambda x: x, "derating_on", "", "MOTOR_CONTROLLER.mc_rl.derating_on"),
        (16, 16, True, lambda x: x, "speed", "rpm", "MOTOR_CONTROLLER.mc_rl.speed"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "torque_current", "Nm", "MOTOR_CONTROLLER.mc_rl.torque_current"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A", "MOTOR_CONTROLLER.mc_rl.magnetizing_current"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA2] = (parse_MC3_status, "MC3_status") 

#done
def parse_MC4_status(data, id = None, time=None):
    msg_id = 0xA3
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (1,  8, False, lambda x: x, "system_ready", "", "MOTOR_CONTROLLER.mc_rr.system_ready"),
        (1, -1, False, lambda x: x, "error", "", "MOTOR_CONTROLLER.mc_rr.error"),
        (1, -1, False, lambda x: x, "warning", "", "MOTOR_CONTROLLER.mc_rr.warning"),
        (1, -1, False, lambda x: x, "quit_dc_on", "", "MOTOR_CONTROLLER.mc_rr.quit_dc_on"),
        (1, -1, False, lambda x: x, "dc_on", "", "MOTOR_CONTROLLER.mc_rr.dc_on"),
        (1, -1, False, lambda x: x, "quit_inverter_on", "", "MOTOR_CONTROLLER.mc_rr.quit_inverter_on"),
        (1, -1, False, lambda x: x, "inverter_on", "", "MOTOR_CONTROLLER.mc_rr.inverter_on"),
        (1, -1, False, lambda x: x, "derating_on", "", "MOTOR_CONTROLLER.mc_rr.derating_on"),
        (16, 16, True, lambda x: x, "speed", "rpm", "MOTOR_CONTROLLER.mc_rr.speed"),
        (16, -1, True, lambda x: x*(NOMINAL_TORQUE/1000), "torque_current", "Nm", "MOTOR_CONTROLLER.mc_rr.torque_current"),
        (16, -1, True, lambda x: x/10, "magnetizing_current", "A", "MOTOR_CONTROLLER.mc_rr.magnetizing_current"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA3] = (parse_MC4_status, "MC4_status") 

#done
def parse_MC1_temps(data, id = None, time=None):
    msg_id = 0xA4
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C", "MOTOR_CONTROLLER.mc_fl.motor_temp"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C", "MOTOR_CONTROLLER.mc_fl.inverter_temp"),
        (16, -1, False, lambda x: x, "diagnostic_number", "", "MOTOR_CONTROLLER.mc_fl.diagnostic_number"),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C", "MOTOR_CONTROLLER.mc_fl.igbt_temp"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA4] = (parse_MC1_temps, "MC1_temps") 

#done
def parse_MC2_temps(data, id = None, time=None):
    msg_id = 0xA5
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C", "MOTOR_CONTROLLER.mc_fr.motor_temp"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C", "MOTOR_CONTROLLER.mc_fr.inverter_temp"),
        (16, -1, False, lambda x: x, "diagnostic_number", "", "MOTOR_CONTROLLER.mc_fr.diagnostic_number"),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C", "MOTOR_CONTROLLER.mc_fr.igbt_temp"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA5] = (parse_MC2_temps, "MC2_temps") 

#done
def parse_MC3_temps(data, id = None, time=None):
    msg_id = 0xA6
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C", "MOTOR_CONTROLLER.mc_rl.motor_temp"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C", "MOTOR_CONTROLLER.mc_rl.inverter_temp"),
        (16, -1, False, lambda x: x, "diagnostic_number", "", "MOTOR_CONTROLLER.mc_rl.diagnostic_number"),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C", "MOTOR_CONTROLLER.mc_rl.igbt_temp"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA6] = (parse_MC3_temps, "MC3_temps") 

#done
def parse_MC4_temps(data, id = None, time=None):
    msg_id = 0xA7
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x/10, "motor_temp", "C", "MOTOR_CONTROLLER.mc_rr.motor_temp"),
        (16, -1, True, lambda x: x/10, "inverter_temp", "C", "MOTOR_CONTROLLER.mc_rr.inverter_temp"),
        (16, -1, False, lambda x: x, "diagnostic_number", "", "MOTOR_CONTROLLER.mc_rr.diagnostic_number"),
        (16, -1, True, lambda x: x/10, "igbt_temp", "C", "MOTOR_CONTROLLER.mc_rr.igbt_temp"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0xA7] = (parse_MC4_temps, "MC4_temps") 

def parse_SAB_readings_front(data, id = None, time=None):
    msg_id = 0x92
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x92] = (parse_SAB_readings_front, "SAB_readings_front") 

def parse_SAB_readings_rear(data, id = None, time=None):
    msg_id = 0x93
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    


    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x93] = (parse_SAB_readings_rear, "SAB_readings_rear") 

#done
def parse_EM_status(data, id = None, time=None):
    msg_id = 0x100
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (4, -1, False, lambda x: x, "voltage_gain", "V", "ENERGY_METER.voltage_gain"),
        (4, -1, False, lambda x: x, "current_gain", "A", "ENERGY_METER.current_gain"),
        (1, -1, False, lambda x: x, "overvoltage", "", "ENERGY_METER.overvoltage"),
        (1, -1, False, lambda x: x, "overpower", "", "ENERGY_METER.overpower"),
        (1, -1, False, lambda x: x, "logging", "", "ENERGY_METER.logging"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x100] = (parse_EM_status, "EM_status") 

#done
def parse_EM_measurement(data, id = None, time=None):
    msg_id = 0x400
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (32, -1, False, lambda x: x/(2**16), "voltage", "", "ENERGY_METER.voltage"),
        (32, -1, False, lambda x: x/(2**16), "current", "", "ENERGY_METER.current"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x400] = (parse_EM_measurement, "EM_measurement") 

#TODO
def parse_IMU_accelerometer(data, id = None, time=None):
    msg_id = 0x90
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x*.001, "lat_accel"   , "m/s^2", "IMU.lat_accel"),
        (16, -1, True, lambda x: x*.001, "long_accel"  , "m/s^2", "IMU.long_accel"),
        (16, -1, True, lambda x: x*.001, "vert_accel"  , "m/s^2", "IMU.vert_accel"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x90] = (parse_IMU_accelerometer, "IMU_accelerometer") 

#TODO
def parse_IMU_gryoscope(data, id = None, time=None):
    msg_id = 0x91
    if id is not None:
        mask = id==msg_id
        data = data[mask]
        if time is not None:
            time = time[mask]
    
    vectors = [
        (16, -1, True, lambda x: x*.01, "yaw"    , "deg/s", "IMU.yaw"),
        (16, -1, True, lambda x: x*.01, "pitch"  , "deg/s", "IMU.pitch"),
        (16, -1, True, lambda x: x*.01, "roll"   , "deg/s", "IMU.roll"),
    ]

    bitoffsets, bitmasks = get_offsets_masks(vectors)
    arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

    #df = pd.DataFrame(arr, columns='cols')
    cols = [vector[4] for vector in vectors]
    units = [vector[5] for vector in vectors]
    directory = [vector[6] for vector in vectors]
    return [(arr, cols, units, directory)]
MESSAGE_DICT[0x91] = (parse_IMU_gryoscope, "IMU_gryoscope") 
