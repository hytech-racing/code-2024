#include "HT_Data.h"

HT_Data::HT_Data() {
    mcu_status.set_bms_ok_high(false);
    mcu_status.set_imd_ok_high(false);
    mcu_status.set_inverters_error(false);
    mcu_status.set_max_torque(TORQUE_3);
    mcu_status.set_torque_mode(3);
}


//HT_Data ht_data;
HT_Data HT_Data::ht_data;
HT_Data* HT_Data::getInstance() {
    return &ht_data;
}
