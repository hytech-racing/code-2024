#include "STEERING_SPI.h"
#include <SPI.h>

/*
 * Initialize STEERING SPI using default CS pin
 */
STEERING_SPI::STEERING_SPI() {
	init(DEFAULT_STEERING_CS, DEFAULT_STEERING_SPI_SPEED);
}

/*
 * Initialize STEERING SPI using custom CS pin
 * param CS Pin to use for Chip Select
 */
STEERING_SPI::STEERING_SPI(uint8_t CS) {
	init(CS, DEFAULT_STEERING_SPI_SPEED);
}

STEERING_SPI::STEERING_SPI(uint8_t CS, uint32_t SPIspeed) {
	init(CS, SPIspeed);
}

/*
 * Initialization helper
 */
void STEERING_SPI::init(uint8_t CS, uint32_t SPIspeed) {
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
int16_t STEERING_SPI::read_steering() {
	digitalWrite(STEERING_SPI_CS, LOW);
	delayMicroseconds(50);
	  
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));

	char encoder_pos_hi = SPI.transfer(0);
	char encoder_pos_low_and_status = SPI.transfer(0);
	char crc = SPI.transfer(0);

	SPI.endTransaction();
	digitalWrite(STEERING_SPI_CS, HIGH);

	encoder_position = (encoder_pos_hi << 6) + (encoder_pos_low_and_status >> 2);
	error = (encoder_pos_low_and_status & 2) >> 1;
	warning = encoder_pos_low_and_status & 1;
    //steering increases in value in CCW direction
     if ((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION <= (MAX_POSITION / 2)) {
         steering_position = -((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION);
     } else {
         steering_position = MAX_POSITION - ((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION);
     }
     return steering_position;

}
