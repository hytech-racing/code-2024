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
#pragma once
#include <FlexCAN_T4.h>
#include "Metro.h"

Metro timer_CAN_inverter_setpoints_send = Metro(20);
Metro timer_CAN_coloumb_count_send = Metro(1000);
Metro timer_CAN_mcu_status_send = Metro(100);
Metro timer_CAN_mcu_pedal_readings_send = Metro(50);
Metro timer_CAN_mcu_analog_readings_send = Metro(50);
Metro timer_CAN_mcu_load_cells_send = Metro(20);
Metro timer_CAN_mcu_potentiometers_send = Metro(20);
Metro timer_CAN_inverter_telem_send = Metro(200);

Metro timer_ready_sound = Metro(2000); // Time to play RTD sound

Metro timer_read_all_adcs = Metro(20);

Metro timer_inverter_enable = Metro(5000);
Metro timer_reset_inverter = Metro(5000);
Metro timer_watchdog_timer = Metro(7);

elapsedMillis pedal_implausability_duration;

Metro timer_debug = Metro(200);
Metro timer_debug2 = Metro(1000);

// this abuses Metro timer functionality to stay faulting once a fault has occurred
// autoreset makes the timer update to the current time and not by the interval
// this allows me to set the interval as 0 once a fault has occurred, leading to continuous faulting
// until a CAN message comes in which resets the timer and the interval
Metro timer_bms_heartbeat = Metro(0, 1);