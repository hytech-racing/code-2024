//from car
#include <WiFi.h>
#include <esp_wifi.h>

const char *ssid = "TechGeorgia";
const char *password = "";
uint8_t mac[6];
String macS;
const char* serverName = "http://";

static bool slaveConnectedCAN = false;
//signal LED
void setLED(char x) {
  switch (x) {
    case 'r':
      neopixelWrite(RGB_BUILTIN, 20, 0, 0); // Red
      break;
    case 'g':
      neopixelWrite(RGB_BUILTIN, 0, 20, 0); // Green
      break;
    case 'b':
      neopixelWrite(RGB_BUILTIN, 0, 0, 20); // Blue
      break;
  }
};
//server set up
WiFiServer server(80);
String header;
//start wifi
void getMAC() {
  WiFi.mode(WIFI_AP_STA);
  esp_wifi_get_mac(WIFI_IF_STA, mac);
  for (int i = 0; i < sizeof(mac); i++) {
    macS += (String(mac[i]) + ",");
  }
  Serial.println("MAC is" + macS);
  Serial.println(WiFi.macAddress());
}
void serverSetup() {
  Serial.println("beginning hotspot");
  WiFi.softAP(ssid, NULL);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  serverName += WiFi.softAPIP();
  serverName += '/';
  server.begin();
}
inline void broadCast() {
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {
    setLED('g');
    Serial.println("new client");

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (header.indexOf("GET /mac") >= 0) {
            Serial.println("Client Get MAC");
            client.println("mac=" + WiFi.macAddress() + "///"); //send mac, with ending of the message with ///
          } else if (header.indexOf("GET /can") >= 0) {
            slaveConnectedCAN = true;
            client.stop();
          }
          Serial.println(header);
          header = "";
        }
        neopixelWrite(RGB_BUILTIN, 0, 20, 20);

      }

      //Serial.print("dead 2");
    }
    neopixelWrite(RGB_BUILTIN, 20, 20, 20);
  } else {
    setLED('f');
  }
}

void setup() {
  Serial.begin(115200);
  setLED('r');
  getMAC();
  serverSetup();
  while (!(slaveConnectedCAN)) {
    broadCast();
    Serial.print("dead 5");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(header);
}
