#include <Arduino.h>
#include "RideHeightRS485.h"


RideHeightRS485 ride_height_sensor(115200);
void setup() {

  Serial.begin(9600);

  ride_height_sensor.begin();

}

void loop() {

  float dist = ride_height_sensor.readDistance();

  Serial.print("Distance = ");
  Serial.print(dist, 1);
  Serial.println("mm");
  
}

