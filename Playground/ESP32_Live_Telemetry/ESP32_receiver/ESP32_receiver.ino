/**
 * @brief: Receives raw live data from the ESP32 transmitter and passes serially to the connected laptop.
 * 
 * @author: Bo Han Zhu
 * @date: 03/31/2022
 */

#include "CAN_ID.h"
#include "Metro.h"
#include "Hytech_CAN.h"

// ESP-Now
#include <esp_now.h>
#include <WiFi.h>

typedef struct transmit_msg_t {
  uint16_t  can_id;
  uint64_t  raw_data;
} transmit_msg_t;

static transmit_msg_t msg;

MC_temperatures_1 temperatures_1;


void data_receive(const uint8_t * mac, const uint8_t *incomingData, int len);

void setup() {
  Serial.begin(115200); // Must match baudrate of Python script that expects serial data
  
  //ESP-Now connecion
  WiFi.mode(WIFI_STA); // Set ESP32 to stationary mode

  if (esp_now_init() != ESP_OK) {
    printf("Error initializing ESP-NOW\n");
    exit(0); // Terminate program if cannot establish ESP-Now comms and wait for reset
  }
  
  esp_now_register_recv_cb(data_receive); // Register callback function
}

void loop() {
  // Not needed
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief: Copies in the received raw message and adds it to the raw message queue
 * 
 * @param[in]: mac - pointer to the MAC address, automatically filled in and not used
 * @param[in]: incomingData - pointer to the data, automatically filled in
 * @param[in]: len - length of the data received, automatically filled in and not used
 */
void data_receive(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&msg, incomingData, sizeof(msg));
  printf("%u,%u", msg.can_id, msg.raw_data);

  switch(msg.can_id) {
    case ID_MC_TEMPERATURES_1: memcpy(&temperatures_1, &(msg.raw_data), sizeof(temperatures_1));
  }
}
