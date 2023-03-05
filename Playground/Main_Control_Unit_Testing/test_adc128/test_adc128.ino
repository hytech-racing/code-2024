
#include "Metro.h"
#include "drivers.h"
#include "SPI.h"
#include "MCU_rev12_dfs.h"
#include "ADC_SPI.h"


#define DRIVER DAVID
#include "driver_constants.h"
#define DRIVER DAVID
Metro timer_load_cells_read = Metro(10); //change to 10
int out = 0;

ADC_SPI ADC1(ADC1_CS, ADC_SPI_SPEED);
ADC_SPI ADC2(ADC2_CS, ADC_SPI_SPEED);
ADC_SPI ADC3(ADC3_CS, ADC_SPI_SPEED);

uint16_t adc1_vals[8];
uint16_t adc2_vals[8];
uint16_t adc3_vals[8];

void setup() {
  // put your setup code here, to run once:
  pinMode(33, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(34, OUTPUT);
  digitalWrite(33, HIGH);
  digitalWrite(29, HIGH);
  digitalWrite(38, HIGH);
  digitalWrite(34, HIGH);
  SPI.begin();
  Serial.begin(115200);
}

void loop() {
   if(timer_load_cells_read.check()) {
//    ADC1.read_all_channels(adc1_vals);
//    ADC2.read_all_channels(adc2_vals);
//    ADC3.read_all_channels(adc3_vals);
//    for (int i = 0; i < 8; i++) {
//      Serial.print("ADC1 Channel ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(adc1_vals[i]);
//      Serial.print(",");
//    }
//    for (int i = 0; i < 8; i++) {
//      Serial.print("ADC2 Channel ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(adc2_vals[i]);
//      Serial.print(",");
//    }
//    for (int i = 0; i < 8; i++) {
//      Serial.print("ADC3 Channel ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(adc3_vals[i]);
//      Serial.print(",");
//    }
//    Serial.println("End: 0");
      Serial.println(ADC1.read_channel(7));
  }
}
