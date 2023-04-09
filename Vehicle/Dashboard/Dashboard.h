
#include <Adafruit_NeoPixel.h>
#include "DebouncedButton.h"
#include "MCP23S08.h"
#include "DialVectoring.h"
#include "HyTech_CAN.h"
#include "FlexCAN_T4.h"
// Button pin definition
#define BTN_SAFE_CTRL 20
#define BTN_START 19
#define BTN_MC_CYCLE 18
#define BTN_TORQUE_MODE 22
#define BTN_LED_DIMMER 21

//Dial pin definition (RANDOM FOR NOW)
#define DIAL_SIZE 6
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
#define LED_MECH_BRAKE 10

#define LED_OFF 0x00
#define LED_ON_GREEN 0xFF00
#define LED_YELLOW 0xFFFF00
#define LED_RED 0xFF0000
#define LED_INIT 0xFF007F

// enum class LED_COLORS { OFF = 0, ON = 0xFF00 , BLUE = 0xFF , RED = 0xFF0000};
// OFF: OFF, ON: GREEN/OK, YELLOW : WARNING/MISC RED : CRITICAL
enum class LED_MODES { OFF = 0, ON = 1, YELLOW = 2, RED = 3};
enum LED_LIST { AMS = 0, IMD = 1, MC_ERR = 2, GEN_PURP = 3, INERTIA = 4, BOTS = 5, 
                COCKPIT_BRB = 6, CRIT_CHARGE = 7, GLV = 8, BRAKE_ENGAGE = 9, LAUNCH_CTRL = 10, 
                TORQUE_MODE = 11, RDY_DRIVE = 12};

// MCP IO Expander pin definition //
#define IO_CS 10
#define IO_ADDR 0

#define SHUTDOWN_THERSHOLD 223 // 5V on the line

#define MCU_HEARTBEAT_TIMEOUT 1000

#define LED_MIN_FAULT 1000
// Critical read pin definitions
#define BOTS_READ A2 //BOTS
#define BRB_READ A3 //BRB
#define INERTIA_READ A1

//GLV threshold
#define GLV_THRESHOLD 60000
#define OUTSIDE_BRIGHTNESS 255
#define LOW_BRIGHTNESS 16
