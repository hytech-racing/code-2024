#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// DBC file version
#define VER_HYTECH_MAJ_FMON (6U)
#define VER_HYTECH_MIN_FMON (0U)

#include "hytech-config.h"

#ifdef HYTECH_USE_DIAG_MONITORS

#include "canmonitorutil.h"
/*
This file contains the prototypes of all the functions that will be called
from each Unpack_*name* function to detect DBC related errors
It is the user responsibility to defined these functions in the
separated .c file. If it won't be done the linkage error will happen
*/

#ifdef HYTECH_USE_MONO_FMON

void _FMon_MONO_hytech(FrameMonitor_t* _mon, uint32_t msgid);

#define FMon_TCU_DRIVER_MSG_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_TCU_LAP_TIMES_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC1_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC2_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC3_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC4_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC1_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC2_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC3_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC4_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC1_ENERGY_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC2_ENERGY_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC3_ENERGY_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC4_ENERGY_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC1_SETPOINTS_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC2_SETPOINTS_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC3_SETPOINTS_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC4_SETPOINTS_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC1_TORQUE_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC2_TORQUE_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC3_TORQUE_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MC4_TORQUE_COMMAND_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MCU_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MCU_PEDAL_READINGS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MCU_LOAD_CELLS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_SAB_LOAD_CELLS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MCU_FRONT_POTS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MCU_REAR_POTS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_MCU_ANALOG_READINGS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_ONBOARD_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_ONBOARD_DETAILED_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_VOLTAGES_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_DETAILED_VOLTAGES_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_DETAILED_TEMPS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_CCU_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_BALANCING_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_BMS_COULOMB_COUNTS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_DASHBOARD_MCU_STATE_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_DASHBOARD_STATE_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_EM_STATUS_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_EM_MEASUREMENT_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_CHARGER_CONTROL_hytech(x, y) _FMon_MONO_hytech((x), (y))
#define FMon_CHARGER_DATA_hytech(x, y) _FMon_MONO_hytech((x), (y))

#else

void _FMon_TCU_DRIVER_MSG_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_TCU_LAP_TIMES_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC1_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC2_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC3_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC4_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC1_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC2_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC3_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC4_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC1_ENERGY_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC2_ENERGY_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC3_ENERGY_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC4_ENERGY_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC1_SETPOINTS_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC2_SETPOINTS_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC3_SETPOINTS_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC4_SETPOINTS_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC1_TORQUE_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC2_TORQUE_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC3_TORQUE_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MC4_TORQUE_COMMAND_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MCU_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MCU_PEDAL_READINGS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MCU_LOAD_CELLS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_SAB_LOAD_CELLS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MCU_FRONT_POTS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MCU_REAR_POTS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_MCU_ANALOG_READINGS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_ONBOARD_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_ONBOARD_DETAILED_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_VOLTAGES_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_DETAILED_VOLTAGES_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_DETAILED_TEMPS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_CCU_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_BALANCING_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_BMS_COULOMB_COUNTS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_DASHBOARD_MCU_STATE_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_DASHBOARD_STATE_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_EM_STATUS_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_EM_MEASUREMENT_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_CHARGER_CONTROL_hytech(FrameMonitor_t* _mon, uint32_t msgid);
void _FMon_CHARGER_DATA_hytech(FrameMonitor_t* _mon, uint32_t msgid);

#define FMon_TCU_DRIVER_MSG_hytech(x, y) _FMon_TCU_DRIVER_MSG_hytech((x), (y))
#define FMon_TCU_LAP_TIMES_hytech(x, y) _FMon_TCU_LAP_TIMES_hytech((x), (y))
#define FMon_MC1_STATUS_hytech(x, y) _FMon_MC1_STATUS_hytech((x), (y))
#define FMon_MC2_STATUS_hytech(x, y) _FMon_MC2_STATUS_hytech((x), (y))
#define FMon_MC3_STATUS_hytech(x, y) _FMon_MC3_STATUS_hytech((x), (y))
#define FMon_MC4_STATUS_hytech(x, y) _FMon_MC4_STATUS_hytech((x), (y))
#define FMon_MC1_TEMPS_hytech(x, y) _FMon_MC1_TEMPS_hytech((x), (y))
#define FMon_MC2_TEMPS_hytech(x, y) _FMon_MC2_TEMPS_hytech((x), (y))
#define FMon_MC3_TEMPS_hytech(x, y) _FMon_MC3_TEMPS_hytech((x), (y))
#define FMon_MC4_TEMPS_hytech(x, y) _FMon_MC4_TEMPS_hytech((x), (y))
#define FMon_MC1_ENERGY_hytech(x, y) _FMon_MC1_ENERGY_hytech((x), (y))
#define FMon_MC2_ENERGY_hytech(x, y) _FMon_MC2_ENERGY_hytech((x), (y))
#define FMon_MC3_ENERGY_hytech(x, y) _FMon_MC3_ENERGY_hytech((x), (y))
#define FMon_MC4_ENERGY_hytech(x, y) _FMon_MC4_ENERGY_hytech((x), (y))
#define FMon_MC1_SETPOINTS_COMMAND_hytech(x, y) _FMon_MC1_SETPOINTS_COMMAND_hytech((x), (y))
#define FMon_MC2_SETPOINTS_COMMAND_hytech(x, y) _FMon_MC2_SETPOINTS_COMMAND_hytech((x), (y))
#define FMon_MC3_SETPOINTS_COMMAND_hytech(x, y) _FMon_MC3_SETPOINTS_COMMAND_hytech((x), (y))
#define FMon_MC4_SETPOINTS_COMMAND_hytech(x, y) _FMon_MC4_SETPOINTS_COMMAND_hytech((x), (y))
#define FMon_MC1_TORQUE_COMMAND_hytech(x, y) _FMon_MC1_TORQUE_COMMAND_hytech((x), (y))
#define FMon_MC2_TORQUE_COMMAND_hytech(x, y) _FMon_MC2_TORQUE_COMMAND_hytech((x), (y))
#define FMon_MC3_TORQUE_COMMAND_hytech(x, y) _FMon_MC3_TORQUE_COMMAND_hytech((x), (y))
#define FMon_MC4_TORQUE_COMMAND_hytech(x, y) _FMon_MC4_TORQUE_COMMAND_hytech((x), (y))
#define FMon_MCU_STATUS_hytech(x, y) _FMon_MCU_STATUS_hytech((x), (y))
#define FMon_MCU_PEDAL_READINGS_hytech(x, y) _FMon_MCU_PEDAL_READINGS_hytech((x), (y))
#define FMon_MCU_LOAD_CELLS_hytech(x, y) _FMon_MCU_LOAD_CELLS_hytech((x), (y))
#define FMon_SAB_LOAD_CELLS_hytech(x, y) _FMon_SAB_LOAD_CELLS_hytech((x), (y))
#define FMon_MCU_FRONT_POTS_hytech(x, y) _FMon_MCU_FRONT_POTS_hytech((x), (y))
#define FMon_MCU_REAR_POTS_hytech(x, y) _FMon_MCU_REAR_POTS_hytech((x), (y))
#define FMon_MCU_ANALOG_READINGS_hytech(x, y) _FMon_MCU_ANALOG_READINGS_hytech((x), (y))
#define FMon_BMS_ONBOARD_TEMPS_hytech(x, y) _FMon_BMS_ONBOARD_TEMPS_hytech((x), (y))
#define FMon_BMS_ONBOARD_DETAILED_TEMPS_hytech(x, y) _FMon_BMS_ONBOARD_DETAILED_TEMPS_hytech((x), (y))
#define FMon_BMS_VOLTAGES_hytech(x, y) _FMon_BMS_VOLTAGES_hytech((x), (y))
#define FMon_BMS_DETAILED_VOLTAGES_hytech(x, y) _FMon_BMS_DETAILED_VOLTAGES_hytech((x), (y))
#define FMon_BMS_TEMPS_hytech(x, y) _FMon_BMS_TEMPS_hytech((x), (y))
#define FMon_BMS_DETAILED_TEMPS_hytech(x, y) _FMon_BMS_DETAILED_TEMPS_hytech((x), (y))
#define FMon_BMS_STATUS_hytech(x, y) _FMon_BMS_STATUS_hytech((x), (y))
#define FMon_CCU_STATUS_hytech(x, y) _FMon_CCU_STATUS_hytech((x), (y))
#define FMon_BMS_BALANCING_STATUS_hytech(x, y) _FMon_BMS_BALANCING_STATUS_hytech((x), (y))
#define FMon_BMS_COULOMB_COUNTS_hytech(x, y) _FMon_BMS_COULOMB_COUNTS_hytech((x), (y))
#define FMon_DASHBOARD_MCU_STATE_hytech(x, y) _FMon_DASHBOARD_MCU_STATE_hytech((x), (y))
#define FMon_DASHBOARD_STATE_hytech(x, y) _FMon_DASHBOARD_STATE_hytech((x), (y))
#define FMon_EM_STATUS_hytech(x, y) _FMon_EM_STATUS_hytech((x), (y))
#define FMon_EM_MEASUREMENT_hytech(x, y) _FMon_EM_MEASUREMENT_hytech((x), (y))
#define FMon_CHARGER_CONTROL_hytech(x, y) _FMon_CHARGER_CONTROL_hytech((x), (y))
#define FMon_CHARGER_DATA_hytech(x, y) _FMon_CHARGER_DATA_hytech((x), (y))

#endif

#endif // HYTECH_USE_DIAG_MONITORS

#ifdef __cplusplus
}
#endif
