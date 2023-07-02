#define PIN_BUTTON    34 // GPIO34
#define PIN_LED_RED   23 // GPIO23
#define PIN_LED_BLUE  22 // GPIO22
#define BUTTON_HOLD_TIME    3000 // The number of millis for which we must hold the button
#define DISCOVERY_TIMEOUT   30000 // 30 seconds in milliseconds for Timeout
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_wifi.h>
#include <WiFi.h>
#define UUID_SERVICE          "d91fdc86-46f8-478f-8dec-ebdc0a1188b2"
#define UUID_CHARACTERISTIC   "56100987-749a-4014-bc22-0be2f5af59d0"

/*----------------------------------------------------------------------------------------------*/
// We use an Enum to define the Mode of our Device
//enum DeviceMode {
//  Waiting,     // Not Discovering, not timed out
//  Discovering, // We're in Discovery mode
//  Discovered,  // Discovery Succeeded
//  Failed,      // Discovery Failed (Timed Out)
//};
/*-----------------------------------------------------------------------------------------------*/
//enum doesn't work

//define deviceMode
#define Waiting 0
#define Discovering 1
#define Discovered 2
#define Failed 3

int deviceMode = Waiting; // We are initially Waiting


inline void startDiscovery() {
  // This method will switch on BLE and set it up to broadcast the WiFi Mac Address
}
inline void stopDiscovery() {
  // This method will switch off BLE.
}
inline void flashBlueLED() {
  // This method will flash the Blue LED at an interval
}
inline void setRedLED(bool ledOn) {
  // This method will switch the Red LED on or Off
}
/*----------------------------------------------------------------------------------------------*/

//button pull up 
//enum ButtonState {
//  ButtonDown = LOW // The button is being pressed/held
//  ButtonUp    // The button has been released
//}; //this part edited out since enum is broken in the sketch.
#define ButtonDown LOW
#define ButtonUp HIGH
int buttonState;

inline int getButtonState() {
  return digitalRead(PIN_BUTTON) == HIGH ? ButtonDown : ButtonUp;
}
//--------------------------------------------------------------------------------------------------
// button set up
unsigned long discoveryStart; // The millis() value at which Discovery started
// The Loop routine when our Device is in Waiting Mode
unsigned long buttonHoldStart;

// The Loop routine when our Device is in Waiting Mode
inline void loopWaiting() {
  int currentState = getButtonState();

  // Down to Up
  if (buttonState == ButtonDown && currentState == ButtonUp) {
    buttonState = currentState; // Update the global variable accordingly
    buttonHoldStart = 0;
    return; // return none
  }

  // Up to Down
  if (buttonState == ButtonUp && currentState == ButtonDown) {
    // The Button was just pressed down...
    buttonHoldStart = millis();
    buttonState = currentState;
    Serial.println("Button Hold Started");
    return; // return none
  }

  // Held Down
  if (buttonState == ButtonDown && currentState == ButtonDown && millis() > buttonHoldStart + BUTTON_HOLD_TIME) {
    // We now initiate Discovery!
    Serial.println("Initiating Discovery");
    deviceMode = Discovering;
    setRedLED(false);
    discoveryStart = millis();
    buttonHoldStart = discoveryStart;
    startDiscovery();
  }
}
// The Loop routine when our Device is in Discovery Mode
inline void loopDiscovering() {
  flashBlueLED();

  int currentState = getButtonState();

  // Down to Up
  if (buttonState == ButtonDown && currentState == ButtonUp) {
    buttonState = currentState; // Update the global variable accordingly
    buttonHoldStart = 0;
    return; // return none
  }

  // Up to Down
  if (buttonState == ButtonUp && currentState == ButtonDown) {
    // The Button was just pressed down...
    buttonHoldStart = millis();
    buttonState = currentState;
    Serial.println("Button Hold Started");
    return; // return none

  // Held Down OR Timed Out
  if (
       (buttonState == ButtonDown && currentState == ButtonDown && millis() > buttonHoldStart + BUTTON_HOLD_TIME) ||
       (millis() > discoveryStart + DISCOVERY_TIMEOUT)
     ){
    // We now initiate Discovery!
    Serial.println("Cancelling Discovery");
    deviceMode = Waiting;
    setRedLED(true);
    digitalWrite(PIN_LED_BLUE, LOW); // Ensure Blue LED is OFF
    buttonHoldStart = millis();
    stopDiscovery();
  }
}
// The Loop routine when our Device is in Discovery Mode

inline void loopDiscovered() {
  
}
// The Loop routine when our Device is in Failed Mode
inline void loopFailed() {
  
}

/*----------------------------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200); // Set Serial Monitor to 115200 baud
  // Set our Pin Modes
  pinMode(PIN_BUTTON, INPUT);     // Button Input
  pinMode(PIN_LED_RED, OUTPUT);   // Red LED Output
  pinMode(PIN_LED_BLUE, OUTPUT);  // Blue LED Output
  // Get the initial state of our Button
  buttonState = getButtonState();
  
  BLEDevice::init("Bluetooth master demo");

  
}


void loop() {
  switch (deviceMode) {
    case (Waiting):
      loopWaiting();
      break;
    case (Discovering):
      loopDiscovering();
      break;
    case (Discovered):
      loopDiscovered();
      break;
  }
}
