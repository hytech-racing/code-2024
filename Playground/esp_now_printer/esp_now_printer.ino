/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

//Structure example to receive data
//Must match the sender structure
typedef struct test_struct {
  int x;
  int y;
} test_struct;

//Create a struct_message called myData
test_struct myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.write(incomingData, len);
  //Serial.println(len);
  //memcpy(&myData, incomingData, sizeof(myData));
}
 


void esp_now_setup() {
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
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

 // memcpy(peerInfo.peer_addr, broadcastAddress, 6);
 // peerInfo.channel = 0;
 // peerInfo.encrypt = false;
//
 // // Add peer
 // if (esp_now_add_peer(&peerInfo) != ESP_OK) {
 //   Serial.println("Failed to add peer");
 //   return;
 // }
 // // Register for a callback function that will be called when data is received
 // esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  esp_now_setup();
}
 
void loop() {

}
