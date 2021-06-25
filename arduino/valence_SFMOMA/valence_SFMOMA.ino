

/** Motor Speed Calibrator For Tauba Auerbach by Phillip David Stearns
   calibration routine to set motor speed
*/

// Libraries
#include <Arduino_JSON.h>
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
unsigned long startupEnd = 5000; // duration to hold open in ms (was 35000)

// Interrupt Pins for Gear Head Motors

// Interrupt pin numbers
#define M1_ENC1_IRQ_PIN 5 //18
#define M1_ENC2_IRQ_PIN 4 //19
#define M2_ENC1_IRQ_PIN 3 //20
#define M2_ENC2_IRQ_PIN 2 //21

// Physical pin numbers
#define M1_ENC1_PIN 18
#define M1_ENC2_PIN 19
#define M2_ENC1_PIN 20
#define M2_ENC2_PIN 21

// control variables
float stepAngle = 1.0 / 4200.0;
boolean moveMotors = true;

// variables for timing
unsigned long currentTime = 0;

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
float powerScalar = 4;
float powerEasing = .85;
int targetWindow = 5; // +/- window for movement cutoff
int powerLimit = 480; // +/- maximum power sent to motors
float powerCutoff = 50; // +/- window for power cutoff
float speedCutoff = 0.02; // +/- window for speed cutoff
float m1Power = 0;
float m2Power = 0;

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

void calcMotorSpeeds() {
  m1Speed = stepAngle * (m1Pos - lastM1Pos) * 10;
  lastM1Pos = m1Pos;
  m2Speed = stepAngle * (m2Pos - lastM2Pos) * 10;
  lastM2Pos = m2Pos;
}
//////////////////////////////////////////////////////////////////
// clearPositionFlags()
//
void clearPositionFlags() {
  isOpen = false;
  isClosed = false;
}

//////////////////////////////////////////////////////////////////
// MOTION CONTROL FUNCTIONS
//
//////////////////////////////////////////////////////////////////
// stopIfFault()
//
// Check if there's something wrong. If so, stop running!

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

//////////////////////////////////////////////////////////////////
// ease()

float ease(float _val, float _target, float _ease) {
  return _ease * (_target - _val);
}

//////////////////////////////////////////////////////////////////
//  moveToTarget()

void moveToTarget(int _target) {

  // apply motor position offsets
  m1PosComp = m1Pos + m1PosOffset;
  // compute the distance between the current motor positions and their targets
  float m1Distance = _target - m1PosComp;
  float m1force = -powerScalar * m1Distance;
  m1Power += ease(m1Power, m1force, powerEasing);
  m1Power = constrain(m1Power, -powerLimit, powerLimit);
  if (isClosed || isOpen) {
    if ((abs(m1Power) < powerCutoff) && (abs(m1Speed) < speedCutoff) ) {
      m1Power = 0;
    }
  }
  md.setM1Speed(m1Power);
  stopM1OnFault();

  m2PosComp = m2Pos + m2PosOffset;
  float m2Distance = _target - m2PosComp;
  float m2Force  = -powerScalar * m2Distance;
  m2Power += ease(m2Power, m2Force, powerEasing);
  m2Power = constrain(m2Power, -powerLimit, powerLimit);
  if (isClosed || isOpen) {
    if ((abs(m2Power) < powerCutoff) && (abs(m2Speed) < speedCutoff) ) {
      m2Power = 0;
    }
  }
  md.setM2Speed(m2Power);
  stopM2OnFault();

}

void stopIfFault() {
  stopM1OnFault();
  stopM2OnFault();
}

//////////////////////////////////////////////////////////////////
// stateMachine()

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
        target = int((1 - pow(sin(0.5 * PI * tp), 2)) * float(targetOpen - targetClosed));
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
        ti = currentTime;
        dT = openDuration;
        tf = ti + dT;
        machineState = OPEN;
        clearPositionFlags();
      }
      break;
  }
}
//////////////////////////////////////////////////////////////////
// setup()

void setup() {
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

  Serial.println("Dual G2 High Power Motor Shield");
  md.init();
  md.calibrateCurrentOffsets();
  delay(1000);

  // Attaching Interrupt Pins to ISR functions to counter encoder changes
  attachInterrupt(M1_ENC1_IRQ_PIN, m1Enc1, CHANGE); // connect encoder to pin 18
  attachInterrupt(M2_ENC1_IRQ_PIN, m2Enc1, CHANGE); // connect encoder to pin 20

  // Uncomment to flip a motor's direction:
  md.flipM1(true);
  //md.flipM2(true);

  startupStart = millis();
}

//////////////////////////////////////////////////////////////////
// loop()

void loop() {
  currentTime = millis();
  if (machineState != CALIBRATE) stateMachine();
  if (moveMotors) {
    moveToTarget(target);
  }
}
