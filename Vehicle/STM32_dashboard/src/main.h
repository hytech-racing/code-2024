#ifndef MAIN_H
#define MAIN_H

#include "DashboardCAN.h"
#include "controls.h"

// Button pin definition
#define BTN_SAFE_CTRL 20
#define BTN_START 19
#define BTN_MC_CYCLE 18
#define BTN_TORQUE_MODE 22
#define BTN_LED_DIMMER 21

//Dial pin definition (RANDOM FOR NOW)
//physical wiring messed up
#define DIAL_SIZE 6
#define DIAL_MODE_ONE 9 //mode 0
#define DIAL_MODE_TWO 8 //mode 1
#define DIAL_ACCELERATION_LAUNCH_CONTROL 7// mode 2
#define DIAL_SKIDPAD 6
#define DIAL_AUTOCROSS 5
#define DIAL_ENDURANCE 4

// LED and BUZZER_CTRL pin definition
#define BUZZER_CTRL 3
#define LED_MECH_BRAKE 10

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
#define GLV_THRESHOLD 3104
#define PACK_THRESHOLD 33000
#define OUTSIDE_BRIGHTNESS 255
#define LOW_BRIGHTNESS 16

#endif