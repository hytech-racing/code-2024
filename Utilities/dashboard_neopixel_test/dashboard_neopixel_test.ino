
#include <Adafruit_NeoPixel.h>
// only send if receiving mcu status messages

// NEOPIXEL Variables
Adafruit_NeoPixel dashboard_neopixels(13, 2, NEO_GRBW + NEO_KHZ800);
uint8_t curr_brightness = 16;



inline void neopixel_init();


void setup() {
  

  neo_pixel_init();


  delay(1000);
}

void loop() {
  
  
}

inline void neo_pixel_init() {

  dashboard_neopixels.begin();
  dashboard_neopixels.setBrightness(curr_brightness);
  dashboard_neopixels.setPixelColor(0, 0xFF);
    
  dashboard_neopixels.setPixelColor(1, 0xFF00);

  dashboard_neopixels.setPixelColor(2, 0xFF0000);
    
  dashboard_neopixels.setPixelColor(3, 0xFF);
  dashboard_neopixels.setPixelColor(4, 0xFF00);
    
  dashboard_neopixels.setPixelColor(5, 0xFF0000);
  dashboard_neopixels.setPixelColor(6, 0xFF);
    
  dashboard_neopixels.setPixelColor(7, 0xFF00);
  dashboard_neopixels.setPixelColor(8, 0xFF0000);
    
  dashboard_neopixels.setPixelColor(9, 0xFF);
  dashboard_neopixels.setPixelColor(10, 0xFF00);
  dashboard_neopixels.setPixelColor(11, 0xFF0000);
  dashboard_neopixels.setPixelColor(12, 0xFF);
  
    
    
    dashboard_neopixels.show();


  }
  
