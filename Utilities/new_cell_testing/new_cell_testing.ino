/*
  Leonid Pozdneev
  Created January 2019

  Jeff Ding
  2-15-19

  This code controls Battery Discharge Monitoring Board. It collects voltage, current and internal resistance
  data from 4 channels on the board and then prints it to Arduino Serial in a tab or comma delimited format
  Use TeraTerm to continuously log large serial files directly to .txt

  In this code, each of 4 channels of the board can be in 3 states: WAIT, DISCHARGE, CHARGE, DONE.

  All channels are in WAIT state if the contactor is not powered. When the contactor is powered,
  if the voltage on a channel is above 3 Volts, the channel enters DISCHARGE state.

  In DISCHARGE state,
  the contactor relay is closed which allowes cell to runn current and discharge (this is when data is
  printed to the Serial port). Once the voltage on channel goes below the set threshold, it is put into
  DONE state, the relay is opened, and the printing of data is stopped.
  data r
*/

//*****************************************************************************************
//********CONFIGURATION********************************************************************
//*****************************************************************************************

int mode = 1;
//mode = 0 means you want to discharge to 0 SOC
//mode = 1 means you want to charge in 0.1 SOC increments
//mode = 2 means you want to cycle discharge and charge

double       END_VOLTAGE   = 2.750;      // voltage threshold for end of test

const int    timestep      = 20;         // datalog timestep (milliseconds)
bool         pulsing_on    = true;       // if pulsing should be used in middle of cycle
int          V_end         = 0;          // 0 = instantaneous voltage ends cycle, 1 = rolling average window ends cycle, 2 = predicted OCV ends cycle
bool         manual_offset = false;      // use manual offset for current sensor voltage offset

int          start_delay   = 5 * 1000;   // time to log data before start milliseconds
int          end_delay     = 60 * 1000;  // time to log data after end milliseconds

const int    channels      = 4;
const int    rollingwin    = 100;        // rolling average window needs to be small compared to pulse time & battery time constant for IR calculations
char*        delimiter     = ",";       // "," comma for (CSV), "\t" for tab delimited files

//*****************************************************************************************
//********CONFIGURATION********************************************************************
//*****************************************************************************************



//////////Data Storage/////////////////////////////////////////////////////////////////////
//////////Arrays for storing information about cells

//////////Real time Parameters
double v_read[channels];                         // cells' voltage reading overwritten per iteration
double i_read[channels];                         // cells' current reading overwritten per iteration

double cell_voltage[channels][rollingwin];      // cells' voltage reading w/ history per timestep
double cell_current[channels][rollingwin];      // cells' current reading w/ history per timestep

double cell_voltage2[channels][rollingwin];     // cells' voltage reading (temp array for shifting)
double cell_current2[channels][rollingwin];     // cells' current reading (temp array for shifting)
double v_avg[channels];                         // used for calculating rolling avg
double i_avg[channels];                         // used for calculating rolling avg
double v_last[channels];                        // used for calculating cell IR
double i_last[channels];                        // used for calculating cell IR

double test_resistance[channels];      // Calculated resistance of the discharge power resistor
int    contactor_command[channels];    // Digital high/low used for contactor (used for post-processing)


//////////State Machine/////////////////////////////////////////////////////////////////////
//////////Each channel can be in 4 states: WAIT -> DISCHARGE -> DONE | NOCELL
//////////The following vatriables are used to keep track of the state of each cell.
int state[channels]; // stores the state of each cell
int low_v[channels];

const int CYCLE         = 0;
const int DONE          = 1;
const int RECHARGE      = 2;
const int NOCELL        = 3;


bool            contactor_voltage_high  = true;   // indicates whether the contactor is powered or not (true = powered)
unsigned int    contactor_voltage       = 0;       // reading on the CONTACTOR_PWR_SENSE (the value is between 0 and 1023, it is NOT in volts)

bool            started[channels]              = {false};  // stores if channel has started testing
unsigned long   test_time[channels]            = {0};     // stores whether start_millis has been recorded or not
unsigned long   start_millis[channels]         = {10306, 10306, 10306, 10306};  // stores the value of millis() when the discharging started; emperically derived
signed long     end_millis[channels]           = {0};     // stores the value of millis() when the discharging ended

double periodStart;
double periodLast;
double period;
double periodSec;
double elapsedtime = 0;
double energyCount[2];
//////////State Machine/////////////////////////////////////////////////////////////////////


//////////Hardware Config/////////////////////////////////////////////////////////////////////
#include <ADC_SPI.h>
#include <Metro.h>
//////////Create an adc object
ADC_SPI adc;
//////////Create a metro timer object
Metro timer[channels]  = Metro(timestep, 1); // return true based on timestep period; ignore missed calls;

//////////Assign teensy pins
const int CONTACTOR_PWR_SENSE = 20;
const int SWITCH[channels] = {A1, A2, A3, A4}; //{15, 16, 17, 18};
const int CONTACTOR_VLT_THRESHOLD = 474; // 7 V

//////////Conversion factors for calculating voltage and current
double voltage_conversion_factor      = 5.033333333333333333 / 4095;   // determined by testing
double current_conversion_factor      = 150   / 1.500;  // L01Z150S05 current sensor outputs 4 V at 150 A, and 2.5 V at 0 A
long int    current_offset[4]         = {0, 0, 0, 0};   // Used to determine the zero current offset of the current senesor, by default it is 2.500V
int    calibration_reads              = 500;            // number of readings for 0 offset calibration

//////////Hardware Config/////////////////////////////////////////////////////////////////////


//////////Functions///////////////////////////////////////////////////////////////////////////
void CellDataLog(int i) {

  if (timer[i].check()) {

    // Make a copy of the array to prepare for shifting
    for (int j = 0; j < rollingwin; j++) {
      cell_voltage2[i][j] = cell_voltage[i][j];
      cell_current2[i][j] = cell_current[i][j];
    }
    // Shift array by one index at every timestep
    for (int j = 0; j < rollingwin - 1; j++) {
      cell_voltage[i][j + 1] = cell_voltage2[i][j];
      cell_current[i][j + 1] = cell_current2[i][j];
    }
    cell_voltage[i][0] = v_read[i]; // update first value
    cell_current[i][0] = -1 * i_read[i];

    //Print data to Serial with delimiters to form columns:
    Serial.print(energyCount[1], 6);    Serial.print("\t");
    Serial.print(elapsedtime, 6);    Serial.print("\t");

    Serial.print(state[1]);               Serial.print(delimiter);
    Serial.print(cell_voltage[1][0], 4);   Serial.print(delimiter);
    Serial.print(cell_current[1][0]);     Serial.print(delimiter);
    Serial.print(i_read[1]);     Serial.print(delimiter);

 


  }
}

//char rx_byte = 0;
//
//void check_input() {
//  if (Serial.available() > 0) {    // is a character available?
//    rx_byte = Serial.read();       // get the character
//
//    // check if a number was received
//    if (rx_byte = 'd');
//    state
//  } // end: if (Serial.available() > 0)
//}

double getBatteryVoltage(int channel) {
  // Method to read the cell voltage in VOLTS
  // Arguments: channel (Note: in the code, channels are numbered 0-3, when on the board they are 1-4, used for voltage sense)
  double voltage_reading = ((double) adc.read_adc(channel)) * voltage_conversion_factor;
  return voltage_reading;
}

double getBatteryCurrent(int channel) {
  // Method to read the cell current in Amps
  // Arguments: channel (Note: in the code, channels are numbered 0-3, when on the board they are 4-7 which are designated as current sense)
  double voltage_reading = ((double) adc.read_adc(channel + 4)) * voltage_conversion_factor;
  double current_reading = (voltage_reading - (current_offset[channel] * voltage_conversion_factor)) * current_conversion_factor;

  return current_reading;
}

int startTime;
//////////Functions///////////////////////////////////////////////////////////////////////////

void setup() {

  adc = ADC_SPI();
  Serial.begin(115200);
  delay(10000); // need a delay after Serial.begin()

  // Set the contactor pin as INPUT
  pinMode(CONTACTOR_PWR_SENSE, INPUT);

  for (int i = 0; i < channels; i++) {

    // Set the mode for SWITCH pins and set them to LOW. When SWITCH pins are low, the relays are open, and cells are not discharging
    contactor_command[i] = 0;
    pinMode(SWITCH[i], OUTPUT);
    digitalWrite(SWITCH[i], contactor_command[i]);

    // Set the default state for all cells as WAIT
    state[i] = NOCELL;

    // Used to find the 0 A offset for the current sensors
    for (int j = 0; j < calibration_reads; j++) {
      current_offset[i] = current_offset[i] + adc.read_adc(i + 4);
    }
    current_offset[i] = round(current_offset[i] / (double)calibration_reads);

    if (manual_offset) {
      current_offset[i] = 2048;
    }

    for (int j = 0; j < rollingwin; j++) { // fill entire array with same value
      cell_voltage[i][j]         = getBatteryVoltage(i); // read cell's voltage
      cell_current[i][j]         = getBatteryCurrent(i);
      cell_voltage2[i][j]        = cell_voltage[i][j]; // save in temp array
      cell_current2[i][j]        = cell_current[i][j];
    }
  }

  // Print Column Headers
  Serial.print("Channel 1 State");              Serial.print(delimiter);
  Serial.print("Channel 1 Voltage");            Serial.print(delimiter);
  Serial.print("Channel 1 Current");            Serial.print(delimiter);

  startTime = millis();
}

void loop() {
  periodLast = periodStart;
  periodStart = micros();
  period = periodStart - periodLast;
  periodSec = period / 1000000;
  elapsedtime += periodSec;
  for (int i = 0; i < 2; i++) {
    energyCount[i] += i_read[i] * periodSec * 0.0002777777;

    contactor_voltage = analogRead(CONTACTOR_PWR_SENSE); // read contactor voltage
    v_read[i]     = getBatteryVoltage(1); // read cell's voltage
    i_read[i]     = getBatteryCurrent(2); // read cell's current
    if (mode == 0 or mode == 2) {
      if (state[i] == NOCELL and getBatteryVoltage(i) > 3.5) {
        state[i] = CYCLE;
      }
  
    }
    if (mode == 1){
      if (state[i] == NOCELL and getBatteryVoltage(i) > 3.0) {
        state[i] = RECHARGE;
      }  
    }
    if (state[i] == CYCLE or state[i] == RECHARGE) {
      if (mode == 1) {
        if (energyCount[i] > 1.8) {
          state[i] = DONE;
        }
        digitalWrite(SWITCH[0], HIGH);
      }
      if (mode == 0 or mode == 2){
        int v_sum = 0;
        int v_avg = 0;
        for (int k = 0; k < 100; k++) {
            v_sum += cell_voltage[i][k];

        }
        v_avg = v_sum / 100;
        if (v_avg < END_VOLTAGE) {
            state[i] = DONE;
        }
      }
      
      if (mode == 2) {
        if (startTime == millis()) {
          digitalWrite(SWITCH[0], HIGH);
        }
        if (millis() == startTime + 1000) {
          digitalWrite(SWITCH[0], LOW);
        }
        if (millis() == startTime + 1200) {
          digitalWrite(SWITCH[1], HIGH);
        }
        if (millis() == startTime + 2200) {
          digitalWrite(SWITCH[1], LOW);
        }
        if (millis() == startTime + 2400) {
          startTime = millis();
        }
      }
      if (mode==0){
        digitalWrite(SWITCH[1], HIGH);
      }


    }
    else if (state[i] == DONE) {
      digitalWrite(SWITCH[0], LOW);
      digitalWrite(SWITCH[1], LOW);
    }
    if (millis() % 20 == 0) {
      CellDataLog(i);
    }

  }

}
