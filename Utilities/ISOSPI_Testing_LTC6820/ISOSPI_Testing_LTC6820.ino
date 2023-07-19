#include <SPI.h>

#define CS 10
#define SCk_SPEED 1000000


void setup() {
  // put your setup code here, to run once:
  pinMode(CS, OUTPUT);

  digitalWrite(CS, HIGH);

  Serial.begin(115200);

//  SPISettings settings(SCK_SPEED, MSBFIRST, SPI_MODE0);

  SPI.begin();

//  SPI.beginTransaction(settings);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t message_sent = 0x22;
  uint8_t message_received;
  uint8_t message_received1;
  uint8_t message_received2;

  digitalWrite(CS, LOW);

  SPI.beginTransaction(SPISettings(SCk_SPEED, MSBFIRST, SPI_MODE0));

  message_received = SPI.transfer(message_sent);
  message_received1 = SPI.transfer(0);
  message_received2 = SPI.transfer(0); 

  SPI.endTransaction();

  digitalWrite(CS, HIGH);

  Serial.println("Message received:");
  Serial.println(message_received);
  Serial.println(message_received1);
  Serial.println(message_received2);

  delay(2000);

}
