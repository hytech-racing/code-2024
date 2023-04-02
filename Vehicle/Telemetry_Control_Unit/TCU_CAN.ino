/*
 * Handles the input of CAN messages
 * absolutely cursed code lmao
 */

#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>

#define MESSAGE_TABLE(F, ...) \
    F(MC_status, ID_MC1_STATUS, 1, 100 ,##__VA_ARGS__)\
    F(MC_temps, ID_MC1_TEMPS, 2, 100 ,##__VA_ARGS__)\
    F(MC_energy, ID_MC1_ENERGY, 3, 100 ,##__VA_ARGS__)

#define GET_CLASS_INST(CLASS, ID, NUM, TIME) CLASS ## _message ## NUM

#define GET_TIMER(CLASS, ID, NUM, TIME) timer_ ## CLASS ## NUM


#define TIMER(CLASS, ID, NUM, TIME)\
    Metro GET_TIMER(CLASS, ID, NUM, TIME) = Metro(TIME);

#define DECLARE_CLASS(CLASS, ID, NUM, TIME)\
    CLASS GET_CLASS_INST(CLASS, ID, NUM, TIME);

#define PARSING_CASES(CLASS, ID, NUM, TIME, MSG)\
    case ID: GET_CLASS_INST(CLASS, ID, NUM, TIME).load(MSG.buf); break;

#define UPLOAD_MSG(CLASS, ID, NUM, TIME, MSG)\
    GET_CLASS_INST(CLASS, ID, NUM, TIME).write(MSG.buf);\
    MSG.len = sizeof(CLASS);\
    MSG.id = ID;\
    write_xbee_data();

#define CHECK_TIMER(CLASS, ID, NUM, TIME, MSG)\
    if (GET_TIMER(CLASS, ID, NUM, TIME).check()) {\
        UPLOAD_MSG(CLASS, ID, TIME, MSG)\
    }


MESSAGE_TABLE(TIMER)

MESSAGE_TABLE(DECLARE_CLASS)


void parse_can_message_macro(CAN_message_t& msg) {
    write_to_SD(&msg);

    switch(msg.id) {
        MESSAGE_TABLE(PARSING_CASES, msg)
    }
}
