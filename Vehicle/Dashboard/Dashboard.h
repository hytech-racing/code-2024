#pragma once

// Button pin definition
#define BTN_MARK 6
#define BTN_MODE 21
#define BTN_START 22
#define BTN_LC A3
#define BTN_MC_CYCLE 20

//RANDOM FOR NOW
#define BTN_TORQUE_MODE 19
#define BTN_LED_DIMMER 18

//Dial pin definition (RANDOM FOR NOW)

#define DIAL_MODE_ONE 8
#define DIAL_MODE_TWO 9
#define DIAL_ACCELERATION_LAUNCH_CONTROL 4
#define DIAL_SKIDPAD 5
#define DIAL_AUTOCROSS 6
#define DIAL_ENDURANCE 7



// LED and BUZZER_CTRL pin definition
#define BUZZER_CTRL 3
#define NEOPIXEL_CTRL 2
#define NEOPIXEL_COUNT 13
//#define LED_MECH_BRAKE 10

#define LED_OFF 0
#define LED_ON_GREEN 0xFF00
#define LED_YELLOW 0xFFFF00
#define LED_RED 0xFF0000

// enum class LED_COLORS { OFF = 0, ON = 0xFF00 , BLUE = 0xFF , RED = 0xFF0000};
enum class LED_MODES { OFF = 0, ON = 1, YELLOW = 2, RED = 3};
enum LED_LIST { AMS = 0, IMD = 1, MC_ERR = 2, GEN_PURP = 3, INERTIA = 4, BOTS = 5, 
                COCKPIT_BRB = 6, CRIT_CHARGE = 7, GLV = 8, BRAKE_ENGAGE = 9, LAUNCH_CTRL = 10, 
                TORQUE_MODE = 11, RDY_DRIVE = 12};

// MCP IO Expander pin definition //
#define IO_CS 10
#define IO_ADDR 0

// Critical read pin definitions
#define SSOK_READ 16
#define SHUTDOWN_H_READ 17
#define INERTIA_READ 15

//GLV threshold
#define GLV_THRESHOLD 60000

// IO Expander Number Encodings
// 1 = Inertia, 2 = MC Err, 3 = Inertia, 4 = IMD, ..., 10 = default off state
uint8_t number_encodings[11] = {0b01000000, 0b01111001, 0b00100100, 0b00110000, 0b00011001, 0b00010010, 0b00000010, 0b01111000, 0b10000000, 0b00011000, 0b11111111};
uint8_t display_list[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

#define SHUTDOWN_THERSHOLD 223 // 5V on the line

#define MCU_HEARTBEAT_TIMEOUT 1000

#define LED_MIN_FAULT 1000
