#include "hytech-binutil.h"

// DBC file version
#if (VER_HYTECH_MAJ != (6U)) || (VER_HYTECH_MIN != (0U))
#error The HYTECH binutil source file has inconsistency with core dbc lib!
#endif

#ifdef __DEF_HYTECH__

hytech_rx_t hytech_rx;

#endif // __DEF_HYTECH__

uint32_t hytech_Receive(hytech_rx_t* _m, const uint8_t* _d, uint32_t _id, uint8_t dlc_)
{
 uint32_t recid = 0;
 if ((_id >= 0x94U) && (_id < 0xC3U)) {
  if ((_id >= 0x94U) && (_id < 0xA9U)) {
   if ((_id >= 0x94U) && (_id < 0xA3U)) {
    if ((_id >= 0x94U) && (_id < 0xA0U)) {
     if (_id == 0x94U) {
      recid = Unpack_TCU_DRIVER_MSG_hytech(&(_m->TCU_DRIVER_MSG), _d, dlc_);
     } else if (_id == 0x95U) {
      recid = Unpack_TCU_LAP_TIMES_hytech(&(_m->TCU_LAP_TIMES), _d, dlc_);
     }
    } else {
     if (_id == 0xA0U) {
      recid = Unpack_MC1_STATUS_hytech(&(_m->MC1_STATUS), _d, dlc_);
     } else {
      if (_id == 0xA1U) {
       recid = Unpack_MC2_STATUS_hytech(&(_m->MC2_STATUS), _d, dlc_);
      } else if (_id == 0xA2U) {
       recid = Unpack_MC3_STATUS_hytech(&(_m->MC3_STATUS), _d, dlc_);
      }
     }
    }
   } else {
    if ((_id >= 0xA3U) && (_id < 0xA6U)) {
     if (_id == 0xA3U) {
      recid = Unpack_MC4_STATUS_hytech(&(_m->MC4_STATUS), _d, dlc_);
     } else {
      if (_id == 0xA4U) {
       recid = Unpack_MC1_TEMPS_hytech(&(_m->MC1_TEMPS), _d, dlc_);
      } else if (_id == 0xA5U) {
       recid = Unpack_MC2_TEMPS_hytech(&(_m->MC2_TEMPS), _d, dlc_);
      }
     }
    } else {
     if (_id == 0xA6U) {
      recid = Unpack_MC3_TEMPS_hytech(&(_m->MC3_TEMPS), _d, dlc_);
     } else {
      if (_id == 0xA7U) {
       recid = Unpack_MC4_TEMPS_hytech(&(_m->MC4_TEMPS), _d, dlc_);
      } else if (_id == 0xA8U) {
       recid = Unpack_MC1_ENERGY_hytech(&(_m->MC1_ENERGY), _d, dlc_);
      }
     }
    }
   }
  } else {
   if ((_id >= 0xA9U) && (_id < 0xB2U)) {
    if ((_id >= 0xA9U) && (_id < 0xABU)) {
     if (_id == 0xA9U) {
      recid = Unpack_MC2_ENERGY_hytech(&(_m->MC2_ENERGY), _d, dlc_);
     } else if (_id == 0xAAU) {
      recid = Unpack_MC3_ENERGY_hytech(&(_m->MC3_ENERGY), _d, dlc_);
     }
    } else {
     if (_id == 0xABU) {
      recid = Unpack_MC4_ENERGY_hytech(&(_m->MC4_ENERGY), _d, dlc_);
     } else {
      if (_id == 0xB0U) {
       recid = Unpack_MC1_SETPOINTS_COMMAND_hytech(&(_m->MC1_SETPOINTS_COMMAND), _d, dlc_);
      } else if (_id == 0xB1U) {
       recid = Unpack_MC2_SETPOINTS_COMMAND_hytech(&(_m->MC2_SETPOINTS_COMMAND), _d, dlc_);
      }
     }
    }
   } else {
    if ((_id >= 0xB2U) && (_id < 0xB5U)) {
     if (_id == 0xB2U) {
      recid = Unpack_MC3_SETPOINTS_COMMAND_hytech(&(_m->MC3_SETPOINTS_COMMAND), _d, dlc_);
     } else {
      if (_id == 0xB3U) {
       recid = Unpack_MC4_SETPOINTS_COMMAND_hytech(&(_m->MC4_SETPOINTS_COMMAND), _d, dlc_);
      } else if (_id == 0xB4U) {
       recid = Unpack_MC1_TORQUE_COMMAND_hytech(&(_m->MC1_TORQUE_COMMAND), _d, dlc_);
      }
     }
    } else {
     if (_id == 0xB5U) {
      recid = Unpack_MC2_TORQUE_COMMAND_hytech(&(_m->MC2_TORQUE_COMMAND), _d, dlc_);
     } else {
      if (_id == 0xB6U) {
       recid = Unpack_MC3_TORQUE_COMMAND_hytech(&(_m->MC3_TORQUE_COMMAND), _d, dlc_);
      } else if (_id == 0xB7U) {
       recid = Unpack_MC4_TORQUE_COMMAND_hytech(&(_m->MC4_TORQUE_COMMAND), _d, dlc_);
      }
     }
    }
   }
  }
 } else {
  if ((_id >= 0xC3U) && (_id < 0xD9U)) {
   if ((_id >= 0xC3U) && (_id < 0xC8U)) {
    if ((_id >= 0xC3U) && (_id < 0xC5U)) {
     if (_id == 0xC3U) {
      recid = Unpack_MCU_STATUS_hytech(&(_m->MCU_STATUS), _d, dlc_);
     } else if (_id == 0xC4U) {
      recid = Unpack_MCU_PEDAL_READINGS_hytech(&(_m->MCU_PEDAL_READINGS), _d, dlc_);
     }
    } else {
     if (_id == 0xC5U) {
      recid = Unpack_MCU_LOAD_CELLS_hytech(&(_m->MCU_LOAD_CELLS), _d, dlc_);
     } else {
      if (_id == 0xC6U) {
       recid = Unpack_SAB_LOAD_CELLS_hytech(&(_m->SAB_LOAD_CELLS), _d, dlc_);
      } else if (_id == 0xC7U) {
       recid = Unpack_MCU_FRONT_POTS_hytech(&(_m->MCU_FRONT_POTS), _d, dlc_);
      }
     }
    }
   } else {
    if ((_id >= 0xC8U) && (_id < 0xD6U)) {
     if (_id == 0xC8U) {
      recid = Unpack_MCU_REAR_POTS_hytech(&(_m->MCU_REAR_POTS), _d, dlc_);
     } else {
      if (_id == 0xCCU) {
       recid = Unpack_MCU_ANALOG_READINGS_hytech(&(_m->MCU_ANALOG_READINGS), _d, dlc_);
      } else if (_id == 0xD5U) {
       recid = Unpack_BMS_ONBOARD_TEMPS_hytech(&(_m->BMS_ONBOARD_TEMPS), _d, dlc_);
      }
     }
    } else {
     if (_id == 0xD6U) {
      recid = Unpack_BMS_ONBOARD_DETAILED_TEMPS_hytech(&(_m->BMS_ONBOARD_DETAILED_TEMPS), _d, dlc_);
     } else {
      if (_id == 0xD7U) {
       recid = Unpack_BMS_VOLTAGES_hytech(&(_m->BMS_VOLTAGES), _d, dlc_);
      } else if (_id == 0xD8U) {
       recid = Unpack_BMS_DETAILED_VOLTAGES_hytech(&(_m->BMS_DETAILED_VOLTAGES), _d, dlc_);
      }
     }
    }
   }
  } else {
   if ((_id >= 0xD9U) && (_id < 0xEBU)) {
    if ((_id >= 0xD9U) && (_id < 0xDDU)) {
     if (_id == 0xD9U) {
      recid = Unpack_BMS_TEMPS_hytech(&(_m->BMS_TEMPS), _d, dlc_);
     } else {
      if (_id == 0xDAU) {
       recid = Unpack_BMS_DETAILED_TEMPS_hytech(&(_m->BMS_DETAILED_TEMPS), _d, dlc_);
      } else if (_id == 0xDBU) {
       recid = Unpack_BMS_STATUS_hytech(&(_m->BMS_STATUS), _d, dlc_);
      }
     }
    } else {
     if (_id == 0xDDU) {
      recid = Unpack_CCU_STATUS_hytech(&(_m->CCU_STATUS), _d, dlc_);
     } else {
      if (_id == 0xDEU) {
       recid = Unpack_BMS_BALANCING_STATUS_hytech(&(_m->BMS_BALANCING_STATUS), _d, dlc_);
      } else if (_id == 0xE2U) {
       recid = Unpack_BMS_COULOMB_COUNTS_hytech(&(_m->BMS_COULOMB_COUNTS), _d, dlc_);
      }
     }
    }
   } else {
    if ((_id >= 0xEBU) && (_id < 0x400U)) {
     if (_id == 0xEBU) {
      recid = Unpack_DASHBOARD_MCU_STATE_hytech(&(_m->DASHBOARD_MCU_STATE), _d, dlc_);
     } else {
      if (_id == 0xECU) {
       recid = Unpack_DASHBOARD_STATE_hytech(&(_m->DASHBOARD_STATE), _d, dlc_);
      } else if (_id == 0x100U) {
       recid = Unpack_EM_STATUS_hytech(&(_m->EM_STATUS), _d, dlc_);
      }
     }
    } else {
     if (_id == 0x400U) {
      recid = Unpack_EM_MEASUREMENT_hytech(&(_m->EM_MEASUREMENT), _d, dlc_);
     } else {
      if (_id == 0x1806E5F4U) {
       recid = Unpack_CHARGER_CONTROL_hytech(&(_m->CHARGER_CONTROL), _d, dlc_);
      } else if (_id == 0x18FF50E5U) {
       recid = Unpack_CHARGER_DATA_hytech(&(_m->CHARGER_DATA), _d, dlc_);
      }
     }
    }
   }
  }
 }

 return recid;
}

