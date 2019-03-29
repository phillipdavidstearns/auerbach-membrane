#include <TimerOne.h>
#include <DualG2HighPowerMotorShield.h>

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

unsigned long lastTime = 0;
unsigned long currentTime = 0;
unsigned long refreshTime = 33; // ~30 fps refresh rate

const int qtyEncoderPins = 4;
int encoderPins[qtyEncoderPins] = { M1_ENC1_PIN, M1_ENC2_PIN, M2_ENC1_PIN, M2_ENC2_PIN };
volatile boolean encoderStates[qtyEncoderPins] = { false, false, false, false };
volatile boolean lastEncoderStates[qtyEncoderPins] = { false, false, false, false };

const int qtyButtons = 4;
int buttonPins[qtyButtons] = { M1_INC_BUTTON, M1_DEC_BUTTON, M2_INC_BUTTON, M2_DEC_BUTTON };
boolean buttonStates[qtyButtons] = { false, false, false, false };

int m1Speed = 0;
int m2Speed = 0;
int m1Position = 0;
int m2Position = 0;

void scanEncoderPins() {
  for (int i = 0 ; i < qtyEncoderPins ; i++) {
    boolean pinState = digitalRead(encoderPins[i]);
    if (encoderStates[i] != pinState) {
      encoderStates[i] = pinState;
    }
  }
}

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

void setup() {

  pinMode(M1_ENC1_PIN, INPUT);
  pinMode(M1_ENC2_PIN, INPUT);
  pinMode(M2_ENC1_PIN, INPUT);
  pinMode(M2_ENC2_PIN, INPUT);

  Serial.begin(115200);
  delay(1);
  Serial.println("Dual G2 High Power Motor Shield");
  md.init();
  md.calibrateCurrentOffsets();
  delay(10);
  // Uncomment to flip a motor's direction:
  //md.flipM1(true);
  md.flipM2(true);
}

void loop() {
  currentTime = millis();
  if (currentTime - lastTime >= refreshTime) {
    // check button states
    // update motor speeds
    lastTime = currentTime;
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
// checkButtons()
//
// should act as a kind of "debounce" for using the analog pins an button inputs

void checkButtons() {

  for ( int i = 0 ; i < qtyButtons ; i++ ) {
    if (analogRead(buttonPins[i]) >= 512 && !buttonStates[i]) {
      buttonStates[i] = true;
    } else if (analogRead(buttonPins[i]) < 512 && buttonStates[i] ) {
      buttonStates[i] = false;
    }
  }

}

//////////////////////////////////////////////////////////////////
// updateMotorSpeeds()
//
//

void updateMotorSpeeds() {

  if (buttonStates[0] && !buttonStates[1]) {
    m1Speed--;
  } else if (!buttonStates[0] && buttonStates[1]) {
    m1Speed++;
  }

  if (buttonStates[2] && !buttonStates[3]) {
    m2Speed--;
  } else if (!buttonStates[2] && buttonStates[3]) {
    m2Speed++;
  }

  md.setM1Speed(m1Speed);
  stopM1OnFault();
  md.setM2Speed(m2Speed);
  stopM2OnFault();

}
