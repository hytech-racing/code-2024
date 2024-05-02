#include "hytech_dashboard.h"
#include "quotes.h"
#include "stdlib.h"
#include <random>
#include <fstream>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>


// Definition of display and neopixel globals
// For some reason, code complains when these are defined in the header file
Adafruit_SharpMem _display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 320, 240);
Adafruit_NeoPixel _neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

//PB5 // 
//PB6 // bots read
// Pb7 // inertia read

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
MCP23S08 _expander(&spi, IO_ADDR, IO_CS);

hytech_dashboard* hytech_dashboard::getInstance() {
    if (_instance == NULL) {
        _instance = new hytech_dashboard();
    }
    return _instance;
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
    init_io_expander();
    init_neopixels();
    display_hytech_animation();
    display_startup_animation(StartupAnimations::NONE);
}

uint32_t max(uint32_t val1, uint32_t val2) {
    return (val1 >= val2) ? val1 : val2;
}

uint32_t min(uint32_t val1, uint32_t val2) {
    return (val1 <= val2) ? val1 : val2;
}


void hytech_dashboard::refresh(DashboardCAN* CAN) {

    /* refresh neopixels, redraw buffer, write to 7-seg display*/
    refresh_neopixels(CAN);
    _expander.digitalWrite(number_encodings[CAN->dash_state.dial_state]);
    draw_background_bitmap();
    if (!initialized) {initialized = true; prev_dial_state = CAN->dash_state.dial_state; }

    /* draw brake pedal */
    float brake_mech_point = HYTECH_mechanical_brake_percent_float_ro_fromS(CAN->mcu_pedal_readings.mechanical_brake_percent_float_ro);
    float accel_pedal_percentage = HYTECH_accel_percent_float_ro_fromS(CAN->mcu_pedal_readings.accel_percent_float_ro);
    float brake_pedal_percentage = HYTECH_brake_percent_float_ro_fromS(CAN->mcu_pedal_readings.brake_percent_float_ro);
    draw_vertical_pedal_bar(brake_pedal_percentage, 17);

    /* set mechanical brake led */
    CAN->dash_mcu_state.mechanical_brake_led = (CAN->mcu_pedal_readings.brake_percent_float_ro >= CAN->mcu_pedal_readings.mechanical_brake_percent_float_ro) ? 2 : 0;

    /** TODO: add real data to these bars*/
    // draw_battery_bar(0);
    // draw_regen_bar(47);
    // draw_current_draw_bar(0);

    switch(current_page) {

        case 0:
            display_speeds(&(CAN->drivetrain_rpms), &(CAN->bms_voltages));
            break;

        case 1:
            display_lowest_segment_voltage(&CAN->bms_voltages);
            break;

        case 2:
            display_min_max_pedals(&CAN->pedal_raw);
            break;        

        case 3:
            display_suspension_data(&(CAN->mcu_suspension), &(CAN->sab_suspension));
            break;

        case 4:
            display_torque_diagnostics(&CAN->controller_boolean, &CAN->controller_power_lim);
            break;

        case 5:
            // show_lap_times(&(CAN->lap_times), &(CAN->driver_msg));
            display_torque_requests();
            break;

        case 6:
            display_raw_pedal_readings(&CAN->pedal_raw);
            break;

        case 7:
            display_tire_data();
            break;

    }

    draw_icons(&CAN->mcu_status, &CAN->vn_status);
    draw_mcu_reported_torque_mode(&CAN->dash_mcu_state, &CAN->mcu_status);

    /* draw launch screen */
    if ((CAN->dash_state.dial_state == 3 || CAN->dash_state.dial_state == 4) && CAN->dash_mcu_state.launch_control_led == 1) draw_launch_screen();

    draw_popup_on_dial_change(&CAN->dash_state, &CAN->mcu_status);
    draw_popup_on_mcu_state_change(&CAN->mcu_status);
    // draw_popup_on_error();

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

void hytech_dashboard::rotate_and_draw_bitmap(const unsigned char bmp[], int size, double angle, int initialX, int Y) {
    float cosA = cos(angle);
    float sinA = sin(angle);
    // for (int i = 0; i < 92; y++) {
    //     const unsigned char hex = bmp[i]; // hex number corresponding to the 8 pixels
    //     for (int j = 0; j < 7; j++) {
    //         // 8 pixels per hex number

    //     }
    //     int x = i % 27;
    //     int y = i /= 27;
    //     // int pixel = hex & 
    //     int newX = (cosA * x) - (sinA * y) + x;
    //     int newY = (sinA * x) + (cosA * y) + y;
    //     _display.drawPixel(newX, newY, pixel);
    // }
}

void hytech_dashboard::draw_bitmap(const unsigned char bmp[], int size, int initialX, int initialY) {
    for (int i = 0; i < 92; i++) {
        for (int j = 0; j < 8; j++) {
            unsigned char val = bmp[i];
            
        }
        int index = i/8;
        int pos = i % 8;
        int bit = (bmp[index] & (1 << (8-pos))) >> (8-pos);
        int y = i / 27;
        int x = i % 27;
        _display.drawPixel(initialX + x, initialY + y, bit);
    }
}

void hytech_dashboard::draw_page_title(String text) {
    _display.setCursor(50, 68);
    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    _display.setFont(&FreeSansBold12pt7b);
    _display.println(text);
    _display.setCursor(50, _display.getCursorY());
    _display.setFont(&FreeSans12pt7b);
}

void hytech_dashboard::display_suspension_data(MCU_SUSPENSION_t* front_suspension, SAB_SUSPENSION_t* rear_suspension) {
    
    draw_quadrants("Sus...");
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

    _display.setCursor(100, 140);
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
    _display.println(twoDigits(mph));
    // _display.println(HYTECH_low_voltage_ro_fromS(bms_voltages->low_voltage_ro));

    // _display.println(mph);
    _display.setTextSize(1);
    _display.setFont(&FreeSans12pt7b);
    _display.setCursor(125, 165);
    _display.print("MPH");
    _display.setFont(&FreeSans12pt7b);
}
        
void hytech_dashboard::display_lowest_segment_voltage(BMS_VOLTAGES_t *v) {
    draw_page_title("Low Volt...");
    _display.setFont(&FreeSans24pt7b);
    _display.setTextSize(2);
    _display.setCursor(75, 160);
    _display.println(HYTECH_low_voltage_ro_fromS(v->low_voltage_ro));
    _display.setTextSize(1);
    _display.setFont(&FreeSans12pt7b);
    _display.setCursor(125, 185);
    _display.print("VOLTS");
    _display.setFont(&FreeSans12pt7b);
}

void hytech_dashboard::display_min_max_pedals(MCU_PEDAL_RAW_t *p) {
    int x = 70;
    _display.setCursor(x,50);
    _display.setTextSize(1);

    max_accel_1 = max(p->accel_1_raw, max_accel_1);
    max_accel_2 = max(p->accel_2_raw, max_accel_2);
    min_accel_1 = min(p->accel_1_raw, min_accel_1);
    min_accel_2 = min(p->accel_2_raw, min_accel_2);

    max_brake_1 = max(p->brake_1_raw, max_brake_1);
    max_brake_2 = max(p->brake_2_raw, max_brake_2);
    min_brake_1 = min(p->brake_1_raw, min_brake_1);
    min_brake_2 = min(p->brake_2_raw, min_brake_2);

    /* ACCEL */
    _display.print("Max A1: ");
    _display.println(max_accel_1);
    _display.setCursor(x, _display.getCursorY()-5);

    _display.print("Min A1: ");
    _display.println(min_accel_1);
    _display.setCursor(x, _display.getCursorY()-5);

    _display.print("Max A2: ");
    _display.println(max_accel_2);
    _display.setCursor(x, _display.getCursorY()-5);

    _display.print("Min A2: ");
    _display.println(min_accel_2);
    _display.setCursor(x, _display.getCursorY()-5);

    
    /* BRAKE */
    _display.print("Max B1: ");
    _display.println(max_brake_1);
    _display.setCursor(x, _display.getCursorY()-5);

    _display.print("Min B1: ");
    _display.println(min_brake_1);
    _display.setCursor(x, _display.getCursorY()-5);

    _display.print("Max B2: ");
    _display.println(max_brake_2);
    _display.setCursor(x, _display.getCursorY()-5);

    _display.print("Min B2: ");
    _display.println(min_brake_2);
    _display.setCursor(x, _display.getCursorY()-5);
}

void hytech_dashboard::display_raw_pedal_readings(MCU_PEDAL_RAW_t *p) {
    _display.setCursor(100,100);
    _display.setTextSize(1);

    _display.print("A1: ");
    _display.println(p->accel_1_raw);
    _display.setCursor(100, _display.getCursorY());

    _display.print("A2: ");
    _display.println(p->accel_2_raw);
    _display.setCursor(100, _display.getCursorY());

    _display.print("B1: ");
    _display.println(p->brake_1_raw);
    _display.setCursor(100, _display.getCursorY());

    _display.print("B2: ");
    _display.println(p->brake_2_raw);
    _display.setCursor(100, _display.getCursorY());
}

void hytech_dashboard::draw_background_bitmap() {
    _display.clearDisplayBuffer();
    _display.drawBitmap(0,0, epd_bitmap_hytech_dashboard, 320, 240, BLACK);
    _display.fillRect(320-40, 30, 40, 200, WHITE);
    _display.fillRect(283, 36, 305-283, 210-36, BLACK);
    _display.fillRect(283-3, (36 + 210 - 36)/2+15, 25, 7, WHITE);
    _display.fillRect(0, 215, 130, 25, WHITE);
    // _display.fillRect(100, 5, 100, 20, BLACK);
}

void hytech_dashboard::display_torque_diagnostics(CONTROLLER_BOOLEAN_t *c, CONTROLLER_POWER_LIM_t *p) {
    _display.setCursor(40,60);
    switch (p->controller_power_lim_status) {
        case 0:
            _display.println("No Power Limit");
            break;
        case 1:
            _display.println("PID Power Limit");
            break;
        case 2:
            _display.println("Mech Power Limit");
            break;
    }
    _display.setCursor(40, _display.getCursorY());

    _display.print("Use Launch: ");
    _display.println(c->controller_use_launch);
    _display.setCursor(40, _display.getCursorY());

    _display.print("Use NF: ");
    _display.println(c->controller_use_normal_force);
    _display.setCursor(40, _display.getCursorY());

    _display.print("Use PID power lim: ");
    _display.println(c->controller_use_pid_power_limit);
    _display.setCursor(40, _display.getCursorY());

    _display.print("Use PID tv: ");
    _display.println(c->controller_use_pid_tv);
    _display.setCursor(40, _display.getCursorY());

    _display.print("Use power lim: ");
    _display.println(c->controller_use_power_limit);
    _display.setCursor(40, _display.getCursorY());
}

void hytech_dashboard::display_torque_requests() {
    draw_page_title("Torque Requests");
    int x_off = 70;
    int bar_height = 70;
    int bar_width = 20;
    int starty = 60;
    int y_offset = bar_height + 10;
    int mid = 320/2;
    _display.fillRect(mid-x_off,starty,bar_width,bar_height, BLACK);
    _display.fillRect(mid+x_off,starty,bar_width,bar_height, BLACK);
    _display.fillRect(mid-x_off,starty+y_offset,bar_width,bar_height, BLACK);
    _display.fillRect(mid+x_off,starty+y_offset,bar_width,bar_height, BLACK);

}

void hytech_dashboard::draw_launch_screen() {
    if (flash()) _display.fillScreen(BLACK);
    else _display.clearDisplayBuffer();
    _display.setCursor(55,135);
    _display.setTextColor(flash() ? WHITE : BLACK);
    _display.setFont(&FreeSansBold24pt7b);
    _display.println("LAUNCH");
}

void hytech_dashboard::display_startup_animation(StartupAnimations animation) {
    switch (animation) {
        case StartupAnimations::DAVID_KNIGHT_GLIZZY:
            _display.drawBitmap(0,0, epd_bitmap_glizzy, 320, 240, BLACK);
            _display.refresh();
            delay(3000);
            break;
        case StartupAnimations::DAVID_KNIGHT_2:
            _display.drawBitmap(0,0, epd_bitmap_david, 320, 240, BLACK);
            _display.refresh();
            delay(3000);
            break;
        case StartupAnimations::MIKHAIL_CAT:
            _display.drawBitmap(60,25, epd_bitmap_cat, 200, 200, BLACK);
            _display.refresh();
            delay(3000);
            break;
        case StartupAnimations::ICE_SPICE:
            for (int i = 0; i < 10; i++) {
                switch(i) {
                    case 1:
                        _display.drawBitmap(0,0, isp1, 320, 240, BLACK);
                        break;
                    case 2:
                        _display.drawBitmap(0,0, isp2, 320, 240, BLACK);
                        break;
                    case 3:
                        _display.drawBitmap(0,0, isp3, 320, 240, BLACK);
                        break;
                    case 4:
                        _display.drawBitmap(0,0, isp4, 320, 240, BLACK);
                        break;
                    case 5:
                        _display.drawBitmap(0,0, isp5, 320, 240, BLACK);
                        break;
                    case 6:
                        _display.drawBitmap(0,0, isp6, 320, 240, BLACK);
                        break;
                    case 7:
                        _display.drawBitmap(0,0, isp7, 320, 240, BLACK);
                        break;
                    case 8:
                        _display.drawBitmap(0,0, isp8, 320, 240, BLACK);
                        break;
                    case 9:
                        _display.drawBitmap(0,0, isp9, 320, 240, BLACK);
                        break;
                    case 10:
                        _display.drawBitmap(0,0, isp10, 320, 240, BLACK);
                        break;
                }
            _display.refresh();
            delay(100);
            _display.clearDisplayBuffer();
            }
        default:
            break;
    }
    _display.clearDisplayBuffer();
    _display.drawBitmap(0,0, epd_bitmap_hytech_dashboard, 320, 240, BLACK);
}

void hytech_dashboard::display_hytech_animation() {
    _display.begin();
    _display.clearDisplay();
    _display.setRotation(0);
    _display.drawBitmap(hytech_logo_x, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
    _display.refresh();
    delay(2000);
    for (int i = 1; i > -116; i-=3) {
        _display.clearDisplayBuffer();
        _display.drawBitmap(hytech_logo_x + i, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
        _display.refresh();
    }
    delay(20);
    _display.drawBitmap(hytech_words_x + 45, hytech_words_y, epd_bitmap_HytechWords, hytech_words_x_size, hytech_words_y_size, BLACK);
    _display.refresh();
    _display.setFont(&FreeSans12pt7b);
    String greeting = "Undefeated";
    int length = greeting.length();
    _display.setCursor(hytech_logo_x-length*3, hytech_logo_y + hytech_logo_size + 30);
    _display.setTextColor(BLACK);
    _display.setTextSize(1);
    _display.println(greeting);
    _display.refresh();
    delay(2000);
    _display.clearDisplay();
}

void hytech_dashboard::init_neopixels() {
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
}

void hytech_dashboard::init_io_expander() {
    _expander.begin();
    for (int i = 0; i < 8; i++) {
        _expander.pinMode(i, OUTPUT);
        _expander.digitalWrite(i, HIGH);
    }
}

void hytech_dashboard::draw_icons(MCU_STATUS_t *m, VN_STATUS_t *v) {

    /* no gps icon   = 0 */
    /* vn flashing   = 1 */
    /* vn solid      = 2 */

    /* all units are pixels */
    int offset = 3;
    int icon_size = 27;

    /* horizontal icons on top of the screen */
    int gps_icon_pos_x = 270 - icon_size - offset;
    int rtd_icon_pos_x = gps_icon_pos_x - icon_size - offset;
    int latched_icon_pos_x = rtd_icon_pos_x - icon_size - 1;
    int icon_pos_y = 40;

    if (v->vn_gps_status >= 2) {
        _display.drawBitmap(gps_icon_pos_x, icon_pos_y, epd_bitmap_gps, 27, 27, BLACK);
    } else if (v->vn_gps_status == 1){
        if (blink()) { _display.drawBitmap(gps_icon_pos_x, icon_pos_y, epd_bitmap_gps, 27, 27, BLACK); }
    } else if (v->vn_gps_status == 0) {
        _display.drawBitmap(gps_icon_pos_x, icon_pos_y, epd_bitmap_nogps, 27, 27, BLACK);
    }

    if (check_ready_to_drive(m)) {
        _display.drawBitmap(rtd_icon_pos_x, icon_pos_y, epd_bitmap_rtd, 27, 27, BLACK);
    } else {
        if (blink()) { _display.drawBitmap(rtd_icon_pos_x, icon_pos_y, epd_bitmap_rtd, 27, 27, BLACK); }
    }

    if (check_latched(m)) {
        _display.drawBitmap(latched_icon_pos_x, icon_pos_y, epd_bitmap_latch_symbol, 27, 27, BLACK);
    } else {
        if (blink()) { _display.drawBitmap(latched_icon_pos_x, icon_pos_y, epd_bitmap_latch_symbol, 27, 27, BLACK); }
    }
}

void hytech_dashboard::draw_mcu_reported_torque_mode(DASHBOARD_MCU_STATE_t *d, MCU_STATUS_t *m) {
/* display current torque mode and torque limit */
_display.setCursor(14, 25);
_display.print(dial_modes[d->dial_state]);
_display.print(": ");
_display.println(m->max_torque);
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

void hytech_dashboard::draw_popup(String title) {
    int width = 200;
    int height = 150;
    _display.fillRect(160-(width/2), 120 - (height/2), width, height, BLACK);
    width-=10;
    height-=10;
    _display.fillRect(160-(width/2), 120 - (height/2), width, height, WHITE);
    _display.setFont(&FreeSansBold12pt7b);
    _display.setCursor(160-(width/2)+1, 120 - (height/2)+20);
    int x = _display.getCursorX();
    _display.println(title);
    _display.setFont(&FreeSans12pt7b);
    _display.setCursor(x, _display.getCursorY());
}

void hytech_dashboard::draw_popup_on_dial_change(DASHBOARD_STATE_t *s, MCU_STATUS_t *m) {
    if (prev_dial_state != s->dial_state) {
        prev_dial_state = s->dial_state;
        dial_prev_time = millis();
    }
    if (dial_prev_time + popup_time > millis()) {
        draw_popup("Dial Change");
        int x = _display.getCursorX();
        _display.print("Mode: ");
        _display.println(s->dial_state);
        _display.setCursor(x, _display.getCursorY());
        _display.println(dial_modes[s->dial_state]);
        _display.setCursor(x, _display.getCursorY());
        _display.print("Max Torque: ");
        _display.println(m->max_torque);
    }
}

void hytech_dashboard::draw_popup_on_mcu_state_change(MCU_STATUS_t *s) {
    if (prev_ecu_state != s->ecu_state) {
        prev_ecu_state = s->ecu_state;
        mcu_prev_time = millis();
    }
    if (mcu_prev_time + mcu_popup_time > millis()) {
        draw_popup("State Change");
        int x = _display.getCursorX();
        _display.print("State: ");
        _display.println(s->ecu_state);
        _display.setCursor(x, _display.getCursorY());
        _display.println(ecu_states[s->ecu_state]);
    }
}

void hytech_dashboard::draw_popup_on_error() {
    if (memcmp(current_errors, prev_errors, sizeof(errors_s)) != 0) {
        *prev_errors = *current_errors;
    }
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
        set_neopixel_color(LED_LIST_e::COCKPIT_BRB, CAN->brb_read ? 1 : 3);
        set_neopixel_color(LED_LIST_e::INERTIA, CAN->inertia_read ? 1 : 3);
        set_neopixel_color(LED_LIST_e::RDY_DRIVE, CAN->dash_mcu_state.start_status_led);
        set_neopixel_color(LED_LIST_e::MC_ERR, CAN->dash_mcu_state.motor_controller_error_led);
        set_neopixel_color(LED_LIST_e::IMD, CAN->dash_mcu_state.imd_led);
        set_neopixel_color(LED_LIST_e::AMS, CAN->dash_mcu_state.ams_led);
        set_neopixel_color(LED_LIST_e::GLV, 0);
        // set_neopixel_color_gradient(LED_LIST_e::GLV, CAN->dash_mcu_state.glv_led);
        set_neopixel_color_gradient(LED_LIST_e::CRIT_CHARGE, CAN->dash_mcu_state.pack_charge_led);
        SerialUSB.println(CAN->mcu_status.no_brake_implausibility);
        if (!CAN->mcu_status.no_brake_implausibility) {
            set_neopixel_color(LED_LIST_e::BRAKE_ENGAGE, 3);
            if (blink()) { set_neopixel_color(LED_LIST_e::BRAKE_ENGAGE, 0); }
        } else {
            set_neopixel_color(LED_LIST_e::BRAKE_ENGAGE, CAN->dash_mcu_state.mechanical_brake_led);
        }
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

bool hytech_dashboard::flash() {
    if((millis() - last_flash_millis) > FLASH_PERIOD) {
        last_flash = !last_flash;
        last_flash_millis = millis();
    }
    return last_flash;
}

void hytech_dashboard::check_for_errors(DashboardCAN *CAN) {
    current_errors->BRAKE_IMPLAUSIBILITY = !CAN->mcu_status.no_brake_implausibility;
    current_errors->ACCEL_IMPLAUSIBILITY = !CAN->mcu_status.no_accel_implausibility;
    current_errors->SOFTWARE_NOT_OK = !CAN->mcu_status.software_ok;
    current_errors->INVERTER_ERROR = CAN->mcu_status.inverter_error;
    current_errors->IMD_FAULT = !CAN->mcu_status.imd_ok_high;
    current_errors->SHUTDOWN = CAN->mcu_status.shutdown_b_above_threshold || CAN->mcu_status.shutdown_c_above_threshold
    || CAN->mcu_status.shutdown_d_above_threshold || CAN->mcu_status.shutdown_e_above_threshold;
}

int hytech_dashboard::check_latched(MCU_STATUS_t *status) { return status->ecu_state>=2; }
int hytech_dashboard::check_ready_to_drive(MCU_STATUS_t *status) { return status->ecu_state==5; }
