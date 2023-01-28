#pragma once

#include <cstdint>
#include <cstddef>

class ADC_SPI {
public:
    constexpr static uint8_t DEFAULT_SPI_CS = 10;
    constexpr static uint32_t DEFAULT_SPI_SPEED = 1000000;

    ADC_SPI();
    explicit ADC_SPI(uint8_t CS = DEFAULT_SPI_CS, uint32_t SPIspeed = DEFAULT_SPI_SPEED);
    uint16_t read_adc(uint8_t channel) const;
private:
    uint8_t ADC_SPI_CS;
    uint32_t SPI_SPEED;
};