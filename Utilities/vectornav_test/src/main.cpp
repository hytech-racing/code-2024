/**
 * Test code for VectorNav library development
*/

#define IMU_RS232_SPEED 115200

#include <Arduino.h>

// Function declarations
void queryModelNumber();

void setup() {
  // Open Serial Monitor
  Serial.begin(115200);

  // Initialize Serial communication for Vector Nav
  Serial1.begin(IMU_RS232_SPEED);
  Serial.println("Serial communication initialized...");

}

void loop() {
  // Test communications
  Serial.println("Ask VectorNav something ^^");
  Serial.println("Let's get raw reponse first");
  Serial.print("Model number: ");
  queryModelNumber();
  Serial.print("Yaw Pitch Roll: ");



  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void queryModelNumber() {

  int sendLength = 17;
  char toSend[sendLength] = "$VNRRG,01";
  Serial1.print(toSend);

  int index = 0;
  int receiveLength = 50;
  char productNameBuffer[receiveLength];
  if (Serial1.available() > 0 && index < receiveLength - 1) {
    productNameBuffer[index++] = Serial1.read();
  }

  Serial.println(productNameBuffer);
  
}

void parseMoldelNumber(char *productName) {

}