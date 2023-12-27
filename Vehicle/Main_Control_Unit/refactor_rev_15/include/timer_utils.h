/**
 * @file timer_utils.h
 * @author Eric Galluzzi
 * @brief timer utils
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

#include "FlexCAN_T4.h"
#include "Metro.h"


extern Metro timer_CAN_inverter_setpoints_send;
extern Metro timer_CAN_coloumb_count_send;
extern Metro timer_CAN_mcu_status_send;
extern Metro timer_CAN_mcu_pedal_readings_send;
extern Metro timer_CAN_mcu_analog_readings_send;
extern Metro timer_CAN_mcu_load_cells_send;
extern Metro timer_CAN_mcu_potentiometers_send;
extern Metro timer_CAN_inverter_telem_send;

extern Metro timer_ready_sound;

extern Metro timer_read_all_adcs;
extern Metro timer_inverter_enable;
extern Metro timer_reset_inverter;
extern Metro timer_watchdog_timer;

extern elapsedMillis pedal_implausability_duration;

extern Metro timer_debug;
extern Metro timer_debug2;

// this abuses Metro timer functionality to stay faulting once a fault has occurred
// autoreset makes the timer update to the current time and not by the interval
// this allows me to set the interval as 0 once a fault has occurred, leading to continuous faulting
// until a CAN message comes in which resets the timer and the interval
extern Metro timer_bms_heartbeat;
#endif // TIMER_UTILS_H