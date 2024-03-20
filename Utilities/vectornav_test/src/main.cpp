/**
 * Test code for VectorNav library development
*/
#include <Arduino.h>

#include "Metro.h"
#include "types.h"

#define IMU_RS232_SPEED 115200
#define DEFAULT_WRITE_BUFFER_SIZE 17
#define DEFAULT_WRITE_BUFFER_LONG 256
#define DEFAULT_READ_BUFFER_SIZE 256
#define BINARY_OUTPUT_GROUP_COUNT 4
#define BINARY_OUTPUT_PAYLOAD_LENGTH 118   // bytes
#define OFFSET_PADDING 24

#define ATOU32 static_cast<uint32_t>(std::atoi(result))
#define NEXT result = getNextData(_data, parseIndex); \
	if (result == NULL) \
		return;

Metro timer_vectornav_read = Metro(25);         // default 40Hz
Metro timer_vectornav_read_binary = Metro(20);  // configured 50Hz

// Function declarations
void queryModelNumber();
void readUserConfiguredBinaryOutput();
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
void ConfigBinaryOutput();

// Global
int counter;

void setup() {
  // Open Serial Monitor
  Serial.begin(115200);

  // Initialize Serial communication for Vector Nav
  Serial2.begin(IMU_RS232_SPEED);
  Serial.println("Serial communication initialized...");
  Serial.println();

  // Init counter
  counter = 0;

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

  ConfigBinaryOutput();
  readUserConfiguredBinaryOutput();


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

void readUserConfiguredBinaryOutput() {
  if (timer_vectornav_read_binary.check() && Serial2.available()) {
    uint8_t data = Serial2.read();

    if (data != 0xFA)
      return;

    int binaryPacketLength = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT + BINARY_OUTPUT_PAYLOAD_LENGTH + 2;  // in bytes: sync, groups, group fields, payload, crc
    uint8_t receiveBuffer[binaryPacketLength];
    int index = 0;

    receiveBuffer[index++] = data;

    while (index < binaryPacketLength) {
      if (Serial2.available())
      {
        data = Serial2.read();
        receiveBuffer[index++] = data;
      }      
    }

    uint8_t syncByte = receiveBuffer[0];

    uint8_t groupsByte = receiveBuffer[1];
    if (groupsByte != 0x2D)
      return;

    uint16_t groupField1 = ((uint16_t)receiveBuffer[3] << 8) | receiveBuffer[2];
    if (groupField1 != 0x1162)
      return;

    uint16_t groupField2 = (receiveBuffer[5] << 8) | receiveBuffer[4];
    if (groupField2 != 0x0084)
      return;

    uint16_t groupField3 = (receiveBuffer[7] << 8) | receiveBuffer[6];
    if (groupField3 != 0x0040)
      return;

    uint16_t groupField4 = (receiveBuffer[9] << 8) | receiveBuffer[8];
    if (groupField4 != 0x0008)
      return;

    uint64_t timeGPS = (receiveBuffer[17] << (8 * 7)) | (receiveBuffer[16] << (8 * 6)) | 
                       (receiveBuffer[15] << (8 * 5)) | (receiveBuffer[14] << (8 * 4)) |
                       (receiveBuffer[13] << (8 * 3)) | (receiveBuffer[12] << (8 * 2)) | 
                       (receiveBuffer[11] << (8 * 1)) | receiveBuffer[10];

    float angularRateBodyX = (receiveBuffer[21] << (8 * 3)) | (receiveBuffer[20] << (8 * 2)) | 
                             (receiveBuffer[19] << (8 * 1)) | receiveBuffer[18];
    float angularRateBodyY = (receiveBuffer[25] << (8 * 3)) | (receiveBuffer[24] << (8 * 2)) | 
                             (receiveBuffer[23] << (8 * 1)) | receiveBuffer[22];
    float angularRateBodyZ = (receiveBuffer[29] << (8 * 3)) | (receiveBuffer[28] << (8 * 2)) | 
                             (receiveBuffer[27] << (8 * 1)) | receiveBuffer[26];

    double latitude = (receiveBuffer[37] << (8 * 7)) | (receiveBuffer[36] << (8 * 6)) | 
                      (receiveBuffer[35] << (8 * 5)) | (receiveBuffer[34] << (8 * 4)) |
                      (receiveBuffer[33] << (8 * 3)) | (receiveBuffer[32] << (8 * 2)) | 
                      (receiveBuffer[31] << (8 * 1)) | receiveBuffer[30];
    double longitude = (receiveBuffer[45] << (8 * 7)) | (receiveBuffer[44] << (8 * 6)) | 
                       (receiveBuffer[43] << (8 * 5)) | (receiveBuffer[42] << (8 * 4)) |
                       (receiveBuffer[41] << (8 * 3)) | (receiveBuffer[40] << (8 * 2)) | 
                       (receiveBuffer[39] << (8 * 1)) | receiveBuffer[38];
    double altitude = (receiveBuffer[53] << (8 * 7)) | (receiveBuffer[52] << (8 * 6)) | 
                      (receiveBuffer[51] << (8 * 5)) | (receiveBuffer[50] << (8 * 4)) |
                      (receiveBuffer[49] << (8 * 3)) | (receiveBuffer[48] << (8 * 2)) | 
                      (receiveBuffer[47] << (8 * 1)) | receiveBuffer[46];

    float accelBodyX = (receiveBuffer[33 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[32 + OFFSET_PADDING] << (8 * 2)) | 
                       (receiveBuffer[31 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[30 + OFFSET_PADDING];
    float accelBodyY = (receiveBuffer[37 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[36 + OFFSET_PADDING] << (8 * 2)) | 
                       (receiveBuffer[35 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[34 + OFFSET_PADDING];
    float accelBodyZ = (receiveBuffer[41 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[40 + OFFSET_PADDING] << (8 * 2)) | 
                       (receiveBuffer[39 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[38 + OFFSET_PADDING];

    uint16_t InsStatus = (receiveBuffer[43 + OFFSET_PADDING] << 8) | receiveBuffer[42 + OFFSET_PADDING];

    float uncompAccelBodyX = (receiveBuffer[47 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[46 + OFFSET_PADDING] << (8 * 2)) | 
                             (receiveBuffer[45 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[44 + OFFSET_PADDING];
    float uncompAccelBodyY = (receiveBuffer[51 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[50 + OFFSET_PADDING] << (8 * 2)) | 
                             (receiveBuffer[49 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[48 + OFFSET_PADDING];
    float uncompAccelBodyZ = (receiveBuffer[55 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[54 + OFFSET_PADDING] << (8 * 2)) | 
                             (receiveBuffer[53 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[52 + OFFSET_PADDING];

    float deltaVelX = (receiveBuffer[59 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[58 + OFFSET_PADDING] << (8 * 2)) | 
                      (receiveBuffer[57 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[56 + OFFSET_PADDING];
    float deltaVelY = (receiveBuffer[63 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[62 + OFFSET_PADDING] << (8 * 2)) | 
                      (receiveBuffer[61 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[60 + OFFSET_PADDING];
    float deltaVelZ = (receiveBuffer[67 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[66 + OFFSET_PADDING] << (8 * 2)) | 
                      (receiveBuffer[65 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[64 + OFFSET_PADDING];

    double posEcef0 = (receiveBuffer[75 + OFFSET_PADDING] << (8 * 7)) | (receiveBuffer[74 + OFFSET_PADDING] << (8 * 6)) | 
                      (receiveBuffer[73 + OFFSET_PADDING] << (8 * 5)) | (receiveBuffer[72 + OFFSET_PADDING] << (8 * 4)) |
                      (receiveBuffer[71 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[70 + OFFSET_PADDING] << (8 * 2)) | 
                      (receiveBuffer[69 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[68 + OFFSET_PADDING];
    double posEcef1 = (receiveBuffer[83 + OFFSET_PADDING] << (8 * 7)) | (receiveBuffer[82 + OFFSET_PADDING] << (8 * 6)) | 
                      (receiveBuffer[81 + OFFSET_PADDING] << (8 * 5)) | (receiveBuffer[80 + OFFSET_PADDING] << (8 * 4)) |
                      (receiveBuffer[79 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[78 + OFFSET_PADDING] << (8 * 2)) | 
                      (receiveBuffer[77 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[76 + OFFSET_PADDING];
    double posEcef2 = (receiveBuffer[91 + OFFSET_PADDING] << (8 * 7)) | (receiveBuffer[90 + OFFSET_PADDING] << (8 * 6)) | 
                      (receiveBuffer[89 + OFFSET_PADDING] << (8 * 5)) | (receiveBuffer[88 + OFFSET_PADDING] << (8 * 4)) |
                      (receiveBuffer[87 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[86 + OFFSET_PADDING] << (8 * 2)) | 
                      (receiveBuffer[85 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[84 + OFFSET_PADDING];

    float velBodyX = (receiveBuffer[95 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[94 + OFFSET_PADDING] << (8 * 2)) | 
                     (receiveBuffer[93 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[92 + OFFSET_PADDING];
    float velBodyY = (receiveBuffer[99 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[98 + OFFSET_PADDING] << (8 * 2)) | 
                     (receiveBuffer[97 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[96 + OFFSET_PADDING];
    float velBodyZ = (receiveBuffer[103 + OFFSET_PADDING] << (8 * 3)) | (receiveBuffer[102 + OFFSET_PADDING] << (8 * 2)) | 
                     (receiveBuffer[101 + OFFSET_PADDING] << (8 * 1)) | receiveBuffer[100 + OFFSET_PADDING];

    uint16_t crc = (receiveBuffer[105 + OFFSET_PADDING] << 8) | receiveBuffer[104 + OFFSET_PADDING];

    Serial.println("Ask Vector Nav something ^^");
    Serial.println("Let's get raw reponse first");

    Serial.println();

    Serial.print("User configured binary output: ");

    Serial.print("Raw string: ");
    for (int i = 0; i < binaryPacketLength; i++)
    {
      Serial.printf("%X ", receiveBuffer[i]);
    }

    Serial.printf("\n\n");

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
  }   // end of timer.check()
}

void ConfigBinaryOutput() {

  // char toSend[256];
  // Packet response;

  uint8_t binaryOutputNumber = 1;
  char toSend[DEFAULT_WRITE_BUFFER_LONG];

  // First determine which groups are present.
  uint16_t groups = 0;
  // if (fields.commonField)
    groups |= 0x0001;         // 0000 0000 0000 0001. common group selected
  // if (fields.timeField)
    // groups |= 0x0002;
  // if (fields.imuField)
    groups |= 0x0004;         // 0000 0000 0000 0100. IMU group selected
  // if (fields.gpsField)
    groups |= 0x0008;         // 0000 0000 0000 1000. GNSS1 group selected
  // if (fields.attitudeField)
    // groups |= 0x0010;
  // if (fields.insField)
    groups |= 0x0020;         // 0000 0000 0010 0000. INS group selected
  // if(fields.gps2Field)
    // groups |= 0x0040;

  // groups = 0010 1101 = 2D

  #if VN_HAVE_SECURE_CRT
  int length = sprintf_s(toSend, sizeof(toSend), "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, fields.asyncMode, fields.rateDivisor, groups);
  #else
  int length = sprintf(toSend, "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, vn::protocol::uart::ASYNCMODE_PORT1, 16, groups); // serial1, 800/16=50Hz, 
  #endif

  // if (fields.commonField)
    #if VN_HAVE_SECURE_CRT
    length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.commonField);
    #else
    length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_TIMEGPS |     // 0001 0001 0110 0010 = 11 62
                                              vn::protocol::uart::COMMONGROUP_ANGULARRATE | 
                                              vn::protocol::uart::COMMONGROUP_POSITION | 
                                              vn::protocol::uart::COMMONGROUP_ACCEL | 
                                              vn::protocol::uart::COMMONGROUP_INSSTATUS);
    #endif
  // if (fields.timeField)
  // 	#if VN_HAVE_SECURE_CRT
  // 	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.timeField);
  // 	#else
  // 	length += sprintf(toSend + length, ",%X", fields.timeField);
  // 	#endif
  // if (fields.imuField)
  	#if VN_HAVE_SECURE_CRT
  	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.imuField);
  	#else
  	length += sprintf(toSend + length, ",%X", vn::protocol::uart::IMUGROUP_UNCOMPACCEL |    // 0000 0000 1000 0100 = 00 84
                                              vn::protocol::uart::IMUGROUP_DELTAVEL);
  	#endif
  // if (fields.gpsField)
  	#if VN_HAVE_SECURE_CRT
  	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.gpsField);
  	#else
  	length += sprintf(toSend + length, ",%X", vn::protocol::uart::GPSGROUP_POSECEF);        // 0000 0000 0100 0000 = 00 40
  	#endif
  // if (fields.attitudeField)
  // 	#if VN_HAVE_SECURE_CRT
  // 	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.attitudeField);
  // 	#else
  // 	length += sprintf(toSend + length, ",%X", fields.attitudeField);
  // 	#endif
  // if (fields.insField)
    #if VN_HAVE_SECURE_CRT
    length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.insField);
    #else
    length += sprintf(toSend + length, ",%X", vn::protocol::uart::INSGROUP_VELBODY);        // 0000 0000 0000 1000 = 00 08
    #endif
  // if(fields.gps2Field)
  //   #if VN_HAVE_SECURE_CRT
  //   length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.gps2Field);
  //   #else
  //   length += sprintf(toSend + length, ",%X", fields.gps2Field);
  //   #endif

  #if VN_HAVE_SECURE_CRT
  length += sprintf_s(toSend + length, sizeof(toSend) - length, "*");
  #else
  length += sprintf(toSend + length, "*");
  #endif

  length += sprintf(toSend + length, "XX\r\n");

  // transaction(toSend, length, waitForReply, &response);

  Serial2.print(toSend);
  Serial2.flush();

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




