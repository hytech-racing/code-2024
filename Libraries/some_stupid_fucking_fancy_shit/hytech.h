#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// DBC file version
#define VER_HYTECH_MAJ (6U)
#define VER_HYTECH_MIN (0U)

// include current dbc-driver compilation config
#include "hytech-config.h"

#ifdef HYTECH_USE_DIAG_MONITORS
// This file must define:
// base monitor struct
#include "canmonitorutil.h"

#endif // HYTECH_USE_DIAG_MONITORS


// DLC maximum value which is used as the limit for frame's data buffer size.
// Client can set its own value (not sure why) in driver-config
// or can test it on some limit specified by application
// e.g.: static_assert(TESTDB_MAX_DLC_VALUE <= APPLICATION_FRAME_DATA_SIZE, "Max DLC value in the driver is too big")
#ifndef HYTECH_MAX_DLC_VALUE
// The value which was found out by generator (real max value)
#define HYTECH_MAX_DLC_VALUE 8U
#endif

// The limit is used for setting frame's data bytes
#define HYTECH_VALIDATE_DLC(msgDlc) (((msgDlc) <= (HYTECH_MAX_DLC_VALUE)) ? (msgDlc) : (HYTECH_MAX_DLC_VALUE))

// Initial byte value to be filles in data bytes of the frame before pack signals
// User can define its own custom value in driver-config file
#ifndef HYTECH_INITIAL_BYTE_VALUE
#define HYTECH_INITIAL_BYTE_VALUE 0U
#endif


// TCU_DRIVER_MSG contains a signal that corresponds to a preset list of messages on the dashboard. It also includes two variables to customize the contents of the message with numerical values. It also includes the target lap time which is determined by the
// def @TCU_DRIVER_MSG CAN Message (148  0x94)
#define TCU_DRIVER_MSG_IDE (0U)
#define TCU_DRIVER_MSG_DLC (8U)
#define TCU_DRIVER_MSG_CANID (0x94U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  // corresponds to a pre-set list of driver messages on the dashboard that the TCU can request to be displayed in event of comms failure
  uint8_t driver_message;                    //      Bits= 8

  uint16_t driver_msg_var_1;                 //      Bits=16

  uint16_t driver_msg_var_2;                 //      Bits=16

  // target lap time is the time determined by the pit crew that the driver should aim for. mostly useful for guiding the driver in endurance races.
  uint32_t target_lap_time;                  //      Bits=24 Unit:'ms'

#else

  // corresponds to a pre-set list of driver messages on the dashboard that the TCU can request to be displayed in event of comms failure
  uint8_t driver_message;                    //      Bits= 8

  uint16_t driver_msg_var_1;                 //      Bits=16

  uint16_t driver_msg_var_2;                 //      Bits=16

  // target lap time is the time determined by the pit crew that the driver should aim for. mostly useful for guiding the driver in endurance races.
  uint32_t target_lap_time;                  //      Bits=24 Unit:'ms'

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} TCU_DRIVER_MSG_t;

// The TCU_LAP_TIMES message contains the best and previous lap times to the dashboard as calculated by the TCU or manually input by the pit crew. It also relays the current state of the TCU's clock to the dashboard so that it can run an approximate stopwatc
// def @TCU_LAP_TIMES CAN Message (149  0x95)
#define TCU_LAP_TIMES_IDE (0U)
#define TCU_LAP_TIMES_DLC (8U)
#define TCU_LAP_TIMES_CANID (0x95U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint32_t best_lap_time;                    //      Bits=24 Unit:'ms'

  uint32_t prev_lap_time;                    //      Bits=24 Unit:'ms'

  uint8_t lap_clock_state;                   //      Bits= 8

#else

  uint32_t best_lap_time;                    //      Bits=24 Unit:'ms'

  uint32_t prev_lap_time;                    //      Bits=24 Unit:'ms'

  uint8_t lap_clock_state;                   //      Bits= 8

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} TCU_LAP_TIMES_t;

// def @MC1_STATUS CAN Message (160  0xa0)
#define MC1_STATUS_IDE (0U)
#define MC1_STATUS_DLC (8U)
#define MC1_STATUS_CANID (0xa0U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t system_ready : 1;                  //      Bits= 1

  uint8_t error : 1;                         //      Bits= 1

  uint8_t warning : 1;                       //      Bits= 1

  uint8_t quit_dc_on : 1;                    //      Bits= 1

  uint8_t dc_on : 1;                         //      Bits= 1

  uint8_t quit_inverter_on : 1;              //      Bits= 1

  uint8_t inverter_on : 1;                   //      Bits= 1

  uint8_t derating_on : 1;                   //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#else

  uint8_t system_ready;                      //      Bits= 1

  uint8_t error;                             //      Bits= 1

  uint8_t warning;                           //      Bits= 1

  uint8_t quit_dc_on;                        //      Bits= 1

  uint8_t dc_on;                             //      Bits= 1

  uint8_t quit_inverter_on;                  //      Bits= 1

  uint8_t inverter_on;                       //      Bits= 1

  uint8_t derating_on;                       //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC1_STATUS_t;

// def @MC2_STATUS CAN Message (161  0xa1)
#define MC2_STATUS_IDE (0U)
#define MC2_STATUS_DLC (8U)
#define MC2_STATUS_CANID (0xa1U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t system_ready : 1;                  //      Bits= 1

  uint8_t error : 1;                         //      Bits= 1

  uint8_t warning : 1;                       //      Bits= 1

  uint8_t quit_dc_on : 1;                    //      Bits= 1

  uint8_t dc_on : 1;                         //      Bits= 1

  uint8_t quit_inverter_on : 1;              //      Bits= 1

  uint8_t inverter_on : 1;                   //      Bits= 1

  uint8_t derating_on : 1;                   //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#else

  uint8_t system_ready;                      //      Bits= 1

  uint8_t error;                             //      Bits= 1

  uint8_t warning;                           //      Bits= 1

  uint8_t quit_dc_on;                        //      Bits= 1

  uint8_t dc_on;                             //      Bits= 1

  uint8_t quit_inverter_on;                  //      Bits= 1

  uint8_t inverter_on;                       //      Bits= 1

  uint8_t derating_on;                       //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC2_STATUS_t;

// def @MC3_STATUS CAN Message (162  0xa2)
#define MC3_STATUS_IDE (0U)
#define MC3_STATUS_DLC (8U)
#define MC3_STATUS_CANID (0xa2U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t system_ready : 1;                  //      Bits= 1

  uint8_t error : 1;                         //      Bits= 1

  uint8_t warning : 1;                       //      Bits= 1

  uint8_t quit_dc_on : 1;                    //      Bits= 1

  uint8_t dc_on : 1;                         //      Bits= 1

  uint8_t quit_inverter_on : 1;              //      Bits= 1

  uint8_t inverter_on : 1;                   //      Bits= 1

  uint8_t derating_on : 1;                   //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#else

  uint8_t system_ready;                      //      Bits= 1

  uint8_t error;                             //      Bits= 1

  uint8_t warning;                           //      Bits= 1

  uint8_t quit_dc_on;                        //      Bits= 1

  uint8_t dc_on;                             //      Bits= 1

  uint8_t quit_inverter_on;                  //      Bits= 1

  uint8_t inverter_on;                       //      Bits= 1

  uint8_t derating_on;                       //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC3_STATUS_t;

// def @MC4_STATUS CAN Message (163  0xa3)
#define MC4_STATUS_IDE (0U)
#define MC4_STATUS_DLC (8U)
#define MC4_STATUS_CANID (0xa3U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t system_ready : 1;                  //      Bits= 1

  uint8_t error : 1;                         //      Bits= 1

  uint8_t warning : 1;                       //      Bits= 1

  uint8_t quit_dc_on : 1;                    //      Bits= 1

  uint8_t dc_on : 1;                         //      Bits= 1

  uint8_t quit_inverter_on : 1;              //      Bits= 1

  uint8_t inverter_on : 1;                   //      Bits= 1

  uint8_t derating_on : 1;                   //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#else

  uint8_t system_ready;                      //      Bits= 1

  uint8_t error;                             //      Bits= 1

  uint8_t warning;                           //      Bits= 1

  uint8_t quit_dc_on;                        //      Bits= 1

  uint8_t dc_on;                             //      Bits= 1

  uint8_t quit_inverter_on;                  //      Bits= 1

  uint8_t inverter_on;                       //      Bits= 1

  uint8_t derating_on;                       //      Bits= 1

  int16_t speed_rpm;                         //  [-] Bits=16

  int16_t torque_current;                    //  [-] Bits=16

  int16_t magnetizing_current;               //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC4_STATUS_t;

// def @MC1_TEMPS CAN Message (164  0xa4)
#define MC1_TEMPS_IDE (0U)
#define MC1_TEMPS_DLC (8U)
#define MC1_TEMPS_CANID (0xa4U)
// signal: @motor_temp_ro
#define HYTECH_motor_temp_ro_CovFactor (0.1)
#define HYTECH_motor_temp_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.1)) )
#define HYTECH_motor_temp_ro_fromS(x) ( (((x) * (0.1)) + (0.0)) )
// signal: @inverter_temp_ro
#define HYTECH_inverter_temp_ro_CovFactor (0.1)
#define HYTECH_inverter_temp_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.1)) )
#define HYTECH_inverter_temp_ro_fromS(x) ( (((x) * (0.1)) + (0.0)) )
// signal: @igbt_temp_ro
#define HYTECH_igbt_temp_ro_CovFactor (0.1)
#define HYTECH_igbt_temp_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.1)) )
#define HYTECH_igbt_temp_ro_fromS(x) ( (((x) * (0.1)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC1_TEMPS_t;

// def @MC2_TEMPS CAN Message (165  0xa5)
#define MC2_TEMPS_IDE (0U)
#define MC2_TEMPS_DLC (8U)
#define MC2_TEMPS_CANID (0xa5U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC2_TEMPS_t;

// def @MC3_TEMPS CAN Message (166  0xa6)
#define MC3_TEMPS_IDE (0U)
#define MC3_TEMPS_DLC (8U)
#define MC3_TEMPS_CANID (0xa6U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC3_TEMPS_t;

// def @MC4_TEMPS CAN Message (167  0xa7)
#define MC4_TEMPS_IDE (0U)
#define MC4_TEMPS_DLC (8U)
#define MC4_TEMPS_CANID (0xa7U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  int16_t motor_temp_ro;                     //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t motor_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t inverter_temp_ro;                  //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t inverter_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t diagnostic_number;                //      Bits=16

  int16_t igbt_temp_ro;                      //  [-] Bits=16 Factor= 0.1

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t igbt_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC4_TEMPS_t;

// def @MC1_ENERGY CAN Message (168  0xa8)
#define MC1_ENERGY_IDE (0U)
#define MC1_ENERGY_DLC (8U)
#define MC1_ENERGY_CANID (0xa8U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#else

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC1_ENERGY_t;

// def @MC2_ENERGY CAN Message (169  0xa9)
#define MC2_ENERGY_IDE (0U)
#define MC2_ENERGY_DLC (8U)
#define MC2_ENERGY_CANID (0xa9U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#else

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC2_ENERGY_t;

// def @MC3_ENERGY CAN Message (170  0xaa)
#define MC3_ENERGY_IDE (0U)
#define MC3_ENERGY_DLC (8U)
#define MC3_ENERGY_CANID (0xaaU)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#else

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC3_ENERGY_t;

// def @MC4_ENERGY CAN Message (171  0xab)
#define MC4_ENERGY_IDE (0U)
#define MC4_ENERGY_DLC (8U)
#define MC4_ENERGY_CANID (0xabU)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#else

  uint16_t dc_bus_voltage;                   //      Bits=16

  uint32_t motor_power;                      //      Bits=32

  int16_t feedback_torque;                   //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC4_ENERGY_t;

// def @MC1_SETPOINTS_COMMAND CAN Message (176  0xb0)
#define MC1_SETPOINTS_COMMAND_IDE (0U)
#define MC1_SETPOINTS_COMMAND_DLC (8U)
#define MC1_SETPOINTS_COMMAND_CANID (0xb0U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t inverter_enable : 1;               //      Bits= 1

  uint8_t hv_enable : 1;                     //      Bits= 1

  uint8_t driver_enable : 1;                 //      Bits= 1

  uint8_t remove_error : 1;                  //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#else

  uint8_t inverter_enable;                   //      Bits= 1

  uint8_t hv_enable;                         //      Bits= 1

  uint8_t driver_enable;                     //      Bits= 1

  uint8_t remove_error;                      //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC1_SETPOINTS_COMMAND_t;

// def @MC2_SETPOINTS_COMMAND CAN Message (177  0xb1)
#define MC2_SETPOINTS_COMMAND_IDE (0U)
#define MC2_SETPOINTS_COMMAND_DLC (8U)
#define MC2_SETPOINTS_COMMAND_CANID (0xb1U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t inverter_enable : 1;               //      Bits= 1

  uint8_t hv_enable : 1;                     //      Bits= 1

  uint8_t driver_enable : 1;                 //      Bits= 1

  uint8_t remove_error : 1;                  //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#else

  uint8_t inverter_enable;                   //      Bits= 1

  uint8_t hv_enable;                         //      Bits= 1

  uint8_t driver_enable;                     //      Bits= 1

  uint8_t remove_error;                      //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC2_SETPOINTS_COMMAND_t;

// def @MC3_SETPOINTS_COMMAND CAN Message (178  0xb2)
#define MC3_SETPOINTS_COMMAND_IDE (0U)
#define MC3_SETPOINTS_COMMAND_DLC (8U)
#define MC3_SETPOINTS_COMMAND_CANID (0xb2U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t inverter_enable : 1;               //      Bits= 1

  uint8_t hv_enable : 1;                     //      Bits= 1

  uint8_t driver_enable : 1;                 //      Bits= 1

  uint8_t remove_error : 1;                  //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#else

  uint8_t inverter_enable;                   //      Bits= 1

  uint8_t hv_enable;                         //      Bits= 1

  uint8_t driver_enable;                     //      Bits= 1

  uint8_t remove_error;                      //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC3_SETPOINTS_COMMAND_t;

// def @MC4_SETPOINTS_COMMAND CAN Message (179  0xb3)
#define MC4_SETPOINTS_COMMAND_IDE (0U)
#define MC4_SETPOINTS_COMMAND_DLC (8U)
#define MC4_SETPOINTS_COMMAND_CANID (0xb3U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t inverter_enable : 1;               //      Bits= 1

  uint8_t hv_enable : 1;                     //      Bits= 1

  uint8_t driver_enable : 1;                 //      Bits= 1

  uint8_t remove_error : 1;                  //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#else

  uint8_t inverter_enable;                   //      Bits= 1

  uint8_t hv_enable;                         //      Bits= 1

  uint8_t driver_enable;                     //      Bits= 1

  uint8_t remove_error;                      //      Bits= 1

  int16_t speed_setpoint_rpm;                //  [-] Bits=16

  int16_t positive_torque_limit;             //  [-] Bits=16

  int16_t negative_torque_limit;             //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC4_SETPOINTS_COMMAND_t;

// def @MC1_TORQUE_COMMAND CAN Message (180  0xb4)
#define MC1_TORQUE_COMMAND_IDE (0U)
#define MC1_TORQUE_COMMAND_DLC (2U)
#define MC1_TORQUE_COMMAND_CANID (0xb4U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t torque_command;                    //  [-] Bits=16

#else

  int16_t torque_command;                    //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC1_TORQUE_COMMAND_t;

// def @MC2_TORQUE_COMMAND CAN Message (181  0xb5)
#define MC2_TORQUE_COMMAND_IDE (0U)
#define MC2_TORQUE_COMMAND_DLC (2U)
#define MC2_TORQUE_COMMAND_CANID (0xb5U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t torque_command;                    //  [-] Bits=16

#else

  int16_t torque_command;                    //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC2_TORQUE_COMMAND_t;

// def @MC3_TORQUE_COMMAND CAN Message (182  0xb6)
#define MC3_TORQUE_COMMAND_IDE (0U)
#define MC3_TORQUE_COMMAND_DLC (2U)
#define MC3_TORQUE_COMMAND_CANID (0xb6U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t torque_command;                    //  [-] Bits=16

#else

  int16_t torque_command;                    //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC3_TORQUE_COMMAND_t;

// def @MC4_TORQUE_COMMAND CAN Message (183  0xb7)
#define MC4_TORQUE_COMMAND_IDE (0U)
#define MC4_TORQUE_COMMAND_DLC (2U)
#define MC4_TORQUE_COMMAND_CANID (0xb7U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t torque_command;                    //  [-] Bits=16

#else

  int16_t torque_command;                    //  [-] Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MC4_TORQUE_COMMAND_t;

// def @MCU_STATUS CAN Message (195  0xc3)
#define MCU_STATUS_IDE (0U)
#define MCU_STATUS_DLC (8U)
#define MCU_STATUS_CANID (0xc3U)

// Value tables for @ecu_state signal

#ifndef ecu_state_MCU_STATUS_STARTUP
#define ecu_state_MCU_STATUS_STARTUP (0)
#endif

#ifndef ecu_state_MCU_STATUS_TRACTIVE_SYSTEM_NOT_ACTIVE
#define ecu_state_MCU_STATUS_TRACTIVE_SYSTEM_NOT_ACTIVE (1)
#endif

#ifndef ecu_state_MCU_STATUS_TRACTIVE_SYSTEM_ACTIVE
#define ecu_state_MCU_STATUS_TRACTIVE_SYSTEM_ACTIVE (2)
#endif

#ifndef ecu_state_MCU_STATUS_ENABLING_INVERTER
#define ecu_state_MCU_STATUS_ENABLING_INVERTER (3)
#endif

#ifndef ecu_state_MCU_STATUS_WAITING_READY_TO_DRIVE_SOUND
#define ecu_state_MCU_STATUS_WAITING_READY_TO_DRIVE_SOUND (4)
#endif

#ifndef ecu_state_MCU_STATUS_READY_TO_DRIVE
#define ecu_state_MCU_STATUS_READY_TO_DRIVE (5)
#endif

// signal: @distance_traveled_m_ro
#define HYTECH_distance_traveled_m_ro_CovFactor (0.01)
#define HYTECH_distance_traveled_m_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_distance_traveled_m_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t imd_ok_high : 1;                        //      Bits= 1

  uint8_t shutdown_b_above_threshold : 1;         //      Bits= 1

  uint8_t bms_ok_high : 1;                        //      Bits= 1

  uint8_t shutdown_c_above_threshold : 1;         //      Bits= 1

  uint8_t bspd_ok_high : 1;                       //      Bits= 1

  uint8_t shutdown_d_above_threshold : 1;         //      Bits= 1

  uint8_t shoftware_ok_high : 1;                  //      Bits= 1

  uint8_t shutdown_e_above_threshold : 1;         //      Bits= 1

  uint8_t mechanical_brake_active : 1;            //      Bits= 1

  uint8_t no_accel_implausability : 1;            //      Bits= 1

  uint8_t no_brake_implausability : 1;            //      Bits= 1

  uint8_t brake_pedal_active : 1;                 //      Bits= 1

  uint8_t bspd_current_high : 1;                  //      Bits= 1

  uint8_t bspd_brake_high : 1;                    //      Bits= 1

  uint8_t no_accel_or_brake_implausability : 1;   //      Bits= 1

  uint8_t drive_buzzer : 1;                       //      Bits= 1

  //  0 : "STARTUP"
  //  1 : "TRACTIVE_SYSTEM_NOT_ACTIVE"
  //  2 : "TRACTIVE_SYSTEM_ACTIVE"
  //  3 : "ENABLING_INVERTER"
  //  4 : "WAITING_READY_TO_DRIVE_SOUND"
  //  5 : "READY_TO_DRIVE"
  uint8_t ecu_state : 3;                          //      Bits= 3

  uint8_t inverter_error : 1;                     //      Bits= 1

  uint8_t energy_meter_present : 1;               //      Bits= 1

  uint8_t software_ok : 1;                        //      Bits= 1

  uint8_t launch_control_active : 1;              //      Bits= 1

  uint8_t pack_charge_critical : 2;               //      Bits= 2

  uint8_t max_torque;                             //      Bits= 8

  uint8_t torque_mode;                            //      Bits= 8

  uint16_t distance_traveled_m_ro;                //      Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t distance_traveled_m_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  uint8_t imd_ok_high;                            //      Bits= 1

  uint8_t shutdown_b_above_threshold;             //      Bits= 1

  uint8_t bms_ok_high;                            //      Bits= 1

  uint8_t shutdown_c_above_threshold;             //      Bits= 1

  uint8_t bspd_ok_high;                           //      Bits= 1

  uint8_t shutdown_d_above_threshold;             //      Bits= 1

  uint8_t shoftware_ok_high;                      //      Bits= 1

  uint8_t shutdown_e_above_threshold;             //      Bits= 1

  uint8_t mechanical_brake_active;                //      Bits= 1

  uint8_t no_accel_implausability;                //      Bits= 1

  uint8_t no_brake_implausability;                //      Bits= 1

  uint8_t brake_pedal_active;                     //      Bits= 1

  uint8_t bspd_current_high;                      //      Bits= 1

  uint8_t bspd_brake_high;                        //      Bits= 1

  uint8_t no_accel_or_brake_implausability;       //      Bits= 1

  uint8_t drive_buzzer;                           //      Bits= 1

  //  0 : "STARTUP"
  //  1 : "TRACTIVE_SYSTEM_NOT_ACTIVE"
  //  2 : "TRACTIVE_SYSTEM_ACTIVE"
  //  3 : "ENABLING_INVERTER"
  //  4 : "WAITING_READY_TO_DRIVE_SOUND"
  //  5 : "READY_TO_DRIVE"
  uint8_t ecu_state;                              //      Bits= 3

  uint8_t inverter_error;                         //      Bits= 1

  uint8_t energy_meter_present;                   //      Bits= 1

  uint8_t software_ok;                            //      Bits= 1

  uint8_t launch_control_active;                  //      Bits= 1

  uint8_t pack_charge_critical;                   //      Bits= 2

  uint8_t max_torque;                             //      Bits= 8

  uint8_t torque_mode;                            //      Bits= 8

  uint16_t distance_traveled_m_ro;                //      Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t distance_traveled_m_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MCU_STATUS_t;

// def @MCU_PEDAL_READINGS CAN Message (196  0xc4)
#define MCU_PEDAL_READINGS_IDE (0U)
#define MCU_PEDAL_READINGS_DLC (8U)
#define MCU_PEDAL_READINGS_CANID (0xc4U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t accel_pedal_1;                    //      Bits=16

  uint16_t accel_pedal_2;                    //      Bits=16

  uint16_t brake_pedal_1;                    //      Bits=16

  uint16_t brake_pedal_2;                    //      Bits=16

#else

  uint16_t accel_pedal_1;                    //      Bits=16

  uint16_t accel_pedal_2;                    //      Bits=16

  uint16_t brake_pedal_1;                    //      Bits=16

  uint16_t brake_pedal_2;                    //      Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MCU_PEDAL_READINGS_t;

// def @MCU_LOAD_CELLS CAN Message (197  0xc5)
#define MCU_LOAD_CELLS_IDE (0U)
#define MCU_LOAD_CELLS_DLC (4U)
#define MCU_LOAD_CELLS_CANID (0xc5U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t FL_load_cell;                     //      Bits=16 Unit:'lbs'

  uint16_t FR_load_cell;                     //      Bits=16 Unit:'lbs'

#else

  uint16_t FL_load_cell;                     //      Bits=16 Unit:'lbs'

  uint16_t FR_load_cell;                     //      Bits=16 Unit:'lbs'

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MCU_LOAD_CELLS_t;

// def @SAB_LOAD_CELLS CAN Message (198  0xc6)
#define SAB_LOAD_CELLS_IDE (0U)
#define SAB_LOAD_CELLS_DLC (4U)
#define SAB_LOAD_CELLS_CANID (0xc6U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t RL_load_cell;                     //      Bits=16 Unit:'lbs'

  uint16_t RR_load_cell;                     //      Bits=16 Unit:'lbs'

#else

  uint16_t RL_load_cell;                     //      Bits=16 Unit:'lbs'

  uint16_t RR_load_cell;                     //      Bits=16 Unit:'lbs'

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} SAB_LOAD_CELLS_t;

// def @MCU_FRONT_POTS CAN Message (199  0xc7)
#define MCU_FRONT_POTS_IDE (0U)
#define MCU_FRONT_POTS_DLC (6U)
#define MCU_FRONT_POTS_CANID (0xc7U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t pot_1;                            //      Bits=16

  uint16_t pot_2;                            //      Bits=16

  uint16_t pot_3;                            //      Bits=16

#else

  uint16_t pot_1;                            //      Bits=16

  uint16_t pot_2;                            //      Bits=16

  uint16_t pot_3;                            //      Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MCU_FRONT_POTS_t;

// def @MCU_REAR_POTS CAN Message (200  0xc8)
#define MCU_REAR_POTS_IDE (0U)
#define MCU_REAR_POTS_DLC (6U)
#define MCU_REAR_POTS_CANID (0xc8U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t pot_4;                            //      Bits=16

  uint16_t pot_5;                            //      Bits=16

  uint16_t pot_6;                            //      Bits=16

#else

  uint16_t pot_4;                            //      Bits=16

  uint16_t pot_5;                            //      Bits=16

  uint16_t pot_6;                            //      Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MCU_REAR_POTS_t;

// def @MCU_ANALOG_READINGS CAN Message (204  0xcc)
#define MCU_ANALOG_READINGS_IDE (0U)
#define MCU_ANALOG_READINGS_DLC (8U)
#define MCU_ANALOG_READINGS_CANID (0xccU)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t steering_1;                        //  [-] Bits=16

  uint16_t steering_2;                       //      Bits=16

  int16_t hall_effect_current;               //  [-] Bits=16

  uint16_t glv_battery_voltage;              //      Bits=16

#else

  int16_t steering_1;                        //  [-] Bits=16

  uint16_t steering_2;                       //      Bits=16

  int16_t hall_effect_current;               //  [-] Bits=16

  uint16_t glv_battery_voltage;              //      Bits=16

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} MCU_ANALOG_READINGS_t;

// def @BMS_ONBOARD_TEMPS CAN Message (213  0xd5)
#define BMS_ONBOARD_TEMPS_IDE (0U)
#define BMS_ONBOARD_TEMPS_DLC (6U)
#define BMS_ONBOARD_TEMPS_CANID (0xd5U)
// signal: @average_temp_ro
#define HYTECH_average_temp_ro_CovFactor (0.01)
#define HYTECH_average_temp_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_average_temp_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )
// signal: @low_temp_ro
#define HYTECH_low_temp_ro_CovFactor (0.01)
#define HYTECH_low_temp_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_low_temp_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )
// signal: @high_temp_ro
#define HYTECH_high_temp_ro_CovFactor (0.01)
#define HYTECH_high_temp_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_high_temp_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t average_temp_ro;                   //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t average_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t low_temp_ro;                       //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t low_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t high_temp_ro;                      //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t high_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  int16_t average_temp_ro;                   //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t average_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t low_temp_ro;                       //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t low_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t high_temp_ro;                      //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t high_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_ONBOARD_TEMPS_t;

// def @BMS_ONBOARD_DETAILED_TEMPS CAN Message (214  0xd6)
#define BMS_ONBOARD_DETAILED_TEMPS_IDE (0U)
#define BMS_ONBOARD_DETAILED_TEMPS_DLC (5U)
#define BMS_ONBOARD_DETAILED_TEMPS_CANID (0xd6U)
// signal: @temp_0_ro
#define HYTECH_temp_0_ro_CovFactor (0.01)
#define HYTECH_temp_0_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_temp_0_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )
// signal: @temp_1_ro
#define HYTECH_temp_1_ro_CovFactor (0.01)
#define HYTECH_temp_1_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_temp_1_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t ic_id;                             //      Bits= 8

  int16_t temp_0_ro;                         //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t temp_0_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t temp_1_ro;                         //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t temp_1_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  uint8_t ic_id;                             //      Bits= 8

  int16_t temp_0_ro;                         //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t temp_0_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t temp_1_ro;                         //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t temp_1_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_ONBOARD_DETAILED_TEMPS_t;

// def @BMS_VOLTAGES CAN Message (215  0xd7)
#define BMS_VOLTAGES_IDE (0U)
#define BMS_VOLTAGES_DLC (8U)
#define BMS_VOLTAGES_CANID (0xd7U)
// signal: @average_voltage_ro
#define HYTECH_average_voltage_ro_CovFactor (0.0001)
#define HYTECH_average_voltage_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_average_voltage_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )
// signal: @low_voltage_ro
#define HYTECH_low_voltage_ro_CovFactor (0.0001)
#define HYTECH_low_voltage_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_low_voltage_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )
// signal: @high_voltage_ro
#define HYTECH_high_voltage_ro_CovFactor (0.0001)
#define HYTECH_high_voltage_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_high_voltage_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )
// signal: @total_voltage_ro
#define HYTECH_total_voltage_ro_CovFactor (0.01)
#define HYTECH_total_voltage_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_total_voltage_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint16_t average_voltage_ro;               //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t average_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t low_voltage_ro;                   //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t low_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t high_voltage_ro;                  //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t high_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t total_voltage_ro;                 //      Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t total_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  uint16_t average_voltage_ro;               //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t average_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t low_voltage_ro;                   //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t low_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t high_voltage_ro;                  //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t high_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t total_voltage_ro;                 //      Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t total_voltage_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_VOLTAGES_t;

// def @BMS_DETAILED_VOLTAGES CAN Message (216  0xd8)
#define BMS_DETAILED_VOLTAGES_IDE (0U)
#define BMS_DETAILED_VOLTAGES_DLC (8U)
#define BMS_DETAILED_VOLTAGES_CANID (0xd8U)
// signal: @voltage_0_ro
#define HYTECH_voltage_0_ro_CovFactor (0.0001)
#define HYTECH_voltage_0_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_voltage_0_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )
// signal: @voltage_1_ro
#define HYTECH_voltage_1_ro_CovFactor (0.0001)
#define HYTECH_voltage_1_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_voltage_1_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )
// signal: @voltage_2_ro
#define HYTECH_voltage_2_ro_CovFactor (0.0001)
#define HYTECH_voltage_2_ro_toS(x) ( (uint16_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_voltage_2_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t group_id : 4;                      //      Bits= 4

  uint8_t ic_id;                             //      Bits= 8

  uint16_t voltage_0_ro;                     //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t voltage_0_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t voltage_1_ro;                     //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t voltage_1_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t voltage_2_ro;                     //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t voltage_2_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  uint8_t group_id;                          //      Bits= 4

  uint8_t ic_id;                             //      Bits= 8

  uint16_t voltage_0_ro;                     //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t voltage_0_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t voltage_1_ro;                     //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t voltage_1_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint16_t voltage_2_ro;                     //      Bits=16 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t voltage_2_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_DETAILED_VOLTAGES_t;

// def @BMS_TEMPS CAN Message (217  0xd9)
#define BMS_TEMPS_IDE (0U)
#define BMS_TEMPS_DLC (6U)
#define BMS_TEMPS_CANID (0xd9U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t average_temp_ro;                   //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t average_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t low_temp_ro;                       //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t low_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t high_temp_ro;                      //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t high_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  int16_t average_temp_ro;                   //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t average_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t low_temp_ro;                       //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t low_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t high_temp_ro;                      //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t high_temp_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_TEMPS_t;

// def @BMS_DETAILED_TEMPS CAN Message (218  0xda)
#define BMS_DETAILED_TEMPS_IDE (0U)
#define BMS_DETAILED_TEMPS_DLC (8U)
#define BMS_DETAILED_TEMPS_CANID (0xdaU)
// signal: @thermistor_id_0_ro
#define HYTECH_thermistor_id_0_ro_CovFactor (0.01)
#define HYTECH_thermistor_id_0_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_thermistor_id_0_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )
// signal: @thermistor_id_1_ro
#define HYTECH_thermistor_id_1_ro_CovFactor (0.01)
#define HYTECH_thermistor_id_1_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_thermistor_id_1_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )
// signal: @thermistor_id_2_ro
#define HYTECH_thermistor_id_2_ro_CovFactor (0.01)
#define HYTECH_thermistor_id_2_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_thermistor_id_2_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t group_id : 4;                      //      Bits= 4

  uint8_t ic_id;                             //      Bits= 8

  int16_t thermistor_id_0_ro;                //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t thermistor_id_0_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t thermistor_id_1_ro;                //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t thermistor_id_1_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t thermistor_id_2_ro;                //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t thermistor_id_2_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  uint8_t group_id;                          //      Bits= 4

  uint8_t ic_id;                             //      Bits= 8

  int16_t thermistor_id_0_ro;                //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t thermistor_id_0_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t thermistor_id_1_ro;                //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t thermistor_id_1_phys;
#endif // HYTECH_USE_SIGFLOAT

  int16_t thermistor_id_2_ro;                //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t thermistor_id_2_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_DETAILED_TEMPS_t;

// def @BMS_STATUS CAN Message (219  0xdb)
#define BMS_STATUS_IDE (0U)
#define BMS_STATUS_DLC (6U)
#define BMS_STATUS_CANID (0xdbU)
// signal: @current_ro
#define HYTECH_current_ro_CovFactor (0.01)
#define HYTECH_current_ro_toS(x) ( (int16_t) (((x) - (0.0)) / (0.01)) )
#define HYTECH_current_ro_fromS(x) ( (((x) * (0.01)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t state;                                  //      Bits= 8

  uint8_t overvoltage_error : 1;                  //      Bits= 1

  uint8_t undervoltage_error : 1;                 //      Bits= 1

  uint8_t total_voltage_high_error : 1;           //      Bits= 1

  uint8_t discharge_overcurrent_error : 1;        //      Bits= 1

  uint8_t charge_overcurrent_error : 1;           //      Bits= 1

  uint8_t discharge_overtemp_error : 1;           //      Bits= 1

  uint8_t charge_overtemp_error : 1;              //      Bits= 1

  uint8_t undertemp_error : 1;                    //      Bits= 1

  uint8_t overtemp_error : 1;                     //      Bits= 1

  int16_t current_ro;                             //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t current_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint8_t shutdown_g_above_threshold_error : 1;   //      Bits= 1

  uint8_t shutdown_h_above_threshold_error : 1;   //      Bits= 1

#else

  uint8_t state;                                  //      Bits= 8

  uint8_t overvoltage_error;                      //      Bits= 1

  uint8_t undervoltage_error;                     //      Bits= 1

  uint8_t total_voltage_high_error;               //      Bits= 1

  uint8_t discharge_overcurrent_error;            //      Bits= 1

  uint8_t charge_overcurrent_error;               //      Bits= 1

  uint8_t discharge_overtemp_error;               //      Bits= 1

  uint8_t charge_overtemp_error;                  //      Bits= 1

  uint8_t undertemp_error;                        //      Bits= 1

  uint8_t overtemp_error;                         //      Bits= 1

  int16_t current_ro;                             //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t current_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint8_t shutdown_g_above_threshold_error;       //      Bits= 1

  uint8_t shutdown_h_above_threshold_error;       //      Bits= 1

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_STATUS_t;

// def @CCU_STATUS CAN Message (221  0xdd)
#define CCU_STATUS_IDE (0U)
#define CCU_STATUS_DLC (1U)
#define CCU_STATUS_CANID (0xddU)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t charger_enabled : 1;               //      Bits= 1

#else

  uint8_t charger_enabled;                   //      Bits= 1

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} CCU_STATUS_t;

// def @BMS_BALANCING_STATUS CAN Message (222  0xde)
#define BMS_BALANCING_STATUS_IDE (0U)
#define BMS_BALANCING_STATUS_DLC (8U)
#define BMS_BALANCING_STATUS_CANID (0xdeU)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t group_id : 4;                      //      Bits= 4

  uint8_t cell_1_balancing_status : 1;       //      Bits= 1

  uint8_t cell_2_balancing_status : 1;       //      Bits= 1

  uint8_t cell_3_balancing_status : 1;       //      Bits= 1

  uint8_t cell_4_balancing_status : 1;       //      Bits= 1

  uint8_t cell_5_balancing_status : 1;       //      Bits= 1

  uint8_t cell_6_balancing_status : 1;       //      Bits= 1

  uint8_t cell_7_balancing_status : 1;       //      Bits= 1

  uint8_t cell_8_balancing_status : 1;       //      Bits= 1

  uint8_t cell_9_balancing_status : 1;       //      Bits= 1

  uint8_t cell_10_balancing_status : 1;      //      Bits= 1

  uint8_t cell_11_balancing_status : 1;      //      Bits= 1

  uint8_t cell_12_balancing_status : 1;      //      Bits= 1

  uint8_t cell_13_balancing_status : 1;      //      Bits= 1

  uint8_t cell_14_balancing_status : 1;      //      Bits= 1

  uint8_t cell_15_balancing_status : 1;      //      Bits= 1

  uint8_t cell_16_balancing_status : 1;      //      Bits= 1

  uint8_t cell_17_balancing_status : 1;      //      Bits= 1

  uint8_t cell_18_balancing_status : 1;      //      Bits= 1

  uint8_t cell_19_balancing_status : 1;      //      Bits= 1

  uint8_t cell_20_balancing_status : 1;      //      Bits= 1

  uint8_t cell_21_balancing_status : 1;      //      Bits= 1

  uint8_t cell_22_balancing_status : 1;      //      Bits= 1

  uint8_t cell_23_balancing_status : 1;      //      Bits= 1

  uint8_t cell_24_balancing_status : 1;      //      Bits= 1

  uint8_t cell_25_balancing_status : 1;      //      Bits= 1

  uint8_t cell_26_balancing_status : 1;      //      Bits= 1

  uint8_t cell_27_balancing_status : 1;      //      Bits= 1

  uint8_t cell_28_balancing_status : 1;      //      Bits= 1

  uint8_t cell_29_balancing_status : 1;      //      Bits= 1

  uint8_t cell_30_balancing_status : 1;      //      Bits= 1

  uint8_t cell_31_balancing_status : 1;      //      Bits= 1

  uint8_t cell_32_balancing_status : 1;      //      Bits= 1

  uint8_t cell_33_balancing_status : 1;      //      Bits= 1

  uint8_t cell_34_balancing_status : 1;      //      Bits= 1

  uint8_t cell_35_balancing_status : 1;      //      Bits= 1

  uint8_t cell_36_balancing_status : 1;      //      Bits= 1

  uint8_t cell_37_balancing_status : 1;      //      Bits= 1

  uint8_t cell_38_balancing_status : 1;      //      Bits= 1

  uint8_t cell_39_balancing_status : 1;      //      Bits= 1

  uint8_t cell_40_balancing_status : 1;      //      Bits= 1

  uint8_t cell_41_balancing_status : 1;      //      Bits= 1

  uint8_t cell_42_balancing_status : 1;      //      Bits= 1

  uint8_t cell_43_balancing_status : 1;      //      Bits= 1

  uint8_t cell_44_balancing_status : 1;      //      Bits= 1

  uint8_t cell_45_balancing_status : 1;      //      Bits= 1

  uint8_t cell_46_balancing_status : 1;      //      Bits= 1

  uint8_t cell_47_balancing_status : 1;      //      Bits= 1

  uint8_t cell_48_balancing_status : 1;      //      Bits= 1

  uint8_t cell_49_balancing_status : 1;      //      Bits= 1

  uint8_t cell_50_balancing_status : 1;      //      Bits= 1

  uint8_t cell_51_balancing_status : 1;      //      Bits= 1

  uint8_t cell_52_balancing_status : 1;      //      Bits= 1

  uint8_t cell_53_balancing_status : 1;      //      Bits= 1

  uint8_t cell_54_balancing_status : 1;      //      Bits= 1

  uint8_t cell_55_balancing_status : 1;      //      Bits= 1

  uint8_t cell_56_balancing_status : 1;      //      Bits= 1

  uint8_t cell_57_balancing_status : 1;      //      Bits= 1

  uint8_t cell_58_balancing_status : 1;      //      Bits= 1

  uint8_t cell_59_balancing_status : 1;      //      Bits= 1

  uint8_t cell_60_balancing_status : 1;      //      Bits= 1

#else

  uint8_t group_id;                          //      Bits= 4

  uint8_t cell_1_balancing_status;           //      Bits= 1

  uint8_t cell_2_balancing_status;           //      Bits= 1

  uint8_t cell_3_balancing_status;           //      Bits= 1

  uint8_t cell_4_balancing_status;           //      Bits= 1

  uint8_t cell_5_balancing_status;           //      Bits= 1

  uint8_t cell_6_balancing_status;           //      Bits= 1

  uint8_t cell_7_balancing_status;           //      Bits= 1

  uint8_t cell_8_balancing_status;           //      Bits= 1

  uint8_t cell_9_balancing_status;           //      Bits= 1

  uint8_t cell_10_balancing_status;          //      Bits= 1

  uint8_t cell_11_balancing_status;          //      Bits= 1

  uint8_t cell_12_balancing_status;          //      Bits= 1

  uint8_t cell_13_balancing_status;          //      Bits= 1

  uint8_t cell_14_balancing_status;          //      Bits= 1

  uint8_t cell_15_balancing_status;          //      Bits= 1

  uint8_t cell_16_balancing_status;          //      Bits= 1

  uint8_t cell_17_balancing_status;          //      Bits= 1

  uint8_t cell_18_balancing_status;          //      Bits= 1

  uint8_t cell_19_balancing_status;          //      Bits= 1

  uint8_t cell_20_balancing_status;          //      Bits= 1

  uint8_t cell_21_balancing_status;          //      Bits= 1

  uint8_t cell_22_balancing_status;          //      Bits= 1

  uint8_t cell_23_balancing_status;          //      Bits= 1

  uint8_t cell_24_balancing_status;          //      Bits= 1

  uint8_t cell_25_balancing_status;          //      Bits= 1

  uint8_t cell_26_balancing_status;          //      Bits= 1

  uint8_t cell_27_balancing_status;          //      Bits= 1

  uint8_t cell_28_balancing_status;          //      Bits= 1

  uint8_t cell_29_balancing_status;          //      Bits= 1

  uint8_t cell_30_balancing_status;          //      Bits= 1

  uint8_t cell_31_balancing_status;          //      Bits= 1

  uint8_t cell_32_balancing_status;          //      Bits= 1

  uint8_t cell_33_balancing_status;          //      Bits= 1

  uint8_t cell_34_balancing_status;          //      Bits= 1

  uint8_t cell_35_balancing_status;          //      Bits= 1

  uint8_t cell_36_balancing_status;          //      Bits= 1

  uint8_t cell_37_balancing_status;          //      Bits= 1

  uint8_t cell_38_balancing_status;          //      Bits= 1

  uint8_t cell_39_balancing_status;          //      Bits= 1

  uint8_t cell_40_balancing_status;          //      Bits= 1

  uint8_t cell_41_balancing_status;          //      Bits= 1

  uint8_t cell_42_balancing_status;          //      Bits= 1

  uint8_t cell_43_balancing_status;          //      Bits= 1

  uint8_t cell_44_balancing_status;          //      Bits= 1

  uint8_t cell_45_balancing_status;          //      Bits= 1

  uint8_t cell_46_balancing_status;          //      Bits= 1

  uint8_t cell_47_balancing_status;          //      Bits= 1

  uint8_t cell_48_balancing_status;          //      Bits= 1

  uint8_t cell_49_balancing_status;          //      Bits= 1

  uint8_t cell_50_balancing_status;          //      Bits= 1

  uint8_t cell_51_balancing_status;          //      Bits= 1

  uint8_t cell_52_balancing_status;          //      Bits= 1

  uint8_t cell_53_balancing_status;          //      Bits= 1

  uint8_t cell_54_balancing_status;          //      Bits= 1

  uint8_t cell_55_balancing_status;          //      Bits= 1

  uint8_t cell_56_balancing_status;          //      Bits= 1

  uint8_t cell_57_balancing_status;          //      Bits= 1

  uint8_t cell_58_balancing_status;          //      Bits= 1

  uint8_t cell_59_balancing_status;          //      Bits= 1

  uint8_t cell_60_balancing_status;          //      Bits= 1

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_BALANCING_STATUS_t;

// def @BMS_COULOMB_COUNTS CAN Message (226  0xe2)
#define BMS_COULOMB_COUNTS_IDE (0U)
#define BMS_COULOMB_COUNTS_DLC (8U)
#define BMS_COULOMB_COUNTS_CANID (0xe2U)
// signal: @total_charge_ro
#define HYTECH_total_charge_ro_CovFactor (0.0001)
#define HYTECH_total_charge_ro_toS(x) ( (uint32_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_total_charge_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )
// signal: @total_discharge_ro
#define HYTECH_total_discharge_ro_CovFactor (0.0001)
#define HYTECH_total_discharge_ro_toS(x) ( (uint32_t) (((x) - (0.0)) / (0.0001)) )
#define HYTECH_total_discharge_ro_fromS(x) ( (((x) * (0.0001)) + (0.0)) )

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint32_t total_charge_ro;                  //      Bits=32 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t total_charge_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint32_t total_discharge_ro;               //      Bits=32 Factor= 0.0001          Unit:'Coulombs'

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t total_discharge_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  uint32_t total_charge_ro;                  //      Bits=32 Factor= 0.0001

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t total_charge_phys;
#endif // HYTECH_USE_SIGFLOAT

  uint32_t total_discharge_ro;               //      Bits=32 Factor= 0.0001          Unit:'Coulombs'

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t total_discharge_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} BMS_COULOMB_COUNTS_t;

// def @DASHBOARD_MCU_STATE CAN Message (235  0xeb)
#define DASHBOARD_MCU_STATE_IDE (0U)
#define DASHBOARD_MCU_STATE_DLC (5U)
#define DASHBOARD_MCU_STATE_CANID (0xebU)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t bots_led : 2;                      //      Bits= 2

  uint8_t launch_control_led : 2;            //      Bits= 2

  uint8_t mode_led : 2;                      //      Bits= 2

  uint8_t mechanical_brake_led : 2;          //      Bits= 2

  uint8_t cockpit_brb_led : 2;               //      Bits= 2

  uint8_t inertia_status_led : 2;            //      Bits= 2

  uint8_t start_status_led : 2;              //      Bits= 2

  uint8_t motor_controller_error_led : 2;    //      Bits= 2

  uint8_t imd_led : 2;                       //      Bits= 2

  uint8_t ams_led : 2;                       //      Bits= 2

  uint8_t drive_buzzer : 1;                  //      Bits= 1

  // glv led has a value from 0 to 255 representing a percentage that can be displayed on dashboard as a gradient between two colors
  uint8_t glv_led;                           //      Bits= 8

  // pack charge led has a value from 0 to 255 representing a percentage that can be displayed on dashboard as a gradient between two colors
  uint8_t pack_charge_led;                   //      Bits= 8

#else

  uint8_t bots_led;                          //      Bits= 2

  uint8_t launch_control_led;                //      Bits= 2

  uint8_t mode_led;                          //      Bits= 2

  uint8_t mechanical_brake_led;              //      Bits= 2

  uint8_t cockpit_brb_led;                   //      Bits= 2

  uint8_t inertia_status_led;                //      Bits= 2

  uint8_t start_status_led;                  //      Bits= 2

  uint8_t motor_controller_error_led;        //      Bits= 2

  uint8_t imd_led;                           //      Bits= 2

  uint8_t ams_led;                           //      Bits= 2

  uint8_t drive_buzzer;                      //      Bits= 1

  // glv led has a value from 0 to 255 representing a percentage that can be displayed on dashboard as a gradient between two colors
  uint8_t glv_led;                           //      Bits= 8

  // pack charge led has a value from 0 to 255 representing a percentage that can be displayed on dashboard as a gradient between two colors
  uint8_t pack_charge_led;                   //      Bits= 8

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} DASHBOARD_MCU_STATE_t;

// def @DASHBOARD_STATE CAN Message (236  0xec)
#define DASHBOARD_STATE_IDE (0U)
#define DASHBOARD_STATE_DLC (3U)
#define DASHBOARD_STATE_CANID (0xecU)

// Value tables for @dial_state signal

#ifndef dial_state_DASHBOARD_STATE_MODE_ONE
#define dial_state_DASHBOARD_STATE_MODE_ONE (0)
#endif

#ifndef dial_state_DASHBOARD_STATE_MODE_TWO
#define dial_state_DASHBOARD_STATE_MODE_TWO (1)
#endif

#ifndef dial_state_DASHBOARD_STATE_ACCELERATION_LAUNCH_CONTROL
#define dial_state_DASHBOARD_STATE_ACCELERATION_LAUNCH_CONTROL (2)
#endif

#ifndef dial_state_DASHBOARD_STATE_SKIDPAD
#define dial_state_DASHBOARD_STATE_SKIDPAD (3)
#endif

#ifndef dial_state_DASHBOARD_STATE_AUTOCROSS
#define dial_state_DASHBOARD_STATE_AUTOCROSS (4)
#endif

#ifndef dial_state_DASHBOARD_STATE_ENDURANCE
#define dial_state_DASHBOARD_STATE_ENDURANCE (5)
#endif


typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t start_button : 1;                    //      Bits= 1

  uint8_t mark_button : 1;                     //      Bits= 1

  uint8_t mode_button : 1;                     //      Bits= 1

  uint8_t motor_controller_cycle_button : 1;   //      Bits= 1

  uint8_t launch_ctrl_button : 1;              //      Bits= 1

  uint8_t torque_mode_button : 1;              //      Bits= 1

  uint8_t led_dimmer_button : 1;               //      Bits= 1

  uint8_t left_shifter_button : 1;             //      Bits= 1

  uint8_t right_shifter_button : 1;            //      Bits= 1

  uint8_t shutdown_h_above_threshold : 1;      //      Bits= 1

  uint8_t ssok_above_threshold : 1;            //      Bits= 1

  uint8_t drive_buzzer : 1;                    //      Bits= 1

  //  0 : "MODE_ONE"
  //  1 : "MODE_TWO"
  //  2 : "ACCELERATION_LAUNCH_CONTROL"
  //  3 : "SKIDPAD"
  //  4 : "AUTOCROSS"
  //  5 : "ENDURANCE"
  uint8_t dial_state;                          //      Bits= 8

#else

  uint8_t start_button;                        //      Bits= 1

  uint8_t mark_button;                         //      Bits= 1

  uint8_t mode_button;                         //      Bits= 1

  uint8_t motor_controller_cycle_button;       //      Bits= 1

  uint8_t launch_ctrl_button;                  //      Bits= 1

  uint8_t torque_mode_button;                  //      Bits= 1

  uint8_t led_dimmer_button;                   //      Bits= 1

  uint8_t left_shifter_button;                 //      Bits= 1

  uint8_t right_shifter_button;                //      Bits= 1

  uint8_t shutdown_h_above_threshold;          //      Bits= 1

  uint8_t ssok_above_threshold;                //      Bits= 1

  uint8_t drive_buzzer;                        //      Bits= 1

  //  0 : "MODE_ONE"
  //  1 : "MODE_TWO"
  //  2 : "ACCELERATION_LAUNCH_CONTROL"
  //  3 : "SKIDPAD"
  //  4 : "AUTOCROSS"
  //  5 : "ENDURANCE"
  uint8_t dial_state;                          //      Bits= 8

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} DASHBOARD_STATE_t;

// def @EM_STATUS CAN Message (256  0x100)
#define EM_STATUS_IDE (0U)
#define EM_STATUS_DLC (8U)
#define EM_STATUS_CANID (0x100U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t voltage_gain : 4;                  //      Bits= 4

  uint8_t current_gain : 4;                  //      Bits= 4

  uint8_t overvoltage_error : 1;             //      Bits= 1

  uint8_t overpower_error : 1;               //      Bits= 1

  uint8_t logging : 1;                       //      Bits= 1

#else

  uint8_t voltage_gain;                      //      Bits= 4

  uint8_t current_gain;                      //      Bits= 4

  uint8_t overvoltage_error;                 //      Bits= 1

  uint8_t overpower_error;                   //      Bits= 1

  uint8_t logging;                           //      Bits= 1

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} EM_STATUS_t;

// def @EM_MEASUREMENT CAN Message (1024 0x400)
#define EM_MEASUREMENT_IDE (0U)
#define EM_MEASUREMENT_DLC (8U)
#define EM_MEASUREMENT_CANID (0x400U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  int16_t current_ro;                        //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t current_phys;
#endif // HYTECH_USE_SIGFLOAT

#else

  int16_t current_ro;                        //  [-] Bits=16 Factor= 0.01

#ifdef HYTECH_USE_SIGFLOAT
  sigfloat_t current_phys;
#endif // HYTECH_USE_SIGFLOAT

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} EM_MEASUREMENT_t;

// def @CHARGER_CONTROL CAN Message (403105268 0x1806e5f4)
#define CHARGER_CONTROL_IDE (1U)
#define CHARGER_CONTROL_DLC (5U)
#define CHARGER_CONTROL_CANID (0x1806e5f4U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t max_charging_voltage_high;         //      Bits= 8

  uint8_t max_charging_voltage_low;          //      Bits= 8

  uint8_t max_charging_current_high;         //      Bits= 8

  uint8_t max_charging_current_low;          //      Bits= 8

  uint8_t control;                           //      Bits= 8

#else

  uint8_t max_charging_voltage_high;         //      Bits= 8

  uint8_t max_charging_voltage_low;          //      Bits= 8

  uint8_t max_charging_current_high;         //      Bits= 8

  uint8_t max_charging_current_low;          //      Bits= 8

  uint8_t control;                           //      Bits= 8

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} CHARGER_CONTROL_t;

// def @CHARGER_DATA CAN Message (419385573 0x18ff50e5)
#define CHARGER_DATA_IDE (1U)
#define CHARGER_DATA_DLC (7U)
#define CHARGER_DATA_CANID (0x18ff50e5U)

typedef struct
{
#ifdef HYTECH_USE_BITS_SIGNAL

  uint8_t output_dc_voltage_high;            //      Bits= 8

  uint8_t output_dc_voltage_low;             //      Bits= 8

  uint8_t output_current_high;               //      Bits= 8

  uint8_t output_current_low;                //      Bits= 8

  uint8_t flags;                             //      Bits= 8

  uint8_t input_ac_voltage_high;             //      Bits= 8

  uint8_t input_ac_voltage_low;              //      Bits= 8

#else

  uint8_t output_dc_voltage_high;            //      Bits= 8

  uint8_t output_dc_voltage_low;             //      Bits= 8

  uint8_t output_current_high;               //      Bits= 8

  uint8_t output_current_low;                //      Bits= 8

  uint8_t flags;                             //      Bits= 8

  uint8_t input_ac_voltage_high;             //      Bits= 8

  uint8_t input_ac_voltage_low;              //      Bits= 8

#endif // HYTECH_USE_BITS_SIGNAL

#ifdef HYTECH_USE_DIAG_MONITORS

  FrameMonitor_t mon1;

#endif // HYTECH_USE_DIAG_MONITORS

} CHARGER_DATA_t;

// Function signatures

uint32_t Unpack_TCU_DRIVER_MSG_hytech(TCU_DRIVER_MSG_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_TCU_DRIVER_MSG_hytech(TCU_DRIVER_MSG_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_TCU_DRIVER_MSG_hytech(TCU_DRIVER_MSG_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_TCU_LAP_TIMES_hytech(TCU_LAP_TIMES_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_TCU_LAP_TIMES_hytech(TCU_LAP_TIMES_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_TCU_LAP_TIMES_hytech(TCU_LAP_TIMES_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_STATUS_hytech(MC1_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC1_STATUS_hytech(MC1_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC1_STATUS_hytech(MC1_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_STATUS_hytech(MC2_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC2_STATUS_hytech(MC2_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC2_STATUS_hytech(MC2_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_STATUS_hytech(MC3_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC3_STATUS_hytech(MC3_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC3_STATUS_hytech(MC3_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_STATUS_hytech(MC4_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC4_STATUS_hytech(MC4_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC4_STATUS_hytech(MC4_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_TEMPS_hytech(MC1_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC1_TEMPS_hytech(MC1_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC1_TEMPS_hytech(MC1_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_TEMPS_hytech(MC2_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC2_TEMPS_hytech(MC2_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC2_TEMPS_hytech(MC2_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_TEMPS_hytech(MC3_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC3_TEMPS_hytech(MC3_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC3_TEMPS_hytech(MC3_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_TEMPS_hytech(MC4_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC4_TEMPS_hytech(MC4_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC4_TEMPS_hytech(MC4_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_ENERGY_hytech(MC1_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC1_ENERGY_hytech(MC1_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC1_ENERGY_hytech(MC1_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_ENERGY_hytech(MC2_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC2_ENERGY_hytech(MC2_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC2_ENERGY_hytech(MC2_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_ENERGY_hytech(MC3_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC3_ENERGY_hytech(MC3_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC3_ENERGY_hytech(MC3_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_ENERGY_hytech(MC4_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC4_ENERGY_hytech(MC4_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC4_ENERGY_hytech(MC4_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_SETPOINTS_COMMAND_hytech(MC1_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC1_SETPOINTS_COMMAND_hytech(MC1_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC1_SETPOINTS_COMMAND_hytech(MC1_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_SETPOINTS_COMMAND_hytech(MC2_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC2_SETPOINTS_COMMAND_hytech(MC2_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC2_SETPOINTS_COMMAND_hytech(MC2_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_SETPOINTS_COMMAND_hytech(MC3_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC3_SETPOINTS_COMMAND_hytech(MC3_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC3_SETPOINTS_COMMAND_hytech(MC3_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_SETPOINTS_COMMAND_hytech(MC4_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC4_SETPOINTS_COMMAND_hytech(MC4_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC4_SETPOINTS_COMMAND_hytech(MC4_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_TORQUE_COMMAND_hytech(MC1_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC1_TORQUE_COMMAND_hytech(MC1_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC1_TORQUE_COMMAND_hytech(MC1_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_TORQUE_COMMAND_hytech(MC2_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC2_TORQUE_COMMAND_hytech(MC2_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC2_TORQUE_COMMAND_hytech(MC2_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_TORQUE_COMMAND_hytech(MC3_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC3_TORQUE_COMMAND_hytech(MC3_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC3_TORQUE_COMMAND_hytech(MC3_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_TORQUE_COMMAND_hytech(MC4_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MC4_TORQUE_COMMAND_hytech(MC4_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MC4_TORQUE_COMMAND_hytech(MC4_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_STATUS_hytech(MCU_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MCU_STATUS_hytech(MCU_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MCU_STATUS_hytech(MCU_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_PEDAL_READINGS_hytech(MCU_PEDAL_READINGS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MCU_PEDAL_READINGS_hytech(MCU_PEDAL_READINGS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MCU_PEDAL_READINGS_hytech(MCU_PEDAL_READINGS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_LOAD_CELLS_hytech(MCU_LOAD_CELLS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MCU_LOAD_CELLS_hytech(MCU_LOAD_CELLS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MCU_LOAD_CELLS_hytech(MCU_LOAD_CELLS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_SAB_LOAD_CELLS_hytech(SAB_LOAD_CELLS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_SAB_LOAD_CELLS_hytech(SAB_LOAD_CELLS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_SAB_LOAD_CELLS_hytech(SAB_LOAD_CELLS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_FRONT_POTS_hytech(MCU_FRONT_POTS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MCU_FRONT_POTS_hytech(MCU_FRONT_POTS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MCU_FRONT_POTS_hytech(MCU_FRONT_POTS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_REAR_POTS_hytech(MCU_REAR_POTS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MCU_REAR_POTS_hytech(MCU_REAR_POTS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MCU_REAR_POTS_hytech(MCU_REAR_POTS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_ANALOG_READINGS_hytech(MCU_ANALOG_READINGS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_MCU_ANALOG_READINGS_hytech(MCU_ANALOG_READINGS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_MCU_ANALOG_READINGS_hytech(MCU_ANALOG_READINGS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_ONBOARD_TEMPS_hytech(BMS_ONBOARD_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_ONBOARD_TEMPS_hytech(BMS_ONBOARD_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_ONBOARD_TEMPS_hytech(BMS_ONBOARD_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_ONBOARD_DETAILED_TEMPS_hytech(BMS_ONBOARD_DETAILED_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_ONBOARD_DETAILED_TEMPS_hytech(BMS_ONBOARD_DETAILED_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_ONBOARD_DETAILED_TEMPS_hytech(BMS_ONBOARD_DETAILED_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_VOLTAGES_hytech(BMS_VOLTAGES_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_VOLTAGES_hytech(BMS_VOLTAGES_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_VOLTAGES_hytech(BMS_VOLTAGES_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_DETAILED_VOLTAGES_hytech(BMS_DETAILED_VOLTAGES_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_DETAILED_VOLTAGES_hytech(BMS_DETAILED_VOLTAGES_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_DETAILED_VOLTAGES_hytech(BMS_DETAILED_VOLTAGES_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_TEMPS_hytech(BMS_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_TEMPS_hytech(BMS_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_TEMPS_hytech(BMS_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_DETAILED_TEMPS_hytech(BMS_DETAILED_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_DETAILED_TEMPS_hytech(BMS_DETAILED_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_DETAILED_TEMPS_hytech(BMS_DETAILED_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_STATUS_hytech(BMS_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_STATUS_hytech(BMS_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_STATUS_hytech(BMS_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_CCU_STATUS_hytech(CCU_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_CCU_STATUS_hytech(CCU_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_CCU_STATUS_hytech(CCU_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_BALANCING_STATUS_hytech(BMS_BALANCING_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_BALANCING_STATUS_hytech(BMS_BALANCING_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_BALANCING_STATUS_hytech(BMS_BALANCING_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_COULOMB_COUNTS_hytech(BMS_COULOMB_COUNTS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_BMS_COULOMB_COUNTS_hytech(BMS_COULOMB_COUNTS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_BMS_COULOMB_COUNTS_hytech(BMS_COULOMB_COUNTS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_DASHBOARD_MCU_STATE_hytech(DASHBOARD_MCU_STATE_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_DASHBOARD_MCU_STATE_hytech(DASHBOARD_MCU_STATE_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_DASHBOARD_MCU_STATE_hytech(DASHBOARD_MCU_STATE_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_DASHBOARD_STATE_hytech(DASHBOARD_STATE_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_DASHBOARD_STATE_hytech(DASHBOARD_STATE_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_DASHBOARD_STATE_hytech(DASHBOARD_STATE_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_EM_STATUS_hytech(EM_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_EM_STATUS_hytech(EM_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_EM_STATUS_hytech(EM_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_EM_MEASUREMENT_hytech(EM_MEASUREMENT_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_EM_MEASUREMENT_hytech(EM_MEASUREMENT_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_EM_MEASUREMENT_hytech(EM_MEASUREMENT_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_CHARGER_CONTROL_hytech(CHARGER_CONTROL_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_CHARGER_CONTROL_hytech(CHARGER_CONTROL_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_CHARGER_CONTROL_hytech(CHARGER_CONTROL_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_CHARGER_DATA_hytech(CHARGER_DATA_t* _m, const uint8_t* _d, uint8_t dlc_);
#ifdef HYTECH_USE_CANSTRUCT
uint32_t Pack_CHARGER_DATA_hytech(CHARGER_DATA_t* _m, __CoderDbcCanFrame_t__* cframe);
#else
uint32_t Pack_CHARGER_DATA_hytech(CHARGER_DATA_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide);
#endif // HYTECH_USE_CANSTRUCT

#ifdef __cplusplus
}
#endif
