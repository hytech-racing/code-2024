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
	init(DEFAULT_ADC_SPI_CS, DEFAULT_ADC_SPI_SPEED);
}

/*
 * Initialize ADC SPI using custom CS pin
 * param CS Pin to use for Chip Select
 */
ADC_SPI::ADC_SPI(int CS)
{
	init(CS, DEFAULT_ADC_SPI_SPEED);
}

ADC_SPI::ADC_SPI(int CS, unsigned int SPIspeed)
{
	init(CS, SPIspeed);
}

/*
 * Initialization helper
 */
void ADC_SPI::init(int CS, unsigned int SPIspeed)
{
	ADC_SPI_CS = CS;
	SPI_SPEED = SPIspeed;

	pinMode(ADC_SPI_CS, OUTPUT);
	digitalWrite(ADC_SPI_CS, HIGH);

	// Initialize SPI:
	SPI.begin();
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

	// Set up channel
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	SPI.transfer(channel << 3);
	SPI.transfer(0);

	uint8_t hi = SPI.transfer(0);
	uint8_t lo = SPI.transfer(0);

	// Take the SS pin high to de-select the chip:
	digitalWrite(ADC_SPI_CS, HIGH);

	// Release control of the SPI port
	SPI.endTransaction();

	return (hi << 8) | lo;
}

void ADC_SPI::read_all_channels(uint16_t *array)
{
	digitalWrite(ADC_SPI_CS, LOW);
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	uint8_t hi;
	uint8_t lo;
	SPI.transfer(0);//start adc converting channel 0
	SPI.transfer(0);	
	for (int i = 0; i < 7; i++)
	{
		hi = SPI.transfer(((i+1)) << 3);
		lo = SPI.transfer(0);
		array[i] = hi << 8 | lo;
	}
	hi =  SPI.transfer(0);
	lo = SPI.transfer(0);
	array[7] = hi << 8 | lo;
	SPI.endTransaction();
	digitalWrite(ADC_SPI_CS, HIGH);
}