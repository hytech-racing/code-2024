// Code to read from 12 separate 8x1 multiplexers. On each tick, this code
// should select the next set of 12 multiplexers and read them all.

#include <cmath>

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

void setup() {
  
  Serial.begin(9600);

  for (int i : select_pins) {
    pinMode(i, OUTPUT);
  }

  for (int i : analog_pins) {
    pinMode(i, INPUT_PULLUP);
  }

  pinMode(LED_BUILTIN, OUTPUT);

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

    // Puts garbage data in the specified slot (temporary)
    // thermistor_readings[i * (int) pow(2.0, number_of_select_pins) + selected_pin] = i * (int) pow(2.0, number_of_select_pins) + selected_pin;

    // Actual code (uncomment when thermistors are connected)
    thermistor_readings[i * (int) pow(2.0, number_of_select_pins) + selected_pin] = analogRead(analog_pins[i]);
  }

  // Prints the entire batch of data once all thermistors have been read
  if (selected_pin == 0) {
    printDataToSerial();
  }

  counter++;
  delay(delay_milliseconds); // Will likely need some delay here for analog inputs to settle.
}

void printDataToSerial() {

  Serial.print(counter * delay_milliseconds / 1000); //Prints time elapsed (MS) in the leftmost column
  Serial.print(",");

  for (u_int i = 0; i < sizeof(thermistor_readings)/sizeof(thermistor_readings[0]); i++) {
    Serial.print(thermistor_readings[i]);
    Serial.print(",");
  }
  Serial.println("");

}
