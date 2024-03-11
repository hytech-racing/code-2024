#pragma once

/*
 * Teensy Pin definitions
 */
#define INVERTER_EN 9
#define INVERTER_24V_EN 8

#define BRAKE_LIGHT_CTRL 7
#define BRAKE_THRESHOLD_MECH_BRAKE 5

#define ECU_CLK 13 
#define ECU_SDI 12
#define ECU_SDO 11

#define STEERING_CS 10
#define STEERING_SPI_SPEED 2000000

#define IMU_DATAREADY 35
#define IMU_SYNC 37
#define IMU_CS 38
#define IMU_RESET 36

#define CAN_ECU_RX1 22
#define CAN_ECU_TX1 23
#define CAN_ECU_RX2 0
#define CAN_ECU_TX2 1
#define CAN_ECU_RX3 30
#define CAN_ECU_TX3 31

#define ADC1_CS 33 
#define ADC2_CS 29 
#define ADC3_CS 34

#define FR_INTERLOCK 40 
#define FL_INTERLOCK 39
#define RR_INTERLOCK 16
#define RL_INTERLOCK 14

#define INTERTIA_SENSE 20
#define SOFTWARE_OK_READ 25 // SHDN_F_READ Watchdog Combined
#define BOTS_SENSE_OK_READ 26 // SHDN_G_READ
#define BOTS_OK_READ 3  // SHDN_B_READ
#define IMD_OK_READ 4 // SHDN_C_READ
#define BMS_OK_READ 5 // SHDN_D_READ
#define BSPD_OK_READ 6 // SHDN_E_READ
#define SOFTWARE_OK 28
// digital outputs
#define WATCHDOG_INPUT 32

/*
 * ADC pin definitions
 */
#define ADC_BRAKE_1_CHANNEL 4
#define ADC_BRAKE_2_CHANNEL 7
#define ADC_ACCEL_1_CHANNEL 2
#define ADC_ACCEL_2_CHANNEL 3

#define ADC_STEERING_CHANNEL 4

#define ADC_GLV_READ_CHANNEL 4

#define ADC_FL_LOAD_CELL_CHANNEL 4
#define ADC_FR_LOAD_CELL_CHANNEL 5
#define ADC_RL_LOAD_CELL_CHANNEL 7
#define ADC_RR_LOAD_CELL_CHANNEL 3
/*
 * Shutdown read thresholds
 */

#define SHUTDOWN_HIGH 350 //teensy ADC reading when the shutdown lines are at 5V


/*
 * Other constants
 */
#define MIN_HV_VOLTAGE 60               // Volts in V * 1 - Used to check if Accumulator is energized


#define ALPHA 0.9772                     // parameter for the sowftware filter used on ADC pedal channels
#define ADC_SPI_SPEED 1000000            // max SPI clokc frequency for MCP3208 is 2MHz in ideal conditions
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

#define PACK_CHARGE_CRIT_THRESHOLD 24
