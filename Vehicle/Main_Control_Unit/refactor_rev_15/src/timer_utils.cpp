#include "timer_utils.h"

Metro timer_CAN_inverter_setpoints_send(20);
Metro timer_CAN_coloumb_count_send(1000);
Metro timer_CAN_mcu_status_send(100);
Metro timer_CAN_mcu_pedal_readings_send(50);
Metro timer_CAN_mcu_analog_readings_send(50);
Metro timer_CAN_mcu_load_cells_send(20);
Metro timer_CAN_mcu_potentiometers_send(20);
Metro timer_CAN_inverter_telem_send(200);

Metro timer_ready_sound(2000); // Time to play RTD sound

Metro timer_read_all_adcs(20);

Metro timer_inverter_enable(5000);
Metro timer_reset_inverter(5000);
Metro timer_watchdog_timer(7);


Metro timer_debug(200);
Metro timer_debug2(1000);

// this abuses Metro timer functionality to stay faulting once a fault has occurred
// autoreset makes the Metro timer update to the current time and not by the interval
// this allows me to set the interval as 0 once a fault has occurred, leading to continuous faulting
// until a CAN message comes in which resets the Metro timer and the interval
Metro timer_bms_heartbeat(0, 1);