
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
        digitalWrite(ESP_LED_OK, !digitalRead(ESP_LED_OK));
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



void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}