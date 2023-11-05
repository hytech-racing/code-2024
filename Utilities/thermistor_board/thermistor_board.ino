// This is the "counter" that will cycle from 0 to 7 and then back
uint8_t counter = 0x00;

// Must set these 3 later. Left to right is MSB to LSB.
const int select_pins[] = {3, 2, 1};

// Must set later.
const int analog_pins[] = {10, 11, 12, 13, 14, 15, 16, 17};

void setup() {
  
  for (int i : select_pins) {
    pinMode(i, OUTPUT);
  }

  for (int i : analog_pins) {
    pinMode(i, INPUT_PULLUP);
  }

}

void loop() {
  // Declares a new array to convert 'counter' back into binary.
  // This array will have the format [0, 0, 0] or [0, 1, 0], etc, where the MSB is on the left and LSB is on the right.
  int counter_binary[] = {(counter & 0x04 == 0x00) ? 0 : 1, (counter & 0x02 == 0x00) ? 0 : 1, (counter & 0x01 == 0x00) ? 0 : 1};

  for (int i = 0; i < 3; i++) {
    digitalWrite(select_pins[i], counter_binary[i] == 0 ? LOW : HIGH);
  }

  int thermistor_readings[8];
  for (int i = 0; i < 8; i++) {
    thermistor_readings[i] = analogRead(analog_pins[i]);
  }

  // WRITE VALUES TO FILE HERE

  // increment counter but keep in the range [0, 7]
  counter++;
  if (counter > 7) {
    counter = 0;
    // Any other data logging stuff should occur here
  }
}
