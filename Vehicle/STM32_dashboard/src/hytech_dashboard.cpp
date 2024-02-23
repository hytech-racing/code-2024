#include "hytech_dashboard.h"
#include "quotes.h"
#include "stdlib.h"
#include <random>
#include <fstream>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

// Definition of display and neopixel globals
// For some reason, code complains when these are defined in the header file
Adafruit_SharpMem _display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 320, 240);
Adafruit_NeoPixel _neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

//PB5 // 
//PB6 // bots read
// Pb7 // inertia read

/* Null, because instance will be initialized on demand. */

hytech_dashboard::hytech_dashboard(){}

/*

    Some notes for architecture of this file. It may be better to eventually move all of these functions
    into separate classes and use inheritance or just separate the functions and pass display and neopixel
    references to basic functions.

    For now, we will have the startup and refresh functions at the top of the file.
    Page draws will be below these. This will be any special page such as the lap times display,
    the suspension display, etc.

    Other miscellaneous helper functions will go below those, this will be any neopixel setter or calculation
    functions, as well as shape drawing functions (pedal bar, pack charge, etc.)
    We can make exceptions for really small helper functions that can go directly below their page draw calls

*/


SPIClass spi(MOSI, MISO, SCLK);
// TODO change pins
MCP23S08 _expander(&spi, IO_ADDR, IO_CS);

hytech_dashboard* hytech_dashboard::getInstance() {
    if (_instance == NULL) {
        _instance = new hytech_dashboard();
    }
    return _instance;
}

unsigned int generateSeed() {
    // You can use any source of variability available on your system
    unsigned int seed = 0;

    // Example: Use the current tick count of the microcontroller
    // seed ^= HAL_GetTick();

    // Example: Use the memory address of a volatile variable
    volatile int dummy;
    // seed ^= reinterpret_cast<unsigned int>(&dummy);

    // Use analog input value
    seed ^= analogRead(PA3);
    seed ^= analogRead(PA2);
    seed ^= analogRead(PA1);
    // seed ^= analogRead(PA0);
    // seed ^= analogRead(PB0);
    // seed ^= analogRead(PB1);
    // seed ^= analogRead(PB1);
    // seed ^= analogRead(PB3);

    uint32_t mem;
    // memcpy(mem, 0x0, 4);
    // seed ^= mem;


    // Add more sources of variability as needed

    return seed;
}




void hytech_dashboard::startup() {

    pinMode(PB5, INPUT);
    pinMode(PB6, INPUT);
    pinMode(PB7, INPUT);

    _expander.begin();

    for (int i = 0; i < 8; i++) {
        _expander.pinMode(i, OUTPUT);
        _expander.digitalWrite(i, HIGH);
    }

    // begin, clear display, set rotation
    _display.begin();
    _display.clearDisplay();
    _display.setRotation(0);

    // draw Hytech logo in center of screen
    _display.drawBitmap(hytech_logo_x, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
    _display.refresh();

    // begin neopixels and set half brightness to not flashbang driver
    _neopixels.begin();
    _neopixels.setBrightness(MIN_BRIGHTNESS);

    //set init color for every led
    for (int i = 0; i < NEOPIXEL_COUNT; i++) {
        _neopixels.setPixelColor(i, LED_INIT);

        // AMS and IMD are off according to rules
        if (i == LED_LIST_e::AMS || i == LED_LIST_e::IMD){
            _neopixels.setPixelColor(i, LED_colors_e::OFF);
        }
            
    }
    // write data to neopixels
    _neopixels.show();

    delay(2000);

    // for loop that continues to write logo with x offset of i
    // shifts logo left to make room for Hytech Racing words
    for (int i = 1; i > -116; i-=3) {
        _display.clearDisplayBuffer();
        _display.drawBitmap(hytech_logo_x + i, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
        _display.refresh();
    }

    delay(20);

    //display "Hytech Racing" on display
    _display.drawBitmap(hytech_words_x + 45, hytech_words_y, epd_bitmap_HytechWords, hytech_words_x_size, hytech_words_y_size, BLACK);
    _display.refresh();

    _display.setFont(&FreeSans12pt7b);

    unsigned int seed = generateSeed();
    std::mt19937 gen(seed);

    // Generate a random number between 1 and 100
    std::uniform_int_distribution<> dis(1, 100000);
    int randomNumber = dis(gen);

    String greeting = greetings[randomNumber % NUMBER_OF_MESSAGES];
    int length = greeting.length();
    _display.setCursor(hytech_logo_x-length*3, hytech_logo_y + hytech_logo_size + 30);
    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    srand(micros());
    _display.println(greetings[randomNumber % NUMBER_OF_MESSAGES]);
    _display.refresh();

    delay(5000);

    _display.clearDisplay();
    _display.drawBitmap(0,0, epd_bitmap_hytech_dashboard, 320, 240, BLACK);
}


void hytech_dashboard::refresh(DashboardCAN* CAN) {
    // update neopixels
    refresh_neopixels(CAN);

    // SerialUSB.print("BRB: ");
    // SerialUSB.println(digitalRead(PB5));

    // SerialUSB.print("BOTS: ");
    // SerialUSB.println(digitalRead(PB6));

    // SerialUSB.print("INERTIA: ");
    // SerialUSB.println(digitalRead(PB7));

    // add page to dashboard showing if shutdown tripped

    _expander.digitalWrite(number_encodings[CAN->dash_state.dial_state]);

    // refresh display

    _display.clearDisplayBuffer();
    _display.drawBitmap(0,0, epd_bitmap_hytech_dashboard, 320, 240, BLACK);


    // draw_vertical_pedal_bar(CAN->mcu_pedal_readings.accel_pedal_1, 17);
    // draw_vertical_pedal_bar(CAN->mcu_pedal_readings.accel_pedal_1, 285);

    draw_vertical_pedal_bar(i, 17);
    draw_vertical_pedal_bar(100-i, 285);
    draw_battery_bar(i);
    draw_regen_bar(i);
    draw_current_draw_bar(i);
    if (forward) {
        i+=1;
    } else {
        i-=1;
    }
    if (forward && i == 100) {
        forward = false;
    } else if (!forward && i == 0) {
        forward = true;
    }

    // for (int i = 100; i >= 0; i -= 3) {
    //     draw_vertical_pedal_bar(i, 17);
    //     _display.refresh();
    // }


    current_page = 0;

    switch(current_page) {
        case 0:
            show_lap_times(&(CAN->lap_times), &(CAN->driver_msg));
            _display.drawBitmap(40, 40, epd_bitmap_gps, 27, 27, BLACK);
            break;
        case 1:
            // suspension
            // loadcell, pot
            display_suspension_data(&(CAN->mcu_load_cells), &(CAN->sab_load_cells));
            break;
        case 2:
            // tires
            //temp,pressure
            display_tire_data();
            break;
        case 3:
            // groundspeed, wheelspeed/rpm, other useful stuff, current draw
            // raw pedal value
            // icon gps lock
            display_speeds();
            break;
        case 4:
            // display_segment_voltages(&(CAN -> bms_voltages_received));
            break;
        default:
            display_error();
            break;
    }

    _display.refresh();
}

void hytech_dashboard::increment_page() {
    current_page++;
    current_page %= NUM_PAGES;
}

void hytech_dashboard::decrement_page() {
    // adding number of pages would be the same value after modulus
    // subtracting one will decrement, but will also wraparound when == 0
    // this prevents overflow when subtracting one from current page
    current_page += NUM_PAGES - 1;
    current_page %= NUM_PAGES;
}

void hytech_dashboard::show_lap_times(TCU_LAP_TIMES_t* lap_times, TCU_DRIVER_MSG_t* driver_msg) {
    
    switch(lap_times->lap_clock_state) {
        case 0:
            // clear timer
            previousTimerState = 0;
            current_time = 0;
            break;
        case 1:
            // start timer
            if(previousTimerState != 1) {
                initialTime = millis();
            } 
            current_time = millis() - initialTime;
            previousTimerState = 1;
            break;
        case 2:
            // end timer
            if(previousTimerState == 1) {
                current_time = millis() - initialTime;
            }
            previousTimerState = 2;
            break;
        default:
            break;
    }
    
    // add logic -> if best time is  0, current time is best time.
    // also don't display anything if time is zero. Can be done in formatting
    // target lap time may be calculated dynamically in the future. For now,
    // it will just be part of the driver message CAN message sent by pit crew
    best_time = lap_times->best_lap_time;
    prev_time = lap_times->prev_lap_time;
    target_time = driver_msg->target_lap_time;

    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    _display.setFont(&FreeSansBold12pt7b);
    if (CENTER_ALIGN) {
        _display.setCursor(105, 90);
        _display.println("Lap Times");
        _display.setFont(&FreeSans12pt7b);
        _display.setCursor(100, _display.getCursorY());
        format_millis("Curr", current_time);
        format_millis("Prev", prev_time);
        format_millis("Best", best_time);
    } else {

    }


}

/* LAP CLOCK HELPERS */

void hytech_dashboard::restart_current_timer() {
    initialTime = millis();
}

void hytech_dashboard::format_millis(String label, uint32_t time) {
    int minutes = time / (1000 * 60);
    int seconds = (time - minutes * 60000) / 1000;
    // Sid put some god damn brackets in this shit ^^ VV idc if you know PEMDAS
    int milliseconds = time - minutes * 60000 - seconds * 1000;

    _display.setCursor(90, _display.getCursorY());
    _display.print(label);
    _display.print(": ");
    _display.print(twoDigits(minutes));
    _display.print(":");
    _display.print(twoDigits(seconds));
    _display.print(".");
    _display.println(milliseconds);
}

String hytech_dashboard::twoDigits(int number) {
    if(number <= 9) {
        return "0" + String(number);
    }
    if(number >= 10 && number <= 99) {
        return String(number);
    }
    if(number >=100 && number <=999) {
        return String(number/10);
    }
    return String(number);
}

void hytech_dashboard::display_suspension_data(MCU_LOAD_CELLS_t* front_load_cells, SAB_LOAD_CELLS_t* rear_load_cells) {
    
    draw_quadrants();

    set_cursor(1);
    _display.print("FR: ");
    _display.println(front_load_cells->FR_load_cell);
    set_cursor(2);
    _display.print("FL: ");
    _display.println(front_load_cells->FL_load_cell);
    set_cursor(3);
    _display.print("RL: ");
    _display.println(rear_load_cells->RL_load_cell);
    set_cursor(4);
    _display.print("RR: ");
    _display.println(rear_load_cells->RR_load_cell);
}

void hytech_dashboard::display_tire_data() {
    draw_quadrants();
}

void hytech_dashboard::display_speeds() {
    _display.setCursor(40, 70);
    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    _display.println("Speed: ");
    _display.setCursor(40, _display.getCursorY());
    _display.println("Pedals:");
    _display.setCursor(40, _display.getCursorY());
    _display.println("Current: ");
}
        
void hytech_dashboard::display_segment_voltages(BMS_VOLTAGES_t* voltages) {
    _display.setCursor(40, 70);
    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    _display.println("S1: ");
    _display.setCursor(40, _display.getCursorY());
    _display.println("S2: ");
    _display.setCursor(40, _display.getCursorY());
    _display.println("S3: ");
    _display.setCursor(40, _display.getCursorY());
    _display.println("S4: ");
    _display.setCursor(40, _display.getCursorY());
}

void hytech_dashboard::display_error() {
    set_cursor(2);
    // _display.fillRoundRect();
    _display.print("Error");
}

/* DISPLAY HELPER FUNCTIONS */

void hytech_dashboard::draw_quadrants() {
    _display.drawLine(160, 40, 160, 200, BLACK);
    _display.drawLine(50, 120, 270, 120, BLACK);
}


void hytech_dashboard::set_cursor(uint8_t quadrant) {
    _display.setTextSize(1);
    _display.setTextColor(BLACK);
    switch(quadrant) {
        case 1:
            _display.setCursor(210,40);
            break;
        case 2:
            _display.setCursor(43,40);
            break;
        case 3:
            _display.setCursor(43,130);
            break;
        case 4:
            _display.setCursor(210,130);
            break;
    }
}

// draws white rect top down
void hytech_dashboard::draw_vertical_pedal_bar(int val, int initial_x_coord) {
    double ZERO_PERCENT_VAL = 175;
    val = (val > 100) ? val = 100 : (val < 0) ? val = 0 : val = val;
    int i = (100-val) * (ZERO_PERCENT_VAL/100.0);
    _display.fillRect(initial_x_coord, 35, 18, i, WHITE);
}

void hytech_dashboard::draw_regen_bar(int percent) {
    // 0%: 65
    // 100%: 0
    int w = (100-percent) * (60.0/100);
    _display.fillRect(73, 17, w, 17, WHITE);
}

void draw_rectangle_right_corner(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    for (int16_t i = x; i > x-w; i--) {
        _display.writeFastVLine(i, y, h, color);
    }
    // _display.refresh();
}

void hytech_dashboard::draw_current_draw_bar(int percent) {
    // 0%: 100
    // 100%: 0
    int w = (100-percent) * (103.0/100);
    draw_rectangle_right_corner(247, 17, w, 17, WHITE);
}

void hytech_dashboard::draw_battery_bar(int percent) {
    // 0%: 59
    // 100% 0
    int w = (100-percent) * (59.0/100);
    draw_rectangle_right_corner(103, 220, w, 8, WHITE);
}

/* NEOPIXEL HELPER FUNCTIONS */

void hytech_dashboard::dim_neopixels() {
    current_brightness -= STEP_BRIGHTNESS;
    // set current brightness to 0xFF (255) if less than min brightness - sid :) DO NOT CHANGE
    if (current_brightness < MIN_BRIGHTNESS) { current_brightness |= 0xFF; }
    _neopixels.setBrightness(current_brightness);
    SerialUSB.println(_neopixels.getBrightness());
}

//set neopixels
void hytech_dashboard::set_neopixel(uint16_t id, uint32_t c) {
    _neopixels.setPixelColor(id, c);
}

void hytech_dashboard::refresh_neopixels(DashboardCAN* CAN) {

    // only refresh if updates were found in the CAN message
    // This does not work for some reason on initial message
    // replace Metro timer if it is figured out
    if (CAN->mcu_state_update || pixel_refresh.check()) {

        // Serial.println("Refreshing Neopixels");

        set_neopixel_color(LED_LIST_e::BOTS, CAN->dash_mcu_state.bots_led);
        set_neopixel_color(LED_LIST_e::LAUNCH_CTRL, CAN->dash_mcu_state.launch_control_led);
        set_neopixel_color(LED_LIST_e::TORQUE_MODE, CAN->dash_mcu_state.mode_led);
        set_neopixel_color(LED_LIST_e::BRAKE_ENGAGE, CAN->dash_mcu_state.mechanical_brake_led);
        set_neopixel_color(LED_LIST_e::COCKPIT_BRB, CAN->dash_mcu_state.cockpit_brb_led);
        set_neopixel_color(LED_LIST_e::INERTIA, CAN->dash_mcu_state.inertia_status_led);
        set_neopixel_color(LED_LIST_e::RDY_DRIVE, CAN->dash_mcu_state.start_status_led);
        set_neopixel_color(LED_LIST_e::MC_ERR, CAN->dash_mcu_state.motor_controller_error_led);
        set_neopixel_color(LED_LIST_e::IMD, CAN->dash_mcu_state.imd_led);
        set_neopixel_color(LED_LIST_e::AMS, CAN->dash_mcu_state.ams_led);
        
        _neopixels.show();
        CAN->mcu_state_update = false;

        pixel_refresh.reset();
    }

}

void hytech_dashboard::set_neopixel_color(LED_LIST_e led, uint8_t state) {
    uint32_t color;
    switch (state)
    {
    case 0:
        color = LED_colors_e::OFF;
        break;
    case 1:
        color = LED_colors_e::ON;
        break;
    case 2:
        color = LED_colors_e::YELLOW;
        break;
    case 3:
        color = LED_colors_e::RED;
        break;
    
    default:
        color = LED_INIT;
        break;
    }

    _neopixels.setPixelColor(led, color);

}
