#include <Arduino.h>
#include "STEERING_RS422.h"
#include "Metro.h"

STEERING_RS422 steering(5);
Metro read_steering_timer(20);
uint8_t valid = -1;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial5.begin(115200);
  steering.init(115200);
  //pinMode(13, OUTPUT);

  //Comment everything out below reset and reset the sensor. Then comment reset and uncomment everything in 
  //Block 1.  Then comment Block 1 and uncomment everything after Block 2.  This should be on three separate
  //loads to the steering sensor.
  //steering.reset_sensor();


  //Block 1
  //Reset zero position to 0 in order to return unaltered reading from read_steering_continous()
  //Uncomment next three lines to set wheel starting position to 0 on every power cycle, must put wheel as straight as possible
  //steering.set_zero_position(0);
  //steering.calculate_zero_position();
  //steering.calibrate_
  //steering(steering.return_zero_position());
  //steering.continuous_setup(32767, 0);
  //Uncomment to save current baud rate, position offset, continuous-response settings (period, command, autostart enable command)
  //steering.save_parameters();
  //steering.read
 
  
  //Block 2 
  //steering.continuous_start();
  Serial.println("get ready..");
  Serial.println();
  delay(500);
//
//  Serial.print("Calibrate offset position to: ");
//  steering.calibrate_steering(317);
//  Serial.println("Saving parameter...");
//  steering.save_parameters();
  
//  steering.self_calibration();
}

void loop() {
  if (read_steering_timer.check()) {
    //steering.read_steering_continuous();
    Serial.print("Check self-calibration status (idk why this is here tbh but): ");
    Serial.println(steering.check_calibration_status(),BIN);
    steering.read_steering();
    Serial.println();
    
  }
}
