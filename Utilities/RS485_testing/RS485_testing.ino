#include <Arduino.h>/*
  RS-485 Passthrough

  This sketch relays data sent and received between the Serial port and the RS-485 interface

  Circuit:
   - MKR board
   - MKR 485 Shield
     - ISO GND connected to GND of the RS-485 device
     - Y connected to A of the RS-485 device
     - Z connected to B of the RS-485 device
     - A connected to Y of the RS-485 device
     - B connected to Z of the RS-485 device
     - Jumper positions
       - FULL set to ON
       - Z \/\/ Y set to ON, if the RS-485 device doesn't provide termination
       - B \/\/ A set to ON, if the RS-485 device doesn't provide termination

  created 4 July 2018
  by Sandeep Mistry
*/


#include <ArduinoRS485.h>
#include "Metro.h"
#define SEND
void sendRS();
void readRS();


#ifdef SEND
  void (*func)(void)= sendRS;
#else
  void (*func)(void) = readRS;
#endif
  Metro send_or_receive(100);
void setup() {
  Serial.begin(9600);
  RS485.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  // enable transmission, can be disabled with: RS485.endTransmission();
  #ifdef SEND
    RS485.beginTransmission();
    RS485.noReceive();
  #else
    RS485.endTransmission();
    RS485.receive();
  #endif

}
bool toggle = 0;
void loop() {
  func();
  


////read code
//  if (RS485.available()) {
//    Serial.write(RS485.read());
//  }
}

void sendRS() {
  if (send_or_receive.check()) {
    RS485.noReceive();
    RS485.beginTransmission();
    RS485.write("lolxd");
    RS485.endTransmission();
    //digitalWrite(13, toggle);
    //toggle = !toggle;
  }
  RS485.receive();
  if (RS485.available()) {
  Serial.println(RS485.read());
  digitalWrite(13, toggle);
  toggle = !toggle;
  }
}
void readRS() {
   RS485.receive();
   if (RS485.available()) {
    Serial.println(RS485.read());
    digitalWrite(13, toggle);
    toggle = !toggle;
  }
}
