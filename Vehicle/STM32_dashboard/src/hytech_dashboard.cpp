#include "hytech_dashboard.h"

// Definition of display and neopixel globals
// For some reason, code complains when these are defined in the header file
Adafruit_SharpMem _display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 320, 240);
Adafruit_NeoPixel _neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

/* Null, because instance will be initialized on demand. */

hytech_dashboard::hytech_dashboard(){}




SPIClass spi(MOSI, MISO, SCLK);
// TODO change pins
MCP23S08 _expander(&spi, IO_ADDR, IO_CS);

hytech_dashboard* hytech_dashboard::getInstance() {
    if (_instance == NULL) {
        _instance = new hytech_dashboard();
    }
    return _instance;
}

// startup function
void hytech_dashboard::startup() {

    _expander.begin();

    for (int i = 0; i < 8; i++) {
        _expander.pinMode(i, OUTPUT);
        _expander.digitalWrite(i, HIGH);
    }

    // begin, clear display, set rotation
    _display.begin();
    _display.clearDisplay();
    _display.setRotation(2);

    // draw Hytech logo in center of screen
    _display.drawBitmap(hytech_logo_x, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
    _display.refresh();

    // begin neopixels and set half brightness to not flashbang driver
    _neopixels.begin();
    _neopixels.setBrightness(50);

    //set init color for every led
    for (int i = 0; i < NEOPIXEL_COUNT; i++) {
        _neopixels.setPixelColor(i, LED_INIT);
    }
    // write data to neopixels
    _neopixels.show();

    delay(2000);

    // for loop that continues to write logo with x offset of i
    // shifts logo left to make room for Hytech Racing words
    for (int i = 1; i > -142; i-=3) {
        _display.clearDisplayBuffer();
        _display.drawBitmap(hytech_logo_x + i, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
        _display.refresh();
    }

    delay(20);

    //display "Hytech Racing" on display
    _display.drawBitmap(hytech_words_x + 53, hytech_words_y, epd_bitmap_HytechWords, hytech_words_x_size, hytech_words_y_size, BLACK);
    _display.refresh();

    delay(5000);

    // display template
    _display.clearDisplay();
    _display.drawBitmap(0,0, epd_bitmap_Displaytest, 400, 240, BLACK);

    // brake pedal
    //9,40,17,143
    // 0% -> 143 (height)
    // 100% -> 40
    // _display.fillRect(9,40, 17, 143, WHITE);
    // draw_vertical_pedal_bar(0, 9);
    _display.refresh();

    // regen bar
    _display.fillRect(83, 7, 72, 16, WHITE);
    _display.fillRect(161+2, 5+2, 158-2, 18-2, WHITE);
    _display.refresh();

    pinMode(BUZZER_CTRL, OUTPUT);
    pinMode(PA3, OUTPUT);
    digitalWrite(PA3, HIGH);
}

// draws white rect top down
void hytech_dashboard::draw_vertical_pedal_bar(double val, int initial_x_coord) {
    // 100%: height of white box = 40
    //   0%: height of white box = 143 (covering the whole black bar)
    _display.fillRect(initial_x_coord, 40, 17, (1 - (((double)val - 500) / 1640)) * 143, WHITE);
    // SerialUSB.println((((double)val - 500) / 1640));
}

void hytech_dashboard::draw_regen_bar(double percent) {
    _display.fillRect(83, 7, (1-percent)*72, 16, WHITE);
}

void hytech_dashboard::draw_current_draw_bar(double percent) {
    _display.fillRect(163+156, 5+2, -156, 18-2, WHITE);
}

void rainbow() {

}



//refresh dashboard
void hytech_dashboard::refresh(DashboardCAN* CAN) {
    // update neopixels
    rainbow();
    _neopixels.show();

    digitalWrite(BUZZER_CTRL, HIGH);
    digitalWrite(PA3, HIGH);
    delay(1000);
    digitalWrite(BUZZER_CTRL, LOW);
    digitalWrite(PA3, LOW);

    delay(1000);


    _expander.digitalWrite(number_encodings[curr_num]);
    curr_num++;
    if (curr_num > 9) {curr_num = 0;}
    delay(200);


    // refresh display
    _display.clearDisplayBuffer();
    _display.drawBitmap(0,0, epd_bitmap_hytech_dashboard, 320, 240, BLACK);
    draw_vertical_pedal_bar(CAN->mcu_pedal_readings.accel_pedal_1, 9);
    draw_vertical_pedal_bar(CAN->mcu_pedal_readings.accel_pedal_1, 374);

    switch(current_state) {
        case 0:
            show_lap_times(&(CAN->lap_times), &(CAN->driver_msg));
            break;
        case 1:
            // suspension
            display_border();
            display_suspension_data(&(CAN->mcu_load_cells), &(CAN->sab_load_cells));
            break;
        case 2:
            display_border();
            break;
    }

    _display.refresh();
}

//set neopixels
void hytech_dashboard::set_neopixel(uint16_t id, uint32_t c) {
    _neopixels.setPixelColor(id, c);
}

void hytech_dashboard::show_lap_times(TCU_LAP_TIMES_t* lap_times, TCU_DRIVER_MSG_t* driver_msg) {
    _display.setCursor(40, 70);
    _display.setTextColor(BLACK);
    _display.setTextSize(3);
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

    _display.println("Lap Times");
    _display.setCursor(40, _display.getCursorY());
    format_millis("Curr", current_time);
    format_millis("Prev", prev_time);
    format_millis("Best", best_time);
}

void hytech_dashboard::restart_current_timer() {
    initialTime = millis();
}

void hytech_dashboard::increment_state() {
    current_state++;
    if(current_state > MAX_STATE) {
        current_state = 0;
    }
}

void hytech_dashboard::display_border() {
    _display.drawLine(160, 40, 160, 200, BLACK);
    _display.drawLine(50, 120, 270, 120, BLACK);
}


void hytech_dashboard::set_cursor(uint8_t quadrant) {
    _display.setTextSize(3);
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

void hytech_dashboard::display_suspension_data(MCU_LOAD_CELLS_t* front_load_cells, SAB_LOAD_CELLS_t* rear_load_cells) {
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

}

void hytech_dashboard::format_millis(String label, uint32_t time) {
    int minutes = time / (1000 * 60);
    int seconds = (time - minutes * 60000) / 1000;
    // Sid put some god damn brackets in this shit ^^ VV idc if you know PEMDAS
    int milliseconds = time - minutes * 60000 - seconds * 1000;

    _display.setCursor(40, _display.getCursorY());
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
