/*
 * Handles the input of CAN messages
 * sorry to whoever looks at this later
 * uwu🥺
 */

#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>

#define MESSAGE_TABLE(F, ...) \
  F(MC_status, ID_MC1_STATUS, 1, 100, ##__VA_ARGS__) \
  F(MC_temps, ID_MC1_TEMPS, 2, 100, ##__VA_ARGS__) \
  F(MC_energy, ID_MC1_ENERGY, 3, 100, ##__VA_ARGS__)

#define GET_CLASS_INST(CLASS, ID, NUM, TIME) CLASS##_message##NUM

#define GET_TIMER(CLASS, ID, NUM, TIME) timer_##CLASS##NUM


#define TIMER(CLASS, ID, NUM, TIME) \
  Metro GET_TIMER(CLASS, ID, NUM, TIME) = Metro(TIME);

#define DECLARE_CLASS(CLASS, ID, NUM, TIME) \
  CLASS GET_CLASS_INST(CLASS, ID, NUM, TIME);

#define PARSING_CASES(CLASS, ID, NUM, TIME, MSG) \
  case ID: \
    GET_CLASS_INST(CLASS, ID, NUM, TIME).load(MSG.buf); \
    break;

#define UPLOAD_MSG(CLASS, ID, NUM, TIME, MSG) \
  GET_CLASS_INST(CLASS, ID, NUM, TIME).write(MSG.buf); \
  MSG.len = sizeof(CLASS); \
  MSG.id = ID; \
  write_xbee_data();

#define CHECK_TIMER(CLASS, ID, NUM, TIME, MSG) \
  if (GET_TIMER(CLASS, ID, NUM, TIME).check()) { \
    UPLOAD_MSG(CLASS, ID, TIME, MSG) \
  }

/*
MESSAGE_TABLE(TIMER)

MESSAGE_TABLE(DECLARE_CLASS)


void parse_can_message_macro(CAN_message_t& msg) {
    write_to_SD(&msg);

    switch(msg.id) {
        MESSAGE_TABLE(PARSING_CASES, msg)
    }
}
*/

Metro mc1_status_timer = Metro(20);
Metro mc2_status_timer = Metro(20);
Metro mc3_status_timer = Metro(20);
Metro mc4_status_timer = Metro(20);
Metro mc1_temps_timer = Metro(20);
Metro mc2_temps_timer = Metro(20);
Metro mc3_temps_timer = Metro(20);
Metro mc4_temps_timer = Metro(20);
Metro mc1_energy_timer = Metro(20);
Metro mc2_energy_timer = Metro(20);
Metro mc3_energy_timer = Metro(20);
Metro mc4_energy_timer = Metro(20);
Metro mc1_setpoints_command_timer = Metro(20);
Metro mc2_setpoints_command_timer = Metro(20);
Metro mc3_setpoints_command_timer = Metro(20);
Metro mc4_setpoints_command_timer = Metro(20);

#pragma pack(push,1)
typedef struct CAN_write_t {
  uint64_t time;
  uint16_t id;
  uint64_t data;  
} CAN_write_t;
#pragma pack(pop)

void parse_can1_message(const CAN_message_t& rx_msg) {
  if(allow_message(rx_msg))
  {
    while (buffer_lock.try_lock() == 0) {
      threads.yield();
    }
    CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg, .time = getTime() });  //unclear what were passing
    buffer_lock.unlock();
    counters.CAN_1_freq++;
  }
}
void parse_can2_message(const CAN_message_t& rx_msg) {
  if(allow_message(rx_msg))
  {
    while (buffer_lock.try_lock() == 0) {
      threads.yield();
    }
    CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg, .time = getTime() });  //unclear what were passing
    buffer_lock.unlock();
    counters.CAN_2_freq++;
  }
}
void parse_can3_message(const CAN_message_t& rx_msg) {
  if(allow_message(rx_msg))
  {
    while (buffer_lock.try_lock() == 0) {
      threads.yield();
    }
    CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg, .time = getTime() });  //unclear what were passing
    buffer_lock.unlock();
    counters.CAN_3_freq++;
  }
}

void parse_can_q() {
  //CAN_write_t packed_msg;
  SD_write_buf *thirdptr;
  while(!CAN_msg_q.isEmpty()) {
    while (buffer_lock.try_lock() == 0) {
      threads.yield();
    }
    cli();
    CAN_msg_time msg_time = CAN_msg_q.pop();
    sei();
    buffer_lock.unlock();
    CAN_message_t msg = msg_time.msg;

    message_cbs(msg);
    
    logger.print(msg_time.time);
    logger.print(",");
    logger.print(msg.id, HEX);
    logger.print(",");
    logger.print(msg.len);
    logger.print(",");
    for (int i = 0; i < msg.len; i++) {
        if (msg.buf[i] < 16) {
            logger.print("0");
        }
        logger.print(msg.buf[i], HEX);
    }
    logger.println();
    counters.bytes_written++;
    
    /*packed_msg.time = msg_time.time;
    packed_msg.id = msg_time.msg.id;
    packed_msg.data = msg_time.msg.data;*/    
    /*incoming_buf->size += snprintf((char *)(incoming_buf->buffer)+(incoming_buf->size), SD_BUFF_SZ-incoming_buf->size, "%llu,%lx,%u,%llx", msg_time.time, msg_time.msg.id,  msg_time.msg.len, *((uint64_t *) msg_time.msg.buf));

    if ((incoming_buf->size) > (.8*SD_BUFF_SZ)) {
      cli();
      thirdptr = incoming_buf;
      incoming_buf = current_write_buf;
      current_write_buf = thirdptr;  
      incoming_buf->size = 0;    
      sei();      
    }*/
  }
}

bool allow_message(const CAN_message_t& rx_msg) {
  switch(rx_msg.id) {
    //case ID_MC1_STATUS: return mc1_status_timer.check(); break;
    //case ID_MC2_STATUS: return mc2_status_timer.check(); break;
    //case ID_MC3_STATUS: return mc3_status_timer.check(); break;
    //case ID_MC4_STATUS: return mc4_status_timer.check(); break;
    //case ID_MC1_TEMPS: return mc1_temps_timer.check(); break;
    //case ID_MC2_TEMPS: return mc2_temps_timer.check(); break;
    //case ID_MC3_TEMPS: return mc3_temps_timer.check(); break;
    //case ID_MC4_TEMPS: return mc4_temps_timer.check(); break;
    //case ID_MC1_ENERGY: return mc1_energy_timer.check(); break;
    //case ID_MC2_ENERGY: return mc2_energy_timer.check(); break;
    //case ID_MC3_ENERGY: return mc3_energy_timer.check(); break;
    //case ID_MC4_ENERGY: return mc4_energy_timer.check(); break;
    //case ID_MC1_SETPOINTS_COMMAND: return mc1_setpoints_command_timer.check(); break;
    //case ID_MC2_SETPOINTS_COMMAND: return mc2_setpoints_command_timer.check(); break;
    //case ID_MC3_SETPOINTS_COMMAND: return mc3_setpoints_command_timer.check(); break;
    //case ID_MC4_SETPOINTS_COMMAND: return mc4_setpoints_command_timer.check(); break;
    default: return true; break;
  }
  return true;
}

void message_cbs(const CAN_message_t& rx_msg) {
  switch(rx_msg.id) {
    case ID_BMS_DETAILED_VOLTAGES: parse_detailed_voltages(rx_msg); break;
    case ID_BMS_VOLTAGES: parse_voltages(rx_msg); break;
    case ID_BMS_DETAILED_TEMPERATURES: parse_detailed_temperatures(rx_msg); break;
    case ID_BMS_ONBOARD_TEMPERATURES: parse_onboard_temperatures(rx_msg); break;
    case ID_BMS_TEMPERATURES: parse_temperatures(rx_msg); break;
    case ID_MC1_TEMPS: parse_mc_temps1(rx_msg); break;
    case ID_MC2_TEMPS: parse_mc_temps2(rx_msg); break;
    case ID_MC3_TEMPS: parse_mc_temps3(rx_msg); break;
    case ID_MC4_TEMPS: parse_mc_temps4(rx_msg); break;
  }
}