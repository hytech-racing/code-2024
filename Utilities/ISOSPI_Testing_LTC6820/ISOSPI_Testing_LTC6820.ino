#include <SPI.h>
#include <Mcp320x.h>

#define FL_CS    	33		   // SPI slave select
#define FR_CS     29
#define ADC_VREF    3300     // 3.3V Vref
#define ADC_CLK     1000000  // SPI clock 1.6MHz

// MCP3208 adc(ADC_VREF, 34);              /////////////// 
MCP3204 adc_fr(ADC_VREF, FR_CS);
MCP3204 adc_fl(ADC_VREF, FL_CS);
void setup() {
  // configure PIN mode
  pinMode(FR_CS, OUTPUT);
  pinMode(FL_CS, OUTPUT);
  pinMode(34, OUTPUT);
  
  
  // set initial PIN state
  digitalWrite(FL_CS, HIGH);
  digitalWrite(FR_CS, HIGH);
  digitalWrite(34, HIGH);
  
  // initialize serial
  Serial.begin(115200);

  // initialize SPI interface for MCP3208
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
  
}

void loop() {
   uint32_t t1;
   uint32_t t2;

  // start sampling
   Serial.println("Reading...");

   t1 = micros();
//  uint16_t accel1 = adc.read(MCP3208::Channel::SINGLE_0);
//  uint16_t accel2 = adc.read(MCP3208::Channel::SINGLE_1);
//  uint16_t brake1 = adc.read(MCP3208::Channel::SINGLE_2);            ///// mcp3204
//  uint16_t brake2 = adc.read(MCP3208::Channel::SINGLE_1);            ////
uint16_t FR_LC_1 = adc_fr.read(MCP3204::Channel::SINGLE_1);
uint16_t FL_LC_1 = adc_fl.read(MCP3204::Channel::SINGLE_1);
uint16_t FR_LC_2 = adc_fr.read(MCP3204::Channel::SINGLE_2);
uint16_t FL_LC_2 = adc_fl.read(MCP3204::Channel::SINGLE_2);
// uint16_t FL_LC = 0;
  //uint16_t raw = analogRead(40);
   t2 = micros();

uint16_t fr_1 = adc_fr.toAnalog(FR_LC_1);
uint16_t fl_1 = adc_fl.toAnalog(FL_LC_1);
uint16_t fr_2 = adc_fr.toAnalog(FR_LC_2);
uint16_t fl_2 = adc_fl.toAnalog(FL_LC_2);
// uint16_t b1 = adc.toAnalog(brake1);
// uint16_t b2= adc.toAnalog(brake2);
   Serial.print("fr value: ");
   Serial.print(FR_LC_1);
   Serial.print(" (");
   Serial.print(fr_1);
   Serial.print(" mV)    ");
   Serial.print(FR_LC_2);
   Serial.print(" (");
   Serial.print(fr_2);
   Serial.println(" mV)");

   Serial.print("fl value: ");
   Serial.print(FL_LC_1);
   Serial.print(" (");
   Serial.print(fl_1);
   Serial.print(" mV)    ");
   Serial.print(FL_LC_2);
   Serial.print(" (");
   Serial.print(fl_2);
   Serial.println(" mV)");

   // get analog value
//   uint16_t acc1_val = adc.toAnalog(accel1);
//   uint16_t acc2_val = adc.toAnalog(accel2);
//   uint16_t  b1_val = adc.toAnalog(b1_val);
//   uint16_t b2_val = adc.toAnalog(b2_val);

//   // readed value
//   Serial.print("accel1 value: ");
//   Serial.print(accel1);
//   Serial.print(" (");
//   Serial.print(acc1_val);
//   Serial.println(" mV)");
//
//   Serial.print("accel2 value: ");
//   Serial.print(accel2);
//   Serial.print(" (");
//   Serial.print(acc2_val);
//   Serial.println(" mV)");
//
// Serial.print("brake1 value: ");
//   Serial.print(brake1);
//   Serial.print(" (");
//   Serial.print(b1);
//   Serial.println(" mV)");

// Serial.print("brake2 value: ");
//   Serial.print(brake2);
//   Serial.print(" (");
//   Serial.print(b2);
//   Serial.println(" mV)");


   // sampling time
   Serial.print("Sampling time: ");
   Serial.print(static_cast<double>(t2 - t1) / 1000, 4);
   Serial.println("ms");

   delay(1000);

  // SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);

  // SPI.beginTransaction(settings);

  // // activate ADC with chip select
  // digitalWrite(SPI_CS, LOW);

  // delayMicroseconds(1);

  // // digitalWrite(SPI_CS, HIGH);

  // delayMicroseconds(16);

  // digitalWrite(SPI_CS, LOW);

  // // // send first command byte
  // // mSpi->transfer(cmd.hiByte);
  // // // send second command byte and receive first(msb) 4 bits
  // // adc.hiByte = mSpi->transfer(cmd.loByte) & 0x0F;
  // // // receive last(lsb) 8 bits
  // // adc.loByte = mSpi->transfer(0x00);

  // delayMicroseconds(1);

  // // deactivate ADC with slave select
  // digitalWrite(SPI_CS, HIGH);

  // delayMicroseconds(1);

  // SPI.endTransaction();

  // Serial.println("I've uploaded code");

  // delay(1000);

}
//#include <SPI.h>
//#include <Mcp320x.h>
//
//#define SPI_CS      29       // SPI slave select
//#define ADC_VREF    3300     // 3.3V Vref
//#define ADC_CLK     1000000  // SPI clock 1.6MHz
//
//MCP3204 adc(ADC_VREF, SPI_CS);
//
//void setup() {
//  // configure PIN mode
//  pinMode(SPI_CS, OUTPUT);
//  
//  
//  // set initial PIN state
//  digitalWrite(SPI_CS, HIGH);
//
//  // initialize serial
//  Serial.begin(115200);
//
//  // initialize SPI interface for MCP3208
//  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
//  SPI.begin();
//  SPI.beginTransaction(settings);
//  
//}
//
//void loop() {
//   uint32_t t1;
//   uint32_t t2;
//
//  // start sampling
//   Serial.println("Reading...");
//
//   t1 = micros();
//  uint16_t accel1 = adc.read(MCP3204::Channel::SINGLE_0);
//  uint16_t accel2 = adc.read(MCP3204::Channel::SINGLE_1);
//  uint16_t brake1 = adc.read(MCP3204::Channel::SINGLE_2);
//  uint16_t brake2 = adc.read(MCP3204::Channel::SINGLE_3);
//  //uint16_t raw = analogRead(40);
//   t2 = micros();
//
//   // get analog value
//   uint16_t acc1_val = adc.toAnalog(accel1);
//   uint16_t acc2_val = adc.toAnalog(accel2);
//   uint16_t  b1_val = adc.toAnalog(b1_val);
//   uint16_t b2_val = adc.toAnalog(b2_val);
//
//   // readed value
//   Serial.print("accel1 value: ");
//   Serial.print(accel1);
//   Serial.print(" (");
//   Serial.print(acc1_val);
//   Serial.println(" mV)");
//
//   Serial.print("accel2 value: ");
//   Serial.print(accel2);
//   Serial.print(" (");
//   Serial.print(acc2_val);
//   Serial.println(" mV)");
//
//Serial.print("brake1 value: ");
//   Serial.print(brake1);
//   Serial.print(" (");
//   Serial.print(b1_val);
//   Serial.println(" mV)");
//
//Serial.print("brake2 value: ");
//   Serial.print(brake2);
//   Serial.print(" (");
//   Serial.print(b2_val);
//   Serial.println(" mV)");
//
//
//   // sampling time
//   Serial.print("Sampling time: ");
//   Serial.print(static_cast<double>(t2 - t1) / 1000, 4);
//   Serial.println("ms");
//
//   delay(1000);
//
//  // SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
//
//  // SPI.beginTransaction(settings);
//
//  // // activate ADC with chip select
//  // digitalWrite(SPI_CS, LOW);
//
//  // delayMicroseconds(1);
//
//  // // digitalWrite(SPI_CS, HIGH);
//
//  // delayMicroseconds(16);
//
//  // digitalWrite(SPI_CS, LOW);
//
//  // // // send first command byte
//  // // mSpi->transfer(cmd.hiByte);
//  // // // send second command byte and receive first(msb) 4 bits
//  // // adc.hiByte = mSpi->transfer(cmd.loByte) & 0x0F;
//  // // // receive last(lsb) 8 bits
//  // // adc.loByte = mSpi->transfer(0x00);
//
//  // delayMicroseconds(1);
//
//  // // deactivate ADC with slave select
//  // digitalWrite(SPI_CS, HIGH);
//
//  // delayMicroseconds(1);
//
//  // SPI.endTransaction();
//
//  // Serial.println("I've uploaded code");
//
//  // delay(1000);
//
//}
//
////#include "MCU_rev15_dfs.h"
//
//
//
///*
// * Teensy Pin definitions
// */
//#define INVERTER_EN 9
//#define INVERTER_24V_EN 7
//
//#define MC_MAX_SPEED 20000
//
//#define BRAKE_LIGHT_CTRL 6
//
//#define ECU_CLK 13 
//#define ECU_SDI 12
//#define ECU_SDO 11
//
//
//#define CAN_ECU_RX2 0
//#define CAN_ECU_TX2 1
//#define CAN_ECU_RX3 30
//#define CAN_ECU_TX3 31
//
//#define ADC_CS 34
//#define FL_CS 33
//#define FR_CS 29
//#define ADC_VREF 3300
//
//
//
//#define INTERTIA_SENSE 20
//#define SOFTWARE_OK_READ 25 // SHDN_F_READ Watchdog Combined
//#define BOTS_SENSE_OK_READ 26 // SHDN_G_READ
//#define BOTS_OK_READ 24  // SHDN_B_READ
//#define IMD_OK_READ 10 // SHDN_C_READ
//#define BMS_OK_READ 16 // SHDN_D_READ
//#define BSPD_OK_READ 39 // SHDN_E_READ
//#define SOFTWARE_OK 8
//// digital outputs
//#define WATCHDOG_INPUT 32
//
///*
// * ADC pin definitions
// */
//#define ADC_BRAKE_1_CHANNEL 5
//#define ADC_BRAKE_2_CHANNEL 3
//#define ADC_ACCEL_1_CHANNEL 2
//#define ADC_ACCEL_2_CHANNEL 1
//
//#define ADC_STEERING_1_CHANNEL 7
//
//#define ADC_GLV_READ_CHANNEL 4
//
//#define ADC_CURRENT_CHANNEL 6
//#define ADC_REFERENCE_CHANNEL 0
//
//#define ADC_FL_LOAD_CELL_CHANNEL 2
//#define ADC_FR_LOAD_CELL_CHANNEL 2
//#define SUS_POT_FL 1
//#define SUS_POT_FR 1
//
//#define THERM_FR 41
//#define THERM_FL 38
//#define STEERING_2 40
//
///*
// * Shutdown read thresholds
// */
//
//#define SHUTDOWN_HIGH 350 //teensy ADC reading when the shutdown lines are at 5V
//
//
///*
// * Other constants
// */
//#define MIN_HV_VOLTAGE 60               // Volts in V * 1 - Used to check if Accumulator is energized
//
//
//#define ALPHA 0.9772                     // parameter for the sowftware filter used on ADC pedal channels
//#define ADC_SPI_SPEED 1000000            
//#define TORQUE_ADJUSTMENT_VOLTAGE 3.5242 
//
//
//#define LOAD_CELL1_OFFSET -33.98
//#define LOAD_CELL1_SLOPE 0.1711
//#define LOAD_CELL2_OFFSET -35.59
//#define LOAD_CELL2_SLOPE 0.1762
//#define LOAD_CELL3_OFFSET -33.59
//#define LOAD_CELL3_SLOPE 0.192
//#define LOAD_CELL4_OFFSET -64.5
//#define LOAD_CELL4_SLOPE 0.1954
//
// #define LOAD_CELL1_OFFSET 0
// #define LOAD_CELL1_SLOPE 1
// #define LOAD_CELL2_OFFSET 0
// #define LOAD_CELL2_SLOPE 1
// #define LOAD_CELL3_OFFSET 0
// #define LOAD_CELL3_SLOPE 1
// #define LOAD_CELL4_OFFSET 0
// #define LOAD_CELL4_SLOPE 1
//
//#define BMS_HEARTBEAT_TIMEOUT 30000
//#define DASH_HEARTBEAT_TIMEOUT 1000
//#define TIMER_SOFTWARE_ENABLE 100
//
//#define PACK_CHARGE_CRIT_TOTAL_THRESHOLD 420
//#define PACK_CHARGE_CRIT_LOWEST_CELL_THRESHOLD 35000
//
//#define MECH_POWER_LIMIT 63
//#define DC_POWER_LIMIT 79
//
//#define REGEN_OFF_START_THRESHOLD 1540
//#include "Metro.h"
//Metro timer_watchdog_timer = Metro(5);
//
//void setup() {
//  //pinMode(BRAKE_LIGHT_CTRL, OUTPUT);
//
//  // change to input if comparator is PUSH PULL
//  //pinMode(INVERTER_EN, OUTPUT);
//  //pinMode(INVERTER_24V_EN, OUTPUT);
//
//  pinMode(WATCHDOG_INPUT, OUTPUT);
//  // the initial state of the watchdog is high
//  // this is reflected in the static watchdog_state
//  // starting high
//  digitalWrite(WATCHDOG_INPUT, HIGH);
//  pinMode(SOFTWARE_OK, OUTPUT);
//  digitalWrite(SOFTWARE_OK, HIGH);
//  //digitalWrite(INVERTER_24V_EN, HIGH);
//  //digitalWrite(INVERTER_EN, HIGH);
//  delay(5000);
//
//}

//inline void software_shutdown() {
//  digitalWrite(SOFTWARE_OK, HIGH);
//  //mcu_status.set_software_is_ok(true);
//
//  // check inputs
//  // BMS heartbeat has not arrived within time interval
//
//  // add BMS software checks
//  // software ok/not ok action
////  if (mcu_status.get_software_is_ok()) {
////    digitalWrite(SOFTWARE_OK, HIGH); //eventually make this HIGH only if software is ok
////  } else {
////    digitalWrite(SOFTWARE_OK, LOW);
////  }
//  /* Watchdog timer */
//  if (timer_watchdog_timer.check()) {
//    static bool watchdog_state = HIGH;
//    watchdog_state = !watchdog_state;
//    digitalWrite(WATCHDOG_INPUT, watchdog_state);
//    Serial.println("toggle");
//  }
//
//}
//void loop() {
//  // put your main code here, to run repeatedly:
//  software_shutdown();
//  
//}
