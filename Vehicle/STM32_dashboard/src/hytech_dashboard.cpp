#include "hytech_dashboard.h"
#include "quotes.h"
#include "stdlib.h"
#include <random>
#include <fstream>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

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

    We also have helper functions to draw quadrants and hexants. - sid :)

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

    // uint32_t mem;
    // memcpy(mem, 0x0, 4);
    // seed ^= mem;


    // Add more sources of variability as needed

    return seed;
}

void hytech_dashboard::init() {
    // set non-needed Display pins low
    pinMode(PC5, OUTPUT);
    pinMode(PB1, OUTPUT);
    pinMode(PA3, OUTPUT);
    digitalWrite(PC5, LOW);
    digitalWrite(PB1, LOW);
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
            _neopixels.setPixelColor(i, 0);
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

    // String greeting = greetings[randomNumber % NUMBER_OF_MESSAGES];
    String greeting = "Undefeated";
    int length = greeting.length();
    _display.setCursor(hytech_logo_x-length*3, hytech_logo_y + hytech_logo_size + 30);
    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    srand(micros());
    _display.println(greeting);
    _display.refresh();

    delay(2000);
    _display.clearDisplay();

    _display.clearDisplay();
    _display.drawBitmap(0,0, epd_bitmap_hytech_dashboard, 320, 240, BLACK);
}


void hytech_dashboard::refresh(DashboardCAN* CAN) {
    // update neopixels
    refresh_neopixels(CAN);

    // add page to dashboard showing if shutdown tripped

    _expander.digitalWrite(number_encodings[CAN->dash_state.dial_state]);

    // refresh display

    _display.clearDisplayBuffer();
    _display.drawBitmap(0,0, epd_bitmap_hytech_dashboard, 320, 240, BLACK);


    /** TODO: scale these values down to [0,100]*/

    double ACCEL1_MIN_VAL = 2167;
    double ACCEL1_MAX_VAL = 3285;
    double ACCEL1_RANGE = ACCEL1_MAX_VAL - ACCEL1_MIN_VAL;

    double BRAKE1_MIN_VAL = 2587;
    double BRAKE1_MAX_VAL = 1947;
    double BRAKE1_RANGE = BRAKE1_MAX_VAL - BRAKE1_MIN_VAL;

    // TODO: add mechanical break point to display
    float brake_mech_point = HYTECH_mechanical_brake_percent_float_ro_fromS(CAN->mcu_pedal_readings.mechanical_brake_percent_float_ro);
    float accel_pedal = HYTECH_accel_percent_float_ro_fromS(CAN->mcu_pedal_readings.accel_percent_float_ro);
    float brake_pedal = HYTECH_brake_percent_float_ro_fromS(CAN->mcu_pedal_readings.brake_percent_float_ro);
    draw_vertical_pedal_bar(accel_pedal, 285);
    draw_vertical_pedal_bar(brake_pedal, 17);

    if (CAN->mcu_pedal_readings.brake_percent_float_ro >= CAN->mcu_pedal_readings.mechanical_brake_percent_float_ro) {
        CAN->dash_mcu_state.mechanical_brake_led = 2;
    } else {
        CAN->dash_mcu_state.mechanical_brake_led = 0;
    }

    /** TODO: add real data to these bars*/
    draw_battery_bar(0);
    draw_regen_bar(0);
    draw_current_draw_bar(0);

    /** TODO: only show this if GPS lock present (use CAN message when it exists)*/
    if (blink()) _display.drawBitmap(270-27, 40, epd_bitmap_gps, 27, 27, BLACK);

    switch(current_page) {
        case 0:
            display_speeds(&(CAN->drivetrain_rpms), &(CAN->bms_voltages));
            break;
        case 1:
            display_suspension_data(&(CAN->mcu_suspension), &(CAN->sab_suspension));
            break;
        case 2:
            // tires
            //temp,pressure
            display_tire_data();
            break;
        case 3:
            show_lap_times(&(CAN->lap_times), &(CAN->driver_msg));
            break;
        case 4:
            display_segment_voltages();
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

    draw_page_title("Lap Times");

    _display.setCursor(50, 100);
    format_millis("Curr", millis());
    format_millis("Prev", prev_time);
    format_millis("Best", best_time);


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

    _display.setCursor(50, _display.getCursorY());
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

void hytech_dashboard::draw_page_title(String text) {
    _display.setCursor(50, 68);
    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    _display.setFont(&FreeSansBold12pt7b);
    _display.print(text);
    _display.setFont(&FreeSans12pt7b);
}

void hytech_dashboard::display_suspension_data(MCU_SUSPENSION_t* front_suspension, SAB_SUSPENSION_t* rear_suspension) {
    
    draw_quadrants("Suspension");
    _display.setFont(&FreeSans9pt7b);

    // q1
    set_cursor_in_quadrant(1, 20);
    int x = _display.getCursorX();
    _display.print("LD: ");
    _display.println(front_suspension->load_cell_fr);
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("POT: ");
    _display.println(front_suspension->potentiometer_fr);

    // q2
    set_cursor_in_quadrant(2, 20);
    x = _display.getCursorX();
    _display.print("LD: ");
    _display.println(front_suspension->load_cell_fl);
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("POT: ");
    _display.println(front_suspension->potentiometer_fl);

    set_cursor_in_quadrant(3, 20);
    x = _display.getCursorX();
    _display.print("LD: ");
    _display.println(rear_suspension->load_cell_rl);
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("POT: ");
    _display.println(rear_suspension->potentiometer_rl);


    set_cursor_in_quadrant(4, 20);
    x = _display.getCursorX();
    _display.print("LD: ");
    _display.println(rear_suspension->load_cell_rr);
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("POT: ");
    _display.println(rear_suspension->potentiometer_rr);

    _display.setFont(&FreeSans12pt7b);
}

void hytech_dashboard::display_tire_data() {
    draw_quadrants("Tire Data");
    // q1
    set_cursor_in_quadrant(1, 20);

    _display.setFont(&FreeSans9pt7b);

    int x = _display.getCursorX();
    _display.print("PRES: ");
    _display.println("0");
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("TEMP: 0");

    // q2
    set_cursor_in_quadrant(2, 20);
    x = _display.getCursorX();
    _display.print("PRES: ");
    _display.println("0");
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("TEMP: 0");

    set_cursor_in_quadrant(3, 20);
    x = _display.getCursorX();
    _display.print("PRES: ");
    _display.println("0");
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("TEMP: 0");


    set_cursor_in_quadrant(4, 20);
    x = _display.getCursorX();
    _display.print("PRES: ");
    _display.println("0");
    _display.setCursor(x, _display.getCursorY()-6);
    _display.print("TEMP: 0");

    _display.setFont(&FreeSans12pt7b);
}

void hytech_dashboard::display_speeds(DRIVETRAIN_RPMS_TELEM_t* drivetrain_rpms, BMS_VOLTAGES_t* bms_voltages) {
    _display.setFont(&FreeSans24pt7b);
    _display.setTextSize(2);

    _display.setCursor(100, 160);
    /** TODO: convert from RPM to MPH*/
    double rpms = drivetrain_rpms->fr_motor_rpm;
    rpms += drivetrain_rpms->fl_motor_rpm;
    rpms += drivetrain_rpms->rr_motor_rpm;
    rpms += drivetrain_rpms->rl_motor_rpm;
    rpms /= 4;
    double wheelspeed = abs(rpms * RPM_TO_METERS_PER_SECOND);
    // SerialUSB.println(wheelspeed);
    uint16_t mph = (int) (wheelspeed * METERS_PER_SECOND_TO_MPH);
    // SerialUSB.println(mph);
    // _display.println(twoDigits(mph));
    _display.println(HYTECH_low_voltage_ro_fromS(bms_voltages->low_voltage_ro));

    // _display.println(mph);
    _display.setTextSize(1);
    _display.setFont(&FreeSans12pt7b);
    _display.setCursor(125, 185);
    _display.print("VOLTS");
    _display.setFont(&FreeSans12pt7b);
}
        
void hytech_dashboard::display_segment_voltages() {
    draw_hexant("Segment Voltages");
}

void hytech_dashboard::display_error() {
    // set_cursor_in_quadrant(2);
    // _display.fillRoundRect();
    _display.print("Error");
}

/* DISPLAY HELPER FUNCTIONS */

void hytech_dashboard::draw_quadrants(String text) {
    draw_page_title(text);
    // vertical lines
    _display.drawLine(160, 75+page_offset, 160, 215+page_offset, BLACK); // middle vertical line
    _display.drawLine(50, 75+page_offset, 50, 215+page_offset, BLACK); // left vertical line
    _display.drawLine(270, 75+page_offset, 270, 215+page_offset, BLACK); // right vertical line

    // horizontal lines
    _display.drawLine(50, 145+page_offset, 270, 145+page_offset, BLACK); // middle horizontal line
    _display.drawLine(50, 75+page_offset, 270, 75+page_offset, BLACK); // top horizontal line
    _display.drawLine(50, 215+page_offset, 270, 215+page_offset, BLACK); // bottom horizontal line
}

void hytech_dashboard::draw_hexant(String text) {
    int offset = 23;
    draw_page_title(text);
    // vertical lines
    _display.drawLine(160, 75+page_offset, 160, 215+page_offset, BLACK); // middle vertical line
    _display.drawLine(50, 75+page_offset, 50, 215+page_offset, BLACK); // left vertical line
    _display.drawLine(270, 75+page_offset, 270, 215+page_offset, BLACK); // right vertical line

    // horizontal lines
    _display.drawLine(50, 75+page_offset, 270, 75+page_offset, BLACK); // top horizontal line
    _display.drawLine(50, 215+page_offset, 270, 215+page_offset, BLACK); // bottom horizontal line
    _display.drawLine(50, 145+page_offset-offset, 270, 145+page_offset-offset, BLACK); // middle horizontal line
    _display.drawLine(50, 145+page_offset+offset, 270, 145+page_offset+offset, BLACK); // middle horizontal line
}

void hytech_dashboard::set_cursor_in_quadrant(uint8_t quadrant, int vertical_offset) {
    _display.setTextSize(1);
    _display.setTextColor(BLACK);
    switch(quadrant) {
        case 1:
            _display.setCursor(160,75+page_offset+vertical_offset);
            break;
        case 2:
            _display.setCursor(50,75+page_offset+vertical_offset);
            break;
        case 3:
            _display.setCursor(50,145+page_offset+vertical_offset);
            break;
        case 4:
            _display.setCursor(160,145+page_offset+vertical_offset);
            break;
    }
}

// draws white rect top down
void hytech_dashboard::draw_vertical_pedal_bar(float val, int initial_x_coord) {
    double ZERO_PERCENT_VAL = 175;
    val = (val > 100) ? val = 100 : (val < 0) ? val = 0 : val = val;
    int i = (int) (100-val) * (ZERO_PERCENT_VAL/100.0);
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

        // Serial.printf("Refreshing Neopixels, %d, %d\n", CAN->brb_read ? LED_colors_e::ON : LED_colors_e::RED, CAN->inertia_read ? LED_colors_e::ON : LED_colors_e::RED);

        set_neopixel_color(LED_LIST_e::BOTS, CAN->dash_mcu_state.bots_led);
        set_neopixel_color(LED_LIST_e::LAUNCH_CTRL, CAN->dash_mcu_state.launch_control_led);
        set_neopixel_color(LED_LIST_e::TORQUE_MODE, CAN->dash_mcu_state.mode_led);
        set_neopixel_color(LED_LIST_e::BRAKE_ENGAGE, CAN->dash_mcu_state.mechanical_brake_led);
        set_neopixel_color(LED_LIST_e::COCKPIT_BRB, CAN->brb_read ? 1 : 3);
        set_neopixel_color(LED_LIST_e::INERTIA, CAN->inertia_read ? 1 : 3);
        set_neopixel_color(LED_LIST_e::RDY_DRIVE, CAN->dash_mcu_state.start_status_led);
        set_neopixel_color(LED_LIST_e::MC_ERR, CAN->dash_mcu_state.motor_controller_error_led);
        set_neopixel_color(LED_LIST_e::IMD, CAN->dash_mcu_state.imd_led);
        set_neopixel_color(LED_LIST_e::AMS, CAN->dash_mcu_state.ams_led);
        set_neopixel_color(LED_LIST_e::GLV, 0);
        // set_neopixel_color_gradient(LED_LIST_e::GLV, CAN->dash_mcu_state.glv_led);
        set_neopixel_color_gradient(LED_LIST_e::CRIT_CHARGE, CAN->dash_mcu_state.pack_charge_led);
        
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
        color = int(LED_colors_e::OFF);
        break;
    case 1:
        color = int(LED_colors_e::ON);
        break;
    case 2:
        color = int(LED_colors_e::YELLOW);
        break;
    case 3:
        color = int(LED_colors_e::RED);
        break;
    
    default:
        color = LED_INIT;
        break;
    }

    _neopixels.setPixelColor(led, color);

}

void hytech_dashboard::set_neopixel_color_gradient(LED_LIST_e led, uint8_t value) {

    uint8_t rgb[] = {0, 0, 0};

    if (value > 128) {
        rgb[0] = map(value, 128, 255, 255, 0);
        rgb[1] = 255;
    } else if (value > 30) {
        rgb[0] = 255;
        rgb[1] = map(value, 0, 128, 0, 255);
    } else {
        rgb[0] = blink() ? 255 : 0;
        rgb[1] = 0;
    }

    uint32_t color = rgb[0] << 16 | rgb[1] << 8 | rgb[2];
    _neopixels.setPixelColor(led, color);

}

bool hytech_dashboard::blink() {

    if((millis() - last_blink_millis) > BLINK_PERIOD) {
        last_blink = !last_blink;
        last_blink_millis = millis();
    }

    return last_blink;
}


// todo
/*
* suspension data (pots, load cells)
  tire data
  skip splash screen
  random msgs
  speed page
  gradient for
  RAINBOW LED
  pop-up msgs
  launch inverting colors
  ice spice
*/
