#include <SPI.h>
#include "Mcp320x.h"
#include "Metro.h"


#define SPI_CS    	10 		   // SPI slave select
#define ADC_VREF    3300     // 3.3V Vref
#define ADC_CLK     100000  // SPI clock 1.6MHz

MCP3204 adc(ADC_VREF, SPI_CS);
Metro sample(20);
SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);

void setup() {
  // configure PIN mode
  pinMode(SPI_CS, OUTPUT);
  
  // set initial PIN state
  digitalWrite(SPI_CS, HIGH);

  // initialize serial
  Serial.begin(115200);

  // initialize SPI interface for MCP3208
 
  SPI.begin();
  SPI.beginTransaction(settings);
  
}

void loop() {
//   uint32_t t1;
//   uint32_t t2;

  // start sampling
   

   
   
//   uint16_t raw = adc.read(MCP3204::Channel::SINGLE_2);
   // SPI.endTransaction();
    //Serial.println(raw);
    //delay(100);
  
//
//   // get analog value
//   uint16_t val = adc.toAnalog(raw);

//  // Get and print the timestamp in milliseconds
//  unsigned long currentMillis = millis();
//  Serial.print("  Timestamp: ");
//  Serial.print(currentMillis);
if (sample.check()) {
  uint16_t raw = adc.read(MCP3204::Channel::SINGLE_2);
  uint16_t val = adc.toAnalog(raw);
  Serial.printf("\tTimestamp: %lu : value: %d mV\n", millis(), val);
}
   // readed value
//   Serial.print("value: ");
//   Serial.print(raw);
//   Serial.print(" (");
//   Serial.print(val);
//   Serial.println(" mV)");
//
//   // sampling time
//   Serial.print("Sampling time: ");
//   Serial.print(static_cast<double>(t2 - t1) / 1000, 4);
//   Serial.println("ms");
//
//   delay(1000);

  // SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);

  // SPI.beginTransaction(settings);

  // // activate ADC with chip select
  // digitalWrite(SPI_CS, LOW);

  // delayMicroseconds(1);

  // // digitalWrite(SPI_CS, HIGH);

  // delayMicroseconds(16);

  // digitalWrite(SPI_CS, LOW);

  // // // send first command byte
  // // mSpi->transfer(cmd.hiByte);
  // // // send second command byte and receive first(msb) 4 bits
  // // adc.hiByte = mSpi->transfer(cmd.loByte) & 0x0F;
  // // // receive last(lsb) 8 bits
  // // adc.loByte = mSpi->transfer(0x00);

  // delayMicroseconds(1);

  // // deactivate ADC with slave select
  // digitalWrite(SPI_CS, HIGH);

  // delayMicroseconds(1);

  // SPI.endTransaction();

  // Serial.println("I've uploaded code");

  //delay(1000);

}
