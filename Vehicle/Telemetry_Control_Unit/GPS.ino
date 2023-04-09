/*
 * Handles the input GPS data from the ESP32
 */

#include <XBTools.h>
#define ESP Serial8
#define ESP_BUF_LEN 512


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

GPS_high_precision gps_high_precision;
GPS_lat_long gps_lat_long;
GPS_other gps_other;

uint8_t decodedData[ESP_BUF_LEN];
uint8_t size = 0;
uint8_t espIn[ESP_BUF_LEN];
uint8_t tail = 0;

void setupESP() {

}

int readESP() {
    while (ESP.available())
    {
        uint8_t in = ESP.read();
        espIn[tail++] = in;
        if (in == 0) {
            decodeFromBuffer();
            tail = 0;
            processGPS();
            return 1;
        }
    }
    return 0;
}

void decodeFromBuffer() {
    size_t result = cobs_decode(espIn, tail, decodedData);
    if (result) {
      size = tail;
    }
    memcpy(&gps_data, decodedData, sizeof(GPS_Data));
}

void processGPS() {
    Serial.printf("Recieved GPS\n");
    gps_high_precision.set_gps_latitudeHp(gps_data.latitudeHp);
    gps_high_precision.set_gps_longitudeHp(gps_data.longitudeHp);
    gps_high_precision.set_gps_ellipsoidHp(gps_data.ellipsoidHp);
    gps_high_precision.set_gps_mslHp(gps_data.mslHp);
    gps_high_precision.set_gps_ellipsoid(gps_data.ellipsoid);
    gps_lat_long.set_gps_latitude(gps_data.latitude);
    gps_lat_long.set_gps_longitude(gps_data.longitude);
    gps_other.set_gps_accuracy(gps_data.accuracy);
    gps_other.set_gps_msl(gps_data.msl);
    CAN_message_t msg;
    gps_high_precision.write(msg.buf);
    msg.len = sizeof(GPS_high_precision);
    msg.id = ID_GPS_HIGH_PRECISION;
    write_to_SD(&msg);
    gps_lat_long.write(msg.buf);
    msg.len = sizeof(GPS_lat_long);
    msg.id = ID_GPS_LAT_LONG;
    write_to_SD(&msg);
    gps_other.write(msg.buf);
    msg.len = sizeof(GPS_other);
    msg.id = ID_GPS_OTHER;
    write_to_SD(&msg);
}

void setTime() {

}