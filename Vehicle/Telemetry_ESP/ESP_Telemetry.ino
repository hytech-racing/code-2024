uint8_t incomingFrame[256];
int frameIndex = 0;
int zeroes = 0;
#define DELIMITER ((uint8_t) 0)
//#define DELIMITER ('\n')
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
    incomingFrame[frameIndex] = incomingByte;
    frameIndex++;

    if (incomingByte == DELIMITER) {
      zeroes++;
    } else {
      zeroes = 0;
    }

    esp_err_t result;
    if (((frameIndex >= 225) && (incomingByte == DELIMITER)) || ((zeroes >= 2) && (frameIndex > 0)) || (frameIndex >= 250)) {

      //Serial.write((uint8_t *) incomingFrame, frameIndex);
      result = esp_now_send(broadcastAddress, (uint8_t *) incomingFrame, frameIndex);

      if (result == ESP_OK) {
        Serial.println("Sent with success");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        digitalWrite(ESP_LED_OK, !digitalRead(ESP_LED_OK));
        counters.send_attempts++;
        counters.send_successes++;
      } else {
        Serial.println("Error sending the data");
        counters.send_attempts++;
      }

      frameIndex = 0;
      zeroes = 0;
    }

    // If shit really hits the fan, just reset it all
    if (frameIndex == 256) {
      frameIndex = 0;
    }
  }
}



void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}