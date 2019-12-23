/** Motor Speed Calibrator For Tauba Auerbach by Phillip David Stearns
   calibration routine to set motor speed
*/

// Libraries
#include <TimerOne.h> // including the TimerOne library for getting accurate speed readings
#include <DualG2HighPowerMotorShield.h>

// DualG2 Board Object
DualG2HighPowerMotorShield18v22 md;

// KEYWORDS FOR STATE MACHINE

#define CALIBRATE   0 //  pause and allow for mid routine adjustments
#define OPEN        1 //  move to targetOpen
#define HOLD_OPEN   2 //  hold on targetOpen
#define CLOSE       3 //  move to targetClosed
#define HOLD_CLOSED 4 //  hold on targetClosed
#define STARTUP     5 //  run EZO-PMP until wires are coated

int machineState = STARTUP;
int lastMachineState = CLOSE;

boolean isOpen = false;
boolean isClosed = false;

unsigned long openDuration = 15000; // the amount of time the movement should take
unsigned long closeDuration = 12500; // the amount of time the movement should take

unsigned long holdOpenStart = 0;
unsigned long holdOpenEnd = 15000; // duration to hold open in ms

unsigned long holdClosedStart = 0;
unsigned long holdClosedEnd = 17500; // duration to hold open in ms

unsigned long startupStart = 0;
unsigned long startupEnd = 35000; // duration to hold open in ms

// Interrupt Pins for Gear Head Motors

// Motor 1
#define M1_ENC1_PIN 18
#define M1_ENC2_PIN 19

// Motor 2
#define M2_ENC1_PIN 20
#define M2_ENC2_PIN 21

// IO pins for Button Matrix
#define BUTTON_COL_0 22
#define BUTTON_COL_1 23
#define BUTTON_COL_2 24
#define BUTTON_COL_3 25
#define BUTTON_ROW_0 26
#define BUTTON_ROW_1 27
#define BUTTON_ROW_2 28
#define BUTTON_ROW_3 29

// control variables

float stepAngle = 1.0 / 4200.0;
boolean debug = true;
boolean moveMotors = true;

// 4x4 Matrix Buttons
const int buttonRows = 4;
const int buttonCols = 4;
boolean buttonStates[buttonRows][buttonCols];
boolean lastButtonStates[buttonRows][buttonCols];

// variables for timing
unsigned long currentTime = 0;

unsigned long buttonInterval = 100; // ms between button polls
unsigned long lastButtonScan = 0;

unsigned long lastTime = 0;
unsigned long statusInterval = 1000; // ms between status update

unsigned long lastMove = 0;
unsigned long moveInterval = 10; // ms between position updates


// Position Variables
const int initTargetOpen = 2550;
const int initTargetClose = 0;
int initPosInc = 3;
int posInc = initPosInc;
int m1NextPos = 0;
int m2NextPos = 0;
int m1PosOffset = 0;
int m2PosOffset = 0;
int m1PosComp = 0;
int m2PosComp = 0;
int target = 0;
int targetOpen = initTargetOpen;
int targetClosed = initTargetClose;
volatile int m1Pos = 0;
volatile int m2Pos = 0;
int lastM1Pos = 0;
int lastM2Pos = 0;

// Speed Variables

float m1Speed = 0; // in rev/s
float m2Speed = 0; // in rev/s

// Power Variables
float powerScalar = 3;
float powerEasing = 1.0;
int targetWindow = 5; // +/- window for movement cutoff
int powerLimit = 500; // +/- maximum power sent to motors
float powerCutoff = 75; // +/- window for power cutoff
float speedCutoff = 0.01; // +/- window for speed cutoff
float m1Power = 0;
float m2Power = 0;

// EZO-PMP variables
float startupFlow = 50;                                       // initial ml/min rate of fluid flow
float runFlow = 8;
String inputstring = "";                              //a string to hold incoming data from the PC
String devicestring = "";                             //a string to hold the data from the Atlas Scientific product
boolean device_string_complete = false;               //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float ml;                                             //used to hold a floating point number that is the volume

unsigned long ti = 0; // initial time when movement was started
unsigned long dT = 1; // the amount of time the movement should take
unsigned long tf = 0; // the time relative to initial time that the movement should complete
unsigned long tc = 0; // the current time
float tp = float(tc - ti) / float(dT); // the amount of progress %
float sigmoid1 = 0;
float sigmoid2 = 0;
float sigmoid3 = 0;
// fun smoothing functions;
//sigmoid1 = 1 / ( 1 + pow(M_E, -(12 * tp - 6))); // natural log 
//sigmoid2 = 0.5 * tanh((2 * PI * tp) - PI) + 0.5; // hyperbolic tan
//sigmoid3 = pow(sin(0.5 * PI * tp), 2); // sine squared

//////////////////////////////////////////////////////////////////
// ISRs
//
// encoder states are captured on interrupt pins
// these functions determine how they're treated
// read the value of the interrupt pins directly from PORTD registers
// if high compare to value of m1Enc2
// if m1Enc2 is high, set m1Dir = false, else true
// if m1Dir is true, m1Pos++, else m1Pos--

// motor 1 encoder pin 1
void m1Enc1() {
  if (digitalRead(M1_ENC1_PIN) != digitalRead(M1_ENC2_PIN)) {
    m1Pos--;
  } else {
    m1Pos++;
  }
}

// motor 2 encoder pin 1
void m2Enc1() {
  if (digitalRead(M2_ENC1_PIN) != digitalRead(M2_ENC2_PIN)) {
    m2Pos++;
  } else {
    m2Pos--;
  }
}

void calcMotorSpeeds(){
  m1Speed = stepAngle*(m1Pos - lastM1Pos)*10;
  lastM1Pos = m1Pos;
  m2Speed = stepAngle*(m2Pos - lastM2Pos)*10;
  lastM2Pos = m2Pos;
}

//////////////////////////////////////////////////////////////////
// initButtons()
//

void initButtons() {
  // 4x4 MATRIX SETUP

  pinMode(BUTTON_COL_0, INPUT_PULLUP);
  pinMode(BUTTON_COL_1, INPUT_PULLUP);
  pinMode(BUTTON_COL_2, INPUT_PULLUP);
  pinMode(BUTTON_COL_3, INPUT_PULLUP);
  pinMode(BUTTON_ROW_0, OUTPUT);
  pinMode(BUTTON_ROW_1, OUTPUT);
  pinMode(BUTTON_ROW_2, OUTPUT);
  pinMode(BUTTON_ROW_3, OUTPUT);

}

//////////////////////////////////////////////////////////////////
// readButtons()
//

int readButtons() {
  int button = -1;
  for (int row = 0 ; row < buttonRows ; row++) {

    // set the row pin low
    digitalWrite(BUTTON_ROW_0, row != 0);
    digitalWrite(BUTTON_ROW_1, row != 1);
    digitalWrite(BUTTON_ROW_2, row != 2);
    digitalWrite(BUTTON_ROW_3, row != 3);

    for (int col = 0 ; col < buttonCols ; col++) {
      buttonStates[col][row] = !boolean(digitalRead(BUTTON_COL_0 + col));

      // if the button was off but now is on, set button to the number corresponding to it
      if (buttonStates[col][row] && !lastButtonStates[col][row]) {
        button =  col + row * buttonRows;
      }

      // store the current state of the buttons for comparision next time around
      lastButtonStates[col][row] = buttonStates[col][row];

    }
  }
  return button;
}

//////////////////////////////////////////////////////////////////
// executeButtonAction()
//

void executeButtonAction(int _button) {

  if (machineState == CALIBRATE) { // if in calibration mode, buttons have these functions
    switch (_button) {
      case 0: // BUTTON 01 - decrease position by ~1 degree
        m1PosOffset -= 11;
        break;

      case 1:  // BUTTON 02 - increase position by ~1 degree
        m1PosOffset += 11;
        break;

      case 2: // BUTTON 03 - decrease position by ~10 degrees
        m2PosOffset -= 11;
        break;

      case 3: // BUTTON 04 - increase position by ~10 degrees
        m2PosOffset += 11;
        break;

      case 4: // BUTTON 05 - set open position
        targetOpen = target;
        break;

      case 5: // BUTTON 06 - reset open position
        targetOpen = initTargetOpen;
        break;

      case 6: // set close
        targetClosed = target;
        break;

      case 7: // reset close
        targetClosed = initTargetClose;
        break;

      case 8: // increase speed
        //target = targetOpen;
        break;

      case 9: // decrease speed
        //target = targetClosed;
        break;

      case 10: // reset speed
        target -= 110;
        break;

      case 11: // pause
        target += 110;
        break;

      case 12: // pump rate = startupFlow
        //        setFlowRate(startupFlow);
        break;

      case 13: // pump rate = runFlow
        //        setFlowRate(runFlow);
        break;

      case 14: // pause / resume pump
        //        pausePump();
        break;

      case 15: //
        machineState = lastMachineState;
        break;
    }
  } else { // otherwise the machine is in run mode and has these functions
    switch (_button) {
      case 0: // BUTTON 01 - set state to Open
        currentTime = millis();
        ti = currentTime;
        dT = 5000;
        tf = ti + dT;
        tp = 0;
        machineState = OPEN;
        clearPositionFlags();
        break;

      case 1:  // BUTTON 02 - set state to Close
        currentTime = millis();
        ti = currentTime;
        dT = 5000;
        tf = ti + dT;
        tp = 0;
        machineState = CLOSE;
        clearPositionFlags();
        break;

      case 2: // BUTTON 03 - set state to Startup
        machineState = STARTUP;
        //        setFlowRate(startupFlow);
        clearPositionFlags();
        break;

      case 3: // BUTTON 04 - enter calibration mode
        lastMachineState = machineState;
        machineState = CALIBRATE;
        clearPositionFlags();
        break;
    }
  }
}

//////////////////////////////////////////////////////////////////
// printButtonStates()
//
// not used. just here for diagnostics

void printButtonStates() {
  Serial.print("Button States: \n");
  for (int row = 0 ; row < buttonRows ; row++) {
    for (int col = 0 ; col < buttonCols ; col++) {
      Serial.print("Col: ");
      Serial.print(col);
      Serial.print(", Row: ");
      Serial.print(row);
      Serial.print(", State = ");
      Serial.print(buttonStates[col][row]);
      Serial.print("\n");
    }
  }
}
//////////////////////////////////////////////////////////////////
// MOTION CONTROL FUNCTIONS
//
//////////////////////////////////////////////////////////////////
//  initMotionControl()

void initMotionControl() {
  // Establish Serial Communication with Arduino

  Serial.begin(115200);
  Serial.print("\nMembrane Sculpture\nBy Tauba Auerbach\nCode by Phillip David Stearns\n");
  Serial.println("Initializing...");

  // TimerOne Setup
  // for regular sampling of speed
  Timer1.initialize(1e5); // period in micro seconds (1e5 = 100ms)
  Timer1.attachInterrupt(calcMotorSpeeds);

  // DUAL G2 SETUP

  // Interrupt Pins
  pinMode(M1_ENC1_PIN, INPUT_PULLUP);
  pinMode(M1_ENC2_PIN, INPUT_PULLUP);
  pinMode(M2_ENC1_PIN, INPUT_PULLUP);
  pinMode(M2_ENC2_PIN, INPUT_PULLUP);

  // Attaching Interrupt Pins to ISR functions to counter encoder changes
  attachInterrupt(digitalPinToInterrupt(M1_ENC1_PIN), m1Enc1, CHANGE); // connect encoder to pin 18
  attachInterrupt(digitalPinToInterrupt(M2_ENC1_PIN), m2Enc1, CHANGE); // connect encoder to pin 20

  Serial.println("Dual G2 High Power Motor Shield");
  md.init();
  md.calibrateCurrentOffsets();
  delay(1000);

  // Uncomment to flip a motor's direction:
  //md.flipM1(true);
  md.flipM2(true);
}

//////////////////////////////////////////////////////////////////
//  moveToTarget()

void moveToTarget(int _target) {

  // apply motor position offsets
  m1PosComp = m1Pos + m1PosOffset;
  m2PosComp = m2Pos + m2PosOffset;

  // compute the distance between the current motor positions and their targets
  float m1Distance = _target - m1PosComp;
  float m2Distance = _target - m2PosComp;

  m1Power += ease(m1Power, powerScalar * (_target - m1PosComp), powerEasing);
  //  m1Power = powerScalar * (_target - m1PosComp);
  m1Power = constrain(m1Power, -powerLimit, powerLimit);
//  if(isClosed || isOpen){
  if ((abs(m1Power) < powerCutoff) && (abs(m1Speed) < speedCutoff) ) {
    m1Power = 0;
  }
  md.setM1Speed(m1Power);
  stopM1OnFault();

  m2Power += ease(m2Power, powerScalar * (_target - m2PosComp), powerEasing);
  //  m2Power = powerScalar * (_target - m2PosComp);
  m2Power = constrain(m2Power, -powerLimit, powerLimit);
//  if(isClosed || isOpen){
  if ((abs(m2Power) < powerCutoff) && (abs(m2Speed) < speedCutoff) ) {
    m2Power = 0;
  }
  md.setM2Speed(m2Power);
  stopM2OnFault();

}

//////////////////////////////////////////////////////////////////
// ease()

float ease(float _val, float _target, float _ease) {
  return _ease * (_target - _val);
}

//////////////////////////////////////////////////////////////////
// stopIfFault()
//
// Check if there's something wrong. If so, stop running!

void stopIfFault() {
  stopM1OnFault();
  stopM2OnFault();
}

void stopM1OnFault() {
  if (md.getM1Fault()) {
    md.disableDrivers();
    delay(1);
    Serial.print("M1 fault. Time:");
    Serial.println(millis());
    while (1);
  }
}

void stopM2OnFault() {
  if (md.getM2Fault()) {
    md.disableDrivers();
    delay(1);
    Serial.print("M2 fault. Time:");
    Serial.println(millis());
    while (1);
  }
}

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

void verboseOutput() {
  Serial.print("Machine State: ");
  Serial.print(machineState);
  Serial.print("\tCurrent Target: ");
  Serial.print(target);
  Serial.print("\tTargetOpen: ");
  Serial.print(targetOpen);
  Serial.print("\tTargetClose: ");
  Serial.print(targetClosed);
  Serial.print("\tMotor 1 Power: ");
  Serial.print(m1Power);
  Serial.print("\tMotor 1 Speed: ");
  Serial.print(m1Speed);
  Serial.print("\tMotor 1 Position: ");
  Serial.print(m1PosComp);
  Serial.print("\tMotor 2 Power: ");
  Serial.print(m2Power);
  Serial.print("\tMotor 2 Speed: ");
  Serial.print(m2Speed);
  Serial.print("\tMotor 2 Position: ");
  Serial.print(m2PosComp);
  Serial.println();
}
//// KEYWORDS FOR STATE MACHINE
//
//#define CALIBRATE   0
//#define OPEN        1
//#define HOLD_OPEN   2
//#define CLOSE       3
//#define HOLD_CLOSE  4
//#define STARTUP     5
//
//boolean isOpen;
//boolean isClosed;

void stateMachine() {

  currentTime = millis();

  switch (machineState) {

    case CALIBRATE: // calibration
      break;

    case OPEN: // open
      if (currentTime >= tf) {
        isOpen = true;
      }
      if (isOpen) {
        machineState = HOLD_OPEN;
      } else {
        tp = float(currentTime - ti) / float(dT);
        target = int(pow(sin(0.5 * PI * tp), 2) * float(targetOpen - targetClosed));
        holdOpenStart = currentTime;
        clearPositionFlags();
      }
      break;

    case HOLD_OPEN: // holding open
      if ( (unsigned long) (currentTime - holdOpenStart) > holdOpenEnd) {
        
        ti = currentTime;
        dT = closeDuration;
        tf = ti + dT;
        tp = 0;
        
        machineState = CLOSE;
        clearPositionFlags();
      } else {
        target = targetOpen;
      }
      break;

    case CLOSE: // close
      if (currentTime >= tf) {
        isClosed = true;
      }
      if (isClosed) {
        machineState = HOLD_CLOSED;
      } else {
        
        tp = float(currentTime - ti) / float(dT);
        target = int((1-pow(sin(0.5 * PI * tp), 2)) * float(targetOpen - targetClosed));
        holdClosedStart = currentTime;
        clearPositionFlags();
      }
      break;

    case HOLD_CLOSED: // holding closed
      if ( (unsigned long) (currentTime - holdClosedStart) > holdClosedEnd) {

        ti = currentTime;
        dT = openDuration;
        tf = ti + dT;
        tp = 0;

        machineState = OPEN;
        clearPositionFlags();
      } else {
        target = targetClosed;
      }
      break;

    case 5: // startup

      if ( (unsigned long) (currentTime - startupStart) > startupEnd) {
        //        setFlowRate(runFlow);

        ti = currentTime;
        dT = openDuration;
        tf = ti + dT;

        machineState = OPEN;
        clearPositionFlags();
      }

      break;
  }
}

void clearPositionFlags() {
  isOpen = false;
  isClosed = false;
}

//////////////////////////////////////////////////////////////////
// setup()

void setup() {

  initButtons();

  initMotionControl();

  // initPump();

  startupStart = millis();
}

//////////////////////////////////////////////////////////////////
// loop()

void loop() {

  currentTime = millis();

  //  pumpCommunication();

  if (machineState != CALIBRATE) stateMachine();

  if ( (currentTime - lastButtonScan) >= buttonInterval) {
    executeButtonAction(readButtons());
    lastButtonScan = currentTime;
  }

  if (moveMotors /*&& ( (currentTime - lastMove) >= moveInterval)*/) {
    moveToTarget(target);
    //    lastMove = currentTime;
  }

  if (debug && ( (currentTime - lastTime) >= statusInterval)) {
    verboseOutput();
    lastTime = currentTime;
  }

}
