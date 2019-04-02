/** Motor Speed Calibrator For Tauba Auerbach by Phillip David Stearns
   calibration routine to set motor speed
*/

// Libraries
#include <TimerOne.h> // including the TimerOne library for highspeed scanning of encoder pins
#include <DualG2HighPowerMotorShield.h>

// DualG2 Board Object
DualG2HighPowerMotorShield18v22 md;

// IO pins for Motor 1
#define M1_ENC1_PIN 3
#define M1_ENC2_PIN 5

// IO pins for Motor 2
#define M2_ENC1_PIN 11
#define M2_ENC2_PIN 13

// IO pin for buttons
#define M1_INC_BUTTON A0
#define M1_DEC_BUTTON A1
#define M2_INC_BUTTON A2
#define M2_DEC_BUTTON A3
#define P1_INC_BUTTON A4
#define P1_DEC_BUTTON A5

// Button Matrix Test Code
// Used for quickly setting up a button interface with
// 4x4 button matrix TS-792 from Tinkersphere.com

// pin definitions for button columns
#define BUTTON_COL_0 22
#define BUTTON_COL_1 23
#define BUTTON_COL_2 24
#define BUTTON_COL_3 25

// pin difinitions for button rows
#define BUTTON_ROW_0 26
#define BUTTON_ROW_1 27
#define BUTTON_ROW_2 28
#define BUTTON_ROW_3 29

boolean debug = true;

const int buttonRows = 4;
const int buttonCols = 4;
boolean buttonStates[buttonRows][buttonCols];
boolean lastButtonStates[buttonRows][buttonCols];

// variables for timing of the scan vs display of status
unsigned long buttonScanInterval = 33;
unsigned long printButtonInterval = 1000;
unsigned long lastButtonScan = 0;
unsigned long lastButtonPrint = 0;

// refresh varaibles
unsigned long frames = 0;
unsigned long seconds = 0;
unsigned long lastTime = 0;
unsigned long refreshTime = 1000;

const int qtyEncoderPins = 4;
int encoderPins[qtyEncoderPins] = { M1_ENC1_PIN, M1_ENC2_PIN, M2_ENC1_PIN, M2_ENC2_PIN };
volatile boolean encoderStates[qtyEncoderPins] = { false, false, false, false };
volatile boolean lastEncoderStates[qtyEncoderPins] = { false, false, false, false };
volatile boolean m1Direction = false;
volatile boolean m2Direction = false;
volatile int m1Position = 0;
volatile int m2Position = 0;

const int buttonThreshold = 512;
const int qtyButtons = 6;
int buttonPins[qtyButtons] = { M1_INC_BUTTON, M1_DEC_BUTTON, M2_INC_BUTTON, M2_DEC_BUTTON, P1_INC_BUTTON, P1_DEC_BUTTON, };
unsigned long lastButtonTime[qtyButtons];
unsigned long debounce = 125; // 125 ms debounce time

int m1Speed = 0;
int m2Speed = 0;

// EZO-PMP variables
String inputstring = "";                              //a string to hold incoming data from the PC
String devicestring = "";                             //a string to hold the data from the Atlas Scientific product
boolean device_string_complete = false;               //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float ml;                                             //used to hold a floating point number that is the volume
float rate = 5.5;

//////////////////////////////////////////////////////////////////
//

void scanEncoderPins() {
  for (int i = 0 ; i < qtyEncoderPins ; i++) {
    volatile boolean pinState = digitalRead(encoderPins[i]);
    if (encoderStates[i] != pinState) {
      encoderStates[i] = pinState;
      switch (i) {
        case 0:
          if (m1Speed < 0) {
            m1Position--;
          } else {
            m1Position++;
          }
          break;
        case 1:
          if (m2Speed < 0) {
            m2Position--;
          } else {
            m2Position++;
          }
          break;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////
//

void checkEncoderPinChanges() {

  if (encoderStates[0] && !lastEncoderStates[0]) {
  }
  if (encoderStates[1] && !lastEncoderStates[1]) {
  }
  if (encoderStates[2] && !lastEncoderStates[2]) {
  }
  if (encoderStates[3] && !lastEncoderStates[3]) {
  }

  for (int i = 0 ; i < qtyEncoderPins ; i++) {
    lastEncoderStates[i] = encoderStates[i];
  }
}

//////////////////////////////////////////////////////////////////
//


void callback() {
  scanEncoderPins();
  checkEncoderPinChanges();
}

//////////////////////////////////////////////////////////////////
//

void setup() {

  // 4x4 MATRIX SETUP

  pinMode(BUTTON_COL_0, INPUT_PULLUP);
  pinMode(BUTTON_COL_1, INPUT_PULLUP);
  pinMode(BUTTON_COL_2, INPUT_PULLUP);
  pinMode(BUTTON_COL_3, INPUT_PULLUP);
  pinMode(BUTTON_ROW_0, OUTPUT);
  pinMode(BUTTON_ROW_1, OUTPUT);
  pinMode(BUTTON_ROW_2, OUTPUT);
  pinMode(BUTTON_ROW_3, OUTPUT);


  //  Timer1.initialize(1e6 / 30); // period in micro seconds (1x10^6 / frames per second)
  //  Timer1.attachInterrupt(callback);

  // DUAL G2 SETUP

  //  pinMode(M1_ENC1_PIN, INPUT);
  //  pinMode(M1_ENC2_PIN, INPUT);
  //  pinMode(M2_ENC1_PIN, INPUT);
  //  pinMode(M2_ENC2_PIN, INPUT);

  Serial.begin(115200);
  delay(1);
  Serial.println("Dual G2 High Power Motor Shield");
  md.init();
  md.calibrateCurrentOffsets();
  delay(10);
  // Uncomment to flip a motor's direction:
  //md.flipM1(true);
  md.flipM2(true);


  // EZO-PMP SETUP

  Serial3.begin(115200);                                //set baud rate for software serial port_3 to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  devicestring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  delay(1);

  Serial.print("Powering up! Checking status\n");
  Serial3.print("status\r");
  delay(1);

  Serial.print("Clearing volume pumped.\n");
  Serial3.print("clear\r");
  delay(1);

  Serial.print("Continuous Mode without Reporting.\n");
  Serial3.print("c,0\r");
  delay(1);

  Serial.print("Setting pump rate to ");
  Serial.print(rate);
  Serial.print(" ml/min \n");
  Serial3.print("dc,");
  Serial3.print(rate);
  Serial3.print(",*\r");
  delay(1);

}

//////////////////////////////////////////////////////////////////
//

void loop() {

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


  readButtons(); // currently has the updateMotors and checkButtonChanges functions inside!!!
  
   unsigned long currentTime = millis();
  if (currentTime - lastTime >= refreshTime) {
    Serial.print("Motor 1 Speed: ");
    Serial.println(m1Speed);
    Serial.print("Motor 1 Position: ");
    Serial.println(m1Position);
    Serial.print("Motor 2 Speed: ");
    Serial.println(m2Speed);
    Serial.print("Motor 2 Position: ");
    Serial.println(m2Position);
    lastTime=currentTime;
  }


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
    Serial.println("M1 fault");
    while (1);
  }
}

void stopM2OnFault() {
  if (md.getM2Fault()) {
    md.disableDrivers();
    delay(1);
    Serial.println("M2 fault");
    while (1);
  }

}


//////////////////////////////////////////////////////////////////
// updateMotorSpeeds()
//
//

void updateMotorSpeeds(int _button) {

  switch (_button) {
    case 0:
      m1Speed--;
      break;
    case 1:
      m1Speed++;
      break;
    case 2:
      m2Speed--;
      break;
    case 3:
      m2Speed++;
      break;
  }

  md.setM1Speed(m1Speed);
  stopM1OnFault();
  md.setM2Speed(m2Speed);
  stopM2OnFault();

}
