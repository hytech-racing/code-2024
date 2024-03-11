#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "Metro.h"
#include <string.h>

#include <Wire.h> //Needed for I2C to GNSS
#include <SparkFun_u-blox_GNSS_v3.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v3
#include <XBTools.h>

#define DEBUG true
typedef struct perf_counters {
  uint16_t recieved = 0;
  uint16_t positions = 0;
  uint16_t send_attempts = 0;
  uint16_t send_successes = 0;
} perf_counters;
perf_counters counters;
Metro perf_timer = Metro(1000);


/* 
 * Pin Definitions
 */
#define ESP_TX 34
#define ESP_RX 33
#define GPS_OK 8
#define GPS_RTK_OK 10
#define ESP_LED_OTHER_1 11
#define ESP_LED_OK 12
#define ESP_LED_OTHER_2 13

#define TCU Serial1

SFE_UBLOX_GNSS myGNSS;

uint8_t broadcastAddress[] = {0x7C, 0xDF, 0xA1, 0x55, 0xB5, 0xA2};

esp_now_peer_info_t peerInfo;

void esp_now_setup() {
  WiFi.mode(WIFI_STA);

  if (!WiFi.setTxPower(WIFI_POWER_19_5dBm)) {
    Serial.println("Error setting transmit power");
  }

  if (esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR) != ESP_OK) {
    Serial.println("Error initializing WIFI LR");
    while (1);
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while(1);
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);
}

void setup() {
  Serial.begin(115200);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  //TCU.begin(115200, SERIAL_8N1, ESP_RX, ESP_TX);
  //Teensy Serial8
  //TCU.begin(115200, SERIAL_8N1, 34, 33);
  //Teensy Serial8 with transmit basically disabled
  TCU.begin(115200, SERIAL_8N1, 34, 9);
  //Teensy Serial2 Not working rn?
  //TCU.begin(115200, SERIAL_8N1, 10, 9);

  TCU.setRxBufferSize(1024);

  pinMode(GPS_OK, OUTPUT);
  pinMode(GPS_RTK_OK, OUTPUT);
  pinMode(ESP_LED_OTHER_1, OUTPUT);
  pinMode(ESP_LED_OK, OUTPUT);
  pinMode(ESP_LED_OTHER_2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  esp_now_setup();
  //GPSSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  //GPSLoop();
  telemLoop();

  if (perf_timer.check()) {
    Serial.printf("Recieved messages: %u Positions: %u\n", counters.recieved, counters.positions);
    Serial.printf("Send attempts: %u Send successes: %u\n\n", counters.send_attempts, counters.send_successes);
    counters.recieved = 0;
    counters.positions = 0;
    counters.send_attempts = 0;
    counters.send_successes = 0;
  }
}
