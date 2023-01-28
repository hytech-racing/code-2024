#include <Arduino.h>
#include <HyTech.h>

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.printf("%ud\n", mcu.pedal.data.accelerator_pedal_1);
    taskContainer.run();
    delay(1);
}