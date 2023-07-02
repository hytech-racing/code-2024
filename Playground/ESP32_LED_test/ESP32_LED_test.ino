#include <Adafruit_NeoPixel.h>

#include <Wire.h>

#define LEDpin 18
Adafruit_NeoPixel pixels(1, LEDpin, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(LEDpin,OUTPUT);
  digitalWrite(LEDpin,HIGH);
  delay(5000);
  digitalWrite(LEDpin,LOW);

}

void loop() {
  // put your main code here, to run repeatedly:

}
