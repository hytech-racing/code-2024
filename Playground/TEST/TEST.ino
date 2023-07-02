
#include <WiFi.h>        // Include the Wi-Fi library

const char *ssid = "ESP recieve"; // The name of the Wi-Fi network that will be created
const char *password = "123456ap";   // The password required to connect to it, leave blank for an open network

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() { }
