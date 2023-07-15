#ifndef __STEERING_SPI_H__
#define __STEERING_SPI_H__

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_STEERING_CS 10
#define DEFAULT_STEERING_SPI_SDI 12
#define DEFAULT_STEERING_SPI_SDO 11
#define DEFAULT_STEERING_SPI_CLK 13
#define DEFAULT_STEERING_SPI_SPEED 500000
#define MAX_POSITION 8192    // Why?

class STEERING_SPI {
public:
	STEERING_SPI();
	STEERING_SPI(uint8_t CS);
	STEERING_SPI(uint8_t CS, uint32_t SPIspeed);
	void init(uint8_t CS, uint32_t SPIspeed);
	int16_t read_steering();
	inline void set_zero_position(int16_t position) { this->zero_position = position; }
	int16_t get_zero_position() const { return zero_position; }
	int16_t get_encoder_position() const { return encoder_position; }
private:
	int STEERING_SPI_CS;
	//int STEERING_SPI_SDI;
	//int STEERING_SPI_SDO;
	//int STEERING_SPI_CLK;
	unsigned int SPI_SPEED;
	//uint8_t STEERING_SPI_CS;
	//uint8_t SPI_SPEED;
	int16_t encoder_position;
	bool error;
	bool warning;
	int16_t zero_position;
	int16_t steering_position;
};


#endif 
#pragma once