#ifndef __STEERING_SPI_H__
#define __STEERING_SPI_H__

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_STEERING_CS 10
#define DEFAULT_STEERING_SPI_SPEED 2000000
#define MAX_POSITION 16384

class STEERING_SPI {
	public:
		STEERING_SPI();
		STEERING_SPI(uint8_t CS);
        STEERING_SPI(uint8_t CS, uint32_t SPIspeed);
		void init(uint8_t CS, uint32_t SPIspeed);
		uint16_t read_steering();
		inline void set_zero_position(uint16_t position) { this->zero_position = position; }
		uint16_t get_encoder_position() const { return encoder_position; }
	private:
		uint8_t STEERING_SPI_CS;
      uint8_t SPI_SPEED;
      uint16_t multi_turn;
		uint16_t encoder_position;
		bool error;
		bool warning;
		uint16_t zero_position;
		int16_t steering_position;
};


#endif 
#pragma once