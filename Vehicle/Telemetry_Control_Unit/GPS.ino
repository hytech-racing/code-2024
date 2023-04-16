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
  while (ESP.available()) {
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
  gps_high_precision.set_gps_latitudeHp(gps_data.latitudeHp);
  gps_high_precision.set_gps_longitudeHp(gps_data.longitudeHp);
  gps_high_precision.set_gps_ellipsoidHp(gps_data.ellipsoidHp);
  gps_high_precision.set_gps_mslHp(gps_data.mslHp);
  gps_high_precision.set_gps_ellipsoid(gps_data.ellipsoid);
  gps_lat_long.set_gps_latitude(gps_data.latitude);
  gps_lat_long.set_gps_longitude(gps_data.longitude);
  gps_other.set_gps_accuracy(gps_data.accuracy);
  gps_other.set_gps_msl(gps_data.msl);
  uint64_t time = getTime();
  CAN_message_t rx_msg1;
  gps_high_precision.write(rx_msg1.buf);
  rx_msg1.len = sizeof(GPS_high_precision);
  rx_msg1.id = ID_GPS_HIGH_PRECISION;
  CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg1, .time = time });  //unclear what were passing
  CAN_message_t rx_msg2;
  gps_lat_long.write(rx_msg2.buf);
  rx_msg2.len = sizeof(GPS_lat_long);
  rx_msg2.id = ID_GPS_LAT_LONG;
  CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg2, .time = time });  //unclear what were passing
  CAN_message_t rx_msg3;
  gps_other.write(rx_msg3.buf);
  rx_msg3.len = sizeof(GPS_other);
  rx_msg3.id = ID_GPS_OTHER;
  CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg3, .time = time });  //unclear what were passing
  counters.GPS_freq++;
}

void setTime() {
}