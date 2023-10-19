#include <hytech_dashboard.h>

Adafruit_SharpMem hytech_dashboard::display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 400, 240);
Adafruit_NeoPixel hytech_dashboard::neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW + NEO_KHZ800);
//start dashboard
void hytech_dashboard::startup() {
    // begin and clear display
    display.begin();
    display.clearDisplay();
    display.setRotation(2);

    // draw Hytech logo in center of screen
    display.drawBitmap(hytech_logo_x, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
    display.refresh();

    // begin neopixels and set half brightness to not flashbang driver
    neopixels.begin();
    neopixels.setBrightness(2);

    //set init color for every led
    for (int i = 0; i < NEOPIXEL_COUNT - 1; i++) {
        neopixels.setPixelColor(i, LED_INIT);
        if (i == 3) {
        // Don't use gen purpose led
        neopixels.setPixelColor(i, 0);
        }
        if (i == 0 || i == 1) {
        // sets IMD and AMS lights off on startup as per rules
        neopixels.setPixelColor(i, LED_OFF);
        }
    }
    // write data to neopixels
    neopixels.show();

    delay(2000);

    // for loop that continues to write logo with x offset of i
    // shifts logo left to make room for Hytech Racing words
    for (int i = 1; i > -142; i-=3) {
        display.clearDisplayBuffer();
        display.drawBitmap(hytech_logo_x + i, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
        display.refresh();
    }

    delay(20);

    //display "Hytech Racing" on display
    display.drawBitmap(hytech_words_x + 53, hytech_words_y, epd_bitmap_HytechWords, hytech_words_x_size, hytech_words_y_size, BLACK);
    display.refresh();

    delay(5000);
}

//refresh dashboard
void hytech_dashboard::refresh() {

}
// refresh display

//set neopixels
void hytech_dashboard::set_neopixel(uint16_t id, uint32_t c) {
    neopixels.setPixelColor(id, c);
}
