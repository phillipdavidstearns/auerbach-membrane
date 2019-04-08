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

unsigned long holdOpenStart = 0;
unsigned long holdOpenEnd = 15000; // duration to hold open in ms

unsigned long holdClosedStart = 0;
unsigned long holdClosedEnd = 15000; // duration to hold open in ms

unsigned long startupStart = 0;
unsigned long startupEnd = 30000; // duration to hold open in ms

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
unsigned long buttonInterval = 100; // ms between button polls
unsigned long lastButtonScan = 0;

unsigned long lastTime = 0;
unsigned long statusInterval = 1000; // ms between status update

unsigned long lastMove = 0;
unsigned long moveInterval = 10; // ms between position updates

// Position Variables

const int initTargetOpen = 3200;
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
int targetWindow = 25; // +/- window for movement cutoff
float powerCutoff = 50; // +/- window for power cutoff
float speedCutoff = 0.01; // +/- window for speed cutoff
float powerDampening = .9;
float m1Power = 0;
float m2Power = 0;
float m1PowerBoost = 0.125;
float m2PowerBoost = 0.125;

// EZO-PMP variables
float startupFlow = 50;                                       // initial ml/min rate of fluid flow
float runFlow = 8; 
String inputstring = "";                              //a string to hold incoming data from the PC
String devicestring = "";                             //a string to hold the data from the Atlas Scientific product
boolean device_string_complete = false;               //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float ml;                                             //used to hold a floating point number that is the volume


//////////////////////////////////////////////////////////////////
// setup()

void setup() {

  // TimerOne Setup
  // for regular sampling of speed
  Timer1.initialize(1e5); // period in micro seconds (1e5 = 100ms)
  Timer1.attachInterrupt(calcMotorSpeeds);

  // Establish Serial Communication with Arduino

  Serial.begin(115200);
  Serial.print("\nMembrane Sculpture\nBy Tauba Auerbach\nCode by Phillip David Stearns\n");
  Serial.println("Initializing...");

  // 4x4 MATRIX SETUP

  pinMode(BUTTON_COL_0, INPUT_PULLUP);
  pinMode(BUTTON_COL_1, INPUT_PULLUP);
  pinMode(BUTTON_COL_2, INPUT_PULLUP);
  pinMode(BUTTON_COL_3, INPUT_PULLUP);
  pinMode(BUTTON_ROW_0, OUTPUT);
  pinMode(BUTTON_ROW_1, OUTPUT);
  pinMode(BUTTON_ROW_2, OUTPUT);
  pinMode(BUTTON_ROW_3, OUTPUT);

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
  // md.flipM1(true);
  md.flipM2(true);

  // EZO-PMP SETUP

  Serial3.begin(115200);                              //set baud rate for software serial port_3
  inputstring.reserve(32);                            //set aside some bytes for receiving data from the PC
  devicestring.reserve(64);                           //set aside some bytes for receiving data from Atlas Scientific product
  delay(100);

  Serial.print("EZO-PMP\n");
  Serial3.print("status\r");
  delay(100);

  Serial.print("Clearing volume pumped.\n");
  Serial3.print("clear\r");
  delay(100);

  Serial.print("Continuous Mode without Reporting.\n");
  Serial3.print("c,0\r");
  delay(100);

//  Serial.print("Setting pump rate to ");
//  Serial.print(startupFlow);
//  Serial.print(" ml/min \n");
//  setFlowRate(startupFlow);
//  delay(100);
  
  startupStart = millis();
}

//////////////////////////////////////////////////////////////////
// loop()

void loop() {

  unsigned long currentTime = millis();

  pumpCommunication();

  if (machineState != CALIBRATE) stateMachine();

  if (currentTime - lastButtonScan >= buttonInterval) {
    executeButtonAction(readButtons());
    lastButtonScan = currentTime;
  }

  if (moveMotors && (currentTime - lastMove >= moveInterval)) {
    moveToTarget(target);
    lastMove = currentTime;
  }

  if (debug && (currentTime - lastTime >= statusInterval)) {
    verboseOutput();
    lastTime = currentTime;
  }

}
