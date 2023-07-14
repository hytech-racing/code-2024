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
	SPI_SPEED = SPIspeed;
	//STEERING_SPI_SDI = DEFAULT_STEERING_SPI_SDI;
	//STEERING_SPI_SDO = DEFAULT_STEERING_SPI_SDO;
	//STEERING_SPI_CLK = DEFAULT_STEERING_SPI_CLK;

	pinMode(STEERING_SPI_CS, OUTPUT);
	digitalWrite(STEERING_SPI_CS, HIGH);

	//pinMode(STEERING_SPI_CS, OUTPUT);
	//pinMode(STEERING_SPI_SDI, INPUT);
	//pinMode(STEERING_SPI_SDO, OUTPUT);
	//pinMode(STEERING_SPI_CLK, OUTPUT);
	//digitalWrite(STEERING_SPI_CS, HIGH);
	//digitalWrite(STEERING_SPI_CLK, LOW);    // Not sure why, corresponds to SPI_MODE0?
	//digitalWrite(STEERING_SPI_SDO, LOW);

	// Initialize SPI:
	SPI.begin();
}



/*
 * Measure steering
 */
int16_t STEERING_SPI::read_steering() {
	uint16_t encoder_pos_hi;    // Data type reconsider
	uint16_t encoder_pos_low_and_status;
	uint8_t crc;
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); 
	digitalWrite(STEERING_SPI_CS, LOW);
	   // SPI_MODE0 also works
	delayMicroseconds(8);    // ts: time after NCS low to first SCK rise edge (?)
	cli(); 
	
	Serial.println("First transfer");
	encoder_pos_hi = SPI.transfer(0) << 8;
	Serial.println("Second transfer");
	encoder_pos_hi |= SPI.transfer(0);
	encoder_pos_low_and_status = SPI.transfer(0) << 8;
	encoder_pos_low_and_status |= SPI.transfer(0);
	crc = SPI.transfer(0);



	

	digitalWrite(STEERING_SPI_CS, HIGH);
	sei();    // Enable interrupt flags, allow ISR again
	delayMicroseconds(40);
	SPI.endTransaction();
	//Serial.println("SPI transaction ended");
	/*digitalWrite(STEERING_SPI_CS, HIGH);
	Serial.println("Chip select set HIGH");*/

	encoder_position = (encoder_pos_hi << 6) + (encoder_pos_low_and_status >> 2);    // Bitwise OR instead?
	error = (encoder_pos_low_and_status & 2) >> 1;
	warning = encoder_pos_low_and_status & 1;

	steering_position = -((encoder_position - zero_position) % MAX_POSITION);

	return steering_position;

}
