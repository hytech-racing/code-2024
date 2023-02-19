#ifndef __STEERING_SPI_H__
#define __STEERING_SPI_H__

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_STEERING_CS 10
#define DEFAULT_SPI_SPEED 2000000

class STEERING_SPI {
	public:
		STEERING_SPI();
		STEERING_SPI(int CS);
        STEERING_SPI(int CS, unsigned int SPIspeed);
		void init(int CS, unsigned int SPIspeed);
		uint16_t read_steering(int channel);
	private:
		int STEERING_SPI_CS;
        unsigned int SPI_SPEED;
        int multi_turn;
		int encoder_position;
		int error;
		int warning;
};


#endif // !<STEERING_SPI.H>
#pragma once