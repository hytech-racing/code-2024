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
	STEERING_SPI_SDI = DEFAULT_STEERING_SPI_SDI;
	STEERING_SPI_SDO = DEFAULT_STEERING_SPI_SDO;
	STEERING_SPI_CLK = DEFAULT_STEERING_SPI_CLK;

	pinMode(STEERING_SPI_CS, OUTPUT);
	digitalWrite(STEERING_SPI_CS, HIGH);

	pinMode(STEERING_SPI_CS, OUTPUT);
	pinMode(STEERING_SPI_SDI, INPUT);
	pinMode(STEERING_SPI_SDO, OUTPUT);
	pinMode(STEERING_SPI_CLK, OUTPUT);
	digitalWrite(STEERING_SPI_CS, HIGH);
	digitalWrite(STEERING_SPI_CLK, LOW);    // Not sure why, corresponds to SPI_MODE0?
	digitalWrite(STEERING_SPI_SDO, LOW);

	// Initialize SPI:
	//SPI.begin();
}



/*
 * Measure steering
 */
int16_t STEERING_SPI::read_steering() {
	//Serial.println("Enter library:");
	//digitalWrite(STEERING_SPI_CS, LOW);
	//Serial.println("Chip select set LOW");
	//delayMicroseconds(50);
	//Serial.println("Delayed 50 us");

	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
	//Serial.println("SPI transaction began");
	digitalWrite(STEERING_SPI_CS, LOW);
	//Serial.println("Chip select set LOW");
	//delayMicroseconds(5);    // ts: time after NCS low to first SCK rise edge (?)

	// Possibly need some delay

	cli();

	//uint8_t encoder_pos_hi;    // Data type reconsider
	//uint8_t encoder_pos_low_and_status;
	//uint8_t crc;
	//uint8_t arr[3];

	int16_t multi_corner_count;
	int16_t encoder_pos_hi;    // Data type reconsider
	int16_t encoder_pos_low_and_status;
	int16_t crc;
	int16_t arr[4];

	for (uint8_t i = 0; i < 4; i++) {
		digitalWrite(STEERING_SPI_CLK, LOW);
		arr[i] = 0;
		//uint8_t shiftout = (6 - i) << 11;
		for (int j = 0; j < 8; j++) {
			// read outputs on the rising edge
			digitalWrite(STEERING_SPI_CLK, HIGH);
			arr[i] |= digitalRead(STEERING_SPI_SDI) << (7 - j);
			delayMicroseconds(1);    // Possibly wrong
			// write channel on the rising edge
			//digitalWrite(ADC_SPI_SDO, shiftout & (0b1000000000000000 >> j) ? LOW : HIGH);
			digitalWrite(STEERING_SPI_SDO, LOW);
			digitalWrite(STEERING_SPI_CLK, LOW);
			delayMicroseconds(1);    // Possibly wrong
		}
	}


	multi_corner_count = arr[0];
	//char encoder_pos_hi = SPI.transfer(0);
	encoder_pos_hi = arr[1];
	//Serial.print("encoder_pos_hi: ");
	//Serial.println(encoder_pos_hi);
	//char encoder_pos_low_and_status = SPI.transfer(0);
	encoder_pos_low_and_status = arr[2];
	//Serial.print("encoder_pos_low_and_status: ");
	//Serial.println(encoder_pos_low_and_status);
	//char crc = SPI.transfer(0);
	crc = arr[3];
	//Serial.print("crc: ");
	//Serial.println(crc);

	// Possibly need some delay
	//delayMicroseconds(40);    // tp: pause time (?)

	digitalWrite(STEERING_SPI_CS, HIGH);
	//Serial.println("Chip select set HIGH");

	sei();

	SPI.endTransaction();
	//Serial.println("SPI transaction ended");
	/*digitalWrite(STEERING_SPI_CS, HIGH);
	Serial.println("Chip select set HIGH");*/

	encoder_position = (encoder_pos_hi << 6) + (encoder_pos_low_and_status >> 2);    // Bitwise OR instead?
	error = (encoder_pos_low_and_status & 2) >> 1;
	warning = encoder_pos_low_and_status & 1;
	//steering increases in value in CCW direction
	if ((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION <= (MAX_POSITION / 2)) {
		steering_position = -((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION);
	}
	else {
		steering_position = MAX_POSITION - ((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION);
	}
	return steering_position;

}
