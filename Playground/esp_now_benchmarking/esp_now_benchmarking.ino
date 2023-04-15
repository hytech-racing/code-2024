#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "Metro.h"

#define DEVBOARD {0x84, 0xF7, 0x03, 0xC2, 0xD4, 0xF2}
#define THINGPLUS {0x7C, 0xDF, 0xA1, 0x55, 0xB5, 0xA2}
#define TX true
#define D_SZ 225


// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/


uint8_t broadcastAddress[] = THINGPLUS;

uint8_t data[D_SZ];

int attempts = 0;
int successes = 0;
int tx_time = 0;
int recieved = 0;
int recieved_bytes = 0;
int loops = 0;

Metro timer = Metro(1000);

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0){
    successes++;
  }
  attempts++;
}

// Callback when data is received
void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  recieved++;
  recieved_bytes += len;
}

void setup(){
  for(uint8_t i = 0; i < D_SZ; i++) {
    data[i] = i;
  }

  Serial.begin(115200);
  //while(!Serial) ;
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  Serial1.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  if (esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR) != ESP_OK) {
    Serial.println("Error initializing WIFI LR");
    while (1);
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while (1);
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
 
void loop(){
  #if TX
  if (timer.check()) {
    Serial.printf("Attempts: %i,  Successes: %i,  Speed: %i B/s,  tx time: %i ms/S, loops: %i\n", attempts, successes, D_SZ*successes, tx_time, loops);
    attempts = 0;
    successes = 0;
    tx_time = 0;
    loops = 0;
  }
  loops++;
  int before = millis();
  esp_err_t result = esp_now_send(broadcastAddress, data, D_SZ);
  tx_time += (millis()-before);
  if(Serial1.availableForWrite()>=D_SZ){
    Serial1.write(data, D_SZ);
  }
  #else
  if (timer.check()) {
    Serial.printf("Recieved: %i,  Speed: %i\n", recieved, recieved_bytes);
    recieved = 0;
    recieved_bytes = 0;
  }
  #endif
}
