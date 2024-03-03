#include "hytech.h"


// DBC file version
#if (VER_HYTECH_MAJ != (6U)) || (VER_HYTECH_MIN != (0U))
#error The HYTECH dbc source files have different versions
#endif

#ifdef HYTECH_USE_DIAG_MONITORS
// Function prototypes to be called each time CAN frame is unpacked
// FMon function may detect RC, CRC or DLC violation
#include "hytech-fmon.h"

#endif // HYTECH_USE_DIAG_MONITORS

// This macro guard for the case when you need to enable
// using diag monitors but there is no necessity in proper
// SysTick provider. For providing one you need define macro
// before this line - in dbccodeconf.h

#ifndef GetSystemTick
#define GetSystemTick() (0u)
#endif

// This macro guard is for the case when you want to build
// app with enabled optoin auto CSM, but don't yet have
// proper getframehash implementation

#ifndef GetFrameHash
#define GetFrameHash(a,b,c,d,e) (0u)
#endif

// This function performs extension of sign for the signals
// which have non-aligned to power of 2 bit's width.
// The types 'bitext_t' and 'ubitext_t' define maximal bit width which
// can be correctly handled. You need to select type which can contain
// n+1 bits where n is the largest signed signal width. For example if
// the most wide signed signal has a width of 31 bits you need to set
// bitext_t as int32_t and ubitext_t as uint32_t
// Defined these typedefs in @dbccodeconf.h or locally in 'dbcdrvname'-config.h
static bitext_t __ext_sig__(ubitext_t val, uint8_t bits)
{
  ubitext_t const m = 1u << (bits - 1);
  return (val ^ m) - m;
}

uint32_t Unpack_TCU_DRIVER_MSG_hytech(TCU_DRIVER_MSG_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->driver_message = (uint8_t) ( (_d[0] & (0xFFU)) );
  _m->driver_msg_var_1 = (uint16_t) ( ((_d[2] & (0xFFU)) << 8U) | (_d[1] & (0xFFU)) );
  _m->driver_msg_var_2 = (uint16_t) ( ((_d[4] & (0xFFU)) << 8U) | (_d[3] & (0xFFU)) );
  _m->target_lap_time = (uint32_t) ( ((_d[7] & (0xFFU)) << 16U) | ((_d[6] & (0xFFU)) << 8U) | (_d[5] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < TCU_DRIVER_MSG_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_TCU_DRIVER_MSG_hytech(&_m->mon1, TCU_DRIVER_MSG_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return TCU_DRIVER_MSG_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_TCU_DRIVER_MSG_hytech(TCU_DRIVER_MSG_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(TCU_DRIVER_MSG_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->driver_message & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( (_m->driver_msg_var_1 & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( ((_m->driver_msg_var_1 >> 8U) & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( (_m->driver_msg_var_2 & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->driver_msg_var_2 >> 8U) & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( (_m->target_lap_time & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( ((_m->target_lap_time >> 8U) & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->target_lap_time >> 16U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) TCU_DRIVER_MSG_CANID;
  cframe->DLC = (uint8_t) TCU_DRIVER_MSG_DLC;
  cframe->IDE = (uint8_t) TCU_DRIVER_MSG_IDE;
  return TCU_DRIVER_MSG_CANID;
}

#else

uint32_t Pack_TCU_DRIVER_MSG_hytech(TCU_DRIVER_MSG_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(TCU_DRIVER_MSG_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->driver_message & (0xFFU)) );
  _d[1] |= (uint8_t) ( (_m->driver_msg_var_1 & (0xFFU)) );
  _d[2] |= (uint8_t) ( ((_m->driver_msg_var_1 >> 8U) & (0xFFU)) );
  _d[3] |= (uint8_t) ( (_m->driver_msg_var_2 & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->driver_msg_var_2 >> 8U) & (0xFFU)) );
  _d[5] |= (uint8_t) ( (_m->target_lap_time & (0xFFU)) );
  _d[6] |= (uint8_t) ( ((_m->target_lap_time >> 8U) & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->target_lap_time >> 16U) & (0xFFU)) );

  *_len = (uint8_t) TCU_DRIVER_MSG_DLC;
  *_ide = (uint8_t) TCU_DRIVER_MSG_IDE;
  return TCU_DRIVER_MSG_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_TCU_LAP_TIMES_hytech(TCU_LAP_TIMES_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->best_lap_time = (uint32_t) ( ((_d[2] & (0xFFU)) << 16U) | ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->prev_lap_time = (uint32_t) ( ((_d[5] & (0xFFU)) << 16U) | ((_d[4] & (0xFFU)) << 8U) | (_d[3] & (0xFFU)) );
  _m->lap_clock_state = (uint8_t) ( (_d[6] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < TCU_LAP_TIMES_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_TCU_LAP_TIMES_hytech(&_m->mon1, TCU_LAP_TIMES_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return TCU_LAP_TIMES_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_TCU_LAP_TIMES_hytech(TCU_LAP_TIMES_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(TCU_LAP_TIMES_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->best_lap_time & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->best_lap_time >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( ((_m->best_lap_time >> 16U) & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( (_m->prev_lap_time & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->prev_lap_time >> 8U) & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->prev_lap_time >> 16U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->lap_clock_state & (0xFFU)) );

  cframe->MsgId = (uint32_t) TCU_LAP_TIMES_CANID;
  cframe->DLC = (uint8_t) TCU_LAP_TIMES_DLC;
  cframe->IDE = (uint8_t) TCU_LAP_TIMES_IDE;
  return TCU_LAP_TIMES_CANID;
}

#else

uint32_t Pack_TCU_LAP_TIMES_hytech(TCU_LAP_TIMES_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(TCU_LAP_TIMES_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->best_lap_time & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->best_lap_time >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( ((_m->best_lap_time >> 16U) & (0xFFU)) );
  _d[3] |= (uint8_t) ( (_m->prev_lap_time & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->prev_lap_time >> 8U) & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->prev_lap_time >> 16U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->lap_clock_state & (0xFFU)) );

  *_len = (uint8_t) TCU_LAP_TIMES_DLC;
  *_ide = (uint8_t) TCU_LAP_TIMES_IDE;
  return TCU_LAP_TIMES_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_STATUS_hytech(MC1_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->system_ready = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->error = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->warning = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->quit_dc_on = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->dc_on = (uint8_t) ( ((_d[1] >> 4U) & (0x01U)) );
  _m->quit_inverter_on = (uint8_t) ( ((_d[1] >> 5U) & (0x01U)) );
  _m->inverter_on = (uint8_t) ( ((_d[1] >> 6U) & (0x01U)) );
  _m->derating_on = (uint8_t) ( ((_d[1] >> 7U) & (0x01U)) );
  _m->speed_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->torque_current = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->magnetizing_current = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC1_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC1_STATUS_hytech(&_m->mon1, MC1_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC1_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC1_STATUS_hytech(MC1_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC1_STATUS_CANID;
  cframe->DLC = (uint8_t) MC1_STATUS_DLC;
  cframe->IDE = (uint8_t) MC1_STATUS_IDE;
  return MC1_STATUS_CANID;
}

#else

uint32_t Pack_MC1_STATUS_hytech(MC1_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  _d[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC1_STATUS_DLC;
  *_ide = (uint8_t) MC1_STATUS_IDE;
  return MC1_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_STATUS_hytech(MC2_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->system_ready = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->error = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->warning = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->quit_dc_on = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->dc_on = (uint8_t) ( ((_d[1] >> 4U) & (0x01U)) );
  _m->quit_inverter_on = (uint8_t) ( ((_d[1] >> 5U) & (0x01U)) );
  _m->inverter_on = (uint8_t) ( ((_d[1] >> 6U) & (0x01U)) );
  _m->derating_on = (uint8_t) ( ((_d[1] >> 7U) & (0x01U)) );
  _m->speed_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->torque_current = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->magnetizing_current = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC2_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC2_STATUS_hytech(&_m->mon1, MC2_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC2_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC2_STATUS_hytech(MC2_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC2_STATUS_CANID;
  cframe->DLC = (uint8_t) MC2_STATUS_DLC;
  cframe->IDE = (uint8_t) MC2_STATUS_IDE;
  return MC2_STATUS_CANID;
}

#else

uint32_t Pack_MC2_STATUS_hytech(MC2_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  _d[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC2_STATUS_DLC;
  *_ide = (uint8_t) MC2_STATUS_IDE;
  return MC2_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_STATUS_hytech(MC3_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->system_ready = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->error = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->warning = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->quit_dc_on = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->dc_on = (uint8_t) ( ((_d[1] >> 4U) & (0x01U)) );
  _m->quit_inverter_on = (uint8_t) ( ((_d[1] >> 5U) & (0x01U)) );
  _m->inverter_on = (uint8_t) ( ((_d[1] >> 6U) & (0x01U)) );
  _m->derating_on = (uint8_t) ( ((_d[1] >> 7U) & (0x01U)) );
  _m->speed_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->torque_current = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->magnetizing_current = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC3_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC3_STATUS_hytech(&_m->mon1, MC3_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC3_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC3_STATUS_hytech(MC3_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC3_STATUS_CANID;
  cframe->DLC = (uint8_t) MC3_STATUS_DLC;
  cframe->IDE = (uint8_t) MC3_STATUS_IDE;
  return MC3_STATUS_CANID;
}

#else

uint32_t Pack_MC3_STATUS_hytech(MC3_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  _d[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC3_STATUS_DLC;
  *_ide = (uint8_t) MC3_STATUS_IDE;
  return MC3_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_STATUS_hytech(MC4_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->system_ready = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->error = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->warning = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->quit_dc_on = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->dc_on = (uint8_t) ( ((_d[1] >> 4U) & (0x01U)) );
  _m->quit_inverter_on = (uint8_t) ( ((_d[1] >> 5U) & (0x01U)) );
  _m->inverter_on = (uint8_t) ( ((_d[1] >> 6U) & (0x01U)) );
  _m->derating_on = (uint8_t) ( ((_d[1] >> 7U) & (0x01U)) );
  _m->speed_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->torque_current = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->magnetizing_current = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC4_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC4_STATUS_hytech(&_m->mon1, MC4_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC4_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC4_STATUS_hytech(MC4_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC4_STATUS_CANID;
  cframe->DLC = (uint8_t) MC4_STATUS_DLC;
  cframe->IDE = (uint8_t) MC4_STATUS_IDE;
  return MC4_STATUS_CANID;
}

#else

uint32_t Pack_MC4_STATUS_hytech(MC4_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->system_ready & (0x01U)) | ((_m->error & (0x01U)) << 1U) | ((_m->warning & (0x01U)) << 2U) | ((_m->quit_dc_on & (0x01U)) << 3U) | ((_m->dc_on & (0x01U)) << 4U) | ((_m->quit_inverter_on & (0x01U)) << 5U) | ((_m->inverter_on & (0x01U)) << 6U) | ((_m->derating_on & (0x01U)) << 7U) );
  _d[2] |= (uint8_t) ( (_m->speed_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->torque_current & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->torque_current >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->magnetizing_current & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->magnetizing_current >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC4_STATUS_DLC;
  *_ide = (uint8_t) MC4_STATUS_IDE;
  return MC4_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_TEMPS_hytech(MC1_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->motor_temp_ro = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_phys = (sigfloat_t)(HYTECH_motor_temp_ro_fromS(_m->motor_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->inverter_temp_ro = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->inverter_temp_phys = (sigfloat_t)(HYTECH_inverter_temp_ro_fromS(_m->inverter_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->diagnostic_number = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
  _m->igbt_temp_ro = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->igbt_temp_phys = (sigfloat_t)(HYTECH_igbt_temp_ro_fromS(_m->igbt_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC1_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC1_TEMPS_hytech(&_m->mon1, MC1_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC1_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC1_TEMPS_hytech(MC1_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC1_TEMPS_CANID;
  cframe->DLC = (uint8_t) MC1_TEMPS_DLC;
  cframe->IDE = (uint8_t) MC1_TEMPS_IDE;
  return MC1_TEMPS_CANID;
}

#else

uint32_t Pack_MC1_TEMPS_hytech(MC1_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC1_TEMPS_DLC;
  *_ide = (uint8_t) MC1_TEMPS_IDE;
  return MC1_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_TEMPS_hytech(MC2_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->motor_temp_ro = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_phys = (sigfloat_t)(HYTECH_motor_temp_ro_fromS(_m->motor_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->inverter_temp_ro = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->inverter_temp_phys = (sigfloat_t)(HYTECH_inverter_temp_ro_fromS(_m->inverter_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->diagnostic_number = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
  _m->igbt_temp_ro = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->igbt_temp_phys = (sigfloat_t)(HYTECH_igbt_temp_ro_fromS(_m->igbt_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC2_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC2_TEMPS_hytech(&_m->mon1, MC2_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC2_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC2_TEMPS_hytech(MC2_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC2_TEMPS_CANID;
  cframe->DLC = (uint8_t) MC2_TEMPS_DLC;
  cframe->IDE = (uint8_t) MC2_TEMPS_IDE;
  return MC2_TEMPS_CANID;
}

#else

uint32_t Pack_MC2_TEMPS_hytech(MC2_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC2_TEMPS_DLC;
  *_ide = (uint8_t) MC2_TEMPS_IDE;
  return MC2_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_TEMPS_hytech(MC3_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->motor_temp_ro = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_phys = (sigfloat_t)(HYTECH_motor_temp_ro_fromS(_m->motor_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->inverter_temp_ro = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->inverter_temp_phys = (sigfloat_t)(HYTECH_inverter_temp_ro_fromS(_m->inverter_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->diagnostic_number = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
  _m->igbt_temp_ro = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->igbt_temp_phys = (sigfloat_t)(HYTECH_igbt_temp_ro_fromS(_m->igbt_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC3_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC3_TEMPS_hytech(&_m->mon1, MC3_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC3_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC3_TEMPS_hytech(MC3_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC3_TEMPS_CANID;
  cframe->DLC = (uint8_t) MC3_TEMPS_DLC;
  cframe->IDE = (uint8_t) MC3_TEMPS_IDE;
  return MC3_TEMPS_CANID;
}

#else

uint32_t Pack_MC3_TEMPS_hytech(MC3_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC3_TEMPS_DLC;
  *_ide = (uint8_t) MC3_TEMPS_IDE;
  return MC3_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_TEMPS_hytech(MC4_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->motor_temp_ro = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_phys = (sigfloat_t)(HYTECH_motor_temp_ro_fromS(_m->motor_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->inverter_temp_ro = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->inverter_temp_phys = (sigfloat_t)(HYTECH_inverter_temp_ro_fromS(_m->inverter_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->diagnostic_number = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
  _m->igbt_temp_ro = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->igbt_temp_phys = (sigfloat_t)(HYTECH_igbt_temp_ro_fromS(_m->igbt_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC4_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC4_TEMPS_hytech(&_m->mon1, MC4_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC4_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC4_TEMPS_hytech(MC4_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC4_TEMPS_CANID;
  cframe->DLC = (uint8_t) MC4_TEMPS_DLC;
  cframe->IDE = (uint8_t) MC4_TEMPS_IDE;
  return MC4_TEMPS_CANID;
}

#else

uint32_t Pack_MC4_TEMPS_hytech(MC4_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->motor_temp_ro = (int16_t) HYTECH_motor_temp_ro_toS(_m->motor_temp_phys);
  _m->inverter_temp_ro = (int16_t) HYTECH_inverter_temp_ro_toS(_m->inverter_temp_phys);
  _m->igbt_temp_ro = (int16_t) HYTECH_igbt_temp_ro_toS(_m->igbt_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->motor_temp_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->motor_temp_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->inverter_temp_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->inverter_temp_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->diagnostic_number & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->diagnostic_number >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->igbt_temp_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->igbt_temp_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC4_TEMPS_DLC;
  *_ide = (uint8_t) MC4_TEMPS_IDE;
  return MC4_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_ENERGY_hytech(MC1_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->dc_bus_voltage = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->motor_power = (uint32_t) ( ((_d[5] & (0xFFU)) << 24U) | ((_d[4] & (0xFFU)) << 16U) | ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->feedback_torque = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC1_ENERGY_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC1_ENERGY_hytech(&_m->mon1, MC1_ENERGY_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC1_ENERGY_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC1_ENERGY_hytech(MC1_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_ENERGY_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC1_ENERGY_CANID;
  cframe->DLC = (uint8_t) MC1_ENERGY_DLC;
  cframe->IDE = (uint8_t) MC1_ENERGY_IDE;
  return MC1_ENERGY_CANID;
}

#else

uint32_t Pack_MC1_ENERGY_hytech(MC1_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_ENERGY_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC1_ENERGY_DLC;
  *_ide = (uint8_t) MC1_ENERGY_IDE;
  return MC1_ENERGY_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_ENERGY_hytech(MC2_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->dc_bus_voltage = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->motor_power = (uint32_t) ( ((_d[5] & (0xFFU)) << 24U) | ((_d[4] & (0xFFU)) << 16U) | ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->feedback_torque = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC2_ENERGY_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC2_ENERGY_hytech(&_m->mon1, MC2_ENERGY_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC2_ENERGY_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC2_ENERGY_hytech(MC2_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_ENERGY_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC2_ENERGY_CANID;
  cframe->DLC = (uint8_t) MC2_ENERGY_DLC;
  cframe->IDE = (uint8_t) MC2_ENERGY_IDE;
  return MC2_ENERGY_CANID;
}

#else

uint32_t Pack_MC2_ENERGY_hytech(MC2_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_ENERGY_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC2_ENERGY_DLC;
  *_ide = (uint8_t) MC2_ENERGY_IDE;
  return MC2_ENERGY_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_ENERGY_hytech(MC3_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->dc_bus_voltage = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->motor_power = (uint32_t) ( ((_d[5] & (0xFFU)) << 24U) | ((_d[4] & (0xFFU)) << 16U) | ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->feedback_torque = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC3_ENERGY_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC3_ENERGY_hytech(&_m->mon1, MC3_ENERGY_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC3_ENERGY_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC3_ENERGY_hytech(MC3_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_ENERGY_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC3_ENERGY_CANID;
  cframe->DLC = (uint8_t) MC3_ENERGY_DLC;
  cframe->IDE = (uint8_t) MC3_ENERGY_IDE;
  return MC3_ENERGY_CANID;
}

#else

uint32_t Pack_MC3_ENERGY_hytech(MC3_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_ENERGY_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC3_ENERGY_DLC;
  *_ide = (uint8_t) MC3_ENERGY_IDE;
  return MC3_ENERGY_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_ENERGY_hytech(MC4_ENERGY_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->dc_bus_voltage = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->motor_power = (uint32_t) ( ((_d[5] & (0xFFU)) << 24U) | ((_d[4] & (0xFFU)) << 16U) | ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->feedback_torque = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC4_ENERGY_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC4_ENERGY_hytech(&_m->mon1, MC4_ENERGY_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC4_ENERGY_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC4_ENERGY_hytech(MC4_ENERGY_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_ENERGY_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC4_ENERGY_CANID;
  cframe->DLC = (uint8_t) MC4_ENERGY_DLC;
  cframe->IDE = (uint8_t) MC4_ENERGY_IDE;
  return MC4_ENERGY_CANID;
}

#else

uint32_t Pack_MC4_ENERGY_hytech(MC4_ENERGY_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_ENERGY_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->dc_bus_voltage & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->dc_bus_voltage >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->motor_power & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->motor_power >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->motor_power >> 16U) & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->motor_power >> 24U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->feedback_torque & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->feedback_torque >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC4_ENERGY_DLC;
  *_ide = (uint8_t) MC4_ENERGY_IDE;
  return MC4_ENERGY_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_SETPOINTS_COMMAND_hytech(MC1_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->inverter_enable = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->hv_enable = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->driver_enable = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->remove_error = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->speed_setpoint_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->positive_torque_limit = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->negative_torque_limit = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC1_SETPOINTS_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC1_SETPOINTS_COMMAND_hytech(&_m->mon1, MC1_SETPOINTS_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC1_SETPOINTS_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC1_SETPOINTS_COMMAND_hytech(MC1_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_SETPOINTS_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC1_SETPOINTS_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC1_SETPOINTS_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC1_SETPOINTS_COMMAND_IDE;
  return MC1_SETPOINTS_COMMAND_CANID;
}

#else

uint32_t Pack_MC1_SETPOINTS_COMMAND_hytech(MC1_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_SETPOINTS_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  _d[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC1_SETPOINTS_COMMAND_DLC;
  *_ide = (uint8_t) MC1_SETPOINTS_COMMAND_IDE;
  return MC1_SETPOINTS_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_SETPOINTS_COMMAND_hytech(MC2_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->inverter_enable = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->hv_enable = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->driver_enable = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->remove_error = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->speed_setpoint_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->positive_torque_limit = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->negative_torque_limit = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC2_SETPOINTS_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC2_SETPOINTS_COMMAND_hytech(&_m->mon1, MC2_SETPOINTS_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC2_SETPOINTS_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC2_SETPOINTS_COMMAND_hytech(MC2_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_SETPOINTS_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC2_SETPOINTS_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC2_SETPOINTS_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC2_SETPOINTS_COMMAND_IDE;
  return MC2_SETPOINTS_COMMAND_CANID;
}

#else

uint32_t Pack_MC2_SETPOINTS_COMMAND_hytech(MC2_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_SETPOINTS_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  _d[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC2_SETPOINTS_COMMAND_DLC;
  *_ide = (uint8_t) MC2_SETPOINTS_COMMAND_IDE;
  return MC2_SETPOINTS_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_SETPOINTS_COMMAND_hytech(MC3_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->inverter_enable = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->hv_enable = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->driver_enable = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->remove_error = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->speed_setpoint_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->positive_torque_limit = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->negative_torque_limit = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC3_SETPOINTS_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC3_SETPOINTS_COMMAND_hytech(&_m->mon1, MC3_SETPOINTS_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC3_SETPOINTS_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC3_SETPOINTS_COMMAND_hytech(MC3_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_SETPOINTS_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC3_SETPOINTS_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC3_SETPOINTS_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC3_SETPOINTS_COMMAND_IDE;
  return MC3_SETPOINTS_COMMAND_CANID;
}

#else

uint32_t Pack_MC3_SETPOINTS_COMMAND_hytech(MC3_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_SETPOINTS_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  _d[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC3_SETPOINTS_COMMAND_DLC;
  *_ide = (uint8_t) MC3_SETPOINTS_COMMAND_IDE;
  return MC3_SETPOINTS_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_SETPOINTS_COMMAND_hytech(MC4_SETPOINTS_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->inverter_enable = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->hv_enable = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->driver_enable = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->remove_error = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->speed_setpoint_rpm = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
  _m->positive_torque_limit = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->negative_torque_limit = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC4_SETPOINTS_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC4_SETPOINTS_COMMAND_hytech(&_m->mon1, MC4_SETPOINTS_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC4_SETPOINTS_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC4_SETPOINTS_COMMAND_hytech(MC4_SETPOINTS_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_SETPOINTS_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  cframe->Data[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC4_SETPOINTS_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC4_SETPOINTS_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC4_SETPOINTS_COMMAND_IDE;
  return MC4_SETPOINTS_COMMAND_CANID;
}

#else

uint32_t Pack_MC4_SETPOINTS_COMMAND_hytech(MC4_SETPOINTS_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_SETPOINTS_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[1] |= (uint8_t) ( (_m->inverter_enable & (0x01U)) | ((_m->hv_enable & (0x01U)) << 1U) | ((_m->driver_enable & (0x01U)) << 2U) | ((_m->remove_error & (0x01U)) << 3U) );
  _d[2] |= (uint8_t) ( (_m->speed_setpoint_rpm & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->speed_setpoint_rpm >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->positive_torque_limit & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->positive_torque_limit >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->negative_torque_limit & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->negative_torque_limit >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC4_SETPOINTS_COMMAND_DLC;
  *_ide = (uint8_t) MC4_SETPOINTS_COMMAND_IDE;
  return MC4_SETPOINTS_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC1_TORQUE_COMMAND_hytech(MC1_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->torque_command = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC1_TORQUE_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC1_TORQUE_COMMAND_hytech(&_m->mon1, MC1_TORQUE_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC1_TORQUE_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC1_TORQUE_COMMAND_hytech(MC1_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_TORQUE_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC1_TORQUE_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC1_TORQUE_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC1_TORQUE_COMMAND_IDE;
  return MC1_TORQUE_COMMAND_CANID;
}

#else

uint32_t Pack_MC1_TORQUE_COMMAND_hytech(MC1_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC1_TORQUE_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC1_TORQUE_COMMAND_DLC;
  *_ide = (uint8_t) MC1_TORQUE_COMMAND_IDE;
  return MC1_TORQUE_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC2_TORQUE_COMMAND_hytech(MC2_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->torque_command = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC2_TORQUE_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC2_TORQUE_COMMAND_hytech(&_m->mon1, MC2_TORQUE_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC2_TORQUE_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC2_TORQUE_COMMAND_hytech(MC2_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_TORQUE_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC2_TORQUE_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC2_TORQUE_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC2_TORQUE_COMMAND_IDE;
  return MC2_TORQUE_COMMAND_CANID;
}

#else

uint32_t Pack_MC2_TORQUE_COMMAND_hytech(MC2_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC2_TORQUE_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC2_TORQUE_COMMAND_DLC;
  *_ide = (uint8_t) MC2_TORQUE_COMMAND_IDE;
  return MC2_TORQUE_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC3_TORQUE_COMMAND_hytech(MC3_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->torque_command = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC3_TORQUE_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC3_TORQUE_COMMAND_hytech(&_m->mon1, MC3_TORQUE_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC3_TORQUE_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC3_TORQUE_COMMAND_hytech(MC3_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_TORQUE_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC3_TORQUE_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC3_TORQUE_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC3_TORQUE_COMMAND_IDE;
  return MC3_TORQUE_COMMAND_CANID;
}

#else

uint32_t Pack_MC3_TORQUE_COMMAND_hytech(MC3_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC3_TORQUE_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC3_TORQUE_COMMAND_DLC;
  *_ide = (uint8_t) MC3_TORQUE_COMMAND_IDE;
  return MC3_TORQUE_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MC4_TORQUE_COMMAND_hytech(MC4_TORQUE_COMMAND_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->torque_command = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MC4_TORQUE_COMMAND_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MC4_TORQUE_COMMAND_hytech(&_m->mon1, MC4_TORQUE_COMMAND_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MC4_TORQUE_COMMAND_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MC4_TORQUE_COMMAND_hytech(MC4_TORQUE_COMMAND_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_TORQUE_COMMAND_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MC4_TORQUE_COMMAND_CANID;
  cframe->DLC = (uint8_t) MC4_TORQUE_COMMAND_DLC;
  cframe->IDE = (uint8_t) MC4_TORQUE_COMMAND_IDE;
  return MC4_TORQUE_COMMAND_CANID;
}

#else

uint32_t Pack_MC4_TORQUE_COMMAND_hytech(MC4_TORQUE_COMMAND_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MC4_TORQUE_COMMAND_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->torque_command & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->torque_command >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MC4_TORQUE_COMMAND_DLC;
  *_ide = (uint8_t) MC4_TORQUE_COMMAND_IDE;
  return MC4_TORQUE_COMMAND_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_STATUS_hytech(MCU_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->imd_ok_high = (uint8_t) ( (_d[0] & (0x01U)) );
  _m->shutdown_b_above_threshold = (uint8_t) ( ((_d[0] >> 1U) & (0x01U)) );
  _m->bms_ok_high = (uint8_t) ( ((_d[0] >> 2U) & (0x01U)) );
  _m->shutdown_c_above_threshold = (uint8_t) ( ((_d[0] >> 3U) & (0x01U)) );
  _m->bspd_ok_high = (uint8_t) ( ((_d[0] >> 4U) & (0x01U)) );
  _m->shutdown_d_above_threshold = (uint8_t) ( ((_d[0] >> 5U) & (0x01U)) );
  _m->shoftware_ok_high = (uint8_t) ( ((_d[0] >> 6U) & (0x01U)) );
  _m->shutdown_e_above_threshold = (uint8_t) ( ((_d[0] >> 7U) & (0x01U)) );
  _m->mechanical_brake_active = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->no_accel_implausability = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->no_brake_implausability = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->brake_pedal_active = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->bspd_current_high = (uint8_t) ( ((_d[1] >> 4U) & (0x01U)) );
  _m->bspd_brake_high = (uint8_t) ( ((_d[1] >> 5U) & (0x01U)) );
  _m->no_accel_or_brake_implausability = (uint8_t) ( ((_d[1] >> 6U) & (0x01U)) );
  _m->drive_buzzer = (uint8_t) ( ((_d[1] >> 7U) & (0x01U)) );
  _m->ecu_state = (uint8_t) ( (_d[2] & (0x07U)) );
  _m->inverter_error = (uint8_t) ( ((_d[2] >> 3U) & (0x01U)) );
  _m->energy_meter_present = (uint8_t) ( ((_d[2] >> 4U) & (0x01U)) );
  _m->software_ok = (uint8_t) ( ((_d[2] >> 6U) & (0x01U)) );
  _m->launch_control_active = (uint8_t) ( ((_d[2] >> 7U) & (0x01U)) );
  _m->pack_charge_critical = (uint8_t) ( (_d[3] & (0x03U)) );
  _m->max_torque = (uint8_t) ( (_d[4] & (0xFFU)) );
  _m->torque_mode = (uint8_t) ( (_d[5] & (0xFFU)) );
  _m->distance_traveled_m_ro = (uint16_t) ( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->distance_traveled_m_phys = (sigfloat_t)(HYTECH_distance_traveled_m_ro_fromS(_m->distance_traveled_m_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MCU_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MCU_STATUS_hytech(&_m->mon1, MCU_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MCU_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MCU_STATUS_hytech(MCU_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->distance_traveled_m_ro = (uint16_t) HYTECH_distance_traveled_m_ro_toS(_m->distance_traveled_m_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->imd_ok_high & (0x01U)) | ((_m->shutdown_b_above_threshold & (0x01U)) << 1U) | ((_m->bms_ok_high & (0x01U)) << 2U) | ((_m->shutdown_c_above_threshold & (0x01U)) << 3U) | ((_m->bspd_ok_high & (0x01U)) << 4U) | ((_m->shutdown_d_above_threshold & (0x01U)) << 5U) | ((_m->shoftware_ok_high & (0x01U)) << 6U) | ((_m->shutdown_e_above_threshold & (0x01U)) << 7U) );
  cframe->Data[1] |= (uint8_t) ( (_m->mechanical_brake_active & (0x01U)) | ((_m->no_accel_implausability & (0x01U)) << 1U) | ((_m->no_brake_implausability & (0x01U)) << 2U) | ((_m->brake_pedal_active & (0x01U)) << 3U) | ((_m->bspd_current_high & (0x01U)) << 4U) | ((_m->bspd_brake_high & (0x01U)) << 5U) | ((_m->no_accel_or_brake_implausability & (0x01U)) << 6U) | ((_m->drive_buzzer & (0x01U)) << 7U) );
  cframe->Data[2] |= (uint8_t) ( (_m->ecu_state & (0x07U)) | ((_m->inverter_error & (0x01U)) << 3U) | ((_m->energy_meter_present & (0x01U)) << 4U) | ((_m->software_ok & (0x01U)) << 6U) | ((_m->launch_control_active & (0x01U)) << 7U) );
  cframe->Data[3] |= (uint8_t) ( (_m->pack_charge_critical & (0x03U)) );
  cframe->Data[4] |= (uint8_t) ( (_m->max_torque & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( (_m->torque_mode & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->distance_traveled_m_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->distance_traveled_m_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MCU_STATUS_CANID;
  cframe->DLC = (uint8_t) MCU_STATUS_DLC;
  cframe->IDE = (uint8_t) MCU_STATUS_IDE;
  return MCU_STATUS_CANID;
}

#else

uint32_t Pack_MCU_STATUS_hytech(MCU_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->distance_traveled_m_ro = (uint16_t) HYTECH_distance_traveled_m_ro_toS(_m->distance_traveled_m_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->imd_ok_high & (0x01U)) | ((_m->shutdown_b_above_threshold & (0x01U)) << 1U) | ((_m->bms_ok_high & (0x01U)) << 2U) | ((_m->shutdown_c_above_threshold & (0x01U)) << 3U) | ((_m->bspd_ok_high & (0x01U)) << 4U) | ((_m->shutdown_d_above_threshold & (0x01U)) << 5U) | ((_m->shoftware_ok_high & (0x01U)) << 6U) | ((_m->shutdown_e_above_threshold & (0x01U)) << 7U) );
  _d[1] |= (uint8_t) ( (_m->mechanical_brake_active & (0x01U)) | ((_m->no_accel_implausability & (0x01U)) << 1U) | ((_m->no_brake_implausability & (0x01U)) << 2U) | ((_m->brake_pedal_active & (0x01U)) << 3U) | ((_m->bspd_current_high & (0x01U)) << 4U) | ((_m->bspd_brake_high & (0x01U)) << 5U) | ((_m->no_accel_or_brake_implausability & (0x01U)) << 6U) | ((_m->drive_buzzer & (0x01U)) << 7U) );
  _d[2] |= (uint8_t) ( (_m->ecu_state & (0x07U)) | ((_m->inverter_error & (0x01U)) << 3U) | ((_m->energy_meter_present & (0x01U)) << 4U) | ((_m->software_ok & (0x01U)) << 6U) | ((_m->launch_control_active & (0x01U)) << 7U) );
  _d[3] |= (uint8_t) ( (_m->pack_charge_critical & (0x03U)) );
  _d[4] |= (uint8_t) ( (_m->max_torque & (0xFFU)) );
  _d[5] |= (uint8_t) ( (_m->torque_mode & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->distance_traveled_m_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->distance_traveled_m_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MCU_STATUS_DLC;
  *_ide = (uint8_t) MCU_STATUS_IDE;
  return MCU_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_PEDAL_READINGS_hytech(MCU_PEDAL_READINGS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->accel_pedal_1 = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->accel_pedal_2 = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->brake_pedal_1 = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
  _m->brake_pedal_2 = (uint16_t) ( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MCU_PEDAL_READINGS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MCU_PEDAL_READINGS_hytech(&_m->mon1, MCU_PEDAL_READINGS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MCU_PEDAL_READINGS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MCU_PEDAL_READINGS_hytech(MCU_PEDAL_READINGS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_PEDAL_READINGS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->accel_pedal_1 & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->accel_pedal_1 >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->accel_pedal_2 & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->accel_pedal_2 >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->brake_pedal_1 & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->brake_pedal_1 >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->brake_pedal_2 & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->brake_pedal_2 >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MCU_PEDAL_READINGS_CANID;
  cframe->DLC = (uint8_t) MCU_PEDAL_READINGS_DLC;
  cframe->IDE = (uint8_t) MCU_PEDAL_READINGS_IDE;
  return MCU_PEDAL_READINGS_CANID;
}

#else

uint32_t Pack_MCU_PEDAL_READINGS_hytech(MCU_PEDAL_READINGS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_PEDAL_READINGS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->accel_pedal_1 & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->accel_pedal_1 >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->accel_pedal_2 & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->accel_pedal_2 >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->brake_pedal_1 & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->brake_pedal_1 >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->brake_pedal_2 & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->brake_pedal_2 >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MCU_PEDAL_READINGS_DLC;
  *_ide = (uint8_t) MCU_PEDAL_READINGS_IDE;
  return MCU_PEDAL_READINGS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_LOAD_CELLS_hytech(MCU_LOAD_CELLS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->FL_load_cell = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->FR_load_cell = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MCU_LOAD_CELLS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MCU_LOAD_CELLS_hytech(&_m->mon1, MCU_LOAD_CELLS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MCU_LOAD_CELLS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MCU_LOAD_CELLS_hytech(MCU_LOAD_CELLS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_LOAD_CELLS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->FL_load_cell & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->FL_load_cell >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->FR_load_cell & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->FR_load_cell >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MCU_LOAD_CELLS_CANID;
  cframe->DLC = (uint8_t) MCU_LOAD_CELLS_DLC;
  cframe->IDE = (uint8_t) MCU_LOAD_CELLS_IDE;
  return MCU_LOAD_CELLS_CANID;
}

#else

uint32_t Pack_MCU_LOAD_CELLS_hytech(MCU_LOAD_CELLS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_LOAD_CELLS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->FL_load_cell & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->FL_load_cell >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->FR_load_cell & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->FR_load_cell >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MCU_LOAD_CELLS_DLC;
  *_ide = (uint8_t) MCU_LOAD_CELLS_IDE;
  return MCU_LOAD_CELLS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_SAB_LOAD_CELLS_hytech(SAB_LOAD_CELLS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->RL_load_cell = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->RR_load_cell = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < SAB_LOAD_CELLS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_SAB_LOAD_CELLS_hytech(&_m->mon1, SAB_LOAD_CELLS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return SAB_LOAD_CELLS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_SAB_LOAD_CELLS_hytech(SAB_LOAD_CELLS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(SAB_LOAD_CELLS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->RL_load_cell & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->RL_load_cell >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->RR_load_cell & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->RR_load_cell >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) SAB_LOAD_CELLS_CANID;
  cframe->DLC = (uint8_t) SAB_LOAD_CELLS_DLC;
  cframe->IDE = (uint8_t) SAB_LOAD_CELLS_IDE;
  return SAB_LOAD_CELLS_CANID;
}

#else

uint32_t Pack_SAB_LOAD_CELLS_hytech(SAB_LOAD_CELLS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(SAB_LOAD_CELLS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->RL_load_cell & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->RL_load_cell >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->RR_load_cell & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->RR_load_cell >> 8U) & (0xFFU)) );

  *_len = (uint8_t) SAB_LOAD_CELLS_DLC;
  *_ide = (uint8_t) SAB_LOAD_CELLS_IDE;
  return SAB_LOAD_CELLS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_FRONT_POTS_hytech(MCU_FRONT_POTS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->pot_1 = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->pot_2 = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->pot_3 = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MCU_FRONT_POTS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MCU_FRONT_POTS_hytech(&_m->mon1, MCU_FRONT_POTS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MCU_FRONT_POTS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MCU_FRONT_POTS_hytech(MCU_FRONT_POTS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_FRONT_POTS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->pot_1 & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->pot_1 >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->pot_2 & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->pot_2 >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->pot_3 & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->pot_3 >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MCU_FRONT_POTS_CANID;
  cframe->DLC = (uint8_t) MCU_FRONT_POTS_DLC;
  cframe->IDE = (uint8_t) MCU_FRONT_POTS_IDE;
  return MCU_FRONT_POTS_CANID;
}

#else

uint32_t Pack_MCU_FRONT_POTS_hytech(MCU_FRONT_POTS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_FRONT_POTS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->pot_1 & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->pot_1 >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->pot_2 & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->pot_2 >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->pot_3 & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->pot_3 >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MCU_FRONT_POTS_DLC;
  *_ide = (uint8_t) MCU_FRONT_POTS_IDE;
  return MCU_FRONT_POTS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_REAR_POTS_hytech(MCU_REAR_POTS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->pot_4 = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
  _m->pot_5 = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->pot_6 = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MCU_REAR_POTS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MCU_REAR_POTS_hytech(&_m->mon1, MCU_REAR_POTS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MCU_REAR_POTS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MCU_REAR_POTS_hytech(MCU_REAR_POTS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_REAR_POTS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->pot_4 & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->pot_4 >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->pot_5 & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->pot_5 >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->pot_6 & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->pot_6 >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MCU_REAR_POTS_CANID;
  cframe->DLC = (uint8_t) MCU_REAR_POTS_DLC;
  cframe->IDE = (uint8_t) MCU_REAR_POTS_IDE;
  return MCU_REAR_POTS_CANID;
}

#else

uint32_t Pack_MCU_REAR_POTS_hytech(MCU_REAR_POTS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_REAR_POTS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->pot_4 & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->pot_4 >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->pot_5 & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->pot_5 >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->pot_6 & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->pot_6 >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MCU_REAR_POTS_DLC;
  *_ide = (uint8_t) MCU_REAR_POTS_IDE;
  return MCU_REAR_POTS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_MCU_ANALOG_READINGS_hytech(MCU_ANALOG_READINGS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->steering_1 = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);
  _m->steering_2 = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
  _m->hall_effect_current = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
  _m->glv_battery_voltage = (uint16_t) ( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < MCU_ANALOG_READINGS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_MCU_ANALOG_READINGS_hytech(&_m->mon1, MCU_ANALOG_READINGS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return MCU_ANALOG_READINGS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_MCU_ANALOG_READINGS_hytech(MCU_ANALOG_READINGS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_ANALOG_READINGS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->steering_1 & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->steering_1 >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->steering_2 & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->steering_2 >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->hall_effect_current & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->hall_effect_current >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->glv_battery_voltage & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->glv_battery_voltage >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) MCU_ANALOG_READINGS_CANID;
  cframe->DLC = (uint8_t) MCU_ANALOG_READINGS_DLC;
  cframe->IDE = (uint8_t) MCU_ANALOG_READINGS_IDE;
  return MCU_ANALOG_READINGS_CANID;
}

#else

uint32_t Pack_MCU_ANALOG_READINGS_hytech(MCU_ANALOG_READINGS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(MCU_ANALOG_READINGS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->steering_1 & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->steering_1 >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->steering_2 & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->steering_2 >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->hall_effect_current & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->hall_effect_current >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->glv_battery_voltage & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->glv_battery_voltage >> 8U) & (0xFFU)) );

  *_len = (uint8_t) MCU_ANALOG_READINGS_DLC;
  *_ide = (uint8_t) MCU_ANALOG_READINGS_IDE;
  return MCU_ANALOG_READINGS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_ONBOARD_TEMPS_hytech(BMS_ONBOARD_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->average_temp_ro = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->average_temp_phys = (sigfloat_t)(HYTECH_average_temp_ro_fromS(_m->average_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->low_temp_ro = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->low_temp_phys = (sigfloat_t)(HYTECH_low_temp_ro_fromS(_m->low_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->high_temp_ro = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->high_temp_phys = (sigfloat_t)(HYTECH_high_temp_ro_fromS(_m->high_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_ONBOARD_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_ONBOARD_TEMPS_hytech(&_m->mon1, BMS_ONBOARD_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_ONBOARD_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_ONBOARD_TEMPS_hytech(BMS_ONBOARD_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_ONBOARD_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->average_temp_ro = (int16_t) HYTECH_average_temp_ro_toS(_m->average_temp_phys);
  _m->low_temp_ro = (int16_t) HYTECH_low_temp_ro_toS(_m->low_temp_phys);
  _m->high_temp_ro = (int16_t) HYTECH_high_temp_ro_toS(_m->high_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->average_temp_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->average_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->low_temp_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->low_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->high_temp_ro & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->high_temp_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) BMS_ONBOARD_TEMPS_CANID;
  cframe->DLC = (uint8_t) BMS_ONBOARD_TEMPS_DLC;
  cframe->IDE = (uint8_t) BMS_ONBOARD_TEMPS_IDE;
  return BMS_ONBOARD_TEMPS_CANID;
}

#else

uint32_t Pack_BMS_ONBOARD_TEMPS_hytech(BMS_ONBOARD_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_ONBOARD_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->average_temp_ro = (int16_t) HYTECH_average_temp_ro_toS(_m->average_temp_phys);
  _m->low_temp_ro = (int16_t) HYTECH_low_temp_ro_toS(_m->low_temp_phys);
  _m->high_temp_ro = (int16_t) HYTECH_high_temp_ro_toS(_m->high_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->average_temp_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->average_temp_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->low_temp_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->low_temp_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->high_temp_ro & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->high_temp_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) BMS_ONBOARD_TEMPS_DLC;
  *_ide = (uint8_t) BMS_ONBOARD_TEMPS_IDE;
  return BMS_ONBOARD_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_ONBOARD_DETAILED_TEMPS_hytech(BMS_ONBOARD_DETAILED_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->ic_id = (uint8_t) ( (_d[0] & (0xFFU)) );
  _m->temp_0_ro = (int16_t) __ext_sig__(( ((_d[2] & (0xFFU)) << 8U) | (_d[1] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->temp_0_phys = (sigfloat_t)(HYTECH_temp_0_ro_fromS(_m->temp_0_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->temp_1_ro = (int16_t) __ext_sig__(( ((_d[4] & (0xFFU)) << 8U) | (_d[3] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->temp_1_phys = (sigfloat_t)(HYTECH_temp_1_ro_fromS(_m->temp_1_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_ONBOARD_DETAILED_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_ONBOARD_DETAILED_TEMPS_hytech(&_m->mon1, BMS_ONBOARD_DETAILED_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_ONBOARD_DETAILED_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_ONBOARD_DETAILED_TEMPS_hytech(BMS_ONBOARD_DETAILED_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_ONBOARD_DETAILED_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->temp_0_ro = (int16_t) HYTECH_temp_0_ro_toS(_m->temp_0_phys);
  _m->temp_1_ro = (int16_t) HYTECH_temp_1_ro_toS(_m->temp_1_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->ic_id & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( (_m->temp_0_ro & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( ((_m->temp_0_ro >> 8U) & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( (_m->temp_1_ro & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->temp_1_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) BMS_ONBOARD_DETAILED_TEMPS_CANID;
  cframe->DLC = (uint8_t) BMS_ONBOARD_DETAILED_TEMPS_DLC;
  cframe->IDE = (uint8_t) BMS_ONBOARD_DETAILED_TEMPS_IDE;
  return BMS_ONBOARD_DETAILED_TEMPS_CANID;
}

#else

uint32_t Pack_BMS_ONBOARD_DETAILED_TEMPS_hytech(BMS_ONBOARD_DETAILED_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_ONBOARD_DETAILED_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->temp_0_ro = (int16_t) HYTECH_temp_0_ro_toS(_m->temp_0_phys);
  _m->temp_1_ro = (int16_t) HYTECH_temp_1_ro_toS(_m->temp_1_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->ic_id & (0xFFU)) );
  _d[1] |= (uint8_t) ( (_m->temp_0_ro & (0xFFU)) );
  _d[2] |= (uint8_t) ( ((_m->temp_0_ro >> 8U) & (0xFFU)) );
  _d[3] |= (uint8_t) ( (_m->temp_1_ro & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->temp_1_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) BMS_ONBOARD_DETAILED_TEMPS_DLC;
  *_ide = (uint8_t) BMS_ONBOARD_DETAILED_TEMPS_IDE;
  return BMS_ONBOARD_DETAILED_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_VOLTAGES_hytech(BMS_VOLTAGES_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->average_voltage_ro = (uint16_t) ( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->average_voltage_phys = (sigfloat_t)(HYTECH_average_voltage_ro_fromS(_m->average_voltage_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->low_voltage_ro = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->low_voltage_phys = (sigfloat_t)(HYTECH_low_voltage_ro_fromS(_m->low_voltage_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->high_voltage_ro = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->high_voltage_phys = (sigfloat_t)(HYTECH_high_voltage_ro_fromS(_m->high_voltage_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->total_voltage_ro = (uint16_t) ( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->total_voltage_phys = (sigfloat_t)(HYTECH_total_voltage_ro_fromS(_m->total_voltage_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_VOLTAGES_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_VOLTAGES_hytech(&_m->mon1, BMS_VOLTAGES_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_VOLTAGES_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_VOLTAGES_hytech(BMS_VOLTAGES_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_VOLTAGES_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->average_voltage_ro = (uint16_t) HYTECH_average_voltage_ro_toS(_m->average_voltage_phys);
  _m->low_voltage_ro = (uint16_t) HYTECH_low_voltage_ro_toS(_m->low_voltage_phys);
  _m->high_voltage_ro = (uint16_t) HYTECH_high_voltage_ro_toS(_m->high_voltage_phys);
  _m->total_voltage_ro = (uint16_t) HYTECH_total_voltage_ro_toS(_m->total_voltage_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->average_voltage_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->average_voltage_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->low_voltage_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->low_voltage_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->high_voltage_ro & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->high_voltage_ro >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->total_voltage_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->total_voltage_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) BMS_VOLTAGES_CANID;
  cframe->DLC = (uint8_t) BMS_VOLTAGES_DLC;
  cframe->IDE = (uint8_t) BMS_VOLTAGES_IDE;
  return BMS_VOLTAGES_CANID;
}

#else

uint32_t Pack_BMS_VOLTAGES_hytech(BMS_VOLTAGES_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_VOLTAGES_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->average_voltage_ro = (uint16_t) HYTECH_average_voltage_ro_toS(_m->average_voltage_phys);
  _m->low_voltage_ro = (uint16_t) HYTECH_low_voltage_ro_toS(_m->low_voltage_phys);
  _m->high_voltage_ro = (uint16_t) HYTECH_high_voltage_ro_toS(_m->high_voltage_phys);
  _m->total_voltage_ro = (uint16_t) HYTECH_total_voltage_ro_toS(_m->total_voltage_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->average_voltage_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->average_voltage_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->low_voltage_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->low_voltage_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->high_voltage_ro & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->high_voltage_ro >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->total_voltage_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->total_voltage_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) BMS_VOLTAGES_DLC;
  *_ide = (uint8_t) BMS_VOLTAGES_IDE;
  return BMS_VOLTAGES_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_DETAILED_VOLTAGES_hytech(BMS_DETAILED_VOLTAGES_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->group_id = (uint8_t) ( (_d[0] & (0x0FU)) );
  _m->ic_id = (uint8_t) ( (_d[1] & (0xFFU)) );
  _m->voltage_0_ro = (uint16_t) ( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->voltage_0_phys = (sigfloat_t)(HYTECH_voltage_0_ro_fromS(_m->voltage_0_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->voltage_1_ro = (uint16_t) ( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->voltage_1_phys = (sigfloat_t)(HYTECH_voltage_1_ro_fromS(_m->voltage_1_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->voltage_2_ro = (uint16_t) ( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->voltage_2_phys = (sigfloat_t)(HYTECH_voltage_2_ro_fromS(_m->voltage_2_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_DETAILED_VOLTAGES_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_DETAILED_VOLTAGES_hytech(&_m->mon1, BMS_DETAILED_VOLTAGES_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_DETAILED_VOLTAGES_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_DETAILED_VOLTAGES_hytech(BMS_DETAILED_VOLTAGES_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_DETAILED_VOLTAGES_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->voltage_0_ro = (uint16_t) HYTECH_voltage_0_ro_toS(_m->voltage_0_phys);
  _m->voltage_1_ro = (uint16_t) HYTECH_voltage_1_ro_toS(_m->voltage_1_phys);
  _m->voltage_2_ro = (uint16_t) HYTECH_voltage_2_ro_toS(_m->voltage_2_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->group_id & (0x0FU)) );
  cframe->Data[1] |= (uint8_t) ( (_m->ic_id & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->voltage_0_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->voltage_0_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->voltage_1_ro & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->voltage_1_ro >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->voltage_2_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->voltage_2_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) BMS_DETAILED_VOLTAGES_CANID;
  cframe->DLC = (uint8_t) BMS_DETAILED_VOLTAGES_DLC;
  cframe->IDE = (uint8_t) BMS_DETAILED_VOLTAGES_IDE;
  return BMS_DETAILED_VOLTAGES_CANID;
}

#else

uint32_t Pack_BMS_DETAILED_VOLTAGES_hytech(BMS_DETAILED_VOLTAGES_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_DETAILED_VOLTAGES_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->voltage_0_ro = (uint16_t) HYTECH_voltage_0_ro_toS(_m->voltage_0_phys);
  _m->voltage_1_ro = (uint16_t) HYTECH_voltage_1_ro_toS(_m->voltage_1_phys);
  _m->voltage_2_ro = (uint16_t) HYTECH_voltage_2_ro_toS(_m->voltage_2_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->group_id & (0x0FU)) );
  _d[1] |= (uint8_t) ( (_m->ic_id & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->voltage_0_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->voltage_0_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->voltage_1_ro & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->voltage_1_ro >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->voltage_2_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->voltage_2_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) BMS_DETAILED_VOLTAGES_DLC;
  *_ide = (uint8_t) BMS_DETAILED_VOLTAGES_IDE;
  return BMS_DETAILED_VOLTAGES_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_TEMPS_hytech(BMS_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->average_temp_ro = (int16_t) __ext_sig__(( ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->average_temp_phys = (sigfloat_t)(HYTECH_average_temp_ro_fromS(_m->average_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->low_temp_ro = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->low_temp_phys = (sigfloat_t)(HYTECH_low_temp_ro_fromS(_m->low_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->high_temp_ro = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->high_temp_phys = (sigfloat_t)(HYTECH_high_temp_ro_fromS(_m->high_temp_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_TEMPS_hytech(&_m->mon1, BMS_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_TEMPS_hytech(BMS_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->average_temp_ro = (int16_t) HYTECH_average_temp_ro_toS(_m->average_temp_phys);
  _m->low_temp_ro = (int16_t) HYTECH_low_temp_ro_toS(_m->low_temp_phys);
  _m->high_temp_ro = (int16_t) HYTECH_high_temp_ro_toS(_m->high_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->average_temp_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->average_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->low_temp_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->low_temp_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->high_temp_ro & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->high_temp_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) BMS_TEMPS_CANID;
  cframe->DLC = (uint8_t) BMS_TEMPS_DLC;
  cframe->IDE = (uint8_t) BMS_TEMPS_IDE;
  return BMS_TEMPS_CANID;
}

#else

uint32_t Pack_BMS_TEMPS_hytech(BMS_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->average_temp_ro = (int16_t) HYTECH_average_temp_ro_toS(_m->average_temp_phys);
  _m->low_temp_ro = (int16_t) HYTECH_low_temp_ro_toS(_m->low_temp_phys);
  _m->high_temp_ro = (int16_t) HYTECH_high_temp_ro_toS(_m->high_temp_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->average_temp_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->average_temp_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->low_temp_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->low_temp_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->high_temp_ro & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->high_temp_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) BMS_TEMPS_DLC;
  *_ide = (uint8_t) BMS_TEMPS_IDE;
  return BMS_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_DETAILED_TEMPS_hytech(BMS_DETAILED_TEMPS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->group_id = (uint8_t) ( (_d[0] & (0x0FU)) );
  _m->ic_id = (uint8_t) ( (_d[1] & (0xFFU)) );
  _m->thermistor_id_0_ro = (int16_t) __ext_sig__(( ((_d[3] & (0xFFU)) << 8U) | (_d[2] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->thermistor_id_0_phys = (sigfloat_t)(HYTECH_thermistor_id_0_ro_fromS(_m->thermistor_id_0_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->thermistor_id_1_ro = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->thermistor_id_1_phys = (sigfloat_t)(HYTECH_thermistor_id_1_ro_fromS(_m->thermistor_id_1_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->thermistor_id_2_ro = (int16_t) __ext_sig__(( ((_d[7] & (0xFFU)) << 8U) | (_d[6] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->thermistor_id_2_phys = (sigfloat_t)(HYTECH_thermistor_id_2_ro_fromS(_m->thermistor_id_2_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_DETAILED_TEMPS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_DETAILED_TEMPS_hytech(&_m->mon1, BMS_DETAILED_TEMPS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_DETAILED_TEMPS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_DETAILED_TEMPS_hytech(BMS_DETAILED_TEMPS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_DETAILED_TEMPS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->thermistor_id_0_ro = (int16_t) HYTECH_thermistor_id_0_ro_toS(_m->thermistor_id_0_phys);
  _m->thermistor_id_1_ro = (int16_t) HYTECH_thermistor_id_1_ro_toS(_m->thermistor_id_1_phys);
  _m->thermistor_id_2_ro = (int16_t) HYTECH_thermistor_id_2_ro_toS(_m->thermistor_id_2_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->group_id & (0x0FU)) );
  cframe->Data[1] |= (uint8_t) ( (_m->ic_id & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->thermistor_id_0_ro & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->thermistor_id_0_ro >> 8U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->thermistor_id_1_ro & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->thermistor_id_1_ro >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->thermistor_id_2_ro & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->thermistor_id_2_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) BMS_DETAILED_TEMPS_CANID;
  cframe->DLC = (uint8_t) BMS_DETAILED_TEMPS_DLC;
  cframe->IDE = (uint8_t) BMS_DETAILED_TEMPS_IDE;
  return BMS_DETAILED_TEMPS_CANID;
}

#else

uint32_t Pack_BMS_DETAILED_TEMPS_hytech(BMS_DETAILED_TEMPS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_DETAILED_TEMPS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->thermistor_id_0_ro = (int16_t) HYTECH_thermistor_id_0_ro_toS(_m->thermistor_id_0_phys);
  _m->thermistor_id_1_ro = (int16_t) HYTECH_thermistor_id_1_ro_toS(_m->thermistor_id_1_phys);
  _m->thermistor_id_2_ro = (int16_t) HYTECH_thermistor_id_2_ro_toS(_m->thermistor_id_2_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->group_id & (0x0FU)) );
  _d[1] |= (uint8_t) ( (_m->ic_id & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->thermistor_id_0_ro & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->thermistor_id_0_ro >> 8U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->thermistor_id_1_ro & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->thermistor_id_1_ro >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->thermistor_id_2_ro & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->thermistor_id_2_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) BMS_DETAILED_TEMPS_DLC;
  *_ide = (uint8_t) BMS_DETAILED_TEMPS_IDE;
  return BMS_DETAILED_TEMPS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_STATUS_hytech(BMS_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->state = (uint8_t) ( (_d[0] & (0xFFU)) );
  _m->overvoltage_error = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->undervoltage_error = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->total_voltage_high_error = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->discharge_overcurrent_error = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->charge_overcurrent_error = (uint8_t) ( ((_d[1] >> 4U) & (0x01U)) );
  _m->discharge_overtemp_error = (uint8_t) ( ((_d[1] >> 5U) & (0x01U)) );
  _m->charge_overtemp_error = (uint8_t) ( ((_d[1] >> 6U) & (0x01U)) );
  _m->undertemp_error = (uint8_t) ( ((_d[1] >> 7U) & (0x01U)) );
  _m->overtemp_error = (uint8_t) ( (_d[2] & (0x01U)) );
  _m->current_ro = (int16_t) __ext_sig__(( ((_d[4] & (0xFFU)) << 8U) | (_d[3] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->current_phys = (sigfloat_t)(HYTECH_current_ro_fromS(_m->current_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->shutdown_g_above_threshold_error = (uint8_t) ( (_d[5] & (0x01U)) );
  _m->shutdown_h_above_threshold_error = (uint8_t) ( ((_d[5] >> 1U) & (0x01U)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_STATUS_hytech(&_m->mon1, BMS_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_STATUS_hytech(BMS_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->current_ro = (int16_t) HYTECH_current_ro_toS(_m->current_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->state & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( (_m->overvoltage_error & (0x01U)) | ((_m->undervoltage_error & (0x01U)) << 1U) | ((_m->total_voltage_high_error & (0x01U)) << 2U) | ((_m->discharge_overcurrent_error & (0x01U)) << 3U) | ((_m->charge_overcurrent_error & (0x01U)) << 4U) | ((_m->discharge_overtemp_error & (0x01U)) << 5U) | ((_m->charge_overtemp_error & (0x01U)) << 6U) | ((_m->undertemp_error & (0x01U)) << 7U) );
  cframe->Data[2] |= (uint8_t) ( (_m->overtemp_error & (0x01U)) );
  cframe->Data[3] |= (uint8_t) ( (_m->current_ro & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( ((_m->current_ro >> 8U) & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( (_m->shutdown_g_above_threshold_error & (0x01U)) | ((_m->shutdown_h_above_threshold_error & (0x01U)) << 1U) );

  cframe->MsgId = (uint32_t) BMS_STATUS_CANID;
  cframe->DLC = (uint8_t) BMS_STATUS_DLC;
  cframe->IDE = (uint8_t) BMS_STATUS_IDE;
  return BMS_STATUS_CANID;
}

#else

uint32_t Pack_BMS_STATUS_hytech(BMS_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->current_ro = (int16_t) HYTECH_current_ro_toS(_m->current_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->state & (0xFFU)) );
  _d[1] |= (uint8_t) ( (_m->overvoltage_error & (0x01U)) | ((_m->undervoltage_error & (0x01U)) << 1U) | ((_m->total_voltage_high_error & (0x01U)) << 2U) | ((_m->discharge_overcurrent_error & (0x01U)) << 3U) | ((_m->charge_overcurrent_error & (0x01U)) << 4U) | ((_m->discharge_overtemp_error & (0x01U)) << 5U) | ((_m->charge_overtemp_error & (0x01U)) << 6U) | ((_m->undertemp_error & (0x01U)) << 7U) );
  _d[2] |= (uint8_t) ( (_m->overtemp_error & (0x01U)) );
  _d[3] |= (uint8_t) ( (_m->current_ro & (0xFFU)) );
  _d[4] |= (uint8_t) ( ((_m->current_ro >> 8U) & (0xFFU)) );
  _d[5] |= (uint8_t) ( (_m->shutdown_g_above_threshold_error & (0x01U)) | ((_m->shutdown_h_above_threshold_error & (0x01U)) << 1U) );

  *_len = (uint8_t) BMS_STATUS_DLC;
  *_ide = (uint8_t) BMS_STATUS_IDE;
  return BMS_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_CCU_STATUS_hytech(CCU_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->charger_enabled = (uint8_t) ( (_d[0] & (0x01U)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < CCU_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_CCU_STATUS_hytech(&_m->mon1, CCU_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return CCU_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_CCU_STATUS_hytech(CCU_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(CCU_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->charger_enabled & (0x01U)) );

  cframe->MsgId = (uint32_t) CCU_STATUS_CANID;
  cframe->DLC = (uint8_t) CCU_STATUS_DLC;
  cframe->IDE = (uint8_t) CCU_STATUS_IDE;
  return CCU_STATUS_CANID;
}

#else

uint32_t Pack_CCU_STATUS_hytech(CCU_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(CCU_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->charger_enabled & (0x01U)) );

  *_len = (uint8_t) CCU_STATUS_DLC;
  *_ide = (uint8_t) CCU_STATUS_IDE;
  return CCU_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_BALANCING_STATUS_hytech(BMS_BALANCING_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->group_id = (uint8_t) ( (_d[0] & (0x0FU)) );
  _m->cell_1_balancing_status = (uint8_t) ( ((_d[0] >> 4U) & (0x01U)) );
  _m->cell_2_balancing_status = (uint8_t) ( ((_d[0] >> 5U) & (0x01U)) );
  _m->cell_3_balancing_status = (uint8_t) ( ((_d[0] >> 6U) & (0x01U)) );
  _m->cell_4_balancing_status = (uint8_t) ( ((_d[0] >> 7U) & (0x01U)) );
  _m->cell_5_balancing_status = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->cell_6_balancing_status = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->cell_7_balancing_status = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->cell_8_balancing_status = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->cell_9_balancing_status = (uint8_t) ( ((_d[1] >> 4U) & (0x01U)) );
  _m->cell_10_balancing_status = (uint8_t) ( ((_d[1] >> 5U) & (0x01U)) );
  _m->cell_11_balancing_status = (uint8_t) ( ((_d[1] >> 6U) & (0x01U)) );
  _m->cell_12_balancing_status = (uint8_t) ( ((_d[1] >> 7U) & (0x01U)) );
  _m->cell_13_balancing_status = (uint8_t) ( (_d[2] & (0x01U)) );
  _m->cell_14_balancing_status = (uint8_t) ( ((_d[2] >> 1U) & (0x01U)) );
  _m->cell_15_balancing_status = (uint8_t) ( ((_d[2] >> 2U) & (0x01U)) );
  _m->cell_16_balancing_status = (uint8_t) ( ((_d[2] >> 3U) & (0x01U)) );
  _m->cell_17_balancing_status = (uint8_t) ( ((_d[2] >> 4U) & (0x01U)) );
  _m->cell_18_balancing_status = (uint8_t) ( ((_d[2] >> 5U) & (0x01U)) );
  _m->cell_19_balancing_status = (uint8_t) ( ((_d[2] >> 6U) & (0x01U)) );
  _m->cell_20_balancing_status = (uint8_t) ( ((_d[2] >> 7U) & (0x01U)) );
  _m->cell_21_balancing_status = (uint8_t) ( (_d[3] & (0x01U)) );
  _m->cell_22_balancing_status = (uint8_t) ( ((_d[3] >> 1U) & (0x01U)) );
  _m->cell_23_balancing_status = (uint8_t) ( ((_d[3] >> 2U) & (0x01U)) );
  _m->cell_24_balancing_status = (uint8_t) ( ((_d[3] >> 3U) & (0x01U)) );
  _m->cell_25_balancing_status = (uint8_t) ( ((_d[3] >> 4U) & (0x01U)) );
  _m->cell_26_balancing_status = (uint8_t) ( ((_d[3] >> 5U) & (0x01U)) );
  _m->cell_27_balancing_status = (uint8_t) ( ((_d[3] >> 6U) & (0x01U)) );
  _m->cell_28_balancing_status = (uint8_t) ( ((_d[3] >> 7U) & (0x01U)) );
  _m->cell_29_balancing_status = (uint8_t) ( (_d[4] & (0x01U)) );
  _m->cell_30_balancing_status = (uint8_t) ( ((_d[4] >> 1U) & (0x01U)) );
  _m->cell_31_balancing_status = (uint8_t) ( ((_d[4] >> 2U) & (0x01U)) );
  _m->cell_32_balancing_status = (uint8_t) ( ((_d[4] >> 3U) & (0x01U)) );
  _m->cell_33_balancing_status = (uint8_t) ( ((_d[4] >> 4U) & (0x01U)) );
  _m->cell_34_balancing_status = (uint8_t) ( ((_d[4] >> 5U) & (0x01U)) );
  _m->cell_35_balancing_status = (uint8_t) ( ((_d[4] >> 6U) & (0x01U)) );
  _m->cell_36_balancing_status = (uint8_t) ( ((_d[4] >> 7U) & (0x01U)) );
  _m->cell_37_balancing_status = (uint8_t) ( (_d[5] & (0x01U)) );
  _m->cell_38_balancing_status = (uint8_t) ( ((_d[5] >> 1U) & (0x01U)) );
  _m->cell_39_balancing_status = (uint8_t) ( ((_d[5] >> 2U) & (0x01U)) );
  _m->cell_40_balancing_status = (uint8_t) ( ((_d[5] >> 3U) & (0x01U)) );
  _m->cell_41_balancing_status = (uint8_t) ( ((_d[5] >> 4U) & (0x01U)) );
  _m->cell_42_balancing_status = (uint8_t) ( ((_d[5] >> 5U) & (0x01U)) );
  _m->cell_43_balancing_status = (uint8_t) ( ((_d[5] >> 6U) & (0x01U)) );
  _m->cell_44_balancing_status = (uint8_t) ( ((_d[5] >> 7U) & (0x01U)) );
  _m->cell_45_balancing_status = (uint8_t) ( (_d[6] & (0x01U)) );
  _m->cell_46_balancing_status = (uint8_t) ( ((_d[6] >> 1U) & (0x01U)) );
  _m->cell_47_balancing_status = (uint8_t) ( ((_d[6] >> 2U) & (0x01U)) );
  _m->cell_48_balancing_status = (uint8_t) ( ((_d[6] >> 3U) & (0x01U)) );
  _m->cell_49_balancing_status = (uint8_t) ( ((_d[6] >> 4U) & (0x01U)) );
  _m->cell_50_balancing_status = (uint8_t) ( ((_d[6] >> 5U) & (0x01U)) );
  _m->cell_51_balancing_status = (uint8_t) ( ((_d[6] >> 6U) & (0x01U)) );
  _m->cell_52_balancing_status = (uint8_t) ( ((_d[6] >> 7U) & (0x01U)) );
  _m->cell_53_balancing_status = (uint8_t) ( (_d[7] & (0x01U)) );
  _m->cell_54_balancing_status = (uint8_t) ( ((_d[7] >> 1U) & (0x01U)) );
  _m->cell_55_balancing_status = (uint8_t) ( ((_d[7] >> 2U) & (0x01U)) );
  _m->cell_56_balancing_status = (uint8_t) ( ((_d[7] >> 3U) & (0x01U)) );
  _m->cell_57_balancing_status = (uint8_t) ( ((_d[7] >> 4U) & (0x01U)) );
  _m->cell_58_balancing_status = (uint8_t) ( ((_d[7] >> 5U) & (0x01U)) );
  _m->cell_59_balancing_status = (uint8_t) ( ((_d[7] >> 6U) & (0x01U)) );
  _m->cell_60_balancing_status = (uint8_t) ( ((_d[7] >> 7U) & (0x01U)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_BALANCING_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_BALANCING_STATUS_hytech(&_m->mon1, BMS_BALANCING_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_BALANCING_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_BALANCING_STATUS_hytech(BMS_BALANCING_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_BALANCING_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->group_id & (0x0FU)) | ((_m->cell_1_balancing_status & (0x01U)) << 4U) | ((_m->cell_2_balancing_status & (0x01U)) << 5U) | ((_m->cell_3_balancing_status & (0x01U)) << 6U) | ((_m->cell_4_balancing_status & (0x01U)) << 7U) );
  cframe->Data[1] |= (uint8_t) ( (_m->cell_5_balancing_status & (0x01U)) | ((_m->cell_6_balancing_status & (0x01U)) << 1U) | ((_m->cell_7_balancing_status & (0x01U)) << 2U) | ((_m->cell_8_balancing_status & (0x01U)) << 3U) | ((_m->cell_9_balancing_status & (0x01U)) << 4U) | ((_m->cell_10_balancing_status & (0x01U)) << 5U) | ((_m->cell_11_balancing_status & (0x01U)) << 6U) | ((_m->cell_12_balancing_status & (0x01U)) << 7U) );
  cframe->Data[2] |= (uint8_t) ( (_m->cell_13_balancing_status & (0x01U)) | ((_m->cell_14_balancing_status & (0x01U)) << 1U) | ((_m->cell_15_balancing_status & (0x01U)) << 2U) | ((_m->cell_16_balancing_status & (0x01U)) << 3U) | ((_m->cell_17_balancing_status & (0x01U)) << 4U) | ((_m->cell_18_balancing_status & (0x01U)) << 5U) | ((_m->cell_19_balancing_status & (0x01U)) << 6U) | ((_m->cell_20_balancing_status & (0x01U)) << 7U) );
  cframe->Data[3] |= (uint8_t) ( (_m->cell_21_balancing_status & (0x01U)) | ((_m->cell_22_balancing_status & (0x01U)) << 1U) | ((_m->cell_23_balancing_status & (0x01U)) << 2U) | ((_m->cell_24_balancing_status & (0x01U)) << 3U) | ((_m->cell_25_balancing_status & (0x01U)) << 4U) | ((_m->cell_26_balancing_status & (0x01U)) << 5U) | ((_m->cell_27_balancing_status & (0x01U)) << 6U) | ((_m->cell_28_balancing_status & (0x01U)) << 7U) );
  cframe->Data[4] |= (uint8_t) ( (_m->cell_29_balancing_status & (0x01U)) | ((_m->cell_30_balancing_status & (0x01U)) << 1U) | ((_m->cell_31_balancing_status & (0x01U)) << 2U) | ((_m->cell_32_balancing_status & (0x01U)) << 3U) | ((_m->cell_33_balancing_status & (0x01U)) << 4U) | ((_m->cell_34_balancing_status & (0x01U)) << 5U) | ((_m->cell_35_balancing_status & (0x01U)) << 6U) | ((_m->cell_36_balancing_status & (0x01U)) << 7U) );
  cframe->Data[5] |= (uint8_t) ( (_m->cell_37_balancing_status & (0x01U)) | ((_m->cell_38_balancing_status & (0x01U)) << 1U) | ((_m->cell_39_balancing_status & (0x01U)) << 2U) | ((_m->cell_40_balancing_status & (0x01U)) << 3U) | ((_m->cell_41_balancing_status & (0x01U)) << 4U) | ((_m->cell_42_balancing_status & (0x01U)) << 5U) | ((_m->cell_43_balancing_status & (0x01U)) << 6U) | ((_m->cell_44_balancing_status & (0x01U)) << 7U) );
  cframe->Data[6] |= (uint8_t) ( (_m->cell_45_balancing_status & (0x01U)) | ((_m->cell_46_balancing_status & (0x01U)) << 1U) | ((_m->cell_47_balancing_status & (0x01U)) << 2U) | ((_m->cell_48_balancing_status & (0x01U)) << 3U) | ((_m->cell_49_balancing_status & (0x01U)) << 4U) | ((_m->cell_50_balancing_status & (0x01U)) << 5U) | ((_m->cell_51_balancing_status & (0x01U)) << 6U) | ((_m->cell_52_balancing_status & (0x01U)) << 7U) );
  cframe->Data[7] |= (uint8_t) ( (_m->cell_53_balancing_status & (0x01U)) | ((_m->cell_54_balancing_status & (0x01U)) << 1U) | ((_m->cell_55_balancing_status & (0x01U)) << 2U) | ((_m->cell_56_balancing_status & (0x01U)) << 3U) | ((_m->cell_57_balancing_status & (0x01U)) << 4U) | ((_m->cell_58_balancing_status & (0x01U)) << 5U) | ((_m->cell_59_balancing_status & (0x01U)) << 6U) | ((_m->cell_60_balancing_status & (0x01U)) << 7U) );

  cframe->MsgId = (uint32_t) BMS_BALANCING_STATUS_CANID;
  cframe->DLC = (uint8_t) BMS_BALANCING_STATUS_DLC;
  cframe->IDE = (uint8_t) BMS_BALANCING_STATUS_IDE;
  return BMS_BALANCING_STATUS_CANID;
}

#else

uint32_t Pack_BMS_BALANCING_STATUS_hytech(BMS_BALANCING_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_BALANCING_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->group_id & (0x0FU)) | ((_m->cell_1_balancing_status & (0x01U)) << 4U) | ((_m->cell_2_balancing_status & (0x01U)) << 5U) | ((_m->cell_3_balancing_status & (0x01U)) << 6U) | ((_m->cell_4_balancing_status & (0x01U)) << 7U) );
  _d[1] |= (uint8_t) ( (_m->cell_5_balancing_status & (0x01U)) | ((_m->cell_6_balancing_status & (0x01U)) << 1U) | ((_m->cell_7_balancing_status & (0x01U)) << 2U) | ((_m->cell_8_balancing_status & (0x01U)) << 3U) | ((_m->cell_9_balancing_status & (0x01U)) << 4U) | ((_m->cell_10_balancing_status & (0x01U)) << 5U) | ((_m->cell_11_balancing_status & (0x01U)) << 6U) | ((_m->cell_12_balancing_status & (0x01U)) << 7U) );
  _d[2] |= (uint8_t) ( (_m->cell_13_balancing_status & (0x01U)) | ((_m->cell_14_balancing_status & (0x01U)) << 1U) | ((_m->cell_15_balancing_status & (0x01U)) << 2U) | ((_m->cell_16_balancing_status & (0x01U)) << 3U) | ((_m->cell_17_balancing_status & (0x01U)) << 4U) | ((_m->cell_18_balancing_status & (0x01U)) << 5U) | ((_m->cell_19_balancing_status & (0x01U)) << 6U) | ((_m->cell_20_balancing_status & (0x01U)) << 7U) );
  _d[3] |= (uint8_t) ( (_m->cell_21_balancing_status & (0x01U)) | ((_m->cell_22_balancing_status & (0x01U)) << 1U) | ((_m->cell_23_balancing_status & (0x01U)) << 2U) | ((_m->cell_24_balancing_status & (0x01U)) << 3U) | ((_m->cell_25_balancing_status & (0x01U)) << 4U) | ((_m->cell_26_balancing_status & (0x01U)) << 5U) | ((_m->cell_27_balancing_status & (0x01U)) << 6U) | ((_m->cell_28_balancing_status & (0x01U)) << 7U) );
  _d[4] |= (uint8_t) ( (_m->cell_29_balancing_status & (0x01U)) | ((_m->cell_30_balancing_status & (0x01U)) << 1U) | ((_m->cell_31_balancing_status & (0x01U)) << 2U) | ((_m->cell_32_balancing_status & (0x01U)) << 3U) | ((_m->cell_33_balancing_status & (0x01U)) << 4U) | ((_m->cell_34_balancing_status & (0x01U)) << 5U) | ((_m->cell_35_balancing_status & (0x01U)) << 6U) | ((_m->cell_36_balancing_status & (0x01U)) << 7U) );
  _d[5] |= (uint8_t) ( (_m->cell_37_balancing_status & (0x01U)) | ((_m->cell_38_balancing_status & (0x01U)) << 1U) | ((_m->cell_39_balancing_status & (0x01U)) << 2U) | ((_m->cell_40_balancing_status & (0x01U)) << 3U) | ((_m->cell_41_balancing_status & (0x01U)) << 4U) | ((_m->cell_42_balancing_status & (0x01U)) << 5U) | ((_m->cell_43_balancing_status & (0x01U)) << 6U) | ((_m->cell_44_balancing_status & (0x01U)) << 7U) );
  _d[6] |= (uint8_t) ( (_m->cell_45_balancing_status & (0x01U)) | ((_m->cell_46_balancing_status & (0x01U)) << 1U) | ((_m->cell_47_balancing_status & (0x01U)) << 2U) | ((_m->cell_48_balancing_status & (0x01U)) << 3U) | ((_m->cell_49_balancing_status & (0x01U)) << 4U) | ((_m->cell_50_balancing_status & (0x01U)) << 5U) | ((_m->cell_51_balancing_status & (0x01U)) << 6U) | ((_m->cell_52_balancing_status & (0x01U)) << 7U) );
  _d[7] |= (uint8_t) ( (_m->cell_53_balancing_status & (0x01U)) | ((_m->cell_54_balancing_status & (0x01U)) << 1U) | ((_m->cell_55_balancing_status & (0x01U)) << 2U) | ((_m->cell_56_balancing_status & (0x01U)) << 3U) | ((_m->cell_57_balancing_status & (0x01U)) << 4U) | ((_m->cell_58_balancing_status & (0x01U)) << 5U) | ((_m->cell_59_balancing_status & (0x01U)) << 6U) | ((_m->cell_60_balancing_status & (0x01U)) << 7U) );

  *_len = (uint8_t) BMS_BALANCING_STATUS_DLC;
  *_ide = (uint8_t) BMS_BALANCING_STATUS_IDE;
  return BMS_BALANCING_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_BMS_COULOMB_COUNTS_hytech(BMS_COULOMB_COUNTS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->total_charge_ro = (uint32_t) ( ((_d[3] & (0xFFU)) << 24U) | ((_d[2] & (0xFFU)) << 16U) | ((_d[1] & (0xFFU)) << 8U) | (_d[0] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->total_charge_phys = (sigfloat_t)(HYTECH_total_charge_ro_fromS(_m->total_charge_ro));
#endif // HYTECH_USE_SIGFLOAT

  _m->total_discharge_ro = (uint32_t) ( ((_d[7] & (0xFFU)) << 24U) | ((_d[6] & (0xFFU)) << 16U) | ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) );
#ifdef HYTECH_USE_SIGFLOAT
  _m->total_discharge_phys = (sigfloat_t)(HYTECH_total_discharge_ro_fromS(_m->total_discharge_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < BMS_COULOMB_COUNTS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_BMS_COULOMB_COUNTS_hytech(&_m->mon1, BMS_COULOMB_COUNTS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return BMS_COULOMB_COUNTS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_BMS_COULOMB_COUNTS_hytech(BMS_COULOMB_COUNTS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_COULOMB_COUNTS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->total_charge_ro = (uint32_t) HYTECH_total_charge_ro_toS(_m->total_charge_phys);
  _m->total_discharge_ro = (uint32_t) HYTECH_total_discharge_ro_toS(_m->total_discharge_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[0] |= (uint8_t) ( (_m->total_charge_ro & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( ((_m->total_charge_ro >> 8U) & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( ((_m->total_charge_ro >> 16U) & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( ((_m->total_charge_ro >> 24U) & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->total_discharge_ro & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->total_discharge_ro >> 8U) & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( ((_m->total_discharge_ro >> 16U) & (0xFFU)) );
  cframe->Data[7] |= (uint8_t) ( ((_m->total_discharge_ro >> 24U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) BMS_COULOMB_COUNTS_CANID;
  cframe->DLC = (uint8_t) BMS_COULOMB_COUNTS_DLC;
  cframe->IDE = (uint8_t) BMS_COULOMB_COUNTS_IDE;
  return BMS_COULOMB_COUNTS_CANID;
}

#else

uint32_t Pack_BMS_COULOMB_COUNTS_hytech(BMS_COULOMB_COUNTS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(BMS_COULOMB_COUNTS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->total_charge_ro = (uint32_t) HYTECH_total_charge_ro_toS(_m->total_charge_phys);
  _m->total_discharge_ro = (uint32_t) HYTECH_total_discharge_ro_toS(_m->total_discharge_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[0] |= (uint8_t) ( (_m->total_charge_ro & (0xFFU)) );
  _d[1] |= (uint8_t) ( ((_m->total_charge_ro >> 8U) & (0xFFU)) );
  _d[2] |= (uint8_t) ( ((_m->total_charge_ro >> 16U) & (0xFFU)) );
  _d[3] |= (uint8_t) ( ((_m->total_charge_ro >> 24U) & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->total_discharge_ro & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->total_discharge_ro >> 8U) & (0xFFU)) );
  _d[6] |= (uint8_t) ( ((_m->total_discharge_ro >> 16U) & (0xFFU)) );
  _d[7] |= (uint8_t) ( ((_m->total_discharge_ro >> 24U) & (0xFFU)) );

  *_len = (uint8_t) BMS_COULOMB_COUNTS_DLC;
  *_ide = (uint8_t) BMS_COULOMB_COUNTS_IDE;
  return BMS_COULOMB_COUNTS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_DASHBOARD_MCU_STATE_hytech(DASHBOARD_MCU_STATE_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->bots_led = (uint8_t) ( (_d[0] & (0x03U)) );
  _m->launch_control_led = (uint8_t) ( ((_d[0] >> 2U) & (0x03U)) );
  _m->mode_led = (uint8_t) ( ((_d[0] >> 4U) & (0x03U)) );
  _m->mechanical_brake_led = (uint8_t) ( ((_d[0] >> 6U) & (0x03U)) );
  _m->cockpit_brb_led = (uint8_t) ( (_d[1] & (0x03U)) );
  _m->inertia_status_led = (uint8_t) ( ((_d[1] >> 2U) & (0x03U)) );
  _m->start_status_led = (uint8_t) ( ((_d[1] >> 4U) & (0x03U)) );
  _m->motor_controller_error_led = (uint8_t) ( ((_d[1] >> 6U) & (0x03U)) );
  _m->imd_led = (uint8_t) ( (_d[2] & (0x03U)) );
  _m->ams_led = (uint8_t) ( ((_d[2] >> 2U) & (0x03U)) );
  _m->drive_buzzer = (uint8_t) ( ((_d[2] >> 4U) & (0x01U)) );
  _m->glv_led = (uint8_t) ( (_d[3] & (0xFFU)) );
  _m->pack_charge_led = (uint8_t) ( (_d[4] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < DASHBOARD_MCU_STATE_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_DASHBOARD_MCU_STATE_hytech(&_m->mon1, DASHBOARD_MCU_STATE_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return DASHBOARD_MCU_STATE_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_DASHBOARD_MCU_STATE_hytech(DASHBOARD_MCU_STATE_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(DASHBOARD_MCU_STATE_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->bots_led & (0x03U)) | ((_m->launch_control_led & (0x03U)) << 2U) | ((_m->mode_led & (0x03U)) << 4U) | ((_m->mechanical_brake_led & (0x03U)) << 6U) );
  cframe->Data[1] |= (uint8_t) ( (_m->cockpit_brb_led & (0x03U)) | ((_m->inertia_status_led & (0x03U)) << 2U) | ((_m->start_status_led & (0x03U)) << 4U) | ((_m->motor_controller_error_led & (0x03U)) << 6U) );
  cframe->Data[2] |= (uint8_t) ( (_m->imd_led & (0x03U)) | ((_m->ams_led & (0x03U)) << 2U) | ((_m->drive_buzzer & (0x01U)) << 4U) );
  cframe->Data[3] |= (uint8_t) ( (_m->glv_led & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->pack_charge_led & (0xFFU)) );

  cframe->MsgId = (uint32_t) DASHBOARD_MCU_STATE_CANID;
  cframe->DLC = (uint8_t) DASHBOARD_MCU_STATE_DLC;
  cframe->IDE = (uint8_t) DASHBOARD_MCU_STATE_IDE;
  return DASHBOARD_MCU_STATE_CANID;
}

#else

uint32_t Pack_DASHBOARD_MCU_STATE_hytech(DASHBOARD_MCU_STATE_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(DASHBOARD_MCU_STATE_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->bots_led & (0x03U)) | ((_m->launch_control_led & (0x03U)) << 2U) | ((_m->mode_led & (0x03U)) << 4U) | ((_m->mechanical_brake_led & (0x03U)) << 6U) );
  _d[1] |= (uint8_t) ( (_m->cockpit_brb_led & (0x03U)) | ((_m->inertia_status_led & (0x03U)) << 2U) | ((_m->start_status_led & (0x03U)) << 4U) | ((_m->motor_controller_error_led & (0x03U)) << 6U) );
  _d[2] |= (uint8_t) ( (_m->imd_led & (0x03U)) | ((_m->ams_led & (0x03U)) << 2U) | ((_m->drive_buzzer & (0x01U)) << 4U) );
  _d[3] |= (uint8_t) ( (_m->glv_led & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->pack_charge_led & (0xFFU)) );

  *_len = (uint8_t) DASHBOARD_MCU_STATE_DLC;
  *_ide = (uint8_t) DASHBOARD_MCU_STATE_IDE;
  return DASHBOARD_MCU_STATE_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_DASHBOARD_STATE_hytech(DASHBOARD_STATE_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->start_button = (uint8_t) ( (_d[0] & (0x01U)) );
  _m->mark_button = (uint8_t) ( ((_d[0] >> 1U) & (0x01U)) );
  _m->mode_button = (uint8_t) ( ((_d[0] >> 2U) & (0x01U)) );
  _m->motor_controller_cycle_button = (uint8_t) ( ((_d[0] >> 3U) & (0x01U)) );
  _m->launch_ctrl_button = (uint8_t) ( ((_d[0] >> 4U) & (0x01U)) );
  _m->torque_mode_button = (uint8_t) ( ((_d[0] >> 5U) & (0x01U)) );
  _m->led_dimmer_button = (uint8_t) ( ((_d[0] >> 6U) & (0x01U)) );
  _m->left_shifter_button = (uint8_t) ( ((_d[0] >> 7U) & (0x01U)) );
  _m->right_shifter_button = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->shutdown_h_above_threshold = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->ssok_above_threshold = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );
  _m->drive_buzzer = (uint8_t) ( ((_d[1] >> 3U) & (0x01U)) );
  _m->dial_state = (uint8_t) ( (_d[2] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < DASHBOARD_STATE_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_DASHBOARD_STATE_hytech(&_m->mon1, DASHBOARD_STATE_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return DASHBOARD_STATE_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_DASHBOARD_STATE_hytech(DASHBOARD_STATE_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(DASHBOARD_STATE_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->start_button & (0x01U)) | ((_m->mark_button & (0x01U)) << 1U) | ((_m->mode_button & (0x01U)) << 2U) | ((_m->motor_controller_cycle_button & (0x01U)) << 3U) | ((_m->launch_ctrl_button & (0x01U)) << 4U) | ((_m->torque_mode_button & (0x01U)) << 5U) | ((_m->led_dimmer_button & (0x01U)) << 6U) | ((_m->left_shifter_button & (0x01U)) << 7U) );
  cframe->Data[1] |= (uint8_t) ( (_m->right_shifter_button & (0x01U)) | ((_m->shutdown_h_above_threshold & (0x01U)) << 1U) | ((_m->ssok_above_threshold & (0x01U)) << 2U) | ((_m->drive_buzzer & (0x01U)) << 3U) );
  cframe->Data[2] |= (uint8_t) ( (_m->dial_state & (0xFFU)) );

  cframe->MsgId = (uint32_t) DASHBOARD_STATE_CANID;
  cframe->DLC = (uint8_t) DASHBOARD_STATE_DLC;
  cframe->IDE = (uint8_t) DASHBOARD_STATE_IDE;
  return DASHBOARD_STATE_CANID;
}

#else

uint32_t Pack_DASHBOARD_STATE_hytech(DASHBOARD_STATE_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(DASHBOARD_STATE_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->start_button & (0x01U)) | ((_m->mark_button & (0x01U)) << 1U) | ((_m->mode_button & (0x01U)) << 2U) | ((_m->motor_controller_cycle_button & (0x01U)) << 3U) | ((_m->launch_ctrl_button & (0x01U)) << 4U) | ((_m->torque_mode_button & (0x01U)) << 5U) | ((_m->led_dimmer_button & (0x01U)) << 6U) | ((_m->left_shifter_button & (0x01U)) << 7U) );
  _d[1] |= (uint8_t) ( (_m->right_shifter_button & (0x01U)) | ((_m->shutdown_h_above_threshold & (0x01U)) << 1U) | ((_m->ssok_above_threshold & (0x01U)) << 2U) | ((_m->drive_buzzer & (0x01U)) << 3U) );
  _d[2] |= (uint8_t) ( (_m->dial_state & (0xFFU)) );

  *_len = (uint8_t) DASHBOARD_STATE_DLC;
  *_ide = (uint8_t) DASHBOARD_STATE_IDE;
  return DASHBOARD_STATE_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_EM_STATUS_hytech(EM_STATUS_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->voltage_gain = (uint8_t) ( (_d[0] & (0x0FU)) );
  _m->current_gain = (uint8_t) ( ((_d[0] >> 4U) & (0x0FU)) );
  _m->overvoltage_error = (uint8_t) ( (_d[1] & (0x01U)) );
  _m->overpower_error = (uint8_t) ( ((_d[1] >> 1U) & (0x01U)) );
  _m->logging = (uint8_t) ( ((_d[1] >> 2U) & (0x01U)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < EM_STATUS_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_EM_STATUS_hytech(&_m->mon1, EM_STATUS_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return EM_STATUS_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_EM_STATUS_hytech(EM_STATUS_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(EM_STATUS_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->voltage_gain & (0x0FU)) | ((_m->current_gain & (0x0FU)) << 4U) );
  cframe->Data[1] |= (uint8_t) ( (_m->overvoltage_error & (0x01U)) | ((_m->overpower_error & (0x01U)) << 1U) | ((_m->logging & (0x01U)) << 2U) );

  cframe->MsgId = (uint32_t) EM_STATUS_CANID;
  cframe->DLC = (uint8_t) EM_STATUS_DLC;
  cframe->IDE = (uint8_t) EM_STATUS_IDE;
  return EM_STATUS_CANID;
}

#else

uint32_t Pack_EM_STATUS_hytech(EM_STATUS_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(EM_STATUS_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->voltage_gain & (0x0FU)) | ((_m->current_gain & (0x0FU)) << 4U) );
  _d[1] |= (uint8_t) ( (_m->overvoltage_error & (0x01U)) | ((_m->overpower_error & (0x01U)) << 1U) | ((_m->logging & (0x01U)) << 2U) );

  *_len = (uint8_t) EM_STATUS_DLC;
  *_ide = (uint8_t) EM_STATUS_IDE;
  return EM_STATUS_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_EM_MEASUREMENT_hytech(EM_MEASUREMENT_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->current_ro = (int16_t) __ext_sig__(( ((_d[5] & (0xFFU)) << 8U) | (_d[4] & (0xFFU)) ), 16);
#ifdef HYTECH_USE_SIGFLOAT
  _m->current_phys = (sigfloat_t)(HYTECH_current_ro_fromS(_m->current_ro));
#endif // HYTECH_USE_SIGFLOAT

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < EM_MEASUREMENT_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_EM_MEASUREMENT_hytech(&_m->mon1, EM_MEASUREMENT_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return EM_MEASUREMENT_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_EM_MEASUREMENT_hytech(EM_MEASUREMENT_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(EM_MEASUREMENT_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->current_ro = (int16_t) HYTECH_current_ro_toS(_m->current_phys);
#endif // HYTECH_USE_SIGFLOAT

  cframe->Data[4] |= (uint8_t) ( (_m->current_ro & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( ((_m->current_ro >> 8U) & (0xFFU)) );

  cframe->MsgId = (uint32_t) EM_MEASUREMENT_CANID;
  cframe->DLC = (uint8_t) EM_MEASUREMENT_DLC;
  cframe->IDE = (uint8_t) EM_MEASUREMENT_IDE;
  return EM_MEASUREMENT_CANID;
}

#else

uint32_t Pack_EM_MEASUREMENT_hytech(EM_MEASUREMENT_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(EM_MEASUREMENT_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

#ifdef HYTECH_USE_SIGFLOAT
  _m->current_ro = (int16_t) HYTECH_current_ro_toS(_m->current_phys);
#endif // HYTECH_USE_SIGFLOAT

  _d[4] |= (uint8_t) ( (_m->current_ro & (0xFFU)) );
  _d[5] |= (uint8_t) ( ((_m->current_ro >> 8U) & (0xFFU)) );

  *_len = (uint8_t) EM_MEASUREMENT_DLC;
  *_ide = (uint8_t) EM_MEASUREMENT_IDE;
  return EM_MEASUREMENT_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_CHARGER_CONTROL_hytech(CHARGER_CONTROL_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->max_charging_voltage_high = (uint8_t) ( (_d[0] & (0xFFU)) );
  _m->max_charging_voltage_low = (uint8_t) ( (_d[1] & (0xFFU)) );
  _m->max_charging_current_high = (uint8_t) ( (_d[2] & (0xFFU)) );
  _m->max_charging_current_low = (uint8_t) ( (_d[3] & (0xFFU)) );
  _m->control = (uint8_t) ( (_d[4] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < CHARGER_CONTROL_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_CHARGER_CONTROL_hytech(&_m->mon1, CHARGER_CONTROL_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return CHARGER_CONTROL_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_CHARGER_CONTROL_hytech(CHARGER_CONTROL_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(CHARGER_CONTROL_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->max_charging_voltage_high & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( (_m->max_charging_voltage_low & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->max_charging_current_high & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( (_m->max_charging_current_low & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->control & (0xFFU)) );

  cframe->MsgId = (uint32_t) CHARGER_CONTROL_CANID;
  cframe->DLC = (uint8_t) CHARGER_CONTROL_DLC;
  cframe->IDE = (uint8_t) CHARGER_CONTROL_IDE;
  return CHARGER_CONTROL_CANID;
}

#else

uint32_t Pack_CHARGER_CONTROL_hytech(CHARGER_CONTROL_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(CHARGER_CONTROL_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->max_charging_voltage_high & (0xFFU)) );
  _d[1] |= (uint8_t) ( (_m->max_charging_voltage_low & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->max_charging_current_high & (0xFFU)) );
  _d[3] |= (uint8_t) ( (_m->max_charging_current_low & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->control & (0xFFU)) );

  *_len = (uint8_t) CHARGER_CONTROL_DLC;
  *_ide = (uint8_t) CHARGER_CONTROL_IDE;
  return CHARGER_CONTROL_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

uint32_t Unpack_CHARGER_DATA_hytech(CHARGER_DATA_t* _m, const uint8_t* _d, uint8_t dlc_)
{
  (void)dlc_;
  _m->output_dc_voltage_high = (uint8_t) ( (_d[0] & (0xFFU)) );
  _m->output_dc_voltage_low = (uint8_t) ( (_d[1] & (0xFFU)) );
  _m->output_current_high = (uint8_t) ( (_d[2] & (0xFFU)) );
  _m->output_current_low = (uint8_t) ( (_d[3] & (0xFFU)) );
  _m->flags = (uint8_t) ( (_d[4] & (0xFFU)) );
  _m->input_ac_voltage_high = (uint8_t) ( (_d[5] & (0xFFU)) );
  _m->input_ac_voltage_low = (uint8_t) ( (_d[6] & (0xFFU)) );

#ifdef HYTECH_USE_DIAG_MONITORS
  _m->mon1.dlc_error = (dlc_ < CHARGER_DATA_DLC);
  _m->mon1.last_cycle = GetSystemTick();
  _m->mon1.frame_cnt++;

  FMon_CHARGER_DATA_hytech(&_m->mon1, CHARGER_DATA_CANID);
#endif // HYTECH_USE_DIAG_MONITORS

  return CHARGER_DATA_CANID;
}

#ifdef HYTECH_USE_CANSTRUCT

uint32_t Pack_CHARGER_DATA_hytech(CHARGER_DATA_t* _m, __CoderDbcCanFrame_t__* cframe)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(CHARGER_DATA_DLC); cframe->Data[i++] = HYTECH_INITIAL_BYTE_VALUE);

  cframe->Data[0] |= (uint8_t) ( (_m->output_dc_voltage_high & (0xFFU)) );
  cframe->Data[1] |= (uint8_t) ( (_m->output_dc_voltage_low & (0xFFU)) );
  cframe->Data[2] |= (uint8_t) ( (_m->output_current_high & (0xFFU)) );
  cframe->Data[3] |= (uint8_t) ( (_m->output_current_low & (0xFFU)) );
  cframe->Data[4] |= (uint8_t) ( (_m->flags & (0xFFU)) );
  cframe->Data[5] |= (uint8_t) ( (_m->input_ac_voltage_high & (0xFFU)) );
  cframe->Data[6] |= (uint8_t) ( (_m->input_ac_voltage_low & (0xFFU)) );

  cframe->MsgId = (uint32_t) CHARGER_DATA_CANID;
  cframe->DLC = (uint8_t) CHARGER_DATA_DLC;
  cframe->IDE = (uint8_t) CHARGER_DATA_IDE;
  return CHARGER_DATA_CANID;
}

#else

uint32_t Pack_CHARGER_DATA_hytech(CHARGER_DATA_t* _m, uint8_t* _d, uint8_t* _len, uint8_t* _ide)
{
  uint8_t i; for (i = 0u; i < HYTECH_VALIDATE_DLC(CHARGER_DATA_DLC); _d[i++] = HYTECH_INITIAL_BYTE_VALUE);

  _d[0] |= (uint8_t) ( (_m->output_dc_voltage_high & (0xFFU)) );
  _d[1] |= (uint8_t) ( (_m->output_dc_voltage_low & (0xFFU)) );
  _d[2] |= (uint8_t) ( (_m->output_current_high & (0xFFU)) );
  _d[3] |= (uint8_t) ( (_m->output_current_low & (0xFFU)) );
  _d[4] |= (uint8_t) ( (_m->flags & (0xFFU)) );
  _d[5] |= (uint8_t) ( (_m->input_ac_voltage_high & (0xFFU)) );
  _d[6] |= (uint8_t) ( (_m->input_ac_voltage_low & (0xFFU)) );

  *_len = (uint8_t) CHARGER_DATA_DLC;
  *_ide = (uint8_t) CHARGER_DATA_IDE;
  return CHARGER_DATA_CANID;
}

#endif // HYTECH_USE_CANSTRUCT

