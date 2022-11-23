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

int mode = 2;
//mode = 0 means you want to discharge to 0 SOC
//mode = 1 means you want to charge in 0.1 SOC increments
//mode = 2 means you want to cycle discharge and charge

double END_VOLTAGE = 2.750;  // voltage threshold for end of test

const int timestep = 20;     // datalog timestep (milliseconds)
bool pulsing_on = true;      // if pulsing should be used in middle of cycle
int V_end = 0;               // 0 = instantaneous voltage ends cycle, 1 = rolling average window ends cycle, 2 = predicted OCV ends cycle
bool manual_offset = false;  // use manual offset for current sensor voltage offset

int start_delay = 5 * 1000;  // time to log data before start milliseconds
int end_delay = 60 * 1000;   // time to log data after end milliseconds

int relays = 4;
const int rollingwin = 100;  // rolling average window needs to be small compared to pulse time & battery time constant for IR calculations
char* delimiter = ",";       // "," comma for (CSV), "\t" for tab delimited files

//*****************************************************************************************
//********CONFIGURATION********************************************************************
//*****************************************************************************************



//////////Data Storage/////////////////////////////////////////////////////////////////////
//////////Arrays for storing information about cells

//////////Real time Parameters
double v_read;  // cells' voltage reading overwritten per iteration
double i_read;  // cells' current reading overwritten per iteration

double cell_voltage[rollingwin];  // cells' voltage reading w/ history per timestep
double cell_current[rollingwin];  // cells' current reading w/ history per timestep

double cell_voltage2[rollingwin];  // cells' voltage reading (temp array for shifting)
double cell_current2[rollingwin];  // cells' current reading (temp array for shifting)
double v_avg;                      // used for calculating rolling avg
double i_avg;                      // used for calculating rolling avg
double v_last;                     // used for calculating cell IR
double i_last;                     // used for calculating cell IR

double test_resistance;  // Calculated resistance of the discharge power resistor
int contactor_command;   // Digital high/low used for contactor (used for post-processing)

//////////Calculated parameters
int num_pulses = 100;
double cell_dvdt;            // Numerical derivative of voltage change
double cell_didt;            // Numerical derivative of current change
//double cell_IR[num_pulses];  // Calculated cell internal resistance for each pulse
double cell_IR_avg = 0.0020;// Calculated cell internal resistance (averaged), initialize at 2.0 mOhm
double cell_OCV;  

//////////State Machine/////////////////////////////////////////////////////////////////////
//////////Each channel can be in 4 states: WAIT -> DISCHARGE -> DONE | NOCELL
//////////The following vatriables are used to keep track of the state of each cell.
int state;  // stores the state of each cell
int low_v;

const int NOCELL         = 0;
const int CYCLE          = 1;
const int RECHARGE       = 2;
const int STOP           = 3;
const int DISCHARGE      = 4;


bool contactor_voltage_high = true;  // indicates whether the contactor is powered or not (true = powered)
unsigned int contactor_voltage = 0;  // reading on the CONTACTOR_PWR_SENSE (the value is between 0 and 1023, it is NOT in volts)

bool started = { false };                                     // stores if channel has started testing
unsigned long test_time = { 0 };                              // stores whether start_millis has been recorded or not
unsigned long start_millis[4] = { 10306, 10306, 10306, 10306 };  // stores the value of millis() when the discharging started; emperically derived
signed long end_millis = { 0 };                               // stores the value of millis() when the discharging ended

double periodStart;
double periodLast;
double period;
double periodSec;
double elapsedtime = 0;
double energyCount;
//////////State Machine/////////////////////////////////////////////////////////////////////


//////////Hardware Config/////////////////////////////////////////////////////////////////////
#include <ADC_SPI.h>
#include <Metro.h>
//////////Create an adc object
ADC_SPI adc;
//////////Create a metro timer object
Metro timer = Metro(timestep, 1);  // return true based on timestep period; ignore missed calls;

//////////Assign teensy pins
const int CONTACTOR_PWR_SENSE = 20;
const int SWITCH[4] = { A1, A2, A3, A4 };  //{15, 16, 17, 18};
//0 is for high curent recharge, 1 is for high current discharge, 2 is for low current recharge
const int CONTACTOR_VLT_THRESHOLD = 474;        // 7 V
const int temp1 = A7;
const int temp2 = A8;
float temp[2];



//////////Conversion factors for calculating voltage and current
double voltage_conversion_factor = 5.033333333333333333 / 4095;  // determined by testing
double current_conversion_factor = 150 / 1.500;                  // L01Z150S05 current sensor outputs 4 V at 150 A, and 2.5 V at 0 A
long int current_offset[4] = { 0, 0, 0, 0 };                     // Used to determine the zero current offset of the current senesor, by default it is 2.500V
int calibration_reads = 500;                                     // number of readings for 0 offset calibration

//////////Hardware Config/////////////////////////////////////////////////////////////////////


//////////Functions///////////////////////////////////////////////////////////////////////////
void CellDataLog() {

  if (timer.check()) {

    // Make a copy of the array to prepare for shifting
    for (int j = 0; j < rollingwin; j++) {
      cell_voltage2[j] = cell_voltage[j];
      cell_current2[j] = cell_current[j];
    }
    // Shift array by one index at every timestep
    for (int j = 0; j < rollingwin - 1; j++) {
      cell_voltage[j + 1] = cell_voltage2[j];
      cell_current[j + 1] = cell_current2[j];
    }
    cell_voltage[0] = v_read;  // update first value
    cell_current[0] = -1 * i_read;

    // Calculating rolling average
    v_avg = 0;
    i_avg = 0;
    for (int j = 0; j < rollingwin; j++) {
      v_avg = v_avg + cell_voltage[j];
      i_avg = i_avg + cell_current[j];
    }
    v_avg = v_avg / rollingwin;
    i_avg = i_avg / rollingwin;

    cell_dvdt = (cell_voltage[0] - cell_voltage[rollingwin - 1]) / (timestep * rollingwin / (double)1000);
    cell_didt = (cell_current[0] - cell_current[rollingwin - 1]) / (timestep * rollingwin / (double)1000);

    cell_OCV = v_avg + (i_avg * cell_IR_avg); // print calculated OCV


    //    Print data to Serial with delimiters to form columns:
    Serial.print(energyCount, 6);
    Serial.print("\t");
    Serial.print(elapsedtime, 6);
    Serial.print("\t");

    Serial.print(mode);
    Serial.print(state);
//    Serial.print(delimiter);
//    Serial.print(temp[0]);
//    Serial.print(delimiter);
//    Serial.print(temp[1]);
    Serial.print(delimiter);
    Serial.print(cell_voltage[0], 4);
    Serial.print(delimiter);
    Serial.print(cell_current[0]);
    Serial.print(delimiter);
    Serial.print(i_read);
    Serial.println(delimiter);
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
  double voltage_reading = ((double)adc.read_adc(channel)) * voltage_conversion_factor;
  return voltage_reading;
}

double getBatteryCurrent(int channel) {
  // Method to read the cell current in Amps
  // Arguments: channel (Note: in the code, channels are numbered 0-3, when on the board they are 4-7 which are designated as current sense)
  double voltage_reading = ((double) adc.read_adc(channel + 4)) * voltage_conversion_factor;
  double current_reading = (voltage_reading - (current_offset[channel]*voltage_conversion_factor)) * current_conversion_factor;
  return current_reading;
}

float B = 3971.0;
float T1 = 298.15;
float rawVoltage[2];
float voltage[2];
float Rtherm[2];
void getTemp(float temp[2]) {
  rawVoltage[0] = analogRead(temp1);
  rawVoltage[1] = analogRead(temp2);
  voltage[0] = rawVoltage[0] / 1024 * 3.3;
  voltage[1] = rawVoltage[1] / 1024 * 3.3;
  Rtherm[0] = 12010 * voltage[0] / (3.3 - voltage[0]);
  Rtherm[1] = 12010 * voltage[1] / (3.3 - voltage[1]);
  temp[0] = -1 * B * T1 / (T1 * log(8500.0 / Rtherm[0]) - B) - 273.15;
  temp[1] = -1 * B * T1 / (T1 * log(10000.0 / Rtherm[1]) - B) - 273.15;
}

int startTime;
//////////Functions///////////////////////////////////////////////////////////////////////////

void setup() {

  adc = ADC_SPI();
  Serial.begin(115200);
  delay(10000);  // need a delay after Serial.begin()

  // Set the contactor pin as INPUT
  pinMode(CONTACTOR_PWR_SENSE, INPUT);

  for (int i = 0; i < 3; i++) {

    // Set the mode for SWITCH pins and set them to LOW. When SWITCH pins are low, the relays are open, and cells are not discharging
    contactor_command = 0;
    pinMode(SWITCH[i], OUTPUT);
    digitalWrite(SWITCH[i], contactor_command);

    // Set the default state for all cells as WAIT
    state = NOCELL;

    // Used to find the 0 A offset for the current sensors
    for (int j = 0; j < calibration_reads; j++) {
      current_offset[i] = current_offset[i] + adc.read_adc(i + 4);
    }
    current_offset[i] = round(current_offset[i] / (double)calibration_reads);

    if (manual_offset) {
      current_offset[i] = 2048;
    }
    for (int j = 0; j < rollingwin; j++) {     // fill entire array with same value
      cell_voltage[j] = getBatteryVoltage(i);  // read cell's voltage
      cell_current[j] = getBatteryCurrent(i);
      cell_voltage2[j] = cell_voltage[j];  // save in temp array
      cell_current2[j] = cell_current[j];
    }
  }

  // Print Column Headers
  Serial.print("Channel 1 State");
  Serial.print(delimiter);
  Serial.print("Channel 1 Voltage");
  Serial.print(delimiter);
  Serial.print("Channel 1 Current");
  Serial.println(delimiter);

  startTime = millis();
}

void loop() {
  getTemp(temp);  // this call updates the temp array temp[2]
  periodLast = periodStart;
  periodStart = micros();
  period = periodStart - periodLast;
  periodSec = period / 1000000;
  elapsedtime += periodSec;
  energyCount += i_read * periodSec * 0.0002777777;

  contactor_voltage = analogRead(CONTACTOR_PWR_SENSE);  // read contactor voltage
  v_read = getBatteryVoltage(1);                        // read cell's voltage
  i_read = getBatteryCurrent(2);                        // read cell's current
  int v_sum = 0;
  int v_avg = 0;
  for (int k = 0; k < 100; k++) {
    v_sum += cell_voltage[k];
  }
  v_avg = v_sum / 100;
  
  if (mode == 0) {                                        //we want to discharge to 0 SOC or about 3.3v
    if (state == NOCELL && getBatteryVoltage(1) > 3.7) {  //if the cell is not above 3.7 it will not register as being installed. this voltage level can be changed in cde if you need to discharge lower SOC cells
      state = DISCHARGE;
      delay(500);
    }
    if (state == DISCHARGE) {
      digitalWrite(SWITCH[1], HIGH);
      if (temp[0] > 60 || temp[1] > 60 || getBatteryVoltage(1) < 3.0) {
        state = STOP;
      }
    }
    if (state == STOP) {
      digitalWrite(SWITCH[0], LOW);
      digitalWrite(SWITCH[1], LOW);
      digitalWrite(SWITCH[2], LOW);
    }
  }
  if (mode == 1) {                                        //we want to charge in 1.8Ah(0.1 SOC) increments for characterization
    if (state == NOCELL && getBatteryVoltage(1) > 2.5) {  //verifying a cell is installed
      state = RECHARGE;
    }
    if (state == RECHARGE) {
      digitalWrite(SWITCH[2], HIGH);
      if (energyCount > 1.8 || temp[0] > 60 || temp[1] > 60) {
        state = STOP;
      }
    }
    if (state == STOP) {
      digitalWrite(SWITCH[0], LOW);
      digitalWrite(SWITCH[1], LOW);
      digitalWrite(SWITCH[2], LOW);
    }
  }
  if (mode == 2) {  //we want to cycle
    // this involves charge/discharge down to 0 SOC, then recharge slowly to 1 SOC and repeat
    // at any point of voltage or temp goes out of range, STOP
    if (state == NOCELL && getBatteryVoltage(1) > 2.5) {  //verifying a cell is installed
      state = CYCLE;
    }
    if (state == CYCLE) {
      if (temp[0] > 60 || temp[1] > 60 || getBatteryVoltage(1) > 4.2) {  //overtemp, overvoltage protection
        state = STOP;
      }
      //calculate small period average of voltage to know when to stop cycle
      
      if (v_avg < END_VOLTAGE) {
        state = RECHARGE;
      }

      //relay flipping to cycle charge and discharge
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
    if (state == RECHARGE) {
      digitalWrite(SWITCH[2], HIGH);
      if (temp[0] > 60 || temp[1] > 60) {
        state = STOP;
      }
      if (getBatteryVoltage(1) > 4.14) {
        digitalWrite(SWITCH[2], LOW);
        state = CYCLE;
      }
    }
    if (state == STOP) {
      digitalWrite(SWITCH[0], LOW);
      digitalWrite(SWITCH[1], LOW);
      digitalWrite(SWITCH[2], LOW);
    }
  }
  if (millis() % 20 == 0) {
    CellDataLog();
  }
}
