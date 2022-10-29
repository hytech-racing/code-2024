#pragma once

// Button pin definition
#define BTN_MARK 6
#define BTN_MODE 7
#define BTN_START A2
#define BTN_LC A3
#define BTN_MC_CYCLE 9

//RANDOM FOR NOW
#define BTN_TORQUE_MODE 22
#define BTN_LED_DIMMER 23

//Dial pin definition (RANDOM FOR NOW)

#define DIAL_MODE_ONE 0
#define DIAL_MODE_TWO 1
#define DIAL_ACCELERATION_LAUNCH_CONTROL 2
#define DIAL_SKIDPAD 3
#define DIAL_AUTOCROSS 4 
#define DIAL_ENDURANCE 5



// LED and buzzer pin definition
#define BUZZER A4
#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 11
#define LED_MECH_BRAKE 10

#define LED_OFF 0
#define LED_ON_GREEN 0xFF00
#define LED_BLUE 0xFF
#define LED_RED 0xFF0000
// enum class LED_COLORS { OFF = 0, ON = 0xFF00 , BLUE = 0xFF , RED = 0xFF0000};
enum class LED_MODES { OFF = 0, ON = 1, BLUE = 2, RED = 3};
enum LED_TYPES { AMS = 0, IMD = 1, MC_ERR = 2, START = 3, MODE = 4, INERTIA = 5};

// MCP IO Expander pin definition //
#define IO_CS 8
#define IO_ADDR 0

// Critical read pin definitions
#define SSOK_READ A6
#define SHUTDOWN_H_READ A7
#define INERTIA_READ A1

// IO Expander Number Encodings
// 1 = Inertia, 2 = MC Err, 3 = Inertia, 4 = IMD, ..., 10 = default off state
uint8_t number_encodings[11] = {0b01000000, 0b01111001, 0b00100100, 0b00110000, 0b00011001, 0b00010010, 0b00000010, 0b01111000, 0b10000000, 0b00011000, 0b11111111};
uint8_t display_list[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

#define SHUTDOWN_THERSHOLD 223 // 5V on the line

#define MCU_HEARTBEAT_TIMEOUT 1000

#define LED_MIN_FAULT 1000
