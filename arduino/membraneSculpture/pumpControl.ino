////////////////////////////////////////////////////////////////////
//// EZO-PMP CONTROL FUNCTIONS
////
////
////////////////////////////////////////////////////////////////////
//// initPump()
////
//// run in setup to initialize the EZO-PMP
//
//void initPump(){
//  Serial3.begin(115200);                              //set baud rate for software serial port_3
//  inputstring.reserve(32);                            //set aside some bytes for receiving data from the PC
//  devicestring.reserve(64);                           //set aside some bytes for receiving data from Atlas Scientific product
//  delay(100);
//
//  Serial.print("EZO-PMP\n");
//  Serial3.print("status\r");
//  delay(100);
//
//  Serial.print("Clearing volume pumped.\n");
//  Serial3.print("clear\r");
//  delay(100);
//
//  Serial.print("Continuous Mode without Reporting.\n");
//  Serial3.print("c,0\r");
//  delay(100);
//
//  Serial.print("Setting pump rate to ");
//  Serial.print(startupFlow);
//  Serial.print(" ml/min \n");
//  setFlowRate(startupFlow);
//  delay(100);
//}
//
////////////////////////////////////////////////////////////////////
//// pumpCommunication()
////
//// copied from the example
//
//void pumpCommunication() {
//  if (device_string_complete == true) {               //if a string from the PC has been received in its entirety
//    Serial3.print(inputstring);                       //send that string to the Atlas Scientific product
//    Serial3.print('\r');                              //add a <CR> to the end of the string
//    inputstring = "";                                 //clear the string
//    device_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the PC
//  }
//
//  if (sensor_string_complete == true) {                          //if a string from the Atlas Scientific product has been received in its entirety
//    Serial.println(devicestring);                                //send that string to the PC's serial monitor
//    if (isdigit(devicestring[0]) || devicestring[0] == '-') {    //if the first character in the string is a digit or a "-" sign
//      ml = devicestring.toFloat();                               //convert the string to a floating point number so it can be evaluated by the Arduino
//    }                                                            //in this code we do not use "ml", But if you need to evaluate the ml as a float, this is how it’s done
//    devicestring = "";                                           //clear the string:
//    sensor_string_complete = false;                              //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
//  }
//}
//
////////////////////////////////////////////////////////////////////
//// FLOW CONTROL
////
//// helper functions for pump settings
//
//void setFlowRate(float _flow) {
//  Serial3.print("dc,");
//  Serial3.print(_flow);
//  Serial3.print(",*\r");
//}
//
//void pausePump() {
//  Serial3.print("p\r");
//}
