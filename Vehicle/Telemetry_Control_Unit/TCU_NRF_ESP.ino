/*
 * Handles outputing data to the ESP32 and NRF9160 for live telem
 */

/*
 * Sends an encoded CAN message over a device in the format below
 * [ fletcher (1) | msg id (4) | msg len (1) | msg contents (8) | checksum (2) | delimiter (1) ]
 * @param serial     device to send the message over
 * @param can_msg    CAN message to send
 */
int send_can_message(HardwareSerial& serial, CAN_message_t& can_msg) {  
    uint8_t cobs_buf[SER_PKT_LEN+2*sizeof(uint8_t)+sizeof(uint16_t)];
    encode_can_message(cobs_buf, can_msg);
    if (serial.availableForWrite() >= (int) sizeof(cobs_buf)) {
      int written = serial.write(cobs_buf, sizeof(cobs_buf));
      return written;
    }
    return 0;
}

/*
 * Serializes a CAN message
 * @param *cobs_buf     output array of SER_PKT_LEN+4
 * @param  can_msg      CAN message
 */
void encode_can_message(uint8_t* cobs_buf, CAN_message_t& can_msg) {
    /*
     * DECODED FRAME STRUCTURE:
     * [ msg id (4) | msg len (1) | msg contents (8) | checksum (2) ]
     * ENCODED FRAME STRUCTURE:
     * [ fletcher (1) | msg id (4) | msg len (1) | msg contents (8) | checksum (2) | delimiter (1) ]
     */
    uint8_t can_buf[SER_PKT_LEN];
    memcpy(can_buf, &can_msg.id, sizeof(can_msg.id));        // msg id
    memcpy(can_buf + sizeof(can_msg.id), &can_msg.len, sizeof(uint8_t));     // msg len
    memcpy(can_buf + sizeof(can_msg.id) + sizeof(uint8_t), can_msg.buf, can_msg.len); // msg contents
    encode_ser_data(cobs_buf, can_buf, SER_PKT_LEN);
}

/*
 * Puts encoded data into a len + 4 long array
 * @param *cobs_buf     output array of len+4                [ fletcher (1) | data (len) | checksum (2) | delimiter (1) ]
 * @param *input        array that is the input data         [ data (len) ]
 * @param  len          amount of data to encode in bytes             
 */
void encode_ser_data(uint8_t *cobs_buf, const uint8_t *input, size_t len) {
    // Calculate checksum
    uint16_t checksum = fletcher16(input, len);
    //Create and populate an intermediate buffer with the checksum
    uint8_t checksum_buffer[len+sizeof(uint16_t)];
    memcpy(checksum_buffer, input, len);
    memcpy(checksum_buffer+len, (uint8_t*)&checksum, sizeof(uint16_t));
    //Encode buffer
    cobs_encode(checksum_buffer, len+sizeof(uint16_t), cobs_buf);
    cobs_buf[len+sizeof(uint16_t)+sizeof(uint8_t)] = 0x0;
}

int write_xbee_data() {
    uint8_t cobs_buf[SER_PKT_LEN+2*sizeof(uint8_t)+sizeof(uint16_t)];
    encode_can_message(cobs_buf, xb_msg);
    int written = NRF.write(cobs_buf, sizeof(cobs_buf));
    ESP.write(cobs_buf, sizeof(cobs_buf));
    return written;
}

/*
 * Read a serial port and decodes
 */
int read_ser(Stream& port) {  
    while (port.available()) {
      
    }
    return 0;
}

/*
 * Decodes serialized data
 * 
 */
size_t decode_ser_data(uint8_t* output, const uint8_t *cobs_buf, size_t len) {
    if (len == 0) return 0;
    uint8_t checksum_buf[len-2*sizeof(uint8_t)];
    size_t result = cobs_decode(cobs_buf, len, checksum_buf);
    if (fletcher16(checksum_buf, len-2*sizeof(uint8_t)-sizeof(uint16_t)) != *((uint16_t*)(checksum_buf+len-2))) {
        return 0;
    }
    memcpy(output, checksum_buf, len-2*sizeof(uint8_t)-sizeof(uint16_t));
    return result;
}

void send_xbee() {
    if (timer_debug_bms_voltages.check()) {
        bms_voltages.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_voltages);
        xb_msg.id = ID_BMS_VOLTAGES;
        write_xbee_data();
        /*NRF.print("BMS VOLTAGE AVERAGE: ");
        NRF.println(bms_voltages.get_average() / (double) 10000, 4);
        NRF.print("BMS VOLTAGE LOW: ");
        NRF.println(bms_voltages.get_low() / (double) 10000, 4);
        NRF.print("BMS VOLTAGE HIGH: ");
        NRF.println(bms_voltages.get_high() / (double) 10000, 4);
        NRF.print("BMS VOLTAGE TOTAL: ");
        NRF.println(bms_voltages.get_total() / (double) 100, 2);*/
    }
    if (timer_debug_bms_detailed_voltages.check()) {
        for (int ic = 0; ic < NUM_BMS_IC; ic++) {
            for (int group = 0; group < ((ic % 2 == 0) ? 4 : 3); group++) {
                bms_detailed_voltages[group][ic].write(xb_msg.buf);
                xb_msg.len = sizeof(BMS_detailed_voltages);
                xb_msg.id = ID_BMS_DETAILED_VOLTAGES;
                write_xbee_data();
            }
        }
    }
    if (timer_debug_bms_temperatures.check()) {
        bms_temperatures.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_temperatures);
        xb_msg.id = ID_BMS_TEMPERATURES;
        write_xbee_data();
        /*NRF.print("BMS AVERAGE TEMPERATURE: ");
        NRF.println(bms_temperatures.get_average_temperature() / (double) 100, 2);
        NRF.print("BMS LOW TEMPERATURE: ");
        NRF.println(bms_temperatures.get_low_temperature() / (double) 100, 2);
        NRF.print("BMS HIGH TEMPERATURE: ");
        NRF.println(bms_temperatures.get_high_temperature() / (double) 100, 2);*/
    }
    if (timer_debug_bms_detailed_temperatures.check()) {
        for (int ic = 0; ic < NUM_BMS_IC; ic++) {
            for (int group = 0; group < 2; group++) {
                bms_detailed_temperatures[group][ic].write(xb_msg.buf);
                xb_msg.len = sizeof(BMS_detailed_temperatures);
                xb_msg.id = ID_BMS_DETAILED_TEMPERATURES;
                write_xbee_data();
            }
        }
    }
    if (timer_debug_bms_status.check()) {
        bms_status.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_status);
        xb_msg.id = ID_BMS_STATUS;
        write_xbee_data();
        /*NRF.print("BMS STATE: ");
        NRF.println(bms_status.get_state());
        NRF.print("BMS ERROR FLAGS: 0x");
        NRF.println(bms_status.get_error_flags(), HEX);
        NRF.print("BMS CURRENT: ");
        NRF.println(bms_status.get_current() / (double) 100, 2);*/
    }
    #if COULOUMB_COUNTING_EN
    if (timer_debug_bms_coulomb_counts.check()) {
        bms_coulomb_counts.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_coulomb_counts);
        xb_msg.id = ID_BMS_COULOMB_COUNTS;
        write_xbee_data();
    }
    #endif
    if (timer_mcu_status.check()) {
        mcu_status.write(xb_msg.buf);
        xb_msg.len = sizeof(MCU_status);
        xb_msg.id = ID_MCU_STATUS;
        // Serial.println("xbee mcu send");
        write_xbee_data();
    }
    if (timer_debug_mcu_pedal_readings.check()) {
        mcu_pedal_readings.write(xb_msg.buf);
        xb_msg.len = sizeof(MCU_pedal_readings);
        xb_msg.id = ID_MCU_PEDAL_READINGS;
        write_xbee_data();
    }
    
    if (timer_debug_bms_balancing_status.check()) {
        for (int i = 0; i < 2; i++) {
            bms_balancing_status[i].write(xb_msg.buf);
            xb_msg.len = sizeof(BMS_balancing_status);
            xb_msg.id = ID_BMS_BALANCING_STATUS;
            write_xbee_data();
        }
    }
    if (timer_debug_dashboard.check()) {
        dashboard_status.write(xb_msg.buf);
        xb_msg.len = sizeof(dashboard_status);
        xb_msg.id = ID_DASHBOARD_STATUS;
        write_xbee_data();
    }
    if (timer_em_status.check()) {
        em_status.write(xb_msg.buf);
        xb_msg.len = sizeof(em_status);
        xb_msg.id = ID_EM_STATUS;
        write_xbee_data();
    }
    if (timer_em_measurement.check()) {
        em_measurement.write(xb_msg.buf);
        xb_msg.len = sizeof(em_measurement);
        xb_msg.id = ID_EM_MEASUREMENT;
        write_xbee_data();
    }
    if (timer_imu_accelerometer.check()) {
        imu_accelerometer.write(xb_msg.buf);
        xb_msg.len = sizeof(imu_accelerometer);
        xb_msg.id = ID_IMU_ACCELEROMETER;
        write_xbee_data();
    }
    if (timer_imu_gyroscope.check()) {
        imu_gyroscope.write(xb_msg.buf);
        xb_msg.len = sizeof(imu_gyroscope);
        xb_msg.id = ID_IMU_GYROSCOPE;
        write_xbee_data();
    }
    if (timer_energy.check()) {
        mc1_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_energy);
        xb_msg.id = ID_MC1_ENERGY;
        write_xbee_data();
        mc2_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_energy);
        xb_msg.id = ID_MC2_ENERGY;
        write_xbee_data();
        mc3_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_energy);
        xb_msg.id = ID_MC3_ENERGY;
        write_xbee_data();
        mc4_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_energy);
        xb_msg.id = ID_MC4_ENERGY;
        write_xbee_data();
    }
    if (timer_setpoints_command.check()) {
        mc1_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_setpoints_command);
        xb_msg.id = ID_MC1_SETPOINTS_COMMAND;
        write_xbee_data();
        mc2_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_setpoints_command);
        xb_msg.id = ID_MC2_SETPOINTS_COMMAND;
        write_xbee_data();
        mc3_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_setpoints_command);
        xb_msg.id = ID_MC3_SETPOINTS_COMMAND;
        write_xbee_data();
        mc4_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_setpoints_command);
        xb_msg.id = ID_MC4_SETPOINTS_COMMAND;
        write_xbee_data();
    }
    if (timer_status.check()) {
        mc1_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_status);
        xb_msg.id = ID_MC1_STATUS;
        write_xbee_data();
        mc2_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_status);
        xb_msg.id = ID_MC2_STATUS;
        write_xbee_data();
        mc3_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_status);
        xb_msg.id = ID_MC3_STATUS;
        write_xbee_data();
        mc4_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_status);
        xb_msg.id = ID_MC4_STATUS;
        write_xbee_data();
    }
    if (timer_temps.check()) {
        mc1_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_temps);
        xb_msg.id = ID_MC1_TEMPS;
        write_xbee_data();
        mc2_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_temps);
        xb_msg.id = ID_MC2_TEMPS;
        write_xbee_data();
        mc3_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_temps);
        xb_msg.id = ID_MC3_TEMPS;
        write_xbee_data();
        mc4_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_temps);
        xb_msg.id = ID_MC4_TEMPS;
        write_xbee_data();
    }
    if (timer_sab_readings_front.check()) {
        sab_readings_front.write(xb_msg.buf);
        xb_msg.len = sizeof(sab_readings_front);
        xb_msg.id = ID_SAB_READINGS_FRONT;
        write_xbee_data();
    }
    if (timer_sab_readings_rear.check()) {
        sab_readings_rear.write(xb_msg.buf);
        xb_msg.len = sizeof(sab_readings_rear);
        xb_msg.id = ID_SAB_READINGS_REAR;
        write_xbee_data();
    }
    if (timer_sab_readings_gps.check()) {
        sab_readings_gps.write(xb_msg.buf);
        xb_msg.len = sizeof(sab_readings_gps);
        xb_msg.id = ID_SAB_READINGS_GPS;
        write_xbee_data();
    }
}