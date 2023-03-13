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
#define ID_CHARGER_CONTROL                                          0x1806E0F4
#define ID_CHARGER_DATA                                             0x18FF50E0

#define ID_EM_STATUS                                                0x100
#define ID_EM_MEASUREMENT                                           0x400

#define ID_IMU_ACCELEROMETER                                        0x90
#define ID_IMU_GYROSCOPE                                            0x91
#define ID_SAB_READINGS_FRONT                                       0x92
#define ID_SAB_READINGS_REAR                                        0x93
