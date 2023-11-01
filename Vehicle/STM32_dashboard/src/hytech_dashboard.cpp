#include <hytech_dashboard.h>
#include <DashboardCAN.h>
#include <SAB_lap_times.h>

// Definition of display and neopixel globals
// For some reason, code complains when these are defined in the header file
Adafruit_SharpMem _display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 400, 240);
Adafruit_NeoPixel _neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

/* Null, because instance will be initialized on demand. */

hytech_dashboard::hytech_dashboard(){}

hytech_dashboard* hytech_dashboard::getInstance() {
    if (_instance == NULL) {
        _instance = new hytech_dashboard();
    }
    return _instance;
}

// startup function
void hytech_dashboard::startup() {

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
    for (int i = 0; i < NEOPIXEL_COUNT - 1; i++) {
        _neopixels.setPixelColor(i, LED_INIT);
        if (i == 3) {
        // Don't use gen purpose led
        _neopixels.setPixelColor(i, 0);
        }
        if (i == 0 || i == 1) {
        // sets IMD and AMS lights off on startup as per rules
        _neopixels.setPixelColor(i, LED_OFF);
        }
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

//refresh dashboard
void hytech_dashboard::refresh(DashboardCAN* CAN) {
    // update neopixels
    _neopixels.show();

    // refresh display
    _display.clearDisplayBuffer();
    _display.drawBitmap(0,0, epd_bitmap_Displaytest, 400, 240, BLACK);
    draw_vertical_pedal_bar(CAN->pedal_readings.get_accelerator_pedal_1(), 40);
    draw_vertical_pedal_bar(CAN->pedal_readings.get_accelerator_pedal_1(), 374);
    show_lap_times(&(CAN->lap_times));
    _display.refresh();
}

//set neopixels
void hytech_dashboard::set_neopixel(uint16_t id, uint32_t c) {
    _neopixels.setPixelColor(id, c);
}

void hytech_dashboard::show_lap_times(SAB_lap_times* lap_times) {
    _display.setCursor(40, 70);
    _display.setTextColor(BLACK);
    _display.setTextSize(3);
    _display.println("Lap Times");
    _display.setCursor(40, _display.getCursorY());
    switch(lap_times->get_state()) {
        case 0:
            // clear timer
            previousTimerState = 0;
            times[current] = 0;
            break;
        case 1:
            // start timer
            if(previousTimerState != 1) {
                initialTime = millis();
            } 
            times[current] = millis() - initialTime;
            previousTimerState = 1;
            break;
        case 2:
            // end timer
            if(previousTimerState == 1) {
                times[current] = millis() - initialTime;
            }
            previousTimerState = 2;
            break;
        default:
            break;
    }
    
    times[lap_times->get_time_1_type()] = lap_times->get_time_1();
    times[lap_times->get_time_2_type()] = lap_times->get_time_2();

    format_millis("Curr", current);
    format_millis("Prev", previous);
    format_millis("Best", best);
}

void hytech_dashboard::format_millis(String label, uint32_t time) {
    int minutes = times[time]/(1000*60);
    int seconds = (times[time]-minutes*60000)/1000;
    int milliseconds = times[time] - minutes*60000 - seconds*1000;
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
