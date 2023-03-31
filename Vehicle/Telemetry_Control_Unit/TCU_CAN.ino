/*
 * Handles the input of CAN messages
 */

#define MESSAGE_TABLE(F, ...) \
    F(MC_status, , 100 ,##__VA_ARGS__)\
    F(MC_status, , 100 ,##__VA_ARGS__)\
    F(MC_status, , 100 ,##__VA_ARGS__)

#define TIMER(CLASS, ID, TIME)\
    Metro timer_ ## CLASS = Metro(TIME);

#define DECLARE_CLASS(CLASS, ID, TIME)\
    CLASS CLASS ## _message;

#define PARSING_CASES(CLASS, ID, TIME, MSG)\
    case ID: CLASS ## _message.load(MSG.buf); break;

#define UPLOAD_MSG(CLASS, ID, TIME, MSG)\
    CLASS ## _message.write(MSG.buf);\
    MSG.len = sizeof(CLASS);\
    MSG.id = ID;\
    write_xbee_data();

#define CHECK_TIMER(CLASS, ID, TIME, MSG)\
    if (timer_ ## CLASS.check()) {\
        UPLOAD_MSG(CLASS, ID, TIME, MSG)\
    }

/*
MESSAGE_TABLE(TIMER)

MESSAGE_TABLE(DECLARE_CLASS)

void parse_can_message(CAN_message_t& msg) {
    write_to_SD(&msg);

    switch(msg.id) {
        MESSAGE_TABLE(PARSING_CASES, msg)
    }
*/