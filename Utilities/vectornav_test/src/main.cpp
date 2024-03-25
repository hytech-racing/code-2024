/**
 * Test code for VectorNav library development
*/
#include <Arduino.h>

#include "Metro.h"
#include "types.h"

#define IMU_RS232_SPEED 115200

#define DEFAULT_WRITE_BUFFER_SIZE 17
#define DEFAULT_WRITE_BUFFER_MIDIUM 21
#define DEFAULT_WRITE_BUFFER_LONG 256
#define DEFAULT_READ_BUFFER_SIZE 256
#define DEFAULT_SERIAL_BUFFER_SIZE 64

#define BINARY_OUTPUT_GROUP_COUNT_1 1
#define BINARY_OUTPUT_GROUP_COUNT_2 2
#define BINARY_OUTPUT_GROUP_COUNT_3 2
#define BINARY_OUTPUT_PAYLOAD_1 44   // bytes
#define BINARY_OUTPUT_PAYLOAD_2 38   // bytes
#define BINARY_OUTPUT_PAYLOAD_3 36   // bytes
#define OFFSET_PADDING_1 4
#define OFFSET_PADDING_2 6
#define OFFSET_PADDING_3 6

#define SANITY_CHECK 0

#define ATOU32 static_cast<uint32_t>(std::atoi(result))
#define NEXT result = getNextData(_data, parseIndex); \
	if (result == NULL) \
		return;

Metro timer_vectornav_read = Metro(25);         // default 40Hz
Metro timer_vectornav_read_binary = Metro(10);  // configured 100Hz
Metro timer_vectornav_config = Metro(200);   // change config at 5Hz
Metro timer_read_imu = Metro(20);               // serial delay from polling request
Metro timer_vectornav_change_reading = Metro(40); // change binary output group 25Hz

// Function declarations
void queryModelNumber();
void readUserConfiguredBinaryOutput_1(uint8_t binaryOutputNumber);
void readUserConfiguredBinaryOutput_2();
void readUserConfiguredBinaryOutput_3();
void pollUserConfiguredBinaryOutput(uint8_t binaryOutputNumber);
void queryYawPitchRoll();
void queryYawPitchRollMagneticAccelerationAndAngularRates();
void queryAsyncDataOutputFrequency();
void readDefaultAsciiOutput_INS_LLA();
void readDefaultAsciiOutput_INS_LLA_40Hz();

// Test utilities
void parseModelNumber(char *productName);
void parseAsyncDataOutputFrequency(uint32_t* adof, char *_data);
char* startAsciiPacketParse(char* packetStart, size_t& index);
char* getNextData(char* str, size_t& startIndex);
char* vnstrtok(char* str, size_t& startIndex);
void configBinaryOutput(uint8_t binaryOutputNumber, uint8_t fields, uint16_t rateDivisor);
void turnOffUnusedBinaryOutput();
void turnOffAsciiOutput();
void checkBinaryOff(uint8_t binaryOutputNumber);
void checkAsciiOff();
void readPollingBinaryOutput();

// Global
int counter;
int binaryOutputNumber;

void setup() {
  // Open Serial Monitor
  Serial.begin(115200);

  // Initialize Serial communication for Vector Nav
  Serial2.begin(IMU_RS232_SPEED);
  Serial.println("Serial communication initialized...");
  Serial.println();

  // Init counter
  counter = 0;

  // Init binaryOutputNumber
  binaryOutputNumber = 0;

  // Configure sensor
  turnOffAsciiOutput();
  // turnOffUnusedBinaryOutput();
  configBinaryOutput(1, 0x01, 0);    // 0000 0001
  configBinaryOutput(2, 0x05, 0);    // 0000 0101
  configBinaryOutput(3, 0x28, 0);    // 0010 1000

/*
  delay(200);
  configBinaryOutput(1, 0x01, 32);    // 0000 0001. 25Hz

  delay(200);
  configBinaryOutput(2, 0x05, 32);    // 0000 0101. 25Hz

  delay(200);
  configBinaryOutput(3, 0x28, 32);    // 0010 1000. 25Hz
  */

}

void loop() {

  /* Test communications */
  // Serial.println("Ask Vector Nav something ^^");
  // Serial.println("Let's get raw reponse first");

  // Serial.print("Model number: ");
  // queryModelNumber();

  // Serial.print("Yaw Pitch Roll: ");
  // queryYawPitchRoll();

  // Serial.print("Yaw Pitch Roll Magnetic Acceleration Angular Rates: ");
  // queryYawPitchRollMagneticAccelerationAndAngularRates();

  // Serial.print("Async rate: ");
  // queryAsyncDataOutputFrequency();

  // Serial.print("Default async ascii output: ");
  // readDefaultAsciiOutput_INS_LLA();
  // readDefaultAsciiOutput_INS_LLA_40Hz();

  // Customized binary output
  // checkBinaryOff(1);    // checked: correct
  // checkBinaryOff(2);     // checked: correct
  // checkBinaryOff(3);      // checked: correct
  // checkAsciiOff();   // checked: ascii is off
  // readUserConfiguredBinaryOutput_1(1);
  // readUserConfiguredBinaryOutput_2();
  // readUserConfiguredBinaryOutput_3();

  // Periodic change configuration
  /*
  switch (binaryOutputNumber)
  {
    case 0:
      readUserConfiguredBinaryOutput_1(1);
      break;

    case 1:
      readUserConfiguredBinaryOutput_1(1);
      break;

    case 2:
      readUserConfiguredBinaryOutput_1(1);
      break;
    
    default:
      break;
  }

  if (timer_vectornav_config.check())
  {
    switch (binaryOutputNumber)
    {
      case 0:
        configBinaryOutput(1, 0x01, 0);    // 0000 0001
        configBinaryOutput(2, 0x05, 32);    // 0000 0101. 25Hz
        binaryOutputNumber = 1;
        break;

      case 1:
        configBinaryOutput(2, 0x05, 0);    // 0000 0101
        configBinaryOutput(3, 0x28, 32);    // 0010 1000. 25Hz
        binaryOutputNumber = 2;
        break;

      case 2:
        configBinaryOutput(3, 0x28, 0);    // 0010 1000
        configBinaryOutput(1, 0x01, 32);    // 0000 0001. 25Hz
        binaryOutputNumber = 0;
        break;
      
      default:
        break;
    }
  }
  */
  

  // Turn binary output reg on only before acquiring data
  // configBinaryOutput(1, 0x01, 32);    // 0000 0001. 25Hz
  // readUserConfiguredBinaryOutput_1(1);
  // configBinaryOutput(1, 0x01, 0);    // 0000 0001

  // configBinaryOutput(2, 0x05, 32);    // 0000 0101. 25Hz
  // readUserConfiguredBinaryOutput_1(2);
  // configBinaryOutput(2, 0x05, 0);    // 0000 0101

  // configBinaryOutput(3, 0x28, 0);    // 0010 1000. 25Hz
  // readUserConfiguredBinaryOutput_1(3);
  // configBinaryOutput(3, 0x28, 0);    // 0010 1000


  if (timer_vectornav_change_reading.check())
  {
    binaryOutputNumber = (binaryOutputNumber + 1) % 3;
  }
  
  pollUserConfiguredBinaryOutput(binaryOutputNumber + 1);
  // readPollingBinaryOutput();

  // while (Serial2.available()) {
  //   Serial.print(Serial2.read(), HEX);
  // }

  // Serial.println();

  // configBinaryOutput(1, 0x01);    // 0000 0001

  /* Parser test for received ascii message */
  // char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = "$VNRRG,07,40*5C";
  // Serial.println(receiveBuffer);
  // uint32_t reg;
	// parseAsyncDataOutputFrequency(&reg, receiveBuffer);
  // Serial.printf("Async Frequency: %d Hz\n", reg);

  /* Attempt to received async data periodically assuming Vecotr Nav is sending */
  // char data = '\0';
  // if (Serial2.available()) {
  //   data = Serial2.read();
  //   counter++;
  //   // Serial.print("Last byte arrived: ");
  //   // Serial.println(data);
  //   Serial.println("Async data received ... Maybe?");
  // }
  // Serial.printf("Number of bytes read: %d\n", counter);
  // Serial.print("Last byte arrived: ");
  // Serial.println(data);

  // Serial.println();
  
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

void queryYawPitchRollMagneticAccelerationAndAngularRates() {

  // int sendLength = 17;
  char toSend[DEFAULT_WRITE_BUFFER_SIZE] = "$VNRRG,27*XX\r\n"; // simple command requesting orientation and IMU data. no checksum or crc
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

void queryAsyncDataOutputFrequency() {

  char toSend[DEFAULT_WRITE_BUFFER_SIZE] = "$VNRRG,07*XX\r\n"; // simple command requesting asynchronize data output rate
  Serial2.print(toSend);
  // Serial2.print(toSend);
  Serial2.flush();

  int index = 0;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  while (Serial2.available() && index <DEFAULT_READ_BUFFER_SIZE - 1) {
    receiveBuffer[index++] = Serial2.read();
  }

  Serial.println(receiveBuffer);

  // uint32_t reg;
	// parseAsyncDataOutputFrequency(&reg, receiveBuffer);
}

void parseAsyncDataOutputFrequency(uint32_t* adof, char *_data)
{
	size_t parseIndex;
	char *result = startAsciiPacketParse(_data, parseIndex);  NEXT
	*adof = ATOU32;
}

char* startAsciiPacketParse(char* packetStart, size_t& index)
{
	index = 7;
	return vnstrtok(packetStart, index);
}

char* getNextData(char* str, size_t& startIndex)
{
	return vnstrtok(str, startIndex);
}

char* vnstrtok(char* str, size_t& startIndex)
{
	size_t origIndex = startIndex;

	if (str[startIndex-1] == '*') // attempting to read too many fields
		return NULL;
		
	while (str[startIndex] != ',' && str[startIndex] != '*')
	{
		if((str[startIndex] < ' ') || (str[startIndex] > '~') || (str[startIndex] == '$')) // check for garbage characters
			return NULL;
		startIndex++;
	}
	
	str[startIndex++] = '\0';

	return str + origIndex;
}

void readUserConfiguredBinaryOutput_2() {

  if (timer_vectornav_read_binary.check()) {
    uint8_t data = Serial2.read();

    if (data != 0xFA)
      return;

    int binaryPacketLength = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT_2 + BINARY_OUTPUT_PAYLOAD_2 + 2;  // in bytes: sync, groups, group fields, payload, crc
    uint8_t receiveBuffer[binaryPacketLength];
    int index = 0;

    receiveBuffer[index++] = data;

    // while (index < binaryPacketLength) {
    //   while (Serial2.available())
    //   {
    //     data = Serial2.read();
    //     receiveBuffer[index++] = data;
    //   }      
    // }

    // while (index < binaryPacketLength) {
      while (Serial2.available())
      {
        if (index < binaryPacketLength) {
          data = Serial2.read();
          receiveBuffer[index++] = data;
        }
      }      
    // }

    uint8_t syncByte = receiveBuffer[0];

    uint8_t groupsByte = receiveBuffer[1];
#if SANITY_CHECK
    if (groupsByte != 0x05)   // 0000 0101
      return;
#endif

    uint16_t groupField1 = ((uint16_t)receiveBuffer[3] << 8) | receiveBuffer[2];
#if SANITY_CHECK
    if (groupField1 != 0x1100)    // 0001 0001 0000 0000
      return;
#endif

    uint16_t groupField2 = (receiveBuffer[5] << 8) | receiveBuffer[4];
#if SANITY_CHECK
    if (groupField2 != 0x0084)    // 0000 0000 1000 0100
      return;
#endif

    float accelBodyX = (receiveBuffer[3 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[2 + OFFSET_PADDING_2] << (8 * 2)) | 
                       (receiveBuffer[1 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[0 + OFFSET_PADDING_2];
    float accelBodyY = (receiveBuffer[7 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[6 + OFFSET_PADDING_2] << (8 * 2)) | 
                       (receiveBuffer[5 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[4 + OFFSET_PADDING_2];
    float accelBodyZ = (receiveBuffer[11 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[10 + OFFSET_PADDING_2] << (8 * 2)) | 
                       (receiveBuffer[9 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[8 + OFFSET_PADDING_2];

    uint16_t InsStatus = (receiveBuffer[13 + OFFSET_PADDING_2] << 8) | receiveBuffer[12 + OFFSET_PADDING_2];

    float uncompAccelBodyX = (receiveBuffer[17 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[16 + OFFSET_PADDING_2] << (8 * 2)) | 
                             (receiveBuffer[15 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[14 + OFFSET_PADDING_2];
    float uncompAccelBodyY = (receiveBuffer[21 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[20 + OFFSET_PADDING_2] << (8 * 2)) | 
                             (receiveBuffer[19 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[18 + OFFSET_PADDING_2];
    float uncompAccelBodyZ = (receiveBuffer[25 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[24 + OFFSET_PADDING_2] << (8 * 2)) | 
                             (receiveBuffer[23 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[22 + OFFSET_PADDING_2];

    float deltaVelX = (receiveBuffer[29 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[28 + OFFSET_PADDING_2] << (8 * 2)) | 
                      (receiveBuffer[27 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[26 + OFFSET_PADDING_2];
    float deltaVelY = (receiveBuffer[33 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[32 + OFFSET_PADDING_2] << (8 * 2)) | 
                      (receiveBuffer[31 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[30 + OFFSET_PADDING_2];
    float deltaVelZ = (receiveBuffer[37 + OFFSET_PADDING_2] << (8 * 3)) | (receiveBuffer[36 + OFFSET_PADDING_2] << (8 * 2)) | 
                      (receiveBuffer[35 + OFFSET_PADDING_2] << (8 * 1)) | receiveBuffer[34 + OFFSET_PADDING_2];

    uint16_t crc = (receiveBuffer[39 + OFFSET_PADDING_2] << 8) | receiveBuffer[38 + OFFSET_PADDING_2];

    Serial.println("Ask Vector Nav something ^^");
    Serial.println("Let's get raw reponse first");

    // Serial.println();

    Serial.println("User configured binary output: ");

    Serial.print("Raw string: ");
    for (int i = 0; i < binaryPacketLength; i++)
    {
      Serial.printf("%X ", receiveBuffer[i]);
    }

    Serial.printf("\nLength: %d", index);

    Serial.printf("\n\n");
/*
    Serial.println("Let's parse it hard core :)");

    Serial.println();

    Serial.printf("Sync: %X\n", syncByte);

    Serial.println();

    Serial.printf("Groups: %X\n", groupsByte);
    Serial.printf("Group 1 Common: %X\n", groupField1);
    Serial.printf("Group 3 IMU: %X\n", groupField2);
    Serial.printf("Group 4 GNSS1: %X\n", groupField3);
    Serial.printf("Group 6 INS: %X\n", groupField4);

    Serial.println();

    Serial.println("Data acquired from Group 1:");
    Serial.printf("TimeGPS: %d[ns]\n", timeGPS);
    Serial.printf("AngularRate: X - %f[rad/s], Y - %f[rad/s], Z - %f[rad/s]\n", angularRateBodyX, angularRateBodyY, angularRateBodyZ);
    Serial.printf("Position: latitude - %f[deg], longitude - %f[deg], altitude - %f[m]\n", latitude, longitude, altitude);
    Serial.printf("Accel: X - %f[m/s^2], Y - %f[m/s^2], Z - %f[m/s^2]\n", accelBodyX, accelBodyY, accelBodyZ);
    Serial.printf("InsStatus: %X\n", InsStatus);
    
    Serial.println();

    Serial.println("Data acquired from Group 3:");
    Serial.printf("UncompAccel: X - %f[m/s^2], Y - %f[m/s^2], Z - %f[m/s^2]\n", uncompAccelBodyX, uncompAccelBodyY, uncompAccelBodyZ);
    Serial.printf("DeltaVel: X - %f[m/s], Y - %f[m/s], Z - %f[m/s]\n", deltaVelX, deltaVelY, deltaVelZ);

    Serial.println();

    Serial.println("Data acquired from Group 4:");
    Serial.printf("PosEcef: (%f, %f, %f)[m]\n", posEcef0, posEcef1, posEcef2);
    
    Serial.println();

    Serial.println("Data acquired from Group 6:");
    Serial.printf("VelBody: X - %f[m/s], Y - %f[m/s], Z - %f[m/s]\n", velBodyX, velBodyY, velBodyZ);

    Serial.printf("CRC: %X\n", crc);

    Serial.println();

    Serial.print("Test bit shift: ");
    uint8_t x = 1;
    uint32_t y = (x << 8) | x;    // 0000 0001 0000 0001
    Serial.printf("%X\n", y);

    Serial.println();

    Serial.println("phew...!");

    Serial.println();

    */
  }   // end of timer.check()
}

void readUserConfiguredBinaryOutput_1(uint8_t binaryOutputNumber) {

  // bool hasRead = false;

  // while (!hasRead) {
  if (timer_vectornav_read_binary.check() && Serial2.available()) {

    uint8_t data = Serial2.read();
    if (data != 0xFA)
      return;

    // data = Serial2.read();

    // if (data != 0x01)
    //   return;

    // int binaryPacketLength = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT_1 + BINARY_OUTPUT_PAYLOAD_1 + 2;  // in bytes: sync, groups, group fields, payload, crc
    int binaryPacketLength = 64;
    uint8_t receiveBuffer[binaryPacketLength];
    int index = 0;

    // receiveBuffer[index++] = 0xFA;
    receiveBuffer[index++] = data;

    // while (index < binaryPacketLength) {
    //   while (Serial2.available())
    //   {
    //     data = Serial2.read();
    //     receiveBuffer[index++] = data;
    //   }      
    // }

    // while (index < binaryPacketLength) {
      while (Serial2.available())
      {
        // if (index < binaryPacketLength) {
          data = Serial2.read();
          receiveBuffer[index++] = data;
        // }
      }
    // }
    // hasRead = true;
/*
    uint8_t syncByte = receiveBuffer[0];

    uint8_t groupsByte = receiveBuffer[1];
#if SANITY_CHECK
    if (groupsByte != 0x01)   // 0000 0001
      return;
#endif

    uint16_t groupField1 = ((uint16_t)receiveBuffer[3] << 8) | receiveBuffer[2];
#if SANITY_CHECK
    if (groupField1 != 0x0062)  // 0000 0000 0110 0010
      return;
#endif
*/
/*
    uint64_t timeGPS = (receiveBuffer[7 + OFFSET_PADDING_1] << (8 * 7)) | (receiveBuffer[6 + OFFSET_PADDING_1] << (8 * 6)) | 
                       (receiveBuffer[5 + OFFSET_PADDING_1] << (8 * 5)) | (receiveBuffer[4 + OFFSET_PADDING_1] << (8 * 4)) |
                       (receiveBuffer[3 + OFFSET_PADDING_1] << (8 * 3)) | (receiveBuffer[2 + OFFSET_PADDING_1] << (8 * 2)) | 
                       (receiveBuffer[1 + OFFSET_PADDING_1] << (8 * 1)) | receiveBuffer[0 + OFFSET_PADDING_1];

    float angularRateBodyX = (receiveBuffer[11 + OFFSET_PADDING_1] << (8 * 3)) | (receiveBuffer[10 + OFFSET_PADDING_1] << (8 * 2)) | 
                             (receiveBuffer[9 + OFFSET_PADDING_1] << (8 * 1)) | receiveBuffer[8 + OFFSET_PADDING_1];
    float angularRateBodyY = (receiveBuffer[15 + OFFSET_PADDING_1] << (8 * 3)) | (receiveBuffer[14 + OFFSET_PADDING_1] << (8 * 2)) | 
                             (receiveBuffer[13 + OFFSET_PADDING_1] << (8 * 1)) | receiveBuffer[12 + OFFSET_PADDING_1];
    float angularRateBodyZ = (receiveBuffer[19 + OFFSET_PADDING_1] << (8 * 3)) | (receiveBuffer[18 + OFFSET_PADDING_1] << (8 * 2)) | 
                             (receiveBuffer[17 + OFFSET_PADDING_1] << (8 * 1)) | receiveBuffer[16 + OFFSET_PADDING_1];

    double latitude = (receiveBuffer[27 + OFFSET_PADDING_1] << (8 * 7)) | (receiveBuffer[26 + OFFSET_PADDING_1] << (8 * 6)) | 
                      (receiveBuffer[25 + OFFSET_PADDING_1] << (8 * 5)) | (receiveBuffer[24 + OFFSET_PADDING_1] << (8 * 4)) |
                      (receiveBuffer[23 + OFFSET_PADDING_1] << (8 * 3)) | (receiveBuffer[22 + OFFSET_PADDING_1] << (8 * 2)) | 
                      (receiveBuffer[21 + OFFSET_PADDING_1] << (8 * 1)) | receiveBuffer[20 + OFFSET_PADDING_1];
    double longitude = (receiveBuffer[35 + OFFSET_PADDING_1] << (8 * 7)) | (receiveBuffer[34 + OFFSET_PADDING_1] << (8 * 6)) | 
                       (receiveBuffer[33 + OFFSET_PADDING_1] << (8 * 5)) | (receiveBuffer[32 + OFFSET_PADDING_1] << (8 * 4)) |
                       (receiveBuffer[31 + OFFSET_PADDING_1] << (8 * 3)) | (receiveBuffer[30 + OFFSET_PADDING_1] << (8 * 2)) | 
                       (receiveBuffer[29 + OFFSET_PADDING_1] << (8 * 1)) | receiveBuffer[28 + OFFSET_PADDING_1];
    double altitude = (receiveBuffer[43 + OFFSET_PADDING_1] << (8 * 7)) | (receiveBuffer[42 + OFFSET_PADDING_1] << (8 * 6)) | 
                      (receiveBuffer[41 + OFFSET_PADDING_1] << (8 * 5)) | (receiveBuffer[40 + OFFSET_PADDING_1] << (8 * 4)) |
                      (receiveBuffer[39 + OFFSET_PADDING_1] << (8 * 3)) | (receiveBuffer[38 + OFFSET_PADDING_1] << (8 * 2)) | 
                      (receiveBuffer[37 + OFFSET_PADDING_1] << (8 * 1)) | receiveBuffer[36 + OFFSET_PADDING_1];

    uint16_t crc = (receiveBuffer[45 + OFFSET_PADDING_1] << 8) | receiveBuffer[44 + OFFSET_PADDING_1];
    */

    Serial.println("Ask Vector Nav something ^^");
    Serial.println("Let's get raw reponse first");

    // Serial.println();

    Serial.println("User configured binary output: ");

    Serial.print("Raw string: ");
    for (int i = 0; i < binaryPacketLength; i++)
    {
      Serial.printf("%X ", receiveBuffer[i]);
    }

    Serial.printf("\nLength: %d", index);

    Serial.printf("\n\n");
/*
    Serial.println("Let's parse it hard core :)");

    Serial.println();

    Serial.printf("Sync: %X\n", syncByte);

    Serial.println();

    Serial.printf("Groups: %X\n", groupsByte);
    Serial.printf("Group 1 Common: %X\n", groupField1);
    Serial.printf("Group 3 IMU: %X\n", groupField2);
    Serial.printf("Group 4 GNSS1: %X\n", groupField3);
    Serial.printf("Group 6 INS: %X\n", groupField4);

    Serial.println();

    Serial.println("Data acquired from Group 1:");
    Serial.printf("TimeGPS: %d[ns]\n", timeGPS);
    Serial.printf("AngularRate: X - %f[rad/s], Y - %f[rad/s], Z - %f[rad/s]\n", angularRateBodyX, angularRateBodyY, angularRateBodyZ);
    Serial.printf("Position: latitude - %f[deg], longitude - %f[deg], altitude - %f[m]\n", latitude, longitude, altitude);
    Serial.printf("Accel: X - %f[m/s^2], Y - %f[m/s^2], Z - %f[m/s^2]\n", accelBodyX, accelBodyY, accelBodyZ);
    Serial.printf("InsStatus: %X\n", InsStatus);
    
    Serial.println();

    Serial.println("Data acquired from Group 3:");
    Serial.printf("UncompAccel: X - %f[m/s^2], Y - %f[m/s^2], Z - %f[m/s^2]\n", uncompAccelBodyX, uncompAccelBodyY, uncompAccelBodyZ);
    Serial.printf("DeltaVel: X - %f[m/s], Y - %f[m/s], Z - %f[m/s]\n", deltaVelX, deltaVelY, deltaVelZ);

    Serial.println();

    Serial.println("Data acquired from Group 4:");
    Serial.printf("PosEcef: (%f, %f, %f)[m]\n", posEcef0, posEcef1, posEcef2);
    
    Serial.println();

    Serial.println("Data acquired from Group 6:");
    Serial.printf("VelBody: X - %f[m/s], Y - %f[m/s], Z - %f[m/s]\n", velBodyX, velBodyY, velBodyZ);

    Serial.printf("CRC: %X\n", crc);

    Serial.println();

    Serial.print("Test bit shift: ");
    uint8_t x = 1;
    uint32_t y = (x << 8) | x;    // 0000 0001 0000 0001
    Serial.printf("%X\n", y);

    Serial.println();

    Serial.println("phew...!");

    Serial.println();

    */

/*
    // Turn binary output off once acquired
    switch (binaryOutputNumber)
    {
      case 1:
        configBinaryOutput(1, 0x01, 0);    // 0000 0001
        break;

      case 2:
        configBinaryOutput(2, 0x05, 0);    // 0000 0101
        break;

      case 3:
        configBinaryOutput(3, 0x28, 0);    // 0010 1000
        break;
      
      default:
        break;
    }
    */
    

  }   // end of timer.check()
  // }   // end of !hasRead
}

void readUserConfiguredBinaryOutput_3() {

  if (timer_vectornav_read_binary.check()) {
    uint8_t data = Serial2.read();

    if (data != 0xFA)
      return;

    int binaryPacketLength = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT_3 + BINARY_OUTPUT_PAYLOAD_3 + 2;  // in bytes: sync, groups, group fields, payload, crc
    uint8_t receiveBuffer[binaryPacketLength];
    int index = 0;

    receiveBuffer[index++] = data;

    // while (index < binaryPacketLength) {
    //   while (Serial2.available())
    //   {
    //     data = Serial2.read();
    //     receiveBuffer[index++] = data;
    //   }      
    // }

    // while (index < binaryPacketLength) {
      while (Serial2.available())
      {
        if (index < binaryPacketLength) {
          data = Serial2.read();
          receiveBuffer[index++] = data;
        }
      }      
    // }

    uint8_t syncByte = receiveBuffer[0];

    uint8_t groupsByte = receiveBuffer[1];
#if SANITY_CHECK
    if (groupsByte != 0x28)   // 0010 1000
      return;
#endif

    uint16_t groupField1 = ((uint16_t)receiveBuffer[3] << 8) | receiveBuffer[2];
#if SANITY_CHECK
    if (groupField1 != 0x0040)
      return;
#endif

    uint16_t groupField2 = (receiveBuffer[5] << 8) | receiveBuffer[4];
#if SANITY_CHECK
    if (groupField2 != 0x0008)
      return;
#endif

    double posEcef0 = (receiveBuffer[7 + OFFSET_PADDING_3] << (8 * 7)) | (receiveBuffer[6 + OFFSET_PADDING_3] << (8 * 6)) | 
                      (receiveBuffer[5 + OFFSET_PADDING_3] << (8 * 5)) | (receiveBuffer[4 + OFFSET_PADDING_3] << (8 * 4)) |
                      (receiveBuffer[3 + OFFSET_PADDING_3] << (8 * 3)) | (receiveBuffer[2 + OFFSET_PADDING_3] << (8 * 2)) | 
                      (receiveBuffer[1 + OFFSET_PADDING_3] << (8 * 1)) | receiveBuffer[0 + OFFSET_PADDING_3];
    double posEcef1 = (receiveBuffer[15 + OFFSET_PADDING_3] << (8 * 7)) | (receiveBuffer[14 + OFFSET_PADDING_3] << (8 * 6)) | 
                      (receiveBuffer[13 + OFFSET_PADDING_3] << (8 * 5)) | (receiveBuffer[12 + OFFSET_PADDING_3] << (8 * 4)) |
                      (receiveBuffer[11 + OFFSET_PADDING_3] << (8 * 3)) | (receiveBuffer[10 + OFFSET_PADDING_3] << (8 * 2)) | 
                      (receiveBuffer[9 + OFFSET_PADDING_3] << (8 * 1)) | receiveBuffer[8 + OFFSET_PADDING_3];
    double posEcef2 = (receiveBuffer[23 + OFFSET_PADDING_3] << (8 * 7)) | (receiveBuffer[22 + OFFSET_PADDING_3] << (8 * 6)) | 
                      (receiveBuffer[21 + OFFSET_PADDING_3] << (8 * 5)) | (receiveBuffer[20 + OFFSET_PADDING_3] << (8 * 4)) |
                      (receiveBuffer[19 + OFFSET_PADDING_3] << (8 * 3)) | (receiveBuffer[18 + OFFSET_PADDING_3] << (8 * 2)) | 
                      (receiveBuffer[17 + OFFSET_PADDING_3] << (8 * 1)) | receiveBuffer[16 + OFFSET_PADDING_3];

    float velBodyX = (receiveBuffer[27 + OFFSET_PADDING_3] << (8 * 3)) | (receiveBuffer[26 + OFFSET_PADDING_3] << (8 * 2)) | 
                     (receiveBuffer[25 + OFFSET_PADDING_3] << (8 * 1)) | receiveBuffer[24 + OFFSET_PADDING_3];
    float velBodyY = (receiveBuffer[31 + OFFSET_PADDING_3] << (8 * 3)) | (receiveBuffer[30 + OFFSET_PADDING_3] << (8 * 2)) | 
                     (receiveBuffer[29 + OFFSET_PADDING_3] << (8 * 1)) | receiveBuffer[28 + OFFSET_PADDING_3];
    float velBodyZ = (receiveBuffer[35 + OFFSET_PADDING_3] << (8 * 3)) | (receiveBuffer[34 + OFFSET_PADDING_3] << (8 * 2)) | 
                     (receiveBuffer[33 + OFFSET_PADDING_3] << (8 * 1)) | receiveBuffer[32 + OFFSET_PADDING_3];

    uint16_t crc = (receiveBuffer[37 + OFFSET_PADDING_3] << 8) | receiveBuffer[36 + OFFSET_PADDING_3];

    Serial.println("Ask Vector Nav something ^^");
    Serial.println("Let's get raw reponse first");

    // Serial.println();

    Serial.println("User configured binary output: ");

    Serial.print("Raw string: ");
    for (int i = 0; i < binaryPacketLength; i++)
    {
      Serial.printf("%X ", receiveBuffer[i]);
    }

    Serial.printf("\nLength: %d", index);

    Serial.printf("\n\n");
/*
    Serial.println("Let's parse it hard core :)");

    Serial.println();

    Serial.printf("Sync: %X\n", syncByte);

    Serial.println();

    Serial.printf("Groups: %X\n", groupsByte);
    Serial.printf("Group 1 Common: %X\n", groupField1);
    Serial.printf("Group 3 IMU: %X\n", groupField2);
    Serial.printf("Group 4 GNSS1: %X\n", groupField3);
    Serial.printf("Group 6 INS: %X\n", groupField4);

    Serial.println();

    Serial.println("Data acquired from Group 1:");
    Serial.printf("TimeGPS: %d[ns]\n", timeGPS);
    Serial.printf("AngularRate: X - %f[rad/s], Y - %f[rad/s], Z - %f[rad/s]\n", angularRateBodyX, angularRateBodyY, angularRateBodyZ);
    Serial.printf("Position: latitude - %f[deg], longitude - %f[deg], altitude - %f[m]\n", latitude, longitude, altitude);
    Serial.printf("Accel: X - %f[m/s^2], Y - %f[m/s^2], Z - %f[m/s^2]\n", accelBodyX, accelBodyY, accelBodyZ);
    Serial.printf("InsStatus: %X\n", InsStatus);
    
    Serial.println();

    Serial.println("Data acquired from Group 3:");
    Serial.printf("UncompAccel: X - %f[m/s^2], Y - %f[m/s^2], Z - %f[m/s^2]\n", uncompAccelBodyX, uncompAccelBodyY, uncompAccelBodyZ);
    Serial.printf("DeltaVel: X - %f[m/s], Y - %f[m/s], Z - %f[m/s]\n", deltaVelX, deltaVelY, deltaVelZ);

    Serial.println();

    Serial.println("Data acquired from Group 4:");
    Serial.printf("PosEcef: (%f, %f, %f)[m]\n", posEcef0, posEcef1, posEcef2);
    
    Serial.println();

    Serial.println("Data acquired from Group 6:");
    Serial.printf("VelBody: X - %f[m/s], Y - %f[m/s], Z - %f[m/s]\n", velBodyX, velBodyY, velBodyZ);

    Serial.printf("CRC: %X\n", crc);

    Serial.println();

    Serial.print("Test bit shift: ");
    uint8_t x = 1;
    uint32_t y = (x << 8) | x;    // 0000 0001 0000 0001
    Serial.printf("%X\n", y);

    Serial.println();

    Serial.println("phew...!");

    Serial.println();

    */
  }   // end of timer.check()
}

void configBinaryOutput(uint8_t binaryOutputNumber, uint8_t fields, uint16_t rateDivisor) {

  char toSend[DEFAULT_WRITE_BUFFER_LONG];

  bool commonField = fields & 0x01;   // Serial.printf("common: %d\n", commonField);
  bool timeField = fields & 0x02;     // Serial.printf("time: %d\n", timeField);
  bool imuField = fields & 0x04;      // Serial.printf("imu: %d\n", imuField);
  bool gpsField = fields & 0x08;      // Serial.printf("gps: %d\n", gpsField);
  bool attitudeField = fields & 0x10; // Serial.printf("attitude: %d\n", attitudeField);
  bool insField = fields & 0x20;      // Serial.printf("ins: %d\n", insField);
  bool gps2Field = fields & 0x40;     // Serial.printf("gps2: %d\n", gps2Field);

  // First determine which groups are present.
  uint16_t groups = 0;
  if (commonField)
    groups |= 0x0001;         // 0000 0000 0000 0001. common group selected
  if (timeField)
    groups |= 0x0002;
  if (imuField)
    groups |= 0x0004;         // 0000 0000 0000 0100. IMU group selected
  if (gpsField)
    groups |= 0x0008;         // 0000 0000 0000 1000. GNSS1 group selected
  if (attitudeField)
    groups |= 0x0010;
  if (insField)
    groups |= 0x0020;         // 0000 0000 0010 0000. INS group selected
  if (gps2Field)
    groups |= 0x0040;

  // groups = 0010 1101 = 2D

  #if VN_HAVE_SECURE_CRT
  int length = sprintf_s(toSend, sizeof(toSend), "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, fields.asyncMode, fields.rateDivisor, groups);
  #else
  int length = sprintf(toSend, "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, vn::protocol::uart::ASYNCMODE_PORT1, rateDivisor, groups); // serial1, 800/16=50Hz, 
  #endif

  if (commonField) {
    #if VN_HAVE_SECURE_CRT
    length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.commonField);
    #else
    if (binaryOutputNumber == 1) {
      length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_TIMEGPS |     // 0000 0000 0110 0010 = 00 62
                                                vn::protocol::uart::COMMONGROUP_ANGULARRATE | 
                                                vn::protocol::uart::COMMONGROUP_POSITION);
    }
    else if (binaryOutputNumber == 2) {
      length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_ACCEL |       // 0001 0001 0000 0000 = 11 00
                                                vn::protocol::uart::COMMONGROUP_INSSTATUS);
    }
    #endif
  }
  if (timeField) {
  	#if VN_HAVE_SECURE_CRT
  	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.timeField);
  	#else
  	// length += sprintf(toSend + length, ",%X", fields.timeField);
  	#endif
  }
  if (imuField) {
  	#if VN_HAVE_SECURE_CRT
  	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.imuField);
  	#else
  	length += sprintf(toSend + length, ",%X", vn::protocol::uart::IMUGROUP_UNCOMPACCEL |    // 0000 0000 1000 0100 = 00 84
                                              vn::protocol::uart::IMUGROUP_DELTAVEL);
  	#endif
  }
  if (gpsField) {
  	#if VN_HAVE_SECURE_CRT
  	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.gpsField);
  	#else
  	length += sprintf(toSend + length, ",%X", vn::protocol::uart::GPSGROUP_POSECEF);        // 0000 0000 0100 0000 = 00 40
  	#endif
  }
  if (attitudeField) {
  	#if VN_HAVE_SECURE_CRT
  	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.attitudeField);
  	#else
  	// length += sprintf(toSend + length, ",%X", fields.attitudeField);
  	#endif
  }
  if (insField) {
    #if VN_HAVE_SECURE_CRT
    length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.insField);
    #else
    length += sprintf(toSend + length, ",%X", vn::protocol::uart::INSGROUP_VELBODY);        // 0000 0000 0000 1000 = 00 08
    #endif
  }
  if(gps2Field) {
    #if VN_HAVE_SECURE_CRT
    length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.gps2Field);
    #else
    // length += sprintf(toSend + length, ",%X", fields.gps2Field);
    #endif
  }

  #if VN_HAVE_SECURE_CRT
  length += sprintf_s(toSend + length, sizeof(toSend) - length, "*");
  #else
  length += sprintf(toSend + length, "*");
  #endif

  length += sprintf(toSend + length, "XX\r\n");

  Serial2.print(toSend);
  Serial2.flush();

}

void pollUserConfiguredBinaryOutput(uint8_t binaryOutputNumber) {

  // configBinaryOutput(1, 0x01, 32);    // 0000 0001

  if (timer_vectornav_read_binary.check()) {

    char toSend[DEFAULT_WRITE_BUFFER_SIZE];

    size_t length = sprintf(toSend, "$VNBOM,%u*", binaryOutputNumber);
    length += sprintf(toSend + length, "XX\r\n");

    Serial2.print(toSend);
    Serial2.flush();

    // delay(500);

    // int index = 0;
    // uint8_t receiveBuffer[DEFAULT_SERIAL_BUFFER_SIZE];
    // while (Serial2.available() && Serial2.read() == 0xFA)
    // {
    //   receiveBuffer[index++] = Serial2.read();
    // }
    
    // Serial.printf("Polled binary output %d raw string:\n", binaryOutputNumber);
    // for (int i = 0; i < index; i++)
    // {
    //   Serial.printf("%X ", receiveBuffer[i]);
    // }

    // Serial.printf("\nLength: %d", index);

    // Serial.printf("\n\n");

    timer_read_imu.reset();   // ideally

    delay(20);
    
    // while (Serial2.available()) {
    //   Serial.print(Serial2.read(), HEX);
    // }

    // Serial.println();

    readPollingBinaryOutput(); 

  }

  // configBinaryOutput(1, 0x01, 0);    // 0000 0001

}

void readPollingBinaryOutput() {

  // if (timer_read_imu.check())
  // {
  //   Serial.println("IMU read timer checked");   // shit does not work
    while (Serial2.available()) {
      Serial.print(Serial2.read(), HEX);
    }

    Serial.println();

    // int index = 0;
    // uint8_t receiveBuffer[DEFAULT_SERIAL_BUFFER_SIZE];

    // while (Serial2.available())
    // {
    //   receiveBuffer[index++] = Serial2.read();
    // }

  // }  

}

void turnOffUnusedBinaryOutput() {

  // Binary output 2
  uint8_t binaryOutputNumber = 2;
  char toSend2[DEFAULT_WRITE_BUFFER_LONG];

  #if VN_HAVE_SECURE_CRT
  int length2 = sprintf_s(toSend2, sizeof(toSend2), "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, fields.asyncMode, fields.rateDivisor, groups);
  #else
  int length2 = sprintf(toSend2, "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, vn::protocol::uart::ASYNCMODE_NONE, 0, 0); // turn off binary output 2
  #endif

  #if VN_HAVE_SECURE_CRT
  length2 += sprintf_s(toSend2 + length2, sizeof(toSend2) - length2, "*");
  #else
  length2 += sprintf(toSend2 + length2, "*");
  #endif

  length2 += sprintf(toSend2 + length2, "XX\r\n");

  Serial2.print(toSend2);
  Serial2.flush();

  // Binary output 3
  binaryOutputNumber = 3;
  char toSend3[DEFAULT_WRITE_BUFFER_LONG];

  #if VN_HAVE_SECURE_CRT
  int length3 = sprintf_s(toSend3, sizeof(toSend3), "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, fields.asyncMode, fields.rateDivisor, groups);
  #else
  int length3 = sprintf(toSend3, "$VNWRG,%u,%u", 74 + binaryOutputNumber, vn::protocol::uart::ASYNCMODE_NONE, 0, 0); // turn off binary output 2
  #endif

  #if VN_HAVE_SECURE_CRT
  length3 += sprintf_s(toSend3 + length3, sizeof(toSend3) - length3, "*");
  #else
  length3 += sprintf(toSend3 + length3, "*");
  #endif

  length3 += sprintf(toSend3 + length3, "XX\r\n");

  Serial2.print(toSend3);
  Serial2.flush();

}

void turnOffAsciiOutput() { // VNOFF

  char toSend[DEFAULT_WRITE_BUFFER_MIDIUM];

  #if VN_HAVE_SECURE_CRT
	size_t length = sprintf_s(toSend, size, "$VNWRG,06,%u,%u", ador, port);
	#else
	size_t length = sprintf(toSend, "$VNWRG,06,%u", vn::protocol::uart::VNOFF);
	#endif

  length += sprintf(toSend + length, "*XX\r\n");

  Serial2.print(toSend);
  Serial2.flush();

}

void checkBinaryOff(uint8_t binaryOutputNumber) {

  char toSend[DEFAULT_WRITE_BUFFER_SIZE];

  #if VN_HAVE_SECURE_CRT
  size_t length = sprintf_s(toSend, sizeof(toSend), "$VNRRG,%u*", 74 + binaryOutputNumber);
  #else
  size_t length = sprintf(toSend, "$VNRRG,%u*", 74 + binaryOutputNumber);
  #endif

  length += sprintf(toSend + length, "XX\r\n");

  Serial2.print(toSend);
  Serial2.flush();

  int index = 0;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  while (Serial2.available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
    receiveBuffer[index++] = Serial2.read();
  }

  Serial.println(receiveBuffer);

}

void checkAsciiOff() {

  char toSend[DEFAULT_WRITE_BUFFER_SIZE] = "$VNRRG,06*XX\r\n";
  Serial2.print(toSend);
  Serial2.print(toSend);
  Serial2.flush();

  int index = 0;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  while (Serial2.available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
    receiveBuffer[index++] = Serial2.read();
  }

  Serial.println(receiveBuffer);

}

void readDefaultAsciiOutput_INS_LLA() {

  int index = 0;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  bool start_countdown = false;
  int count_down = 2;

  while (1) {
    if (Serial2.available()) {
      char data = Serial2.read();

      if (data == '$') {
        receiveBuffer[index++] = data;

        while (1) {

          if (Serial2.available()) {
            data = Serial2.read();
            receiveBuffer[index++] = data;

            if (start_countdown) {
              count_down--;
            }
          }

          if (data == '*') {
            start_countdown = true;
          }

          if (count_down <= 0) {
            break;
          }

        }

        break;

      }

    }
  }

  Serial.println(receiveBuffer);
}

void readDefaultAsciiOutput_INS_LLA_40Hz() {

  if (timer_vectornav_read.check() && Serial2.available()) {

    char data = Serial2.read();

    if (data != '$')
      return;

    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    bool start_countdown = false;
    int count_down = 2;

    receiveBuffer[index++] = data;  // put '$' here

    while (count_down > 0) {

      if (Serial2.available()) {
        data = Serial2.read();
        receiveBuffer[index++] = data;

        if (start_countdown) {
          count_down--;
        }
      }

      if (data == '*') {
        start_countdown = true;
      }

    }

    Serial.println("Ask Vector Nav something ^^");
    Serial.println("Let's get raw reponse first");
    Serial.print("Default async ascii output: ");

    Serial.println(receiveBuffer);

    Serial.println();

  } // end of timer.check()
}




