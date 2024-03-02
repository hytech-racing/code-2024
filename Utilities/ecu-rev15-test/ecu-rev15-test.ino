
#include "MCU_rev15_dfs.h"
#include "Metro.h"
Metro timer_watchdog_timer = Metro(10);

void setup() {
  pinMode(BRAKE_LIGHT_CTRL, OUTPUT);
  pinMode(FR_CS, OUTPUT);
  pinMode(FL_CS, OUTPUT);
  pinMode(ADC_CS, OUTPUT);
  
  // change to input if comparator is PUSH PULL
  pinMode(INVERTER_EN, OUTPUT);
  pinMode(INVERTER_24V_EN, OUTPUT);

  pinMode(WATCHDOG_INPUT, OUTPUT);
  // the initial state of the watchdog is high
  // this is reflected in the static watchdog_state
  // starting high
  digitalWrite(WATCHDOG_INPUT, HIGH);
  pinMode(SOFTWARE_OK, OUTPUT);
  digitalWrite(SOFTWARE_OK, HIGH);
  digitalWrite(INVERTER_24V_EN, HIGH);
  digitalWrite(INVERTER_EN, HIGH);
  digitalWrite(FR_CS, HIGH);
  digitalWrite(FL_CS, HIGH);
  digitalWrite(ADC_CS, HIGH);
  delay(5000);

}

inline void software_shutdown() {
  digitalWrite(SOFTWARE_OK, HIGH);
  //mcu_status.set_software_is_ok(true);

  // check inputs
  // BMS heartbeat has not arrived within time interval

  // add BMS software checks
  // software ok/not ok action
//  if (mcu_status.get_software_is_ok()) {
//    digitalWrite(SOFTWARE_OK, HIGH); //eventually make this HIGH only if software is ok
//  } else {
//    digitalWrite(SOFTWARE_OK, LOW);
//  }
  /* Watchdog timer */
  if (timer_watchdog_timer.check()) {
    static bool watchdog_state = HIGH;
    watchdog_state = !watchdog_state;
    digitalWrite(WATCHDOG_INPUT, watchdog_state);
  }

}
void loop() {
  // put your main code here, to run repeatedly:
  software_shutdown();
  
}
