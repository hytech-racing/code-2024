/*
 * Handles the input GPS data from the ESP32
 */

#include <XBTools.h>
#define ESP Serial8
#define ESP_BUF_LEN 256


typedef struct GPS_Data {
    int32_t latitude;
    int8_t latitudeHp;
    int32_t longitude;
    int8_t longitudeHp;
    int32_t ellipsoid;
    int8_t ellipsoidHp;
    int32_t msl;
    int8_t mslHp;
    uint32_t accuracy;
} GPS_Data;

static GPS_Data gps_data;

uint8_t decodedData[ESP_BUF_LEN];
uint8_t size = 0;
uint8_t espIn[ESP_BUF_LEN];
uint8_t tail = 1;

void setupESP() {

}

void readESP() {
    while (ESP.available())
    {
        uint8_t in = ESP.read();
        espIn[tail++] = in;
        if (in == 0) {
            decodeFromBuffer();
        }
    }
}

void decodeFromBuffer() {
    size_t result = cobs_decode(espIn, tail, decodedData);
    if (result) {
      size = tail;
    }
    memcpy(&gps_data, decodedData, sizeof(GPS_Data));
}

void setTime() {

}