#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define bright 10

#define UUID_SERVICE          "d91fdc86-46f8-478f-8dec-ebdc0a1188b2"
#define UUID_CHARACTERISTIC   "56100987-749a-4014-bc22-0be2f5af59d0"
#define serverName "MasterESP32"
//start bluetooth
static bool bleClientConnected = false;
uint8_t mac[6];

  

class BLECallbacks: BLEServerCallbacks {
   void onConnect(BLEServer* pServer) {
      Serial.println("BLE Client Connected!");
      bleClientConnected = true;
      neopixelWrite(RGB_BUILTIN,0,bright,0);//green color
    };
    void onDisconnect(BLEServer* pServer) {
      Serial.println("BLE Client Disconnected!");
      bleClientConnected = false;
      neopixelWrite(RGB_BUILTIN,bright,0,0);
    };
};
inline void bluetoothStart(){
 BLEDevice::init(serverName);
 BLEServer *bleServer = BLEDevice::createServer();
 BLEService *bleService = bleServer->createService(UUID_SERVICE);
  //service setup
 BLECharacteristic *bleCharacteristic = bleService->createCharacteristic(UUID_CHARACTERISTIC, BLECharacteristic::PROPERTY_READ);  
 bleCharacteristic->setValue(&mac[0],6);
 bleCharacteristic->setBroadcastProperty(true);

  //start service
  bleService->start();
  //advertising part
  BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
  bleAdvertising->addServiceUUID(UUID_SERVICE);
  bleAdvertising->setScanResponse(true);
  bleAdvertising->setMinPreferred(0x06);
  bleAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  bleServer->setCallbacks(new BLECallbacks());
  }
//get and set MAC
void getMAC(){
   WiFi.mode(WIFI_MODE_STA);
    esp_wifi_get_mac(WIFI_IF_STA, mac);
  }

  
void setup() {
  Serial.begin(115200);
  Serial.println("get MAC");
  getMAC();
  bluetoothStart();
}

void loop() {
  // put your main code here, to run repeatedly:

}
