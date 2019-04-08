//////////////////////////////////////////////////////////////////
// EZO-PMP CONTROL FUNCTIONS
//
//
//////////////////////////////////////////////////////////////////
// pumpCommunication()

void pumpCommunication() {
  if (device_string_complete == true) {                //if a string from the PC has been received in its entirety
    Serial3.print(inputstring);                       //send that string to the Atlas Scientific product
    Serial3.print('\r');                              //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    device_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the PC
  }

  if (sensor_string_complete == true) {                          //if a string from the Atlas Scientific product has been received in its entirety
    Serial.println(devicestring);                                //send that string to the PC's serial monitor
    if (isdigit(devicestring[0]) || devicestring[0] == '-') {    //if the first character in the string is a digit or a "-" sign
      ml = devicestring.toFloat();                               //convert the string to a floating point number so it can be evaluated by the Arduino
    }                                                               //in this code we do not use "ml", But if you need to evaluate the ml as a float, this is how it’s done
    devicestring = "";                                             //clear the string:
    sensor_string_complete = false;                                //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }
}

//////////////////////////////////////////////////////////////////
// FLOW CONTROL

void setFlowRate(float _flow) {
  Serial3.print("dc,");
  Serial3.print(_flow);
  Serial3.print(",*\r");
}

void pausePump() {
  Serial3.print("p\r");
}
