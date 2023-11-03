#ifndef HYTECH_DASH_H
#define HYTECH_DASH_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Adafruit_NeoPixel.h>
#include <bitmaps.h>


// Display defines
#define SHARP_SCK  PA5
#define SHARP_MOSI PA7
#define SHARP_MISO PA6
#define SHARP_SS   PC4

#define BLACK 0
#define WHITE 1

// Neopixel defines
#define NEOPIXEL_PIN PA2
#define NEOPIXEL_COUNT 13

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

//Forward declaration to allow use of DashboardCAN pointer in header
class DashboardCAN;
class SAB_lap_times;

//Singleton class header
class hytech_dashboard {
    public:
        // getInstance function returns the one existing insance of hytech_dashboard
        static hytech_dashboard* getInstance();
        // definitions for public functions
        void startup();
        void refresh(DashboardCAN* can);
        void set_neopixel(uint16_t id, uint32_t c);
        void draw_vertical_pedal_bar(double val, int initial_x_coord);
        void draw_regen_bar(double percent);
        void draw_current_draw_bar(double percent);
        void show_lap_times(SAB_lap_times* can);
    
    private:
        // Private constructor to prevent external instantiation
        hytech_dashboard();
        // Private destructor to prevent external deletion
        ~hytech_dashboard() { }
        // Pointer to the one instance of hytech_dashboard
        static hytech_dashboard* _instance;
        uint8_t previousTimerState = 0;
        uint32_t initialTime = 0;
        uint32_t currentTime = 0;
        void format_millis();
        String twoDigits(int number);

};

#endif



