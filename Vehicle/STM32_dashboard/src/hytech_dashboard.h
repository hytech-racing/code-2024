#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Adafruit_NeoPixel.h>
#include <DashboardCAN.h>
#include <bitmaps.h>

namespace hytech_dashboard {
    Adafruit_NeoPixel dashboard_neopixels(NEOPIXEL_COUNT, NEOPIXEL_CTRL, NEO_GRBW + NEO_KHZ800);
    inline void set_neopixel(uint16_t id, uint32_t c);
}





