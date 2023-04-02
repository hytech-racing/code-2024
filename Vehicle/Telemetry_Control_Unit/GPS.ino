#include <XBTools.h>
#define ESP Serial8
#define ESP_BUF_LEN 512



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
}