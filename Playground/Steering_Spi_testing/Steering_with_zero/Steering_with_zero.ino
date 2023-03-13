#include "STEERING_SPI.h"
#include "STEERING_SPI.cpp"
STEERING_SPI steering(10, 2000000);
void setup() {
  pinMode(33, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(34, OUTPUT);
  pinMode(38, OUTPUT);
  steering.set_zero_position(steering.get_encoder_position());
  digitalWrite(33, HIGH);
  digitalWrite(29, HIGH);
  digitalWrite(34, HIGH);
  digitalWrite(38, HIGH);
}

void loop() {
  Serial.println("Encoder Value: " + steering.get_encoder_position());
  Serial.println("Real Steering Value: " + steering.read_steering());
  Serial.println();
}
