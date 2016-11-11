/**
 * Shrivathsav Seshan
 * Oct 23 2016
 * The BMS
 */

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC68041.h"
#include <SPI.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>

/************BATTERY CONSTRAINTS AND CONSTANTS**********************/
#define VOLTAGE_LOW_CUTOFF 3.0
#define VOLTAGE_HIGH_CUTOFF 4.2
#define DISCHARGE_CURRENT_CONSTANT_HIGH 220
#define DISCHARGE_CURRENT_PEAK_HIGH 440
#define CHARGE_CURRENT_CONSTANT_HIGH 220
#define CHARGE_CURRENT_PEAK_HIGH 330
#define CHARGE_CURRENT_LOW_CUTOFF 1.1
#define CHARGE_TEMP_CRITICAL_HIGH 44
#define CHARGE_TEMP_CRITICAL_LOW 0
#define DISCHARGE_TEMP_CRITICAL_HIGH 60
#define DISCHARGE_TEMP_CRITICAL_LOW 15
#define MAX_16BIT_UNSIGNED 65536

/********GLOBAL ARRAYS/VARIABLES CONTAINING DATA FROM CHIP**********/
const uint8_t TOTAL_IC = 8;
uint16_t cell_voltages[TOTAL_IC][12]; // contains 12 battery cell voltages
uint16_t aux_voltagess[TOTAL_IC][6]; // contains auxillary pin voltages (not batteries)

/*!<
  The tx_cfg[][6] stores the LTC6804 configuration data that is going to be written 
  to the LTC6804 ICs on the daisy chain. The LTC6804 configuration data that will be
  written should be stored in blocks of 6 bytes. The array should have the following format:
  
 |  tx_cfg[0][0]| tx_cfg[0][1] |  tx_cfg[0][2]|  tx_cfg[0][3]|  tx_cfg[0][4]|  tx_cfg[0][5]| tx_cfg[1][0] |  tx_cfg[1][1]|  tx_cfg[1][2]|  .....    |
 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
 |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |
 
*/
uint8_t tx_cfg[TOTAL_IC][6]; // data defining how data will be written to daisy chain ICs.

/*!<
  the rx_cfg[][8] array stores the data that is read back from a LTC6804-1 daisy chain. 
  The configuration data for each IC  is stored in blocks of 8 bytes. Below is an table illustrating the array organization:

|rx_config[0][0]|rx_config[0][1]|rx_config[0][2]|rx_config[0][3]|rx_config[0][4]|rx_config[0][5]|rx_config[0][6]  |rx_config[0][7] |rx_config[1][0]|rx_config[1][1]|  .....    |
|---------------|---------------|---------------|---------------|---------------|---------------|-----------------|----------------|---------------|---------------|-----------|
|IC1 CFGR0      |IC1 CFGR1      |IC1 CFGR2      |IC1 CFGR3      |IC1 CFGR4      |IC1 CFGR5      |IC1 PEC High     |IC1 PEC Low     |IC2 CFGR0      |IC2 CFGR1      |  .....    |
*/
uint8_t rx_cfg[TOTAL_IC][8];

/**
 * CAN Variables
 */
FlexCAN can(500000);
static CAN_message_t msg;
long msTimer = 0;

/************CAN MESSAGE ID's **********************/
#define SHUTDOWN_FAULT 0x001
#define HIGHEST_CELL_VOLTAGE 0x0B0
#define LOWEST_CELL_VOLTAGE 0x0B1
#define AVG_CELL_VOLTAGE 0x0B2
#define PACK_STATE_OF_CHARGE 0x0B3
#define PACK_STATE_OF_HEALTH 0x0B4 // Questionable
#define HIGHEST_TEMP 0x0B5
#define LOWEST_TEMP 0x0B6
#define AVG_TEMP 0x0B7
#define DISCHARGE_CURRENT 0x0B8
#define CHARGE_CURRENT 0x0B9

/**
 * BMS State Variables
 */
enum ChargeState {
  DISCHARGE,
  CHARGE
};
ChargeState chargeState = DISCHARGE;
boolean BMSStatusOK;

/**
 * Teensy Communication Pin Constants
 */
#define MOSI_PIN 17
#define MISO_PIN 16
#define SPI_CHIP_SELECT_PIN 15
#define CLOCK_PIN 14
#define T_WAKE 50

// TODO: Calculate Internal Resistance
// TODO: Current goes Negative during Regen Braking.
// TODO: Switching between discharge and charge very often.
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    SPI.setMOSI(MOSI_PIN);
    SPI.setMISO(MISO_PIN);
    SPI.setSCK(CLOCK_PIN);
    pinMode(SPI_CHIP_SELECT_PIN, OUTPUT);
    SPI.begin();

    LTC6804_initialize();
    init_cfg();
    can.begin();
    BMSStatusOK = true;
}

/*
 * Continuously poll voltages and print them to the Serial Monitor.
 */
void loop() {
    // put your main code here, to run repeatedly:
    pollVoltage();
    printCells();

    // TODO:: IMPLEMENT CAN BUS READING
    while (can.read(msg)) {
        switch (msg.id) {
        case 0x001:
            {
                BMSStatusOK = false;
                Serial.print("SHUTDOWN RECEIVED");
                printCanMessage();
            } break;
        case 0x0BC:
            {
                unsigned char chargeStateBit = msg.buf[0] >> 7;
                if (chargeStateBit == 0) {
                    chargeState = DISCHARGE;
                } else if (chargeStateBit == 1) {
                    chargeState = CHARGE;
                }
                Serial.print("Charge State Received");
                printCanMessage();
            } break;
        default:
            {
                printCanMessage();
            } break;
        }
    }

    int maxV = convertToMillivolts(findMaxVoltage());
    int minV = convertToMillivolts(findMinVoltage());
    int avgV = findAverage();

    if (!(voltageInBounds(maxV) && voltageInBounds(minV))) {
        BMSStatusOK = false;
        writeShutdownCANMessage(minV, maxV);
    }

    // write max voltage to CAN as telemetry
    msg.id = HIGHEST_CELL_VOLTAGE;
    msg.len = sizeof(int);
    memcpy(&msg.buf[0], &maxV, sizeof(int));
    can.write(msg);

    // write min voltage to CAN as telemetry
    msg.id = LOWEST_CELL_VOLTAGE;
    msg.len = (sizeof(int));
    memcpy(&msg.buf[0], &minV, sizeof(int));
    can.write(msg);

    // write avg voltage to can as telemetry
    msg.id = AVG_CELL_VOLTAGE;
    msg.len = sizeof(int);
    memcpy(&msg.buf[0], &avgV, sizeof(int));
    can.write(msg);
}

/*!***********************************
 \brief Initializes the configuration array
 **************************************/
void init_cfg()
{
    for(int i = 0; i<TOTAL_IC;i++)
    {
        tx_cfg[i][0] = 0xFE;
        tx_cfg[i][1] = 0x00 ;
        tx_cfg[i][2] = 0x00 ;
        tx_cfg[i][3] = 0x00 ; 
        tx_cfg[i][4] = 0x00 ;
        tx_cfg[i][5] = 0x00 ;
    }
}

void pollVoltage() {
    Serial.println("Polling Voltages...");
    /*
     * Difference between wakeup_sleep and wakeup_idle
     * wakeup_sleep wakes up the LTC6804 from sleep state
     * wakeup_idle wakes up the isoSPI port.
     */
    wakeFromSleepAllChips();
    LTC6804_wrcfg(TOTAL_IC, tx_cfg);
    wakeFromIdleAllChips();
    LTC6804_adcv();
    delay(10);
    wakeup_idle();
    uint8_t error = LTC6804_rdcv(0, TOTAL_IC, cell_voltages); // calls the chip to read and store voltage values in the array.
    if (error == -1) {
        Serial.println("A PEC error was detected in voltage data");
    }
    printCells(); // prints the cell voltages to Serial.
    delay(100);
}

void wakeFromSleepAllChips() {
    for (int i = 0; i < TOTAL_IC / 3; i++) {
        wakeup_sleep();
        delay(3);
    }
}

void wakeFromIdleAllChips() {
    for (int i = 0; i < TOTAL_IC / 3; i++) {
        wakeup_idle();
        delay(3);
    }
}

uint16_t findMaxVoltage() {
    uint16_t maxVolt = cell_voltages[0][0];
    for (int ic = 0; ic < TOTAL_IC; ic++) {
        for (int cell = 0; cell < 12; cell++) {
            if (cell_voltages[ic][cell] > maxVolt) {
                maxVolt = cell_voltages[ic][cell];
            }
        }
    }
    return maxVolt;
}

uint16_t findMinVoltage() {
    uint16_t minVolt = cell_voltages[0][0];
    for (int ic = 0; ic < TOTAL_IC; ic++) {
        for (int cell = 0; cell < 12; cell++) {
            if (cell_voltages[ic][cell] < minVolt) {
                minVolt = cell_voltages[ic][cell];
            }
        }
    }
    return minVolt;
}

int convertToMillivolts(uint16_t v) {
    return (int) ((v / (float) MAX_16BIT_UNSIGNED) * 5000.0f);
}

int findAverage() {
    int average = 0;
    for (int ic = 0; ic < TOTAL_IC; ic++) {
        for (int cell = 0; cell < 12; cell++) {
            average += cell_voltages[ic][cell];
        }
    }
    average = (int) (average / ((float) MAX_16BIT_UNSIGNED * TOTAL_IC * 12) * 5000);
    return average;
}

boolean voltageInBounds() {
    double maxVolt = findMaxVoltage() / MAX_16BIT_UNSIGNED * 5.0f;
    double minVolt = findMinVoltage() / MAX_16BIT_UNSIGNED * 5.0f;
    return !(maxVolt >= VOLTAGE_HIGH_CUTOFF || minVolt >= VOLTAGE_HIGH_CUTOFF || maxVolt <= VOLTAGE_LOW_CUTOFF || minVolt <= VOLTAGE_LOW_CUTOFF);
}

boolean voltageInBounds(int v) {
    double voltage = v / 1000;
    return !(voltage >= VOLTAGE_HIGH_CUTOFF || voltage <= VOLTAGE_LOW_CUTOFF);
}

void printCells() {
    for (int current_ic = 0; current_ic < TOTAL_IC; current_ic++) {
        Serial.print("IC: ");
        Serial.println(current_ic+1);
        for (int i = 0; i < 12; i++) {
            Serial.print("C"); Serial.print(i+1); Serial.print(": ");
            float voltage = cell_voltages[current_ic][i] / MAX_16BIT_UNSIGNED * 5.0f;
            Serial.println(voltage);
        }
        Serial.println();
    }
}

int writeShutdownCANMessage(int minV, int maxV) {
    msg.id = SHUTDOWN_FAULT;
    msg.len = 2 * sizeof(int);
    int counter = 0;
    memcpy(&msg.buf[counter], &maxV, sizeof(int));
    counter += sizeof(int);
    memcpy(&msg.buf[counter], &minV, sizeof(int));
    return can.write(msg);
}

void printCanMessage() {
    uint8_t len = msg.len;
    Serial.print("ID: ");
    Serial.println(msg.id);
    Serial.print("MESSAGE: ");
    for (uint8_t i = 0; i < len; i++) {
        Serial.print(msg.buf[i]);
    }
    Serial.println();
}
