#include "ADC_SPI.h"
#include <SPI.h>

/*
 * This library is used to communicate with
 * the TI ADC128 ADC
 * the Teensy/Arduino SPI library
 */

/*
 * Initialize ADC SPI using default CS pin
 */
ADC_SPI::ADC_SPI()
{
	init(DEFAULT_ADC_SPI_CS, DEFAULT_ADC_SPI_SPEED, DEFAULT_ADC_SPI_SDI, DEFAULT_ADC_SPI_SDO, DEFAULT_ADC_SPI_CLK);
}

/*
 * Initialize ADC SPI using custom CS pin
 * param CS Pin to use for Chip Select
 */
ADC_SPI::ADC_SPI(int CS)
{
	init(CS, DEFAULT_ADC_SPI_SPEED, DEFAULT_ADC_SPI_SDI, DEFAULT_ADC_SPI_SDO, DEFAULT_ADC_SPI_CLK);
}

ADC_SPI::ADC_SPI(int CS, unsigned int SPIspeed)
{
	init(CS, SPIspeed, DEFAULT_ADC_SPI_SDI, DEFAULT_ADC_SPI_SDO, DEFAULT_ADC_SPI_CLK);
}

ADC_SPI::ADC_SPI(int CS, unsigned int SPIspeed, int SDI, int SDO, int CLK)
{
	init(CS, SPIspeed, SDI, SDO, CLK);
}

/*
 * Initialization helper
 */
void ADC_SPI::init(int CS, unsigned int SPIspeed, int SDI, int SDO, int CLK)
{
	ADC_SPI_CS = CS;
	SPI_SPEED = SPIspeed;
	ADC_SPI_SDI = SDI;
	ADC_SPI_SDO = SDO;
	ADC_SPI_CLK = CLK;

	pinMode(ADC_SPI_CS, OUTPUT);
	pinMode(ADC_SPI_SDI, INPUT);
	pinMode(ADC_SPI_SDO, OUTPUT);
	pinMode(ADC_SPI_CLK, OUTPUT);
	digitalWrite(ADC_SPI_CS, HIGH);
	digitalWrite(ADC_SPI_CLK, HIGH);
	
	// SPI.begin();
}

/*
 * Measure an ADC channel
 * param channel ADC128 channel to read
 * return 0-5V measurement scaled to 0-4095
 */
uint16_t ADC_SPI::read_channel(int channel)
{
	// // Gain control of the SPI port
	// // and configure settings
	// SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));

	// // Take the SS pin low to select the chip:
	digitalWrite(ADC_SPI_CS, LOW);

   SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE3));

  SPI.transfer16(0);//start adc converting channel 0

	uint16_t value = SPI.transfer16(channel << 11);

	// Take the SS pin high to de-select the chip:
	

	// Release control of the SPI port
	SPI.endTransaction();
	digitalWrite(ADC_SPI_CS, HIGH);

	return value;
}

void ADC_SPI::read_all_channels(uint16_t *arr)

{
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
  digitalWrite(ADC_SPI_CS, LOW);
  cli();
  for (uint16_t i = 0; i < 8; i++) {
    digitalWrite(ADC_SPI_CLK, HIGH);
    arr[i] = 0;
    uint16_t shiftout = (6-i) << 11;
    for (int j = 0; j < 16; j++) {
      // read outputs on the falling edge
      digitalWrite(ADC_SPI_CLK, LOW);
      arr[i] |= digitalRead(ADC_SPI_SDI) << (15 - j);
      delayMicroseconds(1);
      // write channel on the rising edge
      digitalWrite(ADC_SPI_SDO, shiftout & (0b1000000000000000 >> j) ? LOW : HIGH);
      digitalWrite(ADC_SPI_CLK, HIGH);
      delayMicroseconds(1);
    }
  }
  digitalWrite(ADC_SPI_CS, HIGH);
  sei();
  SPI.endTransaction();
}