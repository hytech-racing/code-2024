#ifndef __HYTECH_CAN_H__
#define __HYTECH_CAN_H__

// ECU state definitions // TODO make these enums?
#define BMS_STATE_DISCHARGING 1
#define BMS_STATE_CHARGING 2
#define BMS_STATE_BALANCING 3
#define BMS_STATE_BALANCING_OVERHEATED 4

#include "CAN_ID.h"

#include "BMS_balancing_status.h"
#include "BMS_coulomb_counts.h"
#include "BMS_detailed_temperatures.h"
#include "BMS_detailed_voltages.h"
#include "BMS_onboard_detailed_temperatures.h"
#include "BMS_onboard_temperatures.h"
#include "BMS_status.h"
#include "BMS_temperatures.h"
#include "BMS_voltages.h"
#include "CCU_status.h"
#include "Charger_configure.h"
#include "Charger_data.h"
#include "Dashboard_status.h"
#include "MCU_GPS_readings.h"
#include "MCU_pedal_readings.h"
#include "MCU_status.h"
#include "MCU_analog_readings.h"
#include "MCU_load_cells.h"
#include "MCU_front_potentiometers.h"
#include "MCU_rear_potentiometers.h"

#include "MC_energy.h"
#include "MC_setpoints_command.h"
#include "MC_status.h"
#include "MC_temps.h"

#include "SAB_readings_front.h"
#include "SAB_readings_rear.h"
#include "SAB_readings_gps.h"
#include "GPS_high_precision.h"
#include "GPS_lat_long.h"
#include "GPS_other.h"
#include "EM_status.h"
#include "EM_measurement.h"
#include "IMU_accelerometer.h"
#include "IMU_gryoscope.h"
#endif
