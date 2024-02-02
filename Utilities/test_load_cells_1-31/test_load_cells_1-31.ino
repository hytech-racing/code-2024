#include <SPI.h>
#include "Mcp320x.h"
#include "Metro.h"


#define SPI_CS      10       // SPI slave select
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

if (sample.check()) {
  uint16_t raw = adc.read(MCP3204::Channel::SINGLE_1);
  uint16_t val = adc.toAnalog(raw);
  Serial.printf("\tTimestamp: %lu : value: %d mV\n", millis(), val);
}

}
