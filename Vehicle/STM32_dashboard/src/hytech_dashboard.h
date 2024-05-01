#ifndef HYTECH_DASH_H
#define HYTECH_DASH_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Adafruit_NeoPixel.h>

#include "bitmaps.h"
#include "DashboardCAN.h"
#include "MCP23S08.h"
#include "hytech.h"


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

#define NUM_PAGES 8

#define BLINK_PERIOD 500
#define FLASH_PERIOD 100

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 3
#define BRIGHTNESS_STEPS 4
#define STEP_BRIGHTNESS (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / BRIGHTNESS_STEPS

// MPH conversion
const float GEARBOX_RATIO =             11.86;
const float WHEEL_DIAMETER =            0.4064; // meters
const float RPM_TO_METERS_PER_SECOND =  WHEEL_DIAMETER * 3.1415 / GEARBOX_RATIO / 60.0;
const float METERS_PER_SECOND_TO_RPM =  1.0 / RPM_TO_METERS_PER_SECOND;
const float METERS_PER_SECOND_TO_MPH =  2.2369;

// OFF: OFF, ON: GREEN/OK, YELLOW : WARNING/MISC RED : CRITICAL
enum LED_LIST_e { BOTS = 0, LAUNCH_CTRL = 1, TORQUE_MODE = 2, BRAKE_ENGAGE = 3, COCKPIT_BRB = 4, INERTIA = 5, 
                GLV = 6, CRIT_CHARGE = 7, RDY_DRIVE = 8, MC_ERR = 9, IMD = 10, 
                AMS = 11};                

enum class LED_colors_e
{
    OFF = 0x00,
    ON = 0xFF00,
    YELLOW = 0xFFFF00,
    RED = 0xFF0000,
};

enum StartupAnimations {
    NONE,
    MIKHAIL_CAT,
    DAVID_KNIGHT_GLIZZY,
    DAVID_KNIGHT_2,
    ICE_SPICE
};

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


class DashboardCAN;
class SAB_lap_times;
class MCU_load_cells;

/*
    hytech_dashboard handles all interaction with the display components of the dash, including
    the display, neopixels, and 7-segment. It is architected as a singleton class, which in hindsight
    was more trouble than it was worth, but why change it now?
*/
class hytech_dashboard {
    public:
        /*!
            Returns the reference to the hytech_dashboard singleton class.
            @return hytech_dashboard pointer to the singleton
        */
        static hytech_dashboard* getInstance();

        /*
            Initializes display pins
        */
        void init();

        /*
            The startup function initializes the display devices and runs a short startup animation
            on the screen. Nothing is acutally starting up during this, but it looks cool.
        */
        void startup();

        /*!
            Refreshes the dashboard. This inlcudes the display, neopixels, and 7-segment display.
            The refresh function is meant to manage calls to the individual components of the dashboard
            as well as handle the "state" or current page of the display.

            @param CAN a pointer to the system's DashboardCAN reference that is used to access data for display.
        */
        void refresh(DashboardCAN* CAN);

        /*!
            A public setter for neopixels that can be used by other classes in the event that they need
            to change the value of a neopixel without going through dashboard_CAN.

            @param id the ID of the neopixel
            @param c the desired color value of the neopixel
        */
        void set_neopixel(uint16_t id, uint32_t c);

        /**
         * Sets neopixel brightness. Can be used by other classes when the dimmer button is pressed.
        */
        void dim_neopixels();

         /* handles increment and rollover for switching pages */
        void increment_page();

        /* handles decrement and rollover for switching pages*/
        void decrement_page();
    
    private:
        // Private constructor to prevent external instantiation
        hytech_dashboard();
        // Private destructor to prevent external deletion
        ~hytech_dashboard() { }
        // Pointer to the one instance of hytech_dashboard
        static hytech_dashboard* _instance;

        Metro pixel_refresh = Metro(100);

        String dial_modes[6] = {"M0", "M1", "CASE", "Simple Launch", "Slip Launch", "None"};
        String ecu_states[6] = {"Startup", "TS Not Active", "TS Active", "Enabling Inverters", "Wait RTD", "RTD"};

        typedef struct Errors {
            bool BRAKE_IMPLAUSIBILITY;
            bool ACCEL_IMPLAUSIBILITY;
            bool SOFTWARE_NOT_OK;
            bool SHUTDOWN;
            bool INVERTER_ERROR;
            bool IMD_FAULT;
        } errors_s;

        errors_s *current_errors = (errors_s *) calloc(1, sizeof(errors_s));
        errors_s *prev_errors = (errors_s *) calloc(1, sizeof(errors_s));

        /* current page displayed */
        uint8_t current_page = 0;
        uint8_t prev_page = 0;
        /* number encodings for use with the IO expander that drives the seven segment display */
        uint8_t number_encodings[11] = {0b01000000, 0b01111001, 0b00100100, 0b00110000, 0b00011001, 0b00010010, 0b00000010, 0b01111000, 0b10000000, 0b00011000, 0b11111111};
        uint8_t curr_num = 0;
        /* the previous timer state used for certain display behaviors when changing state*/
        uint8_t previousTimerState = 0;
        /* time the lap clock started counting */
        uint32_t initialTime = 0;
        /* current time? millis?*/
        uint32_t current_time = 0;
        /* best time as recorded by the TCU*/
        uint32_t best_time = 0;
        /* previous time as recorded by the TCU*/
        uint32_t prev_time = 0;
        /* target time as reported by the TCU*/
        uint32_t target_time = 0;

        uint8_t current_brightness = MIN_BRIGHTNESS;

        int page_offset = -4;

        int prev_dial_state = 0;
        unsigned long dial_prev_time = 0;
        unsigned long popup_time = 1000; // 1 second
        bool time_reset = false;
        int initialized = false;

        int prev_ecu_state = 1;
        unsigned long mcu_prev_time = 0;
        unsigned long mcu_popup_time = 1000; // 1 second


        /* accel max, min*/
        uint32_t max_accel_1 = 0;
        uint32_t max_accel_2 = 0;
        uint32_t min_accel_1 = 4294967295;
        uint32_t min_accel_2 = 4294967295;

        /* brake max, min*/
        uint32_t max_brake_1 = 0;
        uint32_t max_brake_2 = 0;
        uint32_t min_brake_1 = 4294967295;
        uint32_t min_brake_2 = 4294967295;
        

        uint32_t last_blink_millis = 0;
        bool last_blink = false;

        bool last_flash = false;
        uint32_t last_flash_millis;

        /* startup functions */
        void display_startup_animation(StartupAnimations);
        void display_hytech_animation();
        void init_neopixels();
        void init_io_expander();

        void draw_background_bitmap();

        /* Displays the clock and times in m:s.ms format based on the number of milliseconds*/
        void format_millis(String label, uint32_t time);
        
        /* helper function for format_millis that keeps all numbers at two digits*/
        String twoDigits(int number);

        void draw_page_title(String text);
        
        /* helper function that draws the pedal bar */
        void draw_vertical_pedal_bar(float val, int initial_x_coord);

        /* helper function that draws the regen bar based on regen percentage*/
        void draw_regen_bar(int percent);

        /* helper function that draws the regen bar based on current draw percentage*/
        void draw_current_draw_bar(int percent);

        void draw_battery_bar(int percent);

        void draw_popup(String title);
        void draw_popup_on_dial_change(DASHBOARD_STATE_t *, MCU_STATUS_t *m);
        void draw_popup_on_mcu_state_change(MCU_STATUS_t *m);
        void draw_popup_on_error();
        void draw_mcu_reported_torque_mode(DASHBOARD_MCU_STATE_t *t, MCU_STATUS_t *m);
        void rotate_and_draw_bitmap(const unsigned char bmp[], int size, double rotation, int x, int y);
        void draw_bitmap(const unsigned char bmp[], int size, int x, int y);

        /* helper function that draws quadrants on the display for tire/suspension data */
        void draw_quadrants(String text);
        void draw_hexant(String text);

        /*!
            Draws the lap times page on the display. This function keeps track of the state
            of the running lap clock, but this is merely a mirror of the state of the clock
            running on the TCU. Sending can messages every millisecond is a bad idea, so they
            are only sent on each state change of the clock, or when an updated time is available.
            This function will also display a target lap time to the driver that is part of the
            driver_msg architecture.

            @param lap_times* Pointer to the TCU_LAP_TIMES_t struct in dashboard_can
            @param driver_msg* Pointer to the TCU_DRIVER_MSG_t struct in dashboard_can
        */
        void show_lap_times(TCU_LAP_TIMES_t* lap_times, TCU_DRIVER_MSG_t* driver_msg);

        /*!
            Draws the suspension data on the display. This function splits the display into 4 quadrants that
            are used to display data from the corner boards of the car, most notably suspension travel from
            the pots and suspension load from the load cells.

            @param front_load_cells A MCU_LOAD_CELLS_t struct from the CAN library that includes data sent from the ECU, gathered from the front corner boards.
            @param rear_load_cells A SAB_LOAD_CELLS_t struct from the CAN library that includes data sent from the SAB, gathered from the rear corner boards.
        */
        void display_suspension_data(MCU_SUSPENSION_t* front_suspension, SAB_SUSPENSION_t* rear_suspension);

        /* resets the clock back to current time*/
        void restart_current_timer();

        /* helper for displaying data in quadrants, sets the print cursor, based on the seleceted quadrant */
        void set_cursor_in_quadrant(uint8_t quadrant, int vertical_offset);

        /*!
            Draws tire data on the display. Similar to the suspension data, this function segments the display
            and pulls data from the CAN bus when the TTPMS receiver is connected.
        */
        void display_tire_data();

        void display_speeds(DRIVETRAIN_RPMS_TELEM_t* drivetrain_rpms, BMS_VOLTAGES_t* bms_voltages);
        void display_lowest_segment_voltage(BMS_VOLTAGES_t *);
        void display_min_max_pedals(MCU_PEDAL_RAW_t *);
        void display_raw_pedal_readings(MCU_PEDAL_RAW_t *);
        void display_torque_diagnostics(CONTROLLER_BOOLEAN_t *, CONTROLLER_POWER_LIM_t *);
        void display_segment_voltages();
        void display_torque_requests();

        void check_for_errors(DashboardCAN *CAN);
        int check_latched(MCU_STATUS_t *);
        int check_ready_to_drive(MCU_STATUS_t *);
        bool first_latch = false;
        bool first_ready_to_drive = false;
        void draw_icons(MCU_STATUS_t *, VN_STATUS_t *);
        void draw_launch_screen();

        /*!
            refresh_neopixels handles changing the color of indicator LEDs on the display. This function
            is separated from the display refresh in the event we want to decouple the refresh rate between
            the different devices. The Neopixel library has a few caveats to its functionality to do with the
            lack of a PWM DMA controller, and will basically stop the clock while it writes to neopixels.

            @param can takes in a DashboardCAN pointer to access the data of mainly the dash_mcu_state message to correctly color the neopixels based on car state
        */
        void refresh_neopixels(DashboardCAN* can);

        /* Helper function for refresh_neopixels that sets the color of pixels with the enumerated color values */
        void set_neopixel_color(LED_LIST_e led, uint8_t state);

        void set_neopixel_color_gradient(LED_LIST_e led, uint8_t value);

        /*
            Display helper function for blinking display elements. Returns true if disp. element
            should be visible for that refresh cycle. Blink period is defined by the BLINK_PERIOD define
        */
        bool blink();
        bool flash(); // faster


};

#endif



