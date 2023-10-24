#include <hytech_dashboard.h>
#include <DashboardCAN.h>

// Definition of display and neopixel globals
// For some reason, code complains when these are defined in the header file
Adafruit_SharpMem _display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 400, 240);
Adafruit_NeoPixel _neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);

// Empty constructor for hytech_dashboard instance
hytech_dashboard::hytech_dashboard(){}

// getInstance function to return current instance or 
// call constructor if not initialized
hytech_dashboard* hytech_dashboard::getInstance()
{
    if (_instance == NULL)
    {
        _instance = new hytech_dashboard();
    }
    return _instance;
}

// startup function
void hytech_dashboard::startup() {
    // begin and clear display
    _display.begin();
    _display.clearDisplay();
    _display.setRotation(2);

    // draw Hytech logo in center of screen
    _display.drawBitmap(hytech_logo_x, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
    _display.refresh();

    // begin neopixels and set half brightness to not flashbang driver
    _neopixels.begin();
    _neopixels.setBrightness(2);

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

    // wait for 5 seconds, can be shortened
    delay(5000);
}

//refresh dashboard
void hytech_dashboard::refresh(DashboardCAN* CAN) {
    // data to write to display
    Dashboard_status* dash_status = &CAN->dashboard_status;
    MCU_status* mcu_status = &CAN->mcu_status;
    MCU_analog_readings* mcu_analog_readings = &CAN->mcu_analog_readings;
    BMS_voltages* bms_voltages = &CAN->bms_voltages;

    // refresh neopixels
    _neopixels.show();

    // refresh display
    
}

//set neopixels
void hytech_dashboard::set_neopixel(uint16_t id, uint32_t c) {
    _neopixels.setPixelColor(id, c);
}
