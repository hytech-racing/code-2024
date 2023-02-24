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
uint16_t STEERING_SPI::read_steering() {
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
    //zero_position returns 0 all the time
	/*if ((zero_position - encoder_position) <= (MAX_POSITION/2)) { // if steering wheel is to the left of center
		steering_position = (encoder_position - zero_position);
	} else {  //steering wheel is left of center
		steering_position = MAX_POSITION + encoder_position - zero_position;
	} */// 0-8192     could work with -4096
    /*steering_position = ((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION) - 4096;
    //Pretty sure this is right, but negatives get screwed up cuz lmao (maybe the masking?)
	return steering_position & 0x3FFF;*/


     /*Try this one next time, the idea is to bitmask first so we don't screw up the sign at the end, but when everyone
     is positive.*/
     steering_position = ((MAX_POSITION + ((encoder_position & 0x3FFF) - (zero_position & 0x3FFF))) % MAX_POSITION) - (MAX_POSITION / 2);
     return steering_position;

}
