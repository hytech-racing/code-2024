// Code to read from 12 separate 8x1 multiplexers. On each tick, this code
// should select the next set of 12 multiplexers and read them all.

#include <cmath>
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <iostream>
#include <string.h>

// Delay between each read cycle (to allow analog signal to settle).
const int delay_milliseconds = 250;

// Left to right is MSB to LSB.
const int select_pins[] = {4, 3, 2}; // 2 is LSB, 4, is MSB
const int number_of_select_pins = sizeof(select_pins)/sizeof(select_pins[0]);

// 12 separate 8x1 multiplexers (for 96 total thermistors).
const int analog_pins[] = {17, 16, 19, 18, 21, 20, 23, 22, 25, 24, 15, 14};
const int number_of_analog_pins = sizeof(analog_pins)/sizeof(analog_pins[0]);

// Creates an output array for thermistor readings.
int thermistor_readings[number_of_analog_pins * (int) pow(2.0, number_of_select_pins)];

// This is the "counter" that will count upwards indefinitely
int counter = 0;

// SD card constants
Sd2Card card;
File myFile;
String FILE_NAME;



void setup() {

  //I'm not sure if this is valid or not. I'm intending to add seconds_since_epoch to the file name in order to prevent data from being overwritten.
  FILE_NAME = strcat(strcat("data_", std::to_string(now()).c_str()), ".csv"); 
  
  Serial.begin(9600);

  for (int i : select_pins) {
    pinMode(i, OUTPUT);
  }

  for (int i : analog_pins) {
    pinMode(i, INPUT_PULLUP);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  // Initializes SD card
  card.init(SPI_HALF_SPEED, BUILTIN_SDCARD);
  SD.remove(FILE_NAME.c_str()); // Erases the file if it already exists. This SHOULDN'T ever be an issue, since the data is timestamped, but it might.

  printHeaderToSD(); //Puts the header into the CSV

}

void loop() {
  // Declares a new array to convert 'counter' back into binary using the bitwise AND operator.
  // This array will have the format [0, 0, 0] or [0, 1, 0], etc, where the MSB is on the left and LSB is on the right.
  int selected_pin = counter % (int) pow(2.0, number_of_select_pins);
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
    printDataToSerial();
    printDataToSD();
  }

  counter++;
  delay(delay_milliseconds); // Will likely need some delay here for analog inputs to settle.
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

  myFile = SD.open(FILE_NAME.c_str(), FILE_WRITE);

  myFile.print("Time (S),");

  for (int m = 0; m < 12; m++) {
    for (int t = 0; t < 8; t++) {
      myFile.print("M");
      myFile.print(m);
      myFile.print("-T");
      myFile.print(t);
      myFile.print(",");
    }
  }

  myFile.println("");

  myFile.close();

}

void printDataToSD() {

  // Open the file
  myFile = SD.open(FILE_NAME.c_str(), FILE_WRITE);

  myFile.print(counter * delay_milliseconds / 1000); //Prints time elapsed (sec) in the leftmost column
  myFile.print(",");

  for (u_int i = 0; i < sizeof(thermistor_readings)/sizeof(thermistor_readings[0]); i++) {
    myFile.print(convertAnalogToCelsius(thermistor_readings[i]));
    myFile.print(",");
  }

  myFile.println("");

  myFile.close();

}

double convertAnalogToCelsius(int analog) {

  if (analog >= 1023) {
    return -1;
  }

  int BETA = 3492;

  int totalResistance = (analog * 3300) / (1023 - analog);
  double kelvin = (298.15 * BETA) / (BETA + 298.15 * std::log(totalResistance / 10000)/std::log(M_E));

  double celsius = kelvin - 273.15;

  return celsius;

}