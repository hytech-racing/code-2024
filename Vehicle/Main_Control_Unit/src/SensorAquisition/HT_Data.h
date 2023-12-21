/**
 * @file HT_Data.h
 * @author Eric Galluzzi
 * @brief 
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <stdint>
#include <HyTech_CAN.h>

/**
 * @brief Structure to hold htX data
 * 
 */
struct HT_Data {
    MCU_pedal_readings mcu_pedal_readings;
    MCU_status mcu_status{};
    MCU_load_cells mcu_load_cells{};
    uint16_t prev_load_cell_readings[4] = {0, 0, 0, 0};
    MCU_potentiometers mcu_potentiometers{};
    MCU_thermistor_readings mcu_thermistor_readings{};
    MCU_analog_readings mcu_analog_readings{};

    MC_status mc_status[4];
    MC_temps mc_temps[4];
    MC_energy mc_energy[4];
    MC_setpoints_command mc_setpoints_command[4];

    // Inbound CAN messages
    BMS_coulomb_counts bms_coulomb_counts{};
    BMS_status bms_status{};
    BMS_temperatures bms_temperatures{};
    BMS_voltages bms_voltages{};
    Dashboard_status dashboard_status{};
    SAB_CB sab_corner_boards{};
    SAB_thermistor_readings_1 sab_thermistor_readings_1{};
    SAB_thermistor_readings_2 sab_thermistor_readings_2{};

}

HT_Data::HT_Data() {
    mcu_status.set_bms_ok_high(false);
    mcu_status.set_imd_ok_high(false);
    mcu_status.set_inverters_error(false);
    mcu_status.set_max_torque(TORQUE_3);
    mcu_status.set_torque_mode(3);
}


HT_Data ht_data;
