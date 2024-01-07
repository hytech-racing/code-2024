/**
 * @file Sensor_Aq.cpp
 * @author Eric Galluzzi
 * @brief Function Definitions for Sensor_Aq
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "Sensor_Aq.h"

/**
 * @brief Construct a new Sensor_Aq::Sensor_Aq object
 *          initializes adc and rs422 objects
 */
Sensor_Aq::Sensor_Aq() : adc(ADC_VREF, ADC_CS), adc_fl(ADC_VREF, FL_CS), adc_fr(ADC_VREF, FR_CS), settings(ADC_SPI_SPEED, MSBFIRST, SPI_MODE0) {
    ht_data = HT_Data::getInstance();
    begin_steering_rs422();
    // pinMode(ECU_CLK, OUTPUT);
    // pinMode(ECU_SDI, INPUT);
    // pinMode(ECU_SDO, OUTPUT);
    pinMode(THERM_FR, INPUT);
    pinMode(THERM_FL, INPUT);
    pinMode(STEERING_2, INPUT); 
}  
void Sensor_Aq::begin_steering_rs422() {
    //Steering_Top.begin();
}

void Sensor_Aq:: read_all_adc_channels(uint16_t* ecu, uint16_t* fl, uint16_t* fr) {
  SPI.beginTransaction(settings);
  
  ecu[0] = adc.read(MCP3208::Channel::SINGLE_0);
  ecu[1] = adc.read(MCP3208::Channel::SINGLE_1);
  ecu[2] = adc.read(MCP3208::Channel::SINGLE_2);
  ecu[3] = adc.read(MCP3208::Channel::SINGLE_3);
  ecu[4] = adc.read(MCP3208::Channel::SINGLE_4);
  ecu[5] = adc.read(MCP3208::Channel::SINGLE_5);
  ecu[6] = adc.read(MCP3208::Channel::SINGLE_6);
  ecu[7] = adc.read(MCP3208::Channel::SINGLE_7);
  fl[0] = adc_fl.read(MCP3204::Channel::SINGLE_0);
  fl[1] = adc_fl.read(MCP3204::Channel::SINGLE_1);
  fl[2] = adc_fl.read(MCP3204::Channel::SINGLE_2);
  fl[3] = adc_fl.read(MCP3204::Channel::SINGLE_3);
  fr[0] = adc_fr.read(MCP3204::Channel::SINGLE_0);
  fr[1] = adc_fr.read(MCP3204::Channel::SINGLE_1);
  fr[2] = adc_fr.read(MCP3204::Channel::SINGLE_2);
  fr[3] = adc_fr.read(MCP3204::Channel::SINGLE_3);
  SPI.endTransaction();
}
void Sensor_Aq:: read_all_adcs() {
    if (timer_read_all_adcs.check()) {
    //mcu_thermistor_readings.set_therm_fl(analogRead(THERM_FL));
    //mcu_thermistor_readings.set_therm_fr(analogRead(THERM_FR));
    ht_data->mcu_analog_readings.set_steering_2(analogRead(STEERING_2));
    //teensy adc's


    //function pointers for load cells??
    
    uint16_t adc_inputs[8];
    uint16_t FL_inputs[4]; //only two are usable, since CB only connects inputs at 1 and 0. 
    uint16_t FR_inputs[4]; 
    this->read_all_adc_channels(adc_inputs, FL_inputs, FR_inputs);
    ht_data->mcu_pedal_readings.set_accelerator_pedal_1(adc_inputs[ADC_ACCEL_1_CHANNEL]);
    ht_data->mcu_pedal_readings.set_accelerator_pedal_2(adc_inputs[ADC_ACCEL_2_CHANNEL]);
    ht_data->mcu_pedal_readings.set_brake_pedal_1(adc_inputs[ADC_BRAKE_1_CHANNEL]);
    ht_data->mcu_pedal_readings.set_brake_pedal_2(adc_inputs[ADC_BRAKE_2_CHANNEL]);
    ht_data->mcu_analog_readings.set_steering_1(adc_inputs[ADC_STEERING_1_CHANNEL]);
    
    ht_data->mcu_analog_readings.set_glv_battery_voltage(adc_inputs[ADC_GLV_READ_CHANNEL]);
    int hall_current_read = adc_inputs[ADC_CURRENT_CHANNEL] - adc_inputs[ADC_REFERENCE_CHANNEL];
    
    float current = ((((hall_current_read / 819.0) / .1912) / 4.832) * 1000) / 6.67;
    if (current > 300) {
      current = 300;
    } else if (current < -300) {
      current = -300;
    }
    ht_data->mcu_analog_readings.set_hall_effect_current((uint16_t)current * 100);

    ht_data->prev_load_cell_readings[0] = ht_data->mcu_load_cells.get_FL_load_cell();
    ht_data->prev_load_cell_readings[1] = ht_data->mcu_load_cells.get_FR_load_cell();
    ht_data->mcu_load_cells.set_FL_load_cell((uint16_t)((FL_inputs[ADC_FL_LOAD_CELL_CHANNEL]*LOAD_CELL1_SLOPE + LOAD_CELL1_OFFSET) * (1 - load_cell_alpha) + ht_data->prev_load_cell_readings[0]*load_cell_alpha));
    ht_data->mcu_load_cells.set_FR_load_cell((uint16_t)((FR_inputs[ADC_FR_LOAD_CELL_CHANNEL]*LOAD_CELL2_SLOPE + LOAD_CELL2_OFFSET) * (1 - load_cell_alpha) + ht_data->prev_load_cell_readings[1]*load_cell_alpha));
    ht_data->mcu_potentiometers.set_pot1(FL_inputs[SUS_POT_FL]);
    ht_data->mcu_potentiometers.set_pot2(FR_inputs[SUS_POT_FR]);
 
  }
}
void Sensor_Aq:: read_steering_rs422() {
    //ht_data->mcu_steering_values.set_top_steering(Steering_Top.read());
}
void Sensor_Aq:: sampleADCs() {
    read_all_adcs();
}
void Sensor_Aq:: sampleSteeringTop() {
    read_steering_rs422();
}
double Sensor_Aq:: get_thermistor_temperature(uint16_t analog) {
    //insert math here
    return -1;
}

