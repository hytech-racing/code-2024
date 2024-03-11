////
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
    ADC1.read_all_channels(adc1_vals);
//    ADC2.read_all_channels(adc2_vals);
//    ADC3.read_all_channels(adc3_vals);
    for (int i = 0; i < 8; i++) {
      Serial.print("ADC1 Channel ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(adc1_vals[i]);
      Serial.print(", ");
    }
////    Serial.println();
//    for (int i = 0; i < 8; i++) {
//      Serial.print("ADC2 Channel ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(adc2_vals[i]);
//      Serial.print(", ");
//    }
    Serial.println();
//    for (int i = 0; i < 8; i++) {
//      Serial.print("ADC3 Channel ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(adc3_vals[i]);
//      Serial.print(", ");
//    }
//    for (int i = 3; i < 6; i++) {
//      Serial.print("ADC2 Channel ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(adc2_vals[i]);
//      Serial.print(", ");
//    }
//    Serial.println("End: 0");
//      Serial.println(ADC1.read_channel(7));
//ADC2.read_channel(3);
      Serial.println(ADC1.read_channel(7));
  }
}
//
//#include "Metro.h"
//#include "drivers.h"
//#include "SPI.h"
//#include "MCU_rev12_dfs.h"
//#define DRIVER DAVID
//#include "driver_constants.h"
//#define DRIVER DAVID
//Metro timer_load_cells_read = Metro(500); //change to 10
//int out = 0;
//
//int adc1_values[8];
//
//void setup() {
//  // put your setup code here, to run once:
//  pinMode(33, OUTPUT);
//  pinMode(29, OUTPUT);
//  pinMode(38, OUTPUT);
//  pinMode(34, OUTPUT);
//  digitalWrite(33, HIGH);
//  digitalWrite(29, HIGH);
//  digitalWrite(38, HIGH);
//  digitalWrite(34, HIGH);
//  SPI.begin();
//  
//}
//
//int readADC(int CS, int chan) {
//  digitalWrite(CS, LOW);
//  SPI.beginTransaction(SPISettings(ADC_SPI_SPEED, MSBFIRST, SPI_MODE0));
//  int hi = SPI.transfer( chan << 3);
//  int lo = SPI.transfer( 0 );
//  
//  digitalWrite(CS, HIGH);
//  SPI.endTransaction();
//  
//  return (hi << 8) | lo; 
//}
//
//void read_ADC_channels(int CS, int* data_store) {
//  digitalWrite(CS, LOW);
//  SPI.beginTransaction(SPISettings(3200000, MSBFIRST, SPI_MODE0));
// 
//  for (int i = 0; i < 8; i++) {
//    int hi = SPI.transfer(((i + 1)%8) << 3) & 0b1111;
//    int lo = SPI.transfer( 0 );
//    data_store[i] = hi << 8 | lo;
//  }
//
//  SPI.endTransaction();
//  digitalWrite(CS, LOW);
//}
//
//void loop() {
//   if(timer_load_cells_read.check()) {
////    int RR = readADC(ADC2_CS, ADC_RR_LOAD_CELL_CHANNEL);
////    int FL = readADC(ADC2_CS, ADC_FL_LOAD_CELL_CHANNEL);
//
////      Serial.print("FL_LOAD_CELL: ");
////      Serial.println(FL);
////       Serial.print("FR_LOAD_CELL: ");
////      Serial.println(FR);
////      Serial.print("RR_LOAD_CELL: ");
////      Serial.println(RR);
////      Serial.print("RL_LOAD_CELL: ");
////      Serial.println(RL);
////      
//
//    read_ADC_channels(ADC2_CS, adc1_values);
//    for (int i = 0; i < 8; i++) {
//      Serial.print("Channel ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(adc1_values[i]);
//      Serial.print(",");
//    }
//    Serial.println("End: 0");
//  }
//}
