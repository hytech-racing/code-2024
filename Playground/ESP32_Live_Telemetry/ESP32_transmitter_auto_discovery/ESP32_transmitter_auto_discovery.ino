/*
 * @brief: ESP32 test code for the rear Sensor Acquisition Board.
 *         Only provides existing rear SAB functionality, i.e. CAN comms and sensor reading.
 * 
 * @author: Bo Han Zhu
 * @date: 04/16/2022
 */

/*
 * Sensor Acquisition Board Rear Ports:               Adafruit Huzzah32 Pin Number:
 * 0: Cooling Loop Fluid Temperature                  A7 (GPIO_NUM_32)
 * 1: Ambient Air Temperature                         A4 (GPIO_NUM_36)
 * 2: Back-Left Suspension Linear Potentiometer       A3 (GPIO_NUM_39)
 * 3: Back-Right Suspension Linear Potentiometer      A2 (GPIO_NUM_34)
 */

#include "SAB_readings_rear.h"
#include "CAN_ID.h"
#include "Metro.h"
#include "Hytech_CAN.h"
//#include "driver/can.h"
//#include "driver/gpio.h"

// CAN Variables
#define CAN_TX GPIO_NUM_17
#define CAN_RX GPIO_NUM_16
Metro timer_rear_update = Metro(200);

// Pin definitions
#define CAN_LED          GPIO_NUM_13
#define ALPHA 0.75  // Adjust Filtering Strength (Higher = More Filtering)
// #define INPUT_TO_5000mV 5.9082, expression: 3.3V/(30/11)V * 3.3V/1024counts * 5V/3.3V * 1000mV/1V = 5.9082, used just for reference
// Functions scale value up by 1000, will get scaled down 1000 later in parser
#define TICKS_4096_TO_1024 (4096 / 1024)         // ESP32 ADC translates to 4096 ticks per 3.3V instead of Teensy's 1024 ticks per 3.3V


// Options
#define DEBUG (true)


/**
 * @brief: ESP-Now
 */

#include <esp_now.h>
#include <WiFi.h>


// TODO: REPLACE WITH YOUR RECEIVER MAC Address
// TODO: Add faster pairing functionality
// 84:F7:03:C2:D4:74
uint8_t broadcastAddress[] = {0x84, 0xF7, 0x03, 0xC2, 0xD4, 0x74};


typedef struct transmit_msg_t {
  uint16_t  can_id;
  uint64_t  raw_data;
} transmit_msg_t;

transmit_msg_t msg;

MC_temperatures_1 temperatures_1;

esp_now_peer_info_t peerInfo;

void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
/*
 * NEW
 * 
 */
inline void startDiscovery() {

}
inline void stopDiscovery() {
}
inline void flashBlueLED() {
  // This method will flash the Blue LED at an interval
}
inline void setRedLED(bool ledOn) {
  // This method will switch the Red LED on or Off
}


// We use an Enum to define the Mode of our Device
enum DeviceMode {
  Waiting,     // Not Discovering, not timed out
  Discovering, // We're in Discovery mode
  Discovered,  // Discovery Succeeded
  Failed,      // Discovery Failed (Timed Out)
};
DeviceMode deviceMode = Waiting; // We are initially Waiting



enum ButtonState {
  ButtonDown, // The button is being pressed/held
  ButtonUp    // The button has been released
};
ButtonState buttonState;
inline ButtonState getButtonState() {
  return digitalRead(PIN_BUTTON) == HIGH ? ButtonDown : ButtonUp;
}
 /*
  * 
  * END
  */

void setup() {
  pinMode(CAN_LED, OUTPUT);

  uint8_t placeholderdata[] = {0x84, 0xF7, 0x03, 0xC2, 0xD4, 0x74, 0x10, 0x90};
  
  temperatures_1.load(placeholderdata);

  // Start ESP-NOW
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    printf("Error initializing ESP-NOW\n");
    return;
  }

  esp_now_register_send_cb(data_sent);
  
  peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    printf("Failed to add peer\n");
    return;
  }
}

void loop() { 

  msg.can_id = ID_MC_TEMPERATURES_1;
  memcpy(&(msg.raw_data), &(temperatures_1), sizeof(temperatures_1));
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg, sizeof(msg));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(500);
}

/**
 * @brief: Copies in the received raw message and adds it to the raw message queue
 * 
 * @param[in]: mac_addr - pointer to the MAC address
 * @param[in]: status - whether the ESP-Now message successfully transmitted or not
 */
void data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  printf("\r\nStatus of Last Message Sent:\t");
  printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail\n");
}
