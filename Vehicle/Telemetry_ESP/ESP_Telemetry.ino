/* 
 * Setup functions for live telemetry
 */
void telemSetup() {
}

int zeros = 0;
/* 
 * Loop for live telemetry
 */
void telemLoop() {
  while (TCU.available()) {
    uint8_t incomingByte = TCU.read();

    /*if (incomingByte == '\n') {
      zeroes++;
    } else {
      zeroes = 0;
    }*/

    incomingFrame[frameIndex] = incomingByte;
    frameIndex++;
    esp_err_t result;
    if (frameIndex >= 225) {

      //Serial.println((char *) incomingFrame);
      //if (zeros == 3) {
      //  result = esp_now_send(broadcastAddress, (uint8_t *) incomingFrame, frameIndex-2);
      //}
      //else {
        result = esp_now_send(broadcastAddress, (uint8_t *) incomingFrame, frameIndex);
      //}

      if (result == ESP_OK) {
        Serial.println("Sent with success");
        digitalWrite(ESP_LED_OK, !digitalRead(ESP_LED_OK));
        counters.send_attempts++;
        counters.send_successes++;
      } else {
        Serial.println("Error sending the data");
        counters.send_attempts++;
      }

      frameIndex = 0;
      zeros = 0;
    }

    if (frameIndex == 256) {
      frameIndex = 0;
    }
  }
}



void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}