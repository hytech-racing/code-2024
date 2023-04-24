#include "BLEDevice.h"
#include <Wire.h>
uint8_t mac[6];
#define serverName "MasterESP32"
#define UUID_SERVICE          "d91fdc86-46f8-478f-8dec-ebdc0a1188b2"
#define UUID_CHARACTERISTIC   "56100987-749a-4014-bc22-0be2f5af59d0"

static boolean doConnect = false;
static boolean connected = false;
static BLEAddress *pServerAddress;
 
//Characteristic that we want to read
bool connectToServer(BLEAddress pAddress) {
   BLEClient* pClient = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
 
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(UUID_SERVICE);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(UUID_SERVICE);
    return (false);
  }
static BLERemoteCharacteristic* macCharacteristic;
//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};
void bool getMACfromBT(){
    macCharacteristic = pRemoteService->getCharacteristic(UUID_CHARACTERISTIC);
  if (macCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
    }
    //Assign call back
    macCharacteristic->registerForNotify()
  }
}
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
