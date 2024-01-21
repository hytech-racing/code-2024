// Code to read from 12 separate 8x1 multiplexers. On each tick, this code
// should select the next set of 12 multiplexers and read them all.

#include <cmath>
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <iostream>
// #include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#define BETA 3492

// Delay between each read cycle (to allow analog signal to settle).
const int delay_milliseconds = 250;

// Left to right is MSB to LSB.
const int select_pins[] = {4, 3, 2}; // 2 is LSB, 4, is MSB
const int number_of_select_pins = sizeof(select_pins)/sizeof(select_pins[0]);

// 12 separate 8x1 multiplexers (for 96 total thermistors).
const int analog_pins[] = {17, 16, 19, 18, 21, 20, 23, 22, 25, 24, 15, 14};
const int number_of_analog_pins = sizeof(analog_pins)/sizeof(analog_pins[0]);

// Creates an output array for thermistor readings.
int thermistor_readings[number_of_analog_pins * (1<<number_of_select_pins)];

// This is the "counter" that will count upwards indefinitely
int counter = 0;

// SD card constants
// String FILE_NAME = "thermistorData";
String FILE_NAME = "testingData_2024-01-21";



void setup() {

  Serial.begin(9600);
  while(!Serial) { // Wait for SD to open
    ;
  }

  // setSyncProvider(RTC.get());   // the function to get the time from the RTC
  // if(timeStatus() != timeSet) {
  //    Serial.println("Unable to sync with the RTC");
  // } else {
  //    Serial.println("RTC has set the system time"); 
  // }

  //I'm not sure if this is valid or not. I'm intending to add seconds_since_epoch to the file name in order to prevent data from being overwritten.
  char dataPrefix[] = "thermistorData_";
  char csvSuffix[] = ".csv";
  //FILE_NAME = String(dataPrefix) + String(hour()) + "_" + String(minute()) + "_" + String(second()) + String(csvSuffix);
  FILE_NAME = String(dataPrefix) + FILE_NAME + String(csvSuffix);

  // Set up the MUX selecter pins
  for (int i : select_pins) {
    pinMode(i, OUTPUT);
  }

  // Set up the analog read pins
  for (int i : analog_pins) {
    pinMode(i, INPUT_PULLUP);
  }

  // Indicator light
  pinMode(LED_BUILTIN, OUTPUT);

  // Initializes SD card (From Example code)
  Serial.println("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("Card failed, or not present");
    while (1) {
      // No SD card, so don't do anything more - stay stuck here
    }
  }
  Serial.println("card initialized.");
  
  printHeaderToSD(); //Puts the header into the CSV

}

void loop() {
  // Declares a new array to convert 'counter' back into binary using the bitwise AND operator.
  // This array will have the format [0, 0, 0] or [0, 1, 0], etc, where the MSB is on the left and LSB is on the right.
  int selected_pin = counter % (1<<number_of_select_pins);
  int counter_binary[] = {(counter & 0x00000004) == 0 ? 0 : 1, (counter & 0x00000002) == 0 ? 0 : 1, (counter & 0x00000001) == 0 ? 0 : 1};

  // digitalWrite(select_pins[2], counter_binary[2]);
  for (int i = 0; i < number_of_select_pins; i++) {
    digitalWrite(select_pins[i], counter_binary[i] == 0 ? LOW : HIGH);
  }
  
  // Make the LED_BUILTIN blink every time the next batch is sampled (by making it blink with the LSB of counter_binary)
  digitalWrite(LED_BUILTIN, counter_binary[number_of_select_pins-1] == 0 ? LOW : HIGH);

  for (int i = 0; i < number_of_analog_pins; i++) {
    // Writes data into the thermistor_readings array in their respective slots, such that
    // the multiplexer referenced by analog_pins[n] will have its data stored contiguously
    // in thermistor_readings in slots 0-11, 12-23, 24-35, etc.

    // Actual code (uncomment when thermistors are connected)
    thermistor_readings[i * (int) pow(2.0, number_of_select_pins) + selected_pin] = analogRead(analog_pins[i]);
  }

  // Prints the entire batch of data once all thermistors have been read
  if (selected_pin == 0) {
    printDataToSDAndSerial();
  }

  counter++;
  delay(delay_milliseconds); // Delays to allow time for data to settle
}

void printDataToSerial() {

  Serial.print(counter * delay_milliseconds / 1000); //Prints time elapsed (sec) in the leftmost column
  Serial.print(",");

  for (u_int i = 0; i < sizeof(thermistor_readings)/sizeof(thermistor_readings[0]); i++) {
    Serial.print(convertAnalogToCelsius(thermistor_readings[i]));
    Serial.print(",");
  }
  Serial.println("");

}

void printHeaderToSD() {

  // Builds the header String
  String headerString = "Time(sec),";
  for (int m = 0; m < number_of_analog_pins; m++) {
    for (int t = 0; t < 1<<number_of_select_pins; t++) {
      headerString += "M" + String(m) + "-T" + String(t) + ",";
    }
  }

  // open the file.
  File dataFile = SD.open(FILE_NAME.c_str(), FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(headerString);
    dataFile.close();
    // print to the serial port too:
    Serial.println("Successfully opened file. \"" + FILE_NAME + "\"");
    Serial.println(headerString);
  } else {
    // if the file isn't open, pop up an error:
    Serial.println("error opening " + String(FILE_NAME));
  }

}

void printDataToSDAndSerial() {

  // Builds the data String
  String dataString = String(counter * delay_milliseconds / 1000) + ",";
  for (u_int i = 0; i < sizeof(thermistor_readings)/sizeof(thermistor_readings[0]); i++) {
    dataString += String(convertAnalogToCelsius(thermistor_readings[i])) + ",";
  }

  // open the file.
  File dataFile = SD.open(FILE_NAME.c_str(), FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  } else {
    // if the file isn't open, pop up an error:
    Serial.println("error opening " + String(FILE_NAME));
  }

}

double convertAnalogToCelsius(int analog) {

  if (analog > 1023) {
    return -2;
  } else if (analog == 1023) {
    return -1;
  }

  double totalResistance = (analog * 3300) / (double)(1023 - analog);
  double kelvin = (298.15 * BETA) / (BETA + 298.15 * std::log(totalResistance / 10000)/std::log(M_E));

  double celsius = kelvin - 273.15;

  return celsius;

}
