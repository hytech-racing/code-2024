#pragma once

/*
 * Teensy Pin definitions
 */
#define INVERTER_EN 9
#define INVERTER_24V_EN 7

#define MC_MAX_SPEED 20000

#define BRAKE_LIGHT_CTRL 6

#define ECU_CLK 13 
#define ECU_SDI 12
#define ECU_SDO 11


#define CAN_ECU_RX2 0
#define CAN_ECU_TX2 1
#define CAN_ECU_RX3 30
#define CAN_ECU_TX3 31

#define ADC_CS 34
#define FL_CS 33
#define FR_CS 29
#define ADC_VREF 3300



#define INTERTIA_SENSE 20
#define SOFTWARE_OK_READ 25 // SHDN_F_READ Watchdog Combined
#define BOTS_SENSE_OK_READ 26 // SHDN_G_READ
#define BOTS_OK_READ 24  // SHDN_B_READ
#define IMD_OK_READ 10 // SHDN_C_READ
#define BMS_OK_READ 16 // SHDN_D_READ
#define BSPD_OK_READ 39 // SHDN_E_READ
#define SOFTWARE_OK 8
// digital outputs
#define WATCHDOG_INPUT 32

/*
 * ADC pin definitions
 */
#define ADC_BRAKE_1_CHANNEL 5
#define ADC_BRAKE_2_CHANNEL 3
#define ADC_ACCEL_1_CHANNEL 2
#define ADC_ACCEL_2_CHANNEL 1

#define ADC_STEERING_1_CHANNEL 7

#define ADC_GLV_READ_CHANNEL 4

#define ADC_CURRENT_CHANNEL 6
#define ADC_REFERENCE_CHANNEL 0

#define ADC_FL_LOAD_CELL_CHANNEL 2
#define ADC_FR_LOAD_CELL_CHANNEL 2
#define SUS_POT_FL 1
#define SUS_POT_FR 1

#define THERM_FR 41
#define THERM_FL 38
#define STEERING_2 40

/*
 * Shutdown read thresholds
 */

#define SHUTDOWN_HIGH 350 //teensy ADC reading when the shutdown lines are at 5V


/*
 * Other constants
 */
#define MIN_HV_VOLTAGE 60               // Volts in V * 1 - Used to check if Accumulator is energized


#define ALPHA 0.9772                     // parameter for the sowftware filter used on ADC pedal channels
#define ADC_SPI_SPEED 1000000            
#define TORQUE_ADJUSTMENT_VOLTAGE 3.5242 


#define LOAD_CELL1_OFFSET -33.98
#define LOAD_CELL1_SLOPE 0.1711
#define LOAD_CELL2_OFFSET -35.59
#define LOAD_CELL2_SLOPE 0.1762
#define LOAD_CELL3_OFFSET -33.59
#define LOAD_CELL3_SLOPE 0.192
#define LOAD_CELL4_OFFSET -64.5
#define LOAD_CELL4_SLOPE 0.1954
//
// #define LOAD_CELL1_OFFSET 0
// #define LOAD_CELL1_SLOPE 1
// #define LOAD_CELL2_OFFSET 0
// #define LOAD_CELL2_SLOPE 1
// #define LOAD_CELL3_OFFSET 0
// #define LOAD_CELL3_SLOPE 1
// #define LOAD_CELL4_OFFSET 0
// #define LOAD_CELL4_SLOPE 1

#define BMS_HEARTBEAT_TIMEOUT 30000
#define DASH_HEARTBEAT_TIMEOUT 1000
#define TIMER_SOFTWARE_ENABLE 100

#define PACK_CHARGE_CRIT_TOTAL_THRESHOLD 420
#define PACK_CHARGE_CRIT_LOWEST_CELL_THRESHOLD 35000

#define MECH_POWER_LIMIT 63
#define DC_POWER_LIMIT 79

#define REGEN_OFF_START_THRESHOLD 1540