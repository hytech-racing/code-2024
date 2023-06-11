#define ID_MC1_STATUS												0xA0
#define ID_MC2_STATUS												0xA1
#define ID_MC3_STATUS												0xA2
#define ID_MC4_STATUS												0xA3
#define ID_MC1_TEMPS												0xA4
#define ID_MC2_TEMPS												0xA5
#define ID_MC3_TEMPS												0xA6
#define ID_MC4_TEMPS												0xA7
#define ID_MC1_ENERGY												0xA8
#define ID_MC2_ENERGY												0xA9
#define ID_MC3_ENERGY												0xAA
#define ID_MC4_ENERGY												0xAB

#define ID_MC1_SETPOINTS_COMMAND									0xB0
#define ID_MC2_SETPOINTS_COMMAND									0xB1
#define ID_MC3_SETPOINTS_COMMAND									0xB2
#define ID_MC4_SETPOINTS_COMMAND									0xB3
#define ID_MC1_TORQUE_COMMAND										0xB4
#define ID_MC2_TORQUE_COMMAND										0xB5
#define ID_MC3_TORQUE_COMMAND										0xB6
#define ID_MC4_TORQUE_COMMAND										0xB7



#define ID_MCU_STATUS												0xC3
#define ID_MCU_PEDAL_READINGS										0xC4
#define ID_MCU_LOAD_CELLS                                           0xC5
#define ID_MCU_FRONT_POTS                                           0xC6
#define ID_MCU_REAR_POTS                                            0xC7
#define ID_MCU_ANALOG_READINGS										0xCC


#define ID_BMS_ONBOARD_TEMPERATURES									0xD5
#define ID_BMS_ONBOARD_DETAILED_TEMPERATURES						0xD6 // TODO rename to bms_detailed_onboard_temperatures when we're not in the middle of a development cycle
#define ID_BMS_VOLTAGES												0xD7
#define ID_BMS_DETAILED_VOLTAGES									0xD8
#define ID_BMS_TEMPERATURES											0xD9
#define ID_BMS_DETAILED_TEMPERATURES								0xDA
#define ID_BMS_STATUS												0xDB
#define ID_FH_WATCHDOG_TEST											0xDC
#define ID_CCU_STATUS												0xDD
#define ID_BMS_BALANCING_STATUS										0xDE // TODO rename to bms_balancing_cells when we're not in the middle of a development cycle

#define ID_BMS_READ_WRITE_PARAMETER_COMMAND							0xE0 // TODO define this message
#define ID_BMS_PARAMETER_RESPONSE									0xE1 // TODO define this message
#define ID_BMS_COULOMB_COUNTS										0xE2
#define ID_MCU_GPS_READINGS											0xE7
#define ID_MCU_WHEEL_SPEED									    	0xEA
#define ID_DASHBOARD_STATUS                                         0xEB
#define ID_SAB_READINGS_GPS                                         0xEE

//#define ID_CHARGER_CONTROL
#define ID_CHARGER_CONTROL                                          0x1806E5F4
#define ID_CHARGER_DATA                                             0x18FF50E5

#define ID_EM_STATUS                                                0x100
#define ID_EM_MEASUREMENT                                           0x400

#define ID_IMU_ACCELEROMETER                                        0x90
#define ID_IMU_GYROSCOPE                                            0x91
#define ID_SAB_READINGS_FRONT                                       0x92
#define ID_SAB_READINGS_REAR                                        0x93


//TireTempSensors
#define ID_TPMS_LF                                                  0x424
#define ID_TEMP1_LF                                                 0x425
#define ID_TEMP2_LF                                                 0x426
#define ID_TEMP3_LF                                                 0x427
#define ID_TEMP4_LF                                                 0x428
#define ID_TEMP5_LF                                                 0x429

#define ID_TPMS_RF                                                  0x42A
#define ID_TEMP1_RF                                                 0x42B
#define ID_TEMP2_RF                                                 0x42C
#define ID_TEMP3_RF                                                 0x42D
#define ID_TEMP4_RF                                                 0x42E
#define ID_TEMP5_RF                                                 0x42F

#define ID_TPMS_LR                                                  0x430
#define ID_TEMP1_LR                                                 0x431
#define ID_TEMP2_LR                                                 0x432
#define ID_TEMP3_LR                                                 0x433
#define ID_TEMP4_LR                                                 0x434
#define ID_TEMP5_LR                                                 0x435

#define ID_TPMS_RR                                                  0x436
#define ID_TEMP1_RR                                                 0x437
#define ID_TEMP2_RR                                                 0x438
#define ID_TEMP3_RR                                                 0x439
#define ID_TEMP4_RR                                                 0x43A
#define ID_TEMP5_RR                                                 0x43B
   