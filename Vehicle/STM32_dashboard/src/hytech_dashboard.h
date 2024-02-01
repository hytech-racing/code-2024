#ifndef HYTECH_DASH_H
#define HYTECH_DASH_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Adafruit_NeoPixel.h>

#include "bitmaps.h"
#include "DashboardCAN.h"
#include "MCP23S08.h"
#include "ht_can.h"


// Display defines
#define SHARP_SCK  PC10
#define SHARP_MOSI PB0
#define SHARP_SS   PC4

#define BLACK 0
#define WHITE 1

// Buzzer pin definition
#define BUZZER_CTRL PC13

// Neopixel defines
#define NEOPIXEL_PIN PC14
#define NEOPIXEL_COUNT 12

#define NUM_PAGES 2

// OFF: OFF, ON: GREEN/OK, YELLOW : WARNING/MISC RED : CRITICAL
enum class LED_MODES { OFF = 0, ON = 1, YELLOW = 2, RED = 3};
enum LED_LIST { AMS = 0, IMD = 1, MC_ERR = 2, GEN_PURP = 3, INERTIA = 4, BOTS = 5, 
                COCKPIT_BRB = 6, CRIT_CHARGE = 7, GLV = 8, BRAKE_ENGAGE = 9, LAUNCH_CTRL = 10, 
                TORQUE_MODE = 11, RDY_DRIVE = 12};

#define LED_OFF 0x00
#define LED_ON_GREEN 0xFF00
#define LED_YELLOW 0xFFFF00
#define LED_RED 0xFF0000
#define LED_INIT 0xFF007F
#define LED_BLUE 0xFF
#define LED_WHITE 0xFFFFFFFF

// MCP IO Expander pin definition //
#define IO_CS PA15
#define IO_ADDR 0

// SPI class
#define MOSI PC1
#define MISO PC2
#define SCLK PB10

//Forward declaration to allow use of DashboardCAN pointer in header
class DashboardCAN;
class SAB_lap_times;
class MCU_load_cells;

//Singleton class header
class hytech_dashboard {
    public:
        // getInstance function returns the one existing insance of hytech_dashboard
        static hytech_dashboard* getInstance();
        // definitions for public functions
        void startup();

        void refresh(DashboardCAN* can);

        void set_neopixel(uint16_t id, uint32_t c);

        enum Data_Type {
            TIRE = 0,
            SUSPENSION = 1
        };
    
    private:
        // Private constructor to prevent external instantiation
        hytech_dashboard();
        // Private destructor to prevent external deletion
        ~hytech_dashboard() { }
        // Pointer to the one instance of hytech_dashboard
        static hytech_dashboard* _instance;

        uint8_t current_page = 0;
        uint8_t number_encodings[11] = {0b01000000, 0b01111001, 0b00100100, 0b00110000, 0b00011001, 0b00010010, 0b00000010, 0b01111000, 0b10000000, 0b00011000, 0b11111111};
        uint8_t curr_num = 0;

        uint8_t previousTimerState = 0;
        uint32_t initialTime = 0;
        uint32_t current_time = 0;
        uint32_t best_time = 0;
        uint32_t prev_time = 0;
        uint32_t target_time = 0;

        void format_millis(String label, uint32_t time);
        String twoDigits(int number);
        
        void draw_vertical_pedal_bar(double val, int initial_x_coord);
        void draw_regen_bar(double percent);
        void draw_current_draw_bar(double percent);
        void draw_quadrants();
        void show_lap_times(TCU_LAP_TIMES_t* lap_times, TCU_DRIVER_MSG_t* driver_msg);
        void display_suspension_data(MCU_LOAD_CELLS_t* front_load_cells, SAB_LOAD_CELLS_t* rear_load_cells);
        void restart_current_timer();
        void increment_page();
        void decrement_page();
        void set_cursor(uint8_t quadrant);
        void display_tire_data();


};

#endif



