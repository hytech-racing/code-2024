#include <FlexCAN_T4.h>
#include "HyTech_CAN.h"
#include <Metro.h>

FlexCAN_T4<CAN2> CAN;
CAN_message_t msg;
CAN_message_t msg2;
MCU_pedal_readings pedal{};
SAB_lap_times lap_times{};
MCU_load_cells load_cells{};
int state = 1;

int interval = 160000;
Metro state_change(interval);
Metro send_can(100);
Metro load_cell_timer(100);

// pedal reading CAN message constants
Metro send_pedal_readings(10);
int prev_reading = 500;
bool forward_pedal = true;
int pedal_interval = 10;

// load cell CAN message constants
int prev_loadcell_reading = 0;
int loadcell_interval = 1;
bool loadcell_forwards = true;

void setup() {
  Serial.begin(115200);
  CAN.begin();
  CAN.setBaudRate(500000);
  CAN.enableMBInterrupts();
}

void loop() {
  CAN.events();

  if(state == 2 || state == 3) {
    state_change.interval(1000);
  } else {
    state_change.interval(interval);
  }

  if (state_change.check()) {
    state++;
    if (state >= 3) {
      state = 0;
    }
  }

  // // prev time = 0, best time = 1, delta = 2, current = 3
  int time_1_type = 0;

  // // best time
  int time_2_type = 1;

  int time_1 = 1000 + millis();
  lap_times.set_time_1(time_1);
  int time_2 = millis();
  lap_times.set_time_2(time_2);

  lap_times.set_state(state);
  lap_times.set_time_1_type(time_1_type);
  lap_times.set_time_2_type(time_2_type);

  if (send_can.check()) {
    msg.len = sizeof(lap_times);
    msg.id = ID_SAB_LAP_TIMES;

    lap_times.write(msg.buf);
    CAN.write(msg);
    send_can.reset();
  } 

  if(send_pedal_readings.check()) {
    int curr = prev_reading;
    msg2.id = ID_MCU_PEDAL_READINGS; // pedal reading ID
    msg2.len = sizeof(pedal);

    if(prev_reading >= 2140) {
      forward_pedal = false;
    } else if (prev_reading <= 500) {
      forward_pedal = true;
    }

    if (forward_pedal) {
      curr += pedal_interval;
    } else {
      curr -= pedal_interval;
    }

    prev_reading = curr;

    pedal.set_accelerator_pedal_1(curr);
    pedal.write(msg2.buf);
    CAN.write(msg2);
    send_pedal_readings.reset();
  }
  
  // LOAD CELLS
  if (load_cell_timer.check()) {
    int curr = prev_loadcell_reading;
    msg2.id = ID_MCU_LOAD_CELLS;
    msg2.len = sizeof(load_cells);

    if(curr >= 100) {
      loadcell_forwards = false;
    } else if (curr <= 0) {
      loadcell_forwards = true;
    }

    if(loadcell_forwards) {
      curr += loadcell_interval;
    } else {
      curr -= loadcell_interval;
    }

    prev_loadcell_reading = curr;

    Serial.println(curr);

    load_cells.set_FL_load_cell(curr);
    load_cells.set_FR_load_cell(100-curr);
    load_cells.set_RL_load_cell(curr);
    load_cells.set_RR_load_cell(100-curr);
    load_cells.write(msg2.buf);
    CAN.write(msg2);
    load_cell_timer.reset();
  }
}
