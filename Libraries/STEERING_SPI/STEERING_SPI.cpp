#include "STEERING_SPI.h"
#include <SPI.h>

/*
 * Initialize STEERING SPI using default CS pin
 */
STEERING_SPI::STEERING_SPI() {
	init(DEFAULT_STEERING_CS, DEFAULT_SPI_SPEED);
}

/*
 * Initialize STEERING SPI using custom CS pin
 * param CS Pin to use for Chip Select
 */
STEERING_SPI::STEERING_SPI(int CS) {
	init(CS, DEFAULT_SPI_SPEED);
}

STEERING_SPI::STEERING_SPI(int CS, unsigned int SPIspeed) {
	init(CS, SPIspeed);
}

/*
 * Initialization helper
 */
void STEERING_SPI::init(int CS, unsigned int SPIspeed) {
	STEERING_SPI_CS = CS;
	SPI_SPEED  = SPIspeed;

	pinMode(STEERING_SPI_CS, OUTPUT);
	digitalWrite(STEERING_SPI_CS, HIGH);

	// Initialize SPI:
	SPI.begin();
}

/*
 * Measure steering
 */
uint16_t STEERING_SPI::read_steering(int channel) {
	digitalWrite(STEERING_SPI_CS, LOW);
	delayMicroseconds(50);
	  
	SPI.beginTransaction(SPISettings(DEFAULT_SPI_SPEED, MSBFIRST, SPI_MODE0));

	char multi_turn_hi = SPI.transfer(0);
	char multi_turn_lo = SPI.transfer(0);
	char status_hi = SPI.transfer(0);
	char status_lo = SPI.transfer(0);
	char crc = SPI.transfer(0);

	SPI.endTransaction();
	digitalWrite(STEERING_SPI_CS, HIGH);

	multi_turn = (multi_turn_hi << 8) + multi_turn_lo;
	encoder_position = (status_lo >> 2) + (status_hi << 6);
	error = (status_lo & 2) >> 1;
	warning = status_lo & 1;

	return encoder_position;
}