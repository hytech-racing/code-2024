#include "Dashboard.h"
#include "HyTech_CAN.h"
#include "FlexCAN_T4.h"
#include "Metro.h"

Dashboard_status dashboard_status{};
MC_fault_codes mc_fault_codes{};

//CAN Variables

Metro CAN_timer = Metro(100);
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> CAN;
CAN_message_t msg;


void setup() {
  // put your setup code here, to run once:
  CAN.begin();
  CAN.setBaudRate(500000);

}

void loop() {
  // put your main code here, to run repeatedly:
delay(500);
if(CAN_timer.check()){
  msg.id = ID_DASHBOARD_STATUS;
  msg.len = sizeof(dashboard_status);
  dashboard_status.write(msg.buf);
  Serial.println("msg buf size: " + String(sizeof(msg.buf)));
  Serial.println("dashboard_status size: " + String(sizeof(dashboard_status)));
  CAN.write(msg);
  CAN_timer.reset();
  
  
}
 //dashboard_status.set_button_flags(0);

}


//void read_can(){
//  while(CAN.read(msg)){ //could this program halt the functioning of the system if can bus doesnt read anything)
//    switch(msg.id){
//      case ID_MCU_STATUS:
//        mcu_status.load(msg.buf);
////        mcu_status_received();
//        break;
//      case ID_MC_FAULT_CODES:
//        mc_fault_codes.load(buf);
////        mc_fault_codes.received;
//        break;
//      default:
//        break;
//    }
//  }
//}
