#include <stdint.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <time.h>
#include <deque>
#include "STEERING_SPI.h"
#include "Metro.h"
#include <HyTech_CAN.h>
#include "MCU_rev12_dfs.h"

MCU_analog_readings mcu_analog_readings{};

// Timers
Metro timer_steering_spi_read = Metro(1000);
Metro timer_debug = Metro(200);

STEERING_SPI STEERING(STEERING_CS, STEERING_SPI_SPEED);

void setup() {
  Serial.begin(9600);

  Serial.println("BRD Steering Sensor Simple Test");
  
  STEERING.set_zero_position(11800);    // Test steering sensor

}

void loop() {
  Serial.println("Print something");
  read_steering_spi_values();  // Test steering sensor 2, a.k.a. steering 1 in ECU code
//
//  if (timer_debug.check()) {
//    Serial.println("STEERINGS");
//    Serial.print("Encoder zero position: ");
//    Serial.println(STEERING.get_zero_position());    
//    Serial.println(mcu_analog_readings.get_steering_1());
//    Serial.print("Encoder position: ");
//    Serial.println(STEERING.get_encoder_position());
//  }

}

void read_steering_spi_values() {
  Serial.println("Start reading");
  if (timer_steering_spi_read.check()) {
    Serial.println("Start steering reading:");
    Serial.println(STEERING_CS);
    mcu_analog_readings.set_steering_1(STEERING.read_steering());
  }
}
