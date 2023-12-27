/**
 * @file Status_Manager.h
 * @author Eric Galluzzi
 * @brief status manager for all main ecu on-board peripherals
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "MCU_rev15_dfs.h"
#include "HT_Data.h"
#include "timer_utils.h"
#include "debugging_utils.h"
#include "driver_constants.h"

class Status_Manager {
    private:
    HT_Data* ht_data;
    void calculate_accel_implausabilities();
    void calculate_bse_implausabilities();
    void calculate_apps_brake_implausabilities();
    public:
    /**
     * @brief Construct a new Status_Manager object
     * 
     */
    Status_Manager();
    /**
     * @brief poll all maintenance functions
     * 
     */
    void status_poll();
    /**
     * @brief checks if brake and start button are simultanously pressed
     * 
     * @return true 
     * @return false 
     */
    bool ready_to_drive();
    /**
     * @brief activate_buzzer
     * 
     * @param on 
     */
    void activate_buzzer(bool on);
    /**
     * @brief handle software shutdown
     * 
     */
    void software_shutdown();
    /**
     * @brief brake outputs
     * 
     */
    void brake_outputs();
    /**
     * @brief read shutdown status values
     * 
     */
    void read_status_values();
    /**
     * @brief calculate implausabilities for pedals
     * 
     */
    void calculate_pedal_implausibilities();
    /**
     * @brief implausability under a certain duration
     * 
     * @return true 
     * @return false 
     */
    bool valid_pedals();
    /**
     * @brief BMS and IMD monitoring
     * 
     * @return true 
     * @return false 
     */
    bool valid_HV();

    /**
     * @brief serialize POTS
     * 
     */
    void debugPOTS();
    /**
     * @brief debug dashboard (dial only currently)
     * 
     */
    void debugDash();
   
};