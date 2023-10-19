#include <hytech_dashboard.h>
using namespace hytech_dashboard;

//refresh dashboard

// refresh display

//set neopixels
void hytech_dashboard::set_neopixel(uint16_t id, uint32_t c) {
    dashboard_neopixels.setPixelColor(id, c);
}
