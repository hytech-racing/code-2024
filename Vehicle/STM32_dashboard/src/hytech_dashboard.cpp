#include <hytech_dashboard.h>
using namespace hytech_dashboard;

Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 400, 240);
Adafruit_NeoPixel neopixels(NEOPIXEL_COUNT, NEOPIXEL_CTRL, NEO_GRBW + NEO_KHZ800);
//start dashboard
void startup() {
    display.begin();
    display.clearDisplay();
    display.setRotation(2);

    display.drawBitmap(hytech_logo_x, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
    display.refresh();

    delay(2000);

    for (int i = 1; i > -142; i-=3) {
    display.clearDisplayBuffer();
    display.drawBitmap(hytech_logo_x + i, hytech_logo_y, epd_bitmap_Hytech_Logo, hytech_logo_size, hytech_logo_size, BLACK);
    display.refresh();
    }

    delay(20);

    display.drawBitmap(hytech_words_x + 53, hytech_words_y, epd_bitmap_HytechWords, hytech_words_x_size, hytech_words_y_size, BLACK);
    display.refresh();
    delay(5000);
}

//refresh dashboard

// refresh display

//set neopixels
void hytech_dashboard::set_neopixel(uint16_t id, uint32_t c) {
    neopixels.setPixelColor(id, c);
}
