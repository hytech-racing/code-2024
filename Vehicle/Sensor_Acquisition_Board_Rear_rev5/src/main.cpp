/**
 * Teensy 4.0 code for Rear Sensor Acquisition Board (Rear SAB) rev5
 * 
 * * Corner boards (SPI/isoSPI)
 * * * Load cells
 * * * Suspension potentiometers
 * * Thermistors (SPI)
 * * Pi shutdown
 * * * El spare button on side panel always fake
 * * VectorNav VN300 INS sensor (UART/RS232)
 * 
 * Cecilia's Version
 */

/**
 *  Include files 
 */

/* Framework */
#include <Arduino.h>

/* Standard libs*/
#include <stdint.h>
#include <string.h>

/* Constants */
#include "rear_sab_dfs.h"

/* Interfaces */
#include "HyTechCANInterface.h"
#include "MCP_ADC.h"
#include "TelemetryInterface.h"
#include "ThermistorInterface.h"
#include "VectorNavInterface.h"

/* Systems */
#include "SysClock.h"

/**
 * Utilities
*/
#include "DebouncedButton.h"

/**
 * Data source
*/

/* One CAN line on rear SAB rev5 */
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;  // Telemetry

/* Sensors */
// ADCs
MCP_ADC<4> ADC1 = MCP_ADC<4>(ADC1_CS, ADC_ISOSPI_SPEED);  // RL corner board
MCP_ADC<4> ADC2 = MCP_ADC<4>(ADC2_CS, ADC_ISOSPI_SPEED);  // RR corner board
MCP_ADC<8> ADC3 = MCP_ADC<8>(ADC3_CS, ADC_SPI_SPEED);  // Thermistors
// Pi shutdown
DebouncedButton btn_pi_shutdown;

/**
 * Interfaces
*/
// Telemetry
TelemetryInterface telem_interface(&CAN2_txBuffer,
                                   {THERM_READ_1, THERM_READ_2, THERM_READ_3, THERM_READ_4, THERM_READ_5, THERM_READ_6, THERM_READ_7, SUS_POT_3, SUS_POT_4, RL_LOAD_CELL, RR_LOAD_CELL});
// Thermistor
ThermistorReadChannel_s<TOTAL_THERMISTOR_COUNT> therm_channels = {{THERM_3, THERM_4, THERM_5, THERM_6, THERM_7, THERM_8, THERM_9}};
ThermistorInterface<TOTAL_THERMISTOR_COUNT> therm_interface(therm_channels, THERM_ALPHA);
// VectorNav
// Async
// vnParams_s vn_params = {&Serial2, VN_RS232_SPEED, true, INIT_HEADING, true, 8};
// Polling
vnParams_s vn_params = {&Serial2, VN_RS232_SPEED, true, INIT_HEADING, false, 0};
VectorNavInterface vn_interface(&CAN2_txBuffer, vn_params);

/**
 * Systems
*/
SysClock sys_clock;

/* Function prototypes */
void init_all_CAN_devices();
void init_all_adcs();
void tick_all_interfaces(const SysTick_s &curr_tick);
void tick_all_systems(const SysTick_s &curr_tick);

void setup() {

    // Tick system clock
    SysTick_s curr_tick = sys_clock.tick(micros());

    // Debug print
    Serial.begin(DEBUG_PRINT_BAUDRATE);
    Serial.println("Starting...");

    // Initialize CAN
    init_all_CAN_devices();

    // Initialize all SPI devices
    init_all_adcs();
    Serial.println("SPI initialized");

    // Initialize debounced button
    btn_pi_shutdown.begin(PI_SHUTDOWN, 100);
    Serial.println("Debounce button initialized for Pi shutdown");

    // RS232
    vn_interface.init(curr_tick);
    Serial.println("VectorNav initialized ... for real this time!");
    Serial.println();

}   /* end of setup */

void loop() {

    // Tick system clock
    SysTick_s curr_tick = sys_clock.tick(micros());

    // Process received CAN messages
    // Not currently needed
    process_ring_buffer(CAN2_rxBuffer, curr_tick.millis);

    // Tick interfaces
    tick_all_interfaces(curr_tick);

    // Tick systems
    // Not currently needed

    // Send outbound CAN messages
    send_all_CAN_msg(CAN2_txBuffer, &TELEM_CAN);

    // Debug prints to see if we're tripping balls
    TriggerBits_s t = curr_tick.triggers;
    if (t.trigger5)
    {
        Serial.println("Thermistors:");
        Serial.print("Raw: ");
        Serial.println(ADC3.get().conversions[therm_channels.channel[0]].raw);
        Serial.print("Converted: ");
        Serial.println(therm_interface.get().temperatures[0]);
        Serial.print("Raw: ");
        Serial.println(ADC3.get().conversions[therm_channels.channel[1]].raw);
        Serial.print("Converted: ");
        Serial.println(therm_interface.get().temperatures[1]);
        Serial.print("Raw: ");
        Serial.println(ADC3.get().conversions[therm_channels.channel[2]].raw);
        Serial.print("Converted: ");
        Serial.println(therm_interface.get().temperatures[2]);
        Serial.print("Raw: ");
        Serial.println(ADC3.get().conversions[therm_channels.channel[3]].raw);
        Serial.print("Converted: ");
        Serial.println(therm_interface.get().temperatures[3]);
        Serial.print("Raw: ");
        Serial.println(ADC3.get().conversions[therm_channels.channel[4]].raw);
        Serial.print("Converted: ");
        Serial.println(therm_interface.get().temperatures[4]);
        Serial.print("Raw: ");
        Serial.println(ADC3.get().conversions[therm_channels.channel[5]].raw);
        Serial.print("Converted: ");
        Serial.println(therm_interface.get().temperatures[5]);
        Serial.print("Raw: ");
        Serial.println(ADC3.get().conversions[therm_channels.channel[6]].raw);
        Serial.print("Converted: ");
        Serial.println(therm_interface.get().temperatures[6]);
        Serial.println();

        Serial.println("Load cells:");
        Serial.println(ADC1.get().conversions[RL_LOAD_CELL].raw);
        Serial.println(ADC2.get().conversions[RR_LOAD_CELL].raw);
        Serial.println();

        Serial.println("Sus pots:");
        Serial.println(ADC1.get().conversions[SUS_POT_3].raw);
        Serial.println(ADC2.get().conversions[SUS_POT_4].raw);
        Serial.println();

        Serial.println("Vector Nav:");
        vn_interface.printBinaryReceiveBuffer();
        vn_interface.printAsciiReceiveBuffer();

        Serial.println();
    }

}   /* end of loop */

/**
 * Initialize CAN lines 
*/
void init_all_CAN_devices()
{
    // Telemetry CAN line
    TELEM_CAN.begin();
    TELEM_CAN.setBaudRate(TELEM_CAN_BAUDRATE);
    TELEM_CAN.setMaxMB(16);
    TELEM_CAN.enableFIFO();
    TELEM_CAN.enableFIFOInterrupt();
    TELEM_CAN.onReceive(on_can2_receive);
    TELEM_CAN.mailboxStatus();
}

/**
 * Initialize all SPI devices
 * MCP ADCs
*/
void init_all_adcs()
{
    SPI.begin();

    ADC1.init();
    ADC2.init();
    ADC3.init();
}

/**
 * Tick interfaces
*/
void tick_all_interfaces(const SysTick_s &curr_tick)
{

    TriggerBits_s t = curr_tick.triggers;

    if (t.trigger100)
    {
        // 100Hz
        // ADC
        ADC1.tick();
        ADC2.tick();
        ADC3.tick();
        // Thermistor interface
        therm_interface.tick(
            {
                {
                    ADC3.get().conversions[therm_channels.channel[0]],
                    ADC3.get().conversions[therm_channels.channel[1]],
                    ADC3.get().conversions[therm_channels.channel[2]],
                    ADC3.get().conversions[therm_channels.channel[3]],
                    ADC3.get().conversions[therm_channels.channel[4]],
                    ADC3.get().conversions[therm_channels.channel[5]],
                    ADC3.get().conversions[therm_channels.channel[6]]
                }
            }
        );
    }

    if (t.trigger50)
    {  
        // 50Hz
        // Telemetry
        telem_interface.tick(ADC1.get(),
                             ADC2.get(),
                             ADC3.get(),
                             btn_pi_shutdown.isPressed(),
                             therm_interface.get());
    }

    // Timing managed internally
    // VectorNav
    vn_interface.tick(curr_tick);

}

/**
 * Tick systems
*/
void tick_all_systems(const SysTick_s &curr_tick)
{
  
}







