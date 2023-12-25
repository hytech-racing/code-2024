/**
 * @file Torque_Vectoring.h
 * @author Eric Galluzzi
 * @brief  Class for handling our torque vectoring strategies
 * @version 0.1
 * @date 2023-12-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "MCU_rev15_dfs.h"
#include "HT_Data.h"
#include "stdint.h"
#include "Inverter_Control.h"
#include "driver_constants.h"
#include <cmath>



class Torque_Vectoring {
    private:
    HT_Data* ht_data;
    Inverter_Control* inverter_control;
    

    float max_torque;
    //design for implausabilities lolz
    struct Accel {
        uint16_t (MCU_pedal_readings::*get_accel)() const;
        int start;
        int end;
        uint16_t val;
    } accel[NUM_APPS] = {{&MCU_pedal_readings::get_accelerator_pedal_1, START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0}, {&MCU_pedal_readings::get_accelerator_pedal_2, START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0}};
        
    // }
    // uint16_t (MCU_pedal_readings::*get_accel[NUM_APPS])() const = { &MCU_pedal_readings::get_accelerator_pedal_1, &MCU_pedal_readings::get_accelerator_pedal_2};
    // uint16_t (MCU_pedal_readings::*get_brake[NUM_APPS])() const = { &MCU_pedal_readings::get_brake_pedal_1, &MCU_pedal_readings::get_brake_pedal_2};
    struct Brake {
        uint16_t (MCU_pedal_readings::*get_brake)() const;
        uint16_t start;
        uint16_t end;
        uint16_t val;
    } brake [NUM_APPS] = {{&MCU_pedal_readings::get_brake_pedal_1, START_BRAKE_PEDAL_1, END_BRAKE_PEDAL_1, 0}, {&MCU_pedal_readings::get_brake_pedal_2, START_BRAKE_PEDAL_2, END_BRAKE_PEDAL_2, 0}};

    
    
    float rear_power_balance;
    float front_power_balance;
    float rear_brake_balance;
    float front_brake_balance;
    int32_t total_torque;
    int32_t total_load_cells;
    float front_load_total;
    float rear_load_total;
    float attesa_def_split;
    float attesa_alt_split;
    float fr_slip_clamped;
    float fr_slip_factor;
    float f_torque;
    float r_torque;
    float rear_lr_slip_clamped;
    float lsd_right_split;
    float lsd_slip_factor;
    float avg_speed;
    int16_t start_derating_rpm;
    int16_t end_derating_rpm;
    const float hairpin_rpm_limit = 5600.0;
    const float harpin_rpm_full = 2800.0;
    float hairpin_rpm_factor;
    const float hairpin_steering_min = 80.0;  // degrees
    const float hairpin_steering_max = 120.0; // degr
    float steering_calibration_slope;
    float steering_calibration_offset;
    float steering_angle;
    float hairpin_reallocation;
    float hairpin_steering_factor;
    enum LAUNCH_STATE
    {
    NOT_READY,
    READY,
    LAUNCH
    };
    float max_front_power;
    float max_rear_power;
    LAUNCH_STATE launch_state;
    int16_t launch_speed_target;
    elapsedMillis time_since_launch;
    const uint16_t LAUNCH_READY_ACCEL_THRESHOLD = 100;
    const uint16_t LAUNCH_READY_BRAKE_THRESHOLD = 300;
    const int16_t LAUNCH_READY_SPEED_THRESHOLD = 500;
    const uint16_t LAUNCH_GO_THRESHOLD = 900;
    const uint16_t LAUNCH_STOP_THRESHOLD = 600;
    float launch_rate_target;

    int16_t max_speed;

    public:
    /**
     * @brief calculating max torque output (power limiting)
     * 
     * @param maxwatts 
     * @param rpm 
     * @return float 
     */
    static float max_allowed_torque(float maxwatts, float rpm);
    /**
     * @brief linear map number to a float
     * 
     * @param x 
     * @param in_min 
     * @param in_max 
     * @param out_min 
     * @param out_max 
     * @return float 
     */
    static float float_map(float x, float in_min, float in_max, float out_min, float out_max);
    /**
     * @brief Construct a new Torque_Vectoring object
     * 
     */
    Torque_Vectoring();
    /**
     * @brief Set the inverter torques based on current state passed in
     * 
     * @param state 
     */
    void set_inverter_torques(int state);

    /**
     * @brief safe_mode (no vectoring and whatnot)
     * 
     */
    void safe_mode();
    /**
     * @brief Mikhail nissan mode
     * 
     */
    void nissan_mode();
    /**
     * @brief launch control mode
     * 
     */
    void launch_control();
    /**
     * @brief endurance mode
     * 
     */
    void endurance_mode();
    /**
     * @brief load cell torque vectoring
     * 
     */
    void load_cell_mode();
    //Torque_Vectoring()

};