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
