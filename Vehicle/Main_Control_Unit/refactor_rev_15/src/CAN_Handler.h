/**
 * @file Sensor_Aq.h
 * @author Eric Galluzzi
 * @brief All interfacing for CAN data transmission (TELEM and INV)
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "MCU_rev15_dfs.h"
#include "HT_Data.h"
#include "Inverter_Control.h"
#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
#include "timer_utils.h"
#include "debugging_utils.h"
#define TELEM_BAUD_RATE 500000
#define INVERTER_BAUD_RATE 500000

class CAN_Handler {
    private:
    static HT_Data* ht_data;
    static Inverter_Control* inverter_control;
    FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> INV_CAN;
    FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
    CAN_message_t msg;
    /**
     * @brief initialize CAN buses
     * 
     */
    void initCAN();

    /**
     * @brief callback for inverterCAN interrupts
     * 
     * @param RX_msg 
     */
    static void parse_inv_can_message(const CAN_message_t &RX_msg);
    /**
     * @brief callback for telemCAN interrupts
     * 
     * @param RX_msg 
     */
    static void parse_telem_can_message(const CAN_message_t &RX_msg);
    void send_CAN_mcu_status();
    void send_CAN_mcu_pedal_readings();
    void send_CAN_mcu_load_cells();
    void send_CAN_mcu_potentiometers();
    void send_CAN_mcu_analog_readings();
    void send_CAN_inverter_setpoints();
    
    public:
    /**
     * @brief Construct a new can handler object
     * 
     */
    CAN_Handler();
    /**
     * @brief poll CAN events during ECU operation
     * 
     */
    void CAN_poll();

    /**
     * @brief write out to CAN BUS during ECU operation
     * 
     */
    void CAN_write();
    void send_CAN_buzzer_data();
    
};