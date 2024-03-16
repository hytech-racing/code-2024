/**
 * Test code for VectorNav library development
*/

#define IMU_RS232_SPEED 115200
#define DEFAULT_WRITE_BUFFER_SIZE 17
#define DEFAULT_READ_BUFFER_SIZE 256

#include <Arduino.h>

// Function declarations
void queryModelNumber();
void parseModelNumber(char *productName);
void queryYawPitchRoll();
void queryYawPitchRollMagneticAccelerationAndAngularRates();

void setup() {
  // Open Serial Monitor
  Serial.begin(115200);

  // Initialize Serial communication for Vector Nav
  Serial2.begin(IMU_RS232_SPEED);
  Serial.println("Serial communication initialized...");

}

void loop() {
  // Test communications
  Serial.println("Ask VectorNav something ^^");
  Serial.println("Let's get raw reponse first");
  Serial.print("Model number: ");
  queryModelNumber();
  // Serial.print("Yaw Pitch Roll: ");
  // queryYawPitchRoll();
  // Serial.print("Yaw Pitch Roll Magnetic Acceleration Angular Rates: ");
  // queryYawPitchRollMagneticAccelerationAndAngularRates();

  Serial.println();
  
}

void queryModelNumber() {

  // int sendLength = 17;
  char toSend[DEFAULT_WRITE_BUFFER_SIZE] = "$VNRRG,01*XX\r\n"; // simple command requesting model number. no checksum or crc  
  Serial2.print(toSend);
  Serial2.print(toSend);
  Serial2.flush();

  int index = 0;
  // int receiveLength = 256;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  while (Serial2.available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
    receiveBuffer[index++] = Serial2.read();
  }

  Serial.println(receiveBuffer);
  
}

void parseMoldelNumber(char *productName) {

}

void queryYawPitchRoll() {

  // int sendLength = 17;
  char toSend[DEFAULT_WRITE_BUFFER_SIZE] = "$VNRRG,08*XX\r\n"; // simple command requesting yaw pitch roll. no checksum or crc
  Serial2.print(toSend);
  Serial2.flush();

  int index = 0;
  // int receiveLength = 256;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  while (Serial2.available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
    receiveBuffer[index++] = Serial2.read();
  }

  Serial.println(receiveBuffer);
  
}

void queryYawPitchRollMagneticAccelerationAndAngularRates() {

  // int sendLength = 17;
  char toSend[DEFAULT_WRITE_BUFFER_SIZE] = "$VNRRG,27*XX\r\n"; // simple command requesting orientation and IMU data. no checksum or crc
  Serial2.print(toSend);
  Serial2.flush();

  int index = 0;
  // int receiveLength = 256;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  if (Serial2.available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
    receiveBuffer[index++] = Serial2.read();
  }

  Serial.println(receiveBuffer);
  
}


