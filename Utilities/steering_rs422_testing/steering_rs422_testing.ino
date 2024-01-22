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
  
  //Reset zero position to 0 in order to return unaltered reading from read_steering_continous()
  steering.set_zero_position(0);
  steering.calculate_zero_position();
  steering.calibrate_steering(steering.return_zero_position());
}

// Uncomment to save current baud rate, position offset, continuous-response settings (period, command, autostart enable command)
// steering.save_parameters();
void loop() {
  if (read_steering_timer.check()) {
    steering.read_steering();
  }
}
