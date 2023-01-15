#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "Metro.h"
#include <string.h>

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

#define TCU Serial2


typedef struct rtcm_message {
  uint8_t placeholder;
} rtcm_message;

rtcm_message incomingRTCM;

uint8_t incomingFrame[256];
int frameIndex = 0;

int zeroes = 0;

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

esp_now_peer_info_t peerInfo;

/* 
 * Setup functions for live telemetry
 */
void telemSetup() {
}

/* 
 * Loop for live telemetry
 */
void telemLoop() {
  while (TCU.available()) {
    uint8_t incomingByte = TCU.read();

    /*if (incomingByte == 0) {
      zeroes++;
    } else {
      zeroes = 0;
    }

    if (zeroes == 3) {
      zeroes = 0;

    }*/

    incomingFrame[frameIndex] = incomingByte;
    frameIndex++;

    if (incomingByte == 0) {

      Serial.println((char *) incomingFrame);
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) incomingFrame, sizeof(frameIndex));

      if (result == ESP_OK) {
        Serial.println("Sent with success");
      } else {
        Serial.println("Error sending the data");
      }

      frameIndex = 0;
    }

    if (frameIndex == 256) {
      frameIndex = 0;
    }
  }
}

/*
 * Sets up the GPS
 */
void GPSSetup() {
}

/*
 * Loop to retreive data from the GPS and send correction data to the GPS
 */
void GPSLoop() {
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingRTCM, incomingData, sizeof(incomingRTCM));
  Serial.print("Bytes received: ");
  Serial.println(len);
}

void esp_now_setup() {
  WiFi.mode(WIFI_STA);
  int a = esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_LR);
  Serial.println(a);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(onDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(onDataRecv);
}

void setup() {
  Serial.begin(115200);

  TCU.begin(115200, SERIAL_8N1, ESP_RX, ESP_TX);
  pinMode(GPS_OK, OUTPUT);
  pinMode(GPS_RTK_OK, OUTPUT);
  pinMode(ESP_LED_OTHER_1, OUTPUT);
  pinMode(ESP_LED_OK, OUTPUT);
  pinMode(ESP_LED_OTHER_2, OUTPUT);

  esp_now_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  GPSLoop();
  telemLoop();
}