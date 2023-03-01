
#include "Metro.h"
#include "drivers.h"
#include "SPI.h"
#include "MCU_rev12_dfs.h"
#define DRIVER DAVID
#include "driver_constants.h"
#define DRIVER DAVID
Metro timer_load_cells_read = Metro(500); //change to 10

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
  
}

int readADC(int CS, int chan) {
  digitalWrite(CS, LOW);
  SPI.beginTransaction(SPISettings(ADC_SPI_SPEED, MSBFIRST, SPI_MODE0));
  int hi = SPI.transfer( chan << 3);
  int lo = SPI.transfer( 0 );
  
  digitalWrite(CS, HIGH);
  SPI.endTransaction();
  
  return (hi << 8) | lo; 
}

void adc_sweep() {
  if(timer_load_cells_read.check()) {
      int FR = readADC(ADC2_CS, ADC_FR_LOAD_CELL_CHANNEL);
//      int FL = readADC(ADC2_CS, ADC_FL_LOAD_CELL_CHANNEL);
// int RR = readADC(ADC2_CS, ADC_RR_LOAD_CELL_CHANNEL);
 //     int RL = readADC(ADC1_CS, ADC_RL_LOAD_CELL_CHANNEL);
//
      Serial.print("FR_LOAD_CELL: ");
      Serial.println(FR);
//      Serial.print("FL_LOAD_CELL: ");
//      Serial.println(FL);
//      Serial.print("RR_LOAD_CELL: ");
////      Serial.println(RR);
//      Serial.print("RL_LOAD_CELL: ");
//      Serial.println(RL);
//      
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  adc_sweep();


}
