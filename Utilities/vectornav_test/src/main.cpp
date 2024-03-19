/**
 * Test code for VectorNav library development
*/
#include <Arduino.h>

#include "Metro.h"
#include "types.h"

#define IMU_RS232_SPEED 115200
#define DEFAULT_WRITE_BUFFER_SIZE 17
#define DEFAULT_READ_BUFFER_SIZE 256

#define ATOU32 static_cast<uint32_t>(std::atoi(result))
#define NEXT result = getNextData(_data, parseIndex); \
	if (result == NULL) \
		return;

Metro timer_vectornav_read = Metro(25);  // default 40Hz

// Function declarations
void queryModelNumber();
void parseModelNumber(char *productName);
void queryYawPitchRoll();
void queryYawPitchRollMagneticAccelerationAndAngularRates();
void queryAsyncDataOutputFrequency();
void readDefaultAsciiOutput_INS_LLA();
void readDefaultAsciiOutput_INS_LLA_40Hz();

// Test utilities
void parseAsyncDataOutputFrequency(uint32_t* adof, char *_data);
char* startAsciiPacketParse(char* packetStart, size_t& index);
char* getNextData(char* str, size_t& startIndex);
char* vnstrtok(char* str, size_t& startIndex);

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
  // Test communications
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
  readDefaultAsciiOutput_INS_LLA_40Hz();

  // char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = "$VNRRG,07,40*5C";

  // Serial.println(receiveBuffer);

  // uint32_t reg;
	// parseAsyncDataOutputFrequency(&reg, receiveBuffer);

  // Serial.printf("Async Frequency: %d Hz\n", reg);

  // Attempt to received async data periodically assuming Vecotr Nav is sending

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

// Compose binary output command
void generateBinaryOutputConfigCommand(uint8_t binaryOutputNumber, char *toSend) {
  // char toSend[256];
  // Packet response;

  // First determine which groups are present.
  uint16_t groups = 0;
  // if (fields.commonField)
    groups |= 0x0001;
  // if (fields.timeField)
    // groups |= 0x0002;
  // if (fields.imuField)
    // groups |= 0x0004;
  // if (fields.gpsField)
    // groups |= 0x0008;
  // if (fields.attitudeField)
    // groups |= 0x0010;
  // if (fields.insField)
    groups |= 0x0020;
  // if(fields.gps2Field)
    // groups |= 0x0040;

  #if VN_HAVE_SECURE_CRT
  int length = sprintf_s(toSend, sizeof(toSend), "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, fields.asyncMode, fields.rateDivisor, groups);
  #else
  int length = sprintf(toSend, "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, vn::protocol::uart::ASYNCMODE_PORT1, 16, groups); // serial1, 800/16=50Hz, 
  #endif

  // if (fields.commonField)
    #if VN_HAVE_SECURE_CRT
    length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.commonField);
    #else
    length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_TIMESTARTUP | 
                                              vn::protocol::uart::COMMONGROUP_TIMEGPS | 
                                              vn::protocol::uart::COMMONGROUP_TIMESYNCIN | 
                                              vn::protocol::uart::COMMONGROUP_YAWPITCHROLL | 
                                              vn::protocol::uart::COMMONGROUP_QUATERNION | 
                                              vn::protocol::uart::COMMONGROUP_ANGULARRATE | 
                                              vn::protocol::uart::COMMONGROUP_POSITION | 
                                              vn::protocol::uart::COMMONGROUP_VELOCITY | 
                                              vn::protocol::uart::COMMONGROUP_ACCEL | 
                                              vn::protocol::uart::COMMONGROUP_IMU | 
                                              vn::protocol::uart::COMMONGROUP_MAGPRES | 
                                              vn::protocol::uart::COMMONGROUP_DELTATHETA | 
                                              vn::protocol::uart::COMMONGROUP_INSSTATUS | 
                                              vn::protocol::uart::COMMONGROUP_SYNCINCNT);
    #endif
  // if (fields.timeField)
  // 	#if VN_HAVE_SECURE_CRT
  // 	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.timeField);
  // 	#else
  // 	length += sprintf(toSend + length, ",%X", fields.timeField);
  // 	#endif
  // if (fields.imuField)
  // 	#if VN_HAVE_SECURE_CRT
  // 	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.imuField);
  // 	#else
  // 	length += sprintf(toSend + length, ",%X", fields.imuField);
  // 	#endif
  // if (fields.gpsField)
  // 	#if VN_HAVE_SECURE_CRT
  // 	length += sprintf_s(toSend + length, sizeof(toSend) - length, ",%X", fields.gpsField);
  // 	#else
  // 	length += sprintf(toSend + length, ",%X", fields.gpsField);
  // 	#endif
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
    length += sprintf(toSend + length, ",%X", vn::protocol::uart::INSGROUP_VELBODY);
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

  if (timer_vectornav_read.check()) {

  Serial.println("Ask Vector Nav something ^^");
  Serial.println("Let's get raw reponse first");
  Serial.print("Default async ascii output: ");

  int index = 0;
  char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
  bool start_countdown = false;
  int count_down = 2;

  // while (1) {
    while (Serial2.available()) {
      char data = Serial2.read();

      if (data == '$') {
        receiveBuffer[index++] = data;

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

          // if (count_down <= 0) {
          //   break;
          // }

        }

        break;

      }

    }
  // }

  Serial.println(receiveBuffer);

  Serial.println();

  } // end of timer.check()
}




