#pragma once

/*
 * Teensy Pin definitions
 */
#define BRAKE_LIGHT_CTRL 0
#define FRONT_LEFT_WHEEL 1
#define FRONT_RIGHT_WHEEL 2
#define FAN_1 5
#define FAN_2 6
#define BACK_LEFT_WHEEL 7
#define BACK_RIGHT_WHEEL 8
#define INVERTER_CTRL 9
#define ADC_CS 10

#define SHUTDOWN_B_READ A0
#define SHUTDOWN_C_READ A1
#define SHUTDOWN_D_READ A2
#define SHUTDOWN_E_READ A3
#define IMD_OK_READ A4
#define BMS_OK_READ A5
#define BSPD_OK_READ A6
#define SOFTWARE_OK_READ A7
// digital outputs
#define WATCHDOG_INPUT A8
#define TEENSY_OK A9

/*
 * ADC pin definitions
 */
#define ADC_BRAKE_1_CHANNEL 1
#define ADC_BRAKE_2_CHANNEL 0
#define ADC_ACCEL_1_CHANNEL 3
#define ADC_ACCEL_2_CHANNEL 2

/*
 * Shutdown read thresholds
 */

#define SHUTDOWN_HIGH 350 //teensy ADC reading when the shutdown lines are at 5V


/*
 * Other constants
 */
#define MIN_HV_VOLTAGE 60               // Volts in V * 1 - Used to check if Accumulator is energized


#define ALPHA 0.9772                     // parameter for the sowftware filter used on ADC pedal channels
#define ADC_SPI_SPEED 1800000            // max SPI clokc frequency for MCP3208 is 2MHz in ideal conditions
#define TORQUE_ADJUSTMENT_VOLTAGE 3.5242 

#define LOAD_CELL1_OFFSET 0
#define LOAD_CELL1_SLOPE 0
#define LOAD_CELL2_OFFSET 0
#define LOAD_CELL2_SLOPE 0
#define LOAD_CELL3_OFFSET 0
#define LOAD_CELL3_SLOPE 0
#define LOAD_CELL4_OFFSET 0
#define LOAD_CELL4_SLOPE 0

#define BMS_HEARTBEAT_TIMEOUT 30000
#define DASH_HEARTBEAT_TIMEOUT 1000
#define TIMER_SOFTWARE_ENABLE 100
