#include <SD.h>
#include <MTP_Teensy.h>
#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
#include <kinetis_flexcan.h>
#include <Wire.h>
//#include <TimeLib.h>
#include <Time.h>
#include <Metro.h>

Metro timer = Metro(1000);
Metro flush = Metro(100);
Metro write = Metro(10);
int loops = 0;
uint32_t writecount = 0;
uint32_t tottime = 0;
uint32_t beforetime = 0;
File logger;

#define STR_SZ 4096

//char string[] = "1546300806143,D7,8,E29CD198CA9EACC5\n ";

uint8_t str[STR_SZ];

void setup() {
   Serial.begin(115200);
  // put your setup code here, to run once:
    for(size_t i = 0; i < STR_SZ; i++) {
      str[i] = (uint8_t) 'a';
    }
  
  /* Set up SD card */
    Serial.println("Initializing SD card...");
    if (!SD.begin(BUILTIN_SDCARD)) { // Begin Arduino SD API (Teensy 3.5)
        Serial.println("SD card failed or not present");
    }


    char filename[] = "data0000.CSV";
    for (uint8_t i = 0; i < 10000; i++) {
        filename[4] = i / 1000     + '0';
        filename[5] = i / 100 % 10 + '0';
        filename[6] = i / 10  % 10 + '0';
        filename[7] = i       % 10 + '0';
        if (!SD.exists(filename)) {
            logger = SD.open(filename, (uint8_t) O_WRITE | (uint8_t) O_CREAT); // Open file for writing
            break;
        }
        if (i == 9999) { // If all possible filenames are in use, print error
            Serial.println("All possible SD card log filenames are in use - please clean up the SD card");
        }
    }
    
    if (logger) {
        Serial.println("Successfully opened SD file");
    } else {
        Serial.println("Failed to open SD file");
    }
    
    //logger.println("time,msg.id,msg.len,data"); // Print CSV heading to the logfile
    //logger.flush();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (timer.check()) {
    Serial.printf("Rate: %u Time: %u Loops: %u\n", writecount, tottime, loops);
    tottime = 0;
    writecount = 0;
    loops = 0;
  }
  if (flush.check()) {
    beforetime = millis();
    logger.flush();
    tottime = tottime + (millis()-beforetime);
  }
  if (write.check()) {
    beforetime = millis();
    logger.write(str, STR_SZ);
    tottime = tottime + (millis()-beforetime);
    writecount += STR_SZ;
  }
  loops++;
}
