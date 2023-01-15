#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "Metro.h"
#include <string.h>

#include <Wire.h> //Needed for I2C to GNSS
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;
#include <XBTools.h>

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

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
//  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
//  Serial.begin(115200);
//  TCU.begin(9600, SERIAL_8N1, ESP_RX, ESP_TX);
}

/*
 * Loop to retreive data from the GPS and send correction data to the GPS
 */
void GPSLoop() {
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer
    
    long latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.print(F(" (mm)"));

    byte SIV = myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.println();

    long lat_long[2] = {latitude, longitude};

    uint8_t cobs_buf[2 + 2 * sizeof(long)];
    cobs_encode((uint8_t *)(lat_long), 2 * sizeof(long), cobs_buf);

    cobs_buf[2 * sizeof(long)+1] = 0x0;
    for (int i = 0; i < 2 * sizeof(long)+2; i++) {
        Serial.print(cobs_buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    //Choose TCU or Serial2 as required
    TCU.write(cobs_buf, (size_t) (2 * sizeof(long))+2);
  }
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
  GPSSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  GPSLoop();
  telemLoop();
}
