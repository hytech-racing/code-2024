#include <Arduino.h>
#include "STEERING_RS422.h"
#include "Metro.h"

STEERING_RS422 steering(5);
Metro read_steering_timer(20);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial5.begin(115200);
  //steering.init(115200);
  pinMode(13, OUTPUT);
  
  
}
bool out;
void loop() {
    if (read_steering_timer.check()) {

      steering.read_steering();
      Serial5.write(0x33);
      if (Serial5.available()) {
        char position_high = Serial5.read();
        char position_low_and_error_warning = Serial.read();
        uint16_t encoder_position = position_high << 6 | position_low_and_error_warning >> 2;
        Serial.println(encoder_position);
//    Serial.println(Serial5.read());
  }
      
//    //Serial.println();
//    steering.read_steering();
  }
  

  
  // put your main code here, to run repeatedly:

}
