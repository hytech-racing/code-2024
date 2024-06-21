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
#define ADC_ISOSPI_SPEED 1000000
#define RS485_SERIAL_SPEED 115200
#define IMU_RS232_SPEED 115200
#define TELEM_CAN_BAUDRATE 500000
#define DEBUG_PRINT_BAUDRATE 115200

/* ADC read channels */
// SAB MCP3208
#define THERM_3 0   // RR motor     // the rest is on ECU. don't try
#define THERM_4 1   // inverter beheaded
#define THERM_5 2   // pump (right)
#define THERM_6 3   // inverter flying leed
#define THERM_7 4   // accumulator across penthouse (right)
#define THERM_8 5   // accumulator (left), also gone unfortunately
#define THERM_9 6   // RL motor
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

/* Vector Nav defines */
// #define DEFAULT_WRITE_BUFFER_SIZE 17
// #define DEFAULT_WRITE_BUFFER_MIDIUM 21
// #define DEFAULT_WRITE_BUFFER_LONG 256
// #define DEFAULT_READ_BUFFER_SIZE 256
// #define DEFAULT_SERIAL_BUFFER_SIZE 64

// #define SERIAL_BAUDRATE1 9600
// #define SERIAL_BAUDRATE2 19200
// #define SERIAL_BAUDRATE3 38400
// #define SERIAL_BAUDRATE4 57600
// #define SERIAL_BAUDRATE5 115200
// #define SERIAL_BAUDRATE6 128000
// #define SERIAL_BAUDRATE7 230400
// #define SERIAL_BAUDRATE8 460800
// #define SERIAL_BAUDRATE9 921600

#define INIT_HEADING 320    // Rome endurance

#define BINARY_OUTPUT_GROUP_COUNT_1 1
#define BINARY_OUTPUT_GROUP_COUNT_2 2
#define BINARY_OUTPUT_GROUP_COUNT_3 2
#define BINARY_OUTPUT_PAYLOAD_1 44   // bytes
#define BINARY_OUTPUT_PAYLOAD_2 50   // bytes
#define BINARY_OUTPUT_PAYLOAD_3 40   // bytes
#define OFFSET_PADDING_1 4
#define OFFSET_PADDING_2_ORGINAL 6
#define OFFSET_PADDING_2 18
#define OFFSET_PADDING_3 6

#define SANITY_CHECK 0
#define DEBUG 0
#define DEBUG_GNSS_HEALTH 0
#define START_UP_DELAY 5000

#define ATOU32 static_cast<uint32_t>(std::atoi(result))
#define ATOFF static_cast<float>(std::atof(result))
#define NEXT result = getNextData(receiveBufferAscii, parseIndex); \
	if (result == NULL) \
		return;



