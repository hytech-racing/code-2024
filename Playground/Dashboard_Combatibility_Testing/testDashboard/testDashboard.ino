
#include "Dashboard.h"
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel dashboard_neopixels(NEOPIXEL_COUNT, NEOPIXEL_CTRL, NEO_GRBW + NEO_KHZ800);
void setup() {
  // put your setup code here, to run once:
  
    dashboard_neopixels.begin();
    
    dashboard_neopixels.show();
    dashboard_neopixels.setBrightness(255);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(200);
  dashboard_neopixels.setPixelColor(0,0xFF);
  dashboard_neopixels.show();
   
}
