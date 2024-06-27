#ifndef __VN_CONSTS__
#define __VN_CONSTS__

// Serial buffer
const int DEFAULT_SERIAL_BUFFER_SIZE = 64;
const int DEFAULT_WRITE_BUFFER_SIZE = 17;
const int DEFAULT_WRITE_BUFFER_MIDIUM = 21;
const int DEFAULT_WRITE_BUFFER_LONG = 256;
const int DEFAULT_READ_BUFFER_SIZE = 256;

// Serial baudrate
const int VN_SERIAL_BAUDRATE1 = 9600;
const int VN_SERIAL_BAUDRATE2 = 19200;
const int VN_SERIAL_BAUDRATE3 = 38400;
const int VN_SERIAL_BAUDRATE4 = 57600;
const int VN_SERIAL_BAUDRATE5 = 115200;
const int VN_SERIAL_BAUDRATE6 = 128000;
const int VN_SERIAL_BAUDRATE7 = 230400;
const int VN_SERIAL_BAUDRATE8 = 460800;
const int VN_SERIAL_BAUDRATE9 = 921600;

// Binary group
const int BINARY_OUTPUT_GROUP_COUNT_TOTAL = 4;
const int BINARY_OUTPUT_GROUP_COUNT_1 = 1;
const int BINARY_OUTPUT_GROUP_COUNT_2 = 2;
const int BINARY_OUTPUT_GROUP_COUNT_3 = 2;
const int BINARY_OUTPUT_PAYLOAD = 134;    // bytes
const int BINARY_OUTPUT_PAYLOAD_1 = 44;   // bytes
const int BINARY_OUTPUT_PAYLOAD_2 = 50;   // bytes
const int BINARY_OUTPUT_PAYLOAD_3 = 40;   // bytes
const int OFFSET_PADDING = 10;
const int OFFSET_PADDING_1 = 4;
const int OFFSET_PADDING_2_ORGINAL = 6;
const int OFFSET_PADDING_2 = 18;
const int OFFSET_PADDING_3 = 6;

// Debug macros
/// @brief EDIT ME to achieve differen debug functions
#define SANITY_CHECK 0
#define DEBUG_VN 0
#define DEBUG_GNSS_HEALTH 0
#define DEBUG_ASYNC 0
#define CHECK_ASCII_RESPONSE 0

// Utility macros
#define ATOU32 static_cast<uint32_t>(std::atoi(result))
#define ATOFF static_cast<float>(std::atof(result))
#define NEXT result = getNextData(receiveBufferAscii, parseIndex); \
	if (result == NULL) \
		return;

#endif