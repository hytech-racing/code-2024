/*
  Use the NEO-D9S L-Band receiver to provide corrections to a ZED-F9x via UBX-RXM-PMP messages
  By: SparkFun Electronics / Paul Clark
  Based on original code by: u-blox AG / Michael Ammann
  v3 updates: Decembe 22nd, 2022
  License: MIT. See license file for more information.

  This example shows how to obtain SPARTN correction data from a NEO-D9S L-Band receiver and push it over I2C to a ZED-F9x.

  This is a proof of concept to show how the UBX-RXM-PMP corrections control the accuracy.

  If you are using the SparkFun Combo Board (SPX-20167), the correction data is transferred from the NEO to the ZED via UART2.
  You don't need to push it over I2C. Doing so just gives the ZED twice as many correction messages.
  Uncomment the "#define noPush" below to disable the I2C push.

  You will need a Thingstream PointPerfect account to be able to access the SPARTN Credentials (L-Band or L-Band + IP Dynamic Keys).
  Copy and paste the Current Key and Next Key into secrets.h.

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  ZED-F9P RTK2: https://www.sparkfun.com/products/16481
  NEO-D9S:      https://www.sparkfun.com/products/19390
  Combo Board:  https://www.sparkfun.com/products/20167

  Hardware Connections:
  Use Qwiic cables to connect the NEO-D9S and ZED-F9x GNSS to your board
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output
*/

#define noPush // Uncomment this line to disable pushing the correction data over I2C. Useful for the combo board which uses UART2 instead.
//#define printGPS
#include "secrets.h" // <- Copy and paste the Current Key and Next Key into secrets.h

#include <SparkFun_u-blox_GNSS_v3.h> //http://librarymanager/All#SparkFun_u-blox_GNSS_v3

SFE_UBLOX_GNSS myGNSS; // ZED-F9x
SFE_UBLOX_GNSS myLBand; // NEO-D9S

const uint32_t myLBandFreq = 1556290000; // Uncomment this line to use the US SPARTN 1.8 service
//const uint32_t myLBandFreq = 1545260000; // Uncomment this line to use the EU SPARTN 1.8 service

#define OK(ok) (ok ? F("  ->  OK") : F("  ->  ERROR!")) // Convert uint8_t into OK/ERROR

GPS_lat_long gps_lat_long;
GPS_other gps_other;

#include "gpsfuncs.h"

int setupGPS() {
  Wire.begin(); //Start I2C

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Begin and configure the ZED-F9x

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial
  uint8_t attempts = 3;
  int gps_init_ok = 0;
  for(int i = 0; i < attempts; i++) {
    if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
    {
      Serial.println(F("u-blox GNSS module not detected at default I2C address. Please check wiring."));
      gps_init_ok = -1;
      delay(1000);
    } else {
      gps_init_ok = 0;
      break;
    }
  }
  if (gps_init_ok != 0) {
    return gps_init_ok;
  }

  Serial.println(F("u-blox GNSS module connected"));

  uint8_t ok = myGNSS.setI2COutput(COM_TYPE_UBX); //Turn off NMEA noise
  if (ok) ok = myGNSS.setI2CInput(COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_SPARTN); //Be sure SPARTN input is enabled
  if (ok) ok = myGNSS.setDGNSSConfiguration(SFE_UBLOX_DGNSS_MODE_FIXED); // Set the differential mode - ambiguities are fixed whenever possible
  if (ok) ok = myGNSS.setNavigationFrequency(5); //Set output in Hz.
  if (ok) ok = myGNSS.setVal8(UBLOX_CFG_SPARTN_USE_SOURCE, 1); // use LBAND PMP message
  if (ok) ok = myGNSS.setVal8(UBLOX_CFG_MSGOUT_UBX_RXM_COR_I2C, 1); // Enable UBX-RXM-COR messages on I2C
  
  //Configure the SPARTN IP Dynamic Keys
  //"When the receiver boots, the host should send 'current' and 'next' keys in one message." - Use setDynamicSPARTNKeys for this.
  //"Every time the 'current' key is expired, 'next' takes its place."
  //"Therefore the host should then retrieve the new 'next' key and send only that." - Use setDynamicSPARTNKey for this.
  // The key can be provided in binary (uint8_t) format or in ASCII Hex (char) format, but in both cases keyLengthBytes _must_ represent the binary key length in bytes.
  if (ok) ok = myGNSS.setDynamicSPARTNKeys(currentKeyLengthBytes, currentKeyGPSWeek, currentKeyGPSToW, currentDynamicKey,
                                           nextKeyLengthBytes, nextKeyGPSWeek, nextKeyGPSToW, nextDynamicKey);

  //if (ok) ok = myGNSS.saveConfiguration(VAL_CFG_SUBSEC_IOPORT | VAL_CFG_SUBSEC_MSGCONF); //Optional: Save the ioPort and message settings to NVM and BBR
  
  Serial.print(F("GNSS: configuration "));
  Serial.println(OK(ok));

  myGNSS.setAutoPVTcallbackPtr(&printPVTdata); // Enable automatic NAV PVT messages with callback to printPVTdata so we can watch the carrier solution go to fixed

  //myGNSS.setRXMCORcallbackPtr(&printRXMCOR); // Print the contents of UBX-RXM-COR messages so we can check if the PMP data is being decrypted successfully

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Begin and configure the NEO-D9S L-Band receiver

  //myLBand.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  int neo_init_ok = 0;
  for(int i = 0; i < attempts; i++) {
    if (myLBand.begin(Wire, 0x43) == false) //Connect to the u-blox module using Wire port
    {
      Serial.println(F("u-blox NEO-D9S not detected at default I2C address. Please check wiring."));
      neo_init_ok = -1;
      delay(1000);
    } else {
      neo_init_ok = 0;
      break;
    }
  }
  if (neo_init_ok != 0) {
    return neo_init_ok;
  }
  Serial.println(F("u-blox NEO-D9S connected"));

  myLBand.newCfgValset(); // Create a new Configuration Interface message - this defaults to VAL_LAYER_RAM_BBR (change in RAM and BBR)
  myLBand.addCfgValset32(UBLOX_CFG_PMP_CENTER_FREQUENCY,   myLBandFreq); // Default 1539812500 Hz
  myLBand.addCfgValset16(UBLOX_CFG_PMP_SEARCH_WINDOW,      2200);        // Default 2200 Hz
  myLBand.addCfgValset8(UBLOX_CFG_PMP_USE_SERVICE_ID,      0);           // Default 1 
  myLBand.addCfgValset16(UBLOX_CFG_PMP_SERVICE_ID,         21845);       // Default 50821
  myLBand.addCfgValset16(UBLOX_CFG_PMP_DATA_RATE,          2400);        // Default 2400 bps
  myLBand.addCfgValset8(UBLOX_CFG_PMP_USE_DESCRAMBLER,     1);           // Default 1
  myLBand.addCfgValset16(UBLOX_CFG_PMP_DESCRAMBLER_INIT,   26969);       // Default 23560
  myLBand.addCfgValset8(UBLOX_CFG_PMP_USE_PRESCRAMBLING,   0);           // Default 0
  myLBand.addCfgValset64(UBLOX_CFG_PMP_UNIQUE_WORD,        16238547128276412563ull); 
  myLBand.addCfgValset8(UBLOX_CFG_MSGOUT_UBX_RXM_PMP_I2C,   1); // Ensure UBX-RXM-PMP is enabled on the I2C port 
  myLBand.addCfgValset8(UBLOX_CFG_MSGOUT_UBX_RXM_PMP_UART1, 1); // Output UBX-RXM-PMP on UART1
  myLBand.addCfgValset8(UBLOX_CFG_UART2OUTPROT_UBX, 1);         // Enable UBX output on UART2
  myLBand.addCfgValset8(UBLOX_CFG_MSGOUT_UBX_RXM_PMP_UART2, 1); // Output UBX-RXM-PMP on UART2
  myLBand.addCfgValset32(UBLOX_CFG_UART1_BAUDRATE,         38400); // match baudrate with ZED default
  myLBand.addCfgValset32(UBLOX_CFG_UART2_BAUDRATE,         38400); // match baudrate with ZED default
  ok = myLBand.sendCfgValset(); // Apply the settings
  
  Serial.print(F("L-Band: configuration "));
  Serial.println(OK(ok));

  myLBand.softwareResetGNSSOnly(); // Do a restart

  //myLBand.setRXMPMPmessageCallbackPtr(&pushRXMPMP); // Call pushRXMPMP when new PMP data arrives. Push it to the GNSS  
  //myGNSS.setI2CpollingWait(100);
}

void gpsLoop() {
  myGNSS.checkUblox(); // Check for the arrival of new GNSS data and process it.
  myGNSS.checkCallbacks(); // Check if any GNSS callbacks are waiting to be processed.
  //myLBand.checkUblox(); // Check for the arrival of new PMP data and process it.
  //myLBand.checkCallbacks(); // Check if any LBand callbacks are waiting to be processed.
}
