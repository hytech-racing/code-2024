//on ground
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "TechGeorgia";
const char* pwd = "";
uint8_t mac[6];
String serverName = "http://192.168.4.1/";
static bool slaveConnectedCAN = false;
static bool payloadRecieved = false;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
WiFiClient client;
String header = "";
String payload = "";
IPAddress server(192,168,4,1);
void setLED(char x){
  switch (x){
    case 'r':
        neopixelWrite(RGB_BUILTIN,20,0,0); // Red
        break;
    case 'g':
         neopixelWrite(RGB_BUILTIN,0,20,0); // Green
         break;
    case 'b':
         neopixelWrite(RGB_BUILTIN,0,0,20); // Blue
         break;
    case 'f':
          neopixelWrite(RGB_BUILTIN,0,0,0);//off
          break;
    }
  };
 uint8_t parseGET(String macS){
  
  }
 void startWiFi(){
  WiFi.begin(ssid,NULL);
  Serial.println("Connecting");
   while(WiFi.status() != WL_CONNECTED) {
    setLED('r');
    delay(500);
    Serial.print(".");
    setLED('f');
  } 
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  setLED('g');
  }
void recieveMAC(){
  if(client.connect(server,80)){
    Serial.println("connected to server");
    Serial.print("getting mac");
    client.print("GET /mac");
    client.print('\n');//end of get message
    unsigned long currentMillis = millis();
  }
  }
 void clientRead(){
   if(client.available()){
            char c = client.read(); 
             Serial.write(c);
             header +=c;
             if(header.indexOf("mac=")>=0){
              payload += c;
              }else if (header.indexOf("///")>=0){
                payloadRecieved = true;
                header = "";
                }
             }
  }
  void parseMAC(String msg){
    
    }
void setup() {
  Serial.begin(115200);  
  startWiFi();
  recieve();
  delay(1000);
  while(!(payloadRecieved)){
    clientRead();
    parseMAC(payload);
    }
}

void loop() {
 if(client.available()){
            char c = client.read(); 
             Serial.write(c);
             header +=c;}
}
