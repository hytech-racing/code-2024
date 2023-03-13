#include "ADC_SPI.h"
#include <SPI.h>

/*
 * This library is used to communicate with
 * the Microchip MCP3208 12-bit ADC using
 * the Teensy/Arduino SPI library
 */

/*
 * Initialize ADC SPI
 */
ADC_SPI::ADC_SPI(uint8_t CS, uint32_t SPIspeed) : ADC_SPI_CS(CS), SPI_SPEED(SPIspeed) {
    pinMode(ADC_SPI_CS, OUTPUT);
    pinMode(ADC_SPI_CS, HIGH);

    // Initialize SPI:
    SPI.begin();
}

/*
 * Measure an ADC channel
 * param channel MCP3208 channel to read
 * return 0-5V measurement scaled to 0-4095
 */
uint16_t ADC_SPI::read_adc(uint8_t channel) const {
    // Gain control of the SPI port
    // and configure settings
    SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));

    // Take the SS pin low to select the chip:
    digitalWrite(ADC_SPI_CS, LOW);

    // Set up channel
    uint8_t b = 0x60; // 01100000
    b |= ((channel << 2));

    // Send in the channel via SPI:
    SPI.transfer(b);

    // Read data from SPI
    uint8_t result1 = SPI.transfer(0);
    uint8_t result2 = SPI.transfer(0);

    // Take the SS pin high to de-select the chip:
    digitalWrite(ADC_SPI_CS, HIGH);

    // Release control of the SPI port
    SPI.endTransaction();

    // TODO: check return type?
    return (result1 << 4) | (result2 >> 4);
}