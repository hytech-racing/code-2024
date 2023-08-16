#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
#include <Metro.h>

Metro CAN_timer = Metro(2); // Timer that spaces apart writes for CAN messages so as to not saturate CAN bus
Metro print_timer = Metro(500);
//Metro timer_CAN_em_forward(100);

// OUTBOUND CAN MESSAGES
EM_measurement em_measurement;
EM_status em_status;

// CAN OBJECT AND VARIABLE DECLARATIONS
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> ENERGY_METER_CAN;
CAN_message_t msg;

void setup() {
  Serial.begin(115200);
  ENERGY_METER_CAN.begin();
  ENERGY_METER_CAN.setBaudRate(500000);
  ENERGY_METER_CAN.enableMBInterrupts();
  ENERGY_METER_CAN.onReceive(parse_energy_meter_can_message);
  Serial.println("Receive test");
}

void loop() {
  ENERGY_METER_CAN.events();
  // Serial.println("Receive test");
 while (ENERGY_METER_CAN.read(msg)) { // Receive a message on CAN
   if (msg.id == ID_EM_MEASUREMENT) {
       Serial.print("Received 0x");
       Serial.print(msg.id, HEX);
       Serial.print(": ");
       for (unsigned int i = 0; i < msg.len; i++) {
           Serial.print(msg.buf[i], HEX);
           Serial.print(" ");
       }
       Serial.println();
   }
 }

  // Serial.println();
  // Serial.println("Parsed data");
  // Serial.print("Voltage: ");
  // Serial.println((float)em_measurement.get_voltage() * 1.5258789063e-005);
  // Serial.print("Current: ");
  // Serial.println((float)em_measurement.get_current() * 1.5258789063e-005);
  // Serial.print("Voltage gain: ");
  // Serial.println(em_status.get_voltage_gain());
  // Serial.print("Current gain: ");
  // Serial.println(em_status.get_current_gain());
  // Serial.print("Overvltage flag: ");
  // Serial.println(em_status.get_overvoltage());
  // Serial.print("Overvpower flag: ");
  // Serial.println(em_status.get_overpower());
  // Serial.print("Logging flag: ");
  // Serial.println(em_status.get_logging());
  // delay(100);
  
//  forward_CAN_em();
}

void parse_energy_meter_can_message(const CAN_message_t& RX_msg) {
  CAN_message_t rx_msg = RX_msg;
//  if (print_timer.check()) {
//    Serial.println(rx_msg.buf);
//    Serial.println(rx_msg.buf);
//    Serial.println();
//  }
  switch (rx_msg.id) {
    case ID_EM_MEASUREMENT:   em_measurement.load_from_emeter(rx_msg.buf);    break;
    case ID_EM_STATUS:        em_status.load(rx_msg.buf);         break;
  }
}
