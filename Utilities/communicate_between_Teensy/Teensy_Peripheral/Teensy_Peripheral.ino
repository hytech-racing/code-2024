//SPI SLAVE (ARDUINO)
//SPI COMMUNICATION BETWEEN TWO ARDUINO 
//CIRCUIT DIGEST
//Pramoth.T
#include <avr/interrupt.h>
#include<SPI.h>
#define LEDpin 7
#define buttonpin 2

volatile boolean received;
volatile byte Slavereceived,Slavesend;

int buttonvalue;
int x;

void onSPIDataReceived() {
  Slavereceived = SPDR; // Dummy data to initiate SPI transfer and read the received data
//  Slavereceived = SPI.transfer(0x00);
//  for (uint8_t i = 0; i < 8; )
  Slavereceived = digitalRead(MOSI);
  received = true; // Sets the 'received' flag to true to indicate data has been received
}

void setup()
{
  Serial.begin(115200);  

//  pinMode(buttonpin,INPUT);               // Setting pin 2 as INPUT
//  pinMode(LEDpin,OUTPUT);                 // Setting pin 7 as OUTPUT
  pinMode(SCK, INPUT);
  pinMode(SS, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);                   //Sets MISO as OUTPUT (Have to Send data to Master IN

//  SPI.begin();

  SPCR |= _BV(SPE);                       //Turn on SPI in Slave Mode

  received = false;

//  SPI.attachInterrupt();                  //Interuupt ON is set for SPI commnucation
  attachInterrupt(digitalPinToInterrupt(SS), onSPIDataReceived, FALLING);
}

//ISR(SPI_STC_vect)                        //Inerrrput routine function 
//{
//  Slavereceived = SPDR;         // Value received from master if store in variable slavereceived
//  received = true;                        //Sets received as True 
//}

void loop()
{
  if(received)                            //Logic to SET LED ON OR OFF depending upon the value recerived from master
   {
      Serial.println(Slavereceived);
      if (Slavereceived==1) 
      {
//        digitalWrite(LEDpin,HIGH);         //Sets pin 7 as HIGH LED ON
        Serial.println("Slave LED ON");
      }
      else
      {
//        digitalWrite(LEDpin,LOW);          //Sets pin 7 as LOW LED OFF
        Serial.println("Slave LED OFF");
      }

      received = false;

//      buttonvalue = digitalRead(buttonpin);  // Reads the status of the pin 2      
//
//      if (buttonvalue == HIGH)               //Logic to set the value of x to send to master
//      {
//        x=1;
//      }
//      else
//      {
//        x=0;
//      }

    x = 1;

    Slavesend=x;

    SPDR = Slavesend;                           //Sends the x value to master via SPDR
//    SPI.transfer(Slavesend);

    delay(1000);
  }
}
