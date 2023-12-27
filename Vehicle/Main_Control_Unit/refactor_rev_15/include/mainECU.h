/**
 * @file mainECU.h
 * @author Eric Galluzzi
 * @brief Abstraction of the ECU as its own entity
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <stdint.h>
#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <time.h>
#include <deque>
#include <Mcp320x.h>
#include "kinetis_flexcan.h"
#include "timer_utils.h"
#include "debugging_utils.h"
#include "HT_Data.h"
#include "MCU_rev15_dfs.h"
#include "driver_constants.h"
#include "drivers.h"
#include "Sensor_Aq.h"
#include "CAN_Handler.h"
#include "Status_Manager.h"
#include "Torque_Vectoring.h"


class MainECU {
    private:
        //make this a singleton
        //could lowkey make everything a singleton for ease of access
        HT_Data* ht_data; //add this in the constructor
        Sensor_Aq sensor_aq;
        CAN_Handler can_handler;
        Inverter_Control* inverter_control;
        Torque_Vectoring torque_vectoring;
        Status_Manager status_manager;
        MCU_STATE mcu_state;
        void set_state(MCU_STATE new_state);
        void state_machine();
    public:
        MainECU();
        void loop();
        
        


};