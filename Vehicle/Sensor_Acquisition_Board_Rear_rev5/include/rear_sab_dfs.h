/*
 * Define constants used in code
 */

/* Pins */
// SPI chip selects
#define ADC1_CS 2
#define ADC2_CS 3
#define ADC3_CS 10  // default Teensy SPI CS
// RS485 ride height
#define RS485_SERIAL_PORT Serial4   // Arduino.h should be included before dfs.h
#define RS485_TX_PIN 17             // dfs should be included before ride height lib
#define RS485_DE_PIN 18
// RS232 VectorNav
#define TX_232 8
#define RX_232 7
#define VN_RESTORE 9
#define VN_SYNC_IN 6
#define VN_SYNC_OUT 5
#define VN_GPS_PPS 4
// Button
#define PI_SHUTDOWN 14  // debounce

/* Protocol speeds */
#define ADC_SPI_SPEED 2000000
#define RS485_SERIAL_SPEED 115200
#define IMU_RS232_SPEED 115200
#define TELEM_CAN_BAUDRATE 500000
#define DEBUG_PRINT_BAUDRATE 115200

/* ADC read channels */
// SAB MCP3208
#define THERM_3 0
#define THERM_4 1
#define THERM_5 2
#define THERM_6 3
#define THERM_7 4
#define THERM_8 5
#define THERM_9 6
// CB RL MCP3204
#define SUS_POT_3 1
#define RL_LOAD_CELL 2
// CB RR MCP3204
#define SUS_POT_4 1
#define RR_LOAD_CELL 2

/* Thermistor parameters */
#define TOTAL_THERMISTOR_COUNT 7
#define THERM_ALPHA 0.8

/* Loadcell parameters */
#define TOTAL_LOADCELL_COUNT 2
#define LOADCELL_ALPHA 0.8



