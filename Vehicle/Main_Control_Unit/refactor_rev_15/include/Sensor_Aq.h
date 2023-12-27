/**
 * @file Sensor_Aq.h
 * @author Eric Galluzzi
 * @brief All interfacing for sensor data, such as our ADCs and Steering sensor goes through here
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <stdint.h>
#include <Mcp320x.h>
#include "MCU_rev15_dfs.h"
#include "HT_Data.h"
#include "timer_utils.h"
const float load_cell_alpha = 0.95;

#define STEERING_SERIAL 5
class Sensor_Aq {
    private:
    HT_Data* ht_data;
    MCP3208 adc;
    MCP3204 adc_fl;
    MCP3204 adc_fr;
    SPISettings settings;
    
    // STEERING_RS422 Steering_Top(STEERING_SERIAL);
    /**
     * @brief init rs422 for steering sensor
     * 
     */
    void begin_steering_rs422();
    /**
     * @brief read all external adc channels 
     * 
     * @param ecu ecu adc buf
     * @param fl fl adc buf
     * @param fr fr adc buf
     */
    void read_all_adc_channels(uint16_t* ecu, uint16_t* fl, uint16_t* fr);
     /**
     * @brief read and store all adc readings
     * 
     */
    void read_all_adcs();
    /**
     * @brief read and store all rs422 data
     * 
     */
    void read_steering_rs422();

    public:
    /**
     * @brief Construct a new Sensor_Aq object
     * 
     */
    Sensor_Aq();

    /**
     * @brief sample the adcs, public interface
     * 
     */
    void sampleADCs();
    /**
     * @brief sample the top steering sensor
     * 
     */
    void sampleSteeringTop();
    /**
     * @brief get the thermistor temeprature for a given analog val
     * 
     * @param analog analog reading of thermistor
     * @return double return celcius
     */
    static double get_thermistor_temperature(uint16_t analog);

}; 