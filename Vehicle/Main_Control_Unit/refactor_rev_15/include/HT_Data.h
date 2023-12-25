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
#include <HyTech_CAN.h>
#include "MCU_rev15_dfs.h";
/**
 * @brief Structure to hold htX data (singleton)
 * 
 */
class HT_Data {
    private:
    HT_Data();
    HT_Data(const HT_Data&);
    HT_Data& operator=(const HT_Data&);
    static HT_Data ht_data;
    public:
    MCU_pedal_readings mcu_pedal_readings;
    MCU_status mcu_status{};
    MCU_load_cells mcu_load_cells{};
    uint16_t prev_load_cell_readings[4] = {0, 0, 0, 0};
    float filtered_max_cell_temp = 40.0;
    float filtered_min_cell_voltage = 3.5;
    MCU_potentiometers mcu_potentiometers{};
    //MCU_thermistor_readings mcu_thermistor_readings{};
    MCU_analog_readings mcu_analog_readings{};

    // Inbound CAN messages
    BMS_coulomb_counts bms_coulomb_counts{};
    BMS_status bms_status{};
    BMS_temperatures bms_temperatures{};
    BMS_voltages bms_voltages{};
    Dashboard_status dashboard_status{};
    SAB_CB sab_cb{};
    //SAB_thermistor_readings_1 sab_thermistor_readings_1{};
    //SAB_thermistor_readings_2 sab_thermistor_readings_2{};
    static HT_Data* getInstance();
};

HT_Data::HT_Data() {
    mcu_status.set_bms_ok_high(false);
    mcu_status.set_imd_ok_high(false);
    mcu_status.set_inverters_error(false);
    mcu_status.set_max_torque(TORQUE_3);
    mcu_status.set_torque_mode(3);
}


//HT_Data ht_data;
HT_Data HT_Data::ht_data;
HT_Data* HT_Data::getInstance() {
    return &ht_data;
}
