#ifndef __ADC_SPI_H__
#define __ADC_SPI_H__

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_ADC_SPI_CS 10
#define DEFAULT_ADC_SPI_SPEED 2000000
#define DEFAULT_ADC_SPI_SDI 12
#define DEFAULT_ADC_SPI_SDO 11
#define DEFAULT_ADC_SPI_CLK 13

class ADC_SPI {
	public:
		ADC_SPI();
		ADC_SPI(int CS);
        ADC_SPI(int CS, unsigned int SPIspeed);
		ADC_SPI(int CS, unsigned int SPIspeed, int SDI, int SDO, int CLK);
		void init(int CS, unsigned int SPIspeed, int SDI, int SDO, int CLK);
		uint16_t read_channel(int channel);
		void read_all_channels(uint16_t* array);
	private:
		int ADC_SPI_CS;
		int ADC_SPI_SDI;
		int ADC_SPI_SDO;
		int ADC_SPI_CLK;
        unsigned int SPI_SPEED;
};


#endif // !<ADC_SPI.H>
#pragma once
