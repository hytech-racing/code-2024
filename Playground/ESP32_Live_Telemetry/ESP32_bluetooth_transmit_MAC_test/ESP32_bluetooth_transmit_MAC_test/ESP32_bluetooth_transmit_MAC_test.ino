
inline void startDiscovery() {

}
inline void stopDiscovery() {
}
inline void flashBlueLED() {
  // This method will flash the Blue LED at an interval
}
inline void setRedLED(bool ledOn) {
  // This method will switch the Red LED on or Off
}


// We use an Enum to define the Mode of our Device
enum DeviceMode {
  Waiting,     // Not Discovering, not timed out
  Discovering, // We're in Discovery mode
  Discovered,  // Discovery Succeeded
  Failed,      // Discovery Failed (Timed Out)
};
DeviceMode deviceMode = Waiting; // We are initially Waiting



enum ButtonState {
  ButtonDown, // The button is being pressed/held
  ButtonUp    // The button has been released
};
ButtonState buttonState;
inline ButtonState getButtonState() {
  return digitalRead(PIN_BUTTON) == HIGH ? ButtonDown : ButtonUp;  void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
