#include "DualG2HighPowerMotorShield.h"

DualG2HighPowerMotorShield18v22 md;


// IO pins for Motor 1
#define M1_ENC1_PIN 3
#define M1_ENC2_PIN 5

// IO pins for Motor 2
#define M2_ENC1_PIN 11
#define M2_ENC2_PIN 13

// IO pin for buttons
#define INC_BUTTON A0
#define DEC_BUTTON A1

unsigned long lastTime = 0;
unsigned long currentTime = 0;

int qtyButtons = 2;
int buttonPins[qtyButtons] = { INC_BUTTON, DEC_BUTTON };
boolean buttonStates[qtyButtons];
boolean lastButtonStates[qtybuttons];

void checkButtons() {

  for ( int i = 0 ; i < qtyButtons ; i++ ) {
    if (!lastButtonStates[i] && analogRead() > 512) {
    } else {
      
    }
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

void loop()
{
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.

  for (int i = 0; i <= 400; i++)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i % 200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 400; i >= -400; i--)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i % 200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = -400; i <= 0; i++)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i % 200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 0; i <= 400; i++)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i % 200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 400; i >= -400; i--)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i % 200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = -400; i <= 0; i++)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i % 200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  md.disableDrivers(); // Put the MOSFET drivers into sleep mode.
  delay(500);

}
//////////////////////////////////////////////////////////////////
// stopIfFault()
//
// Check if there's something wrong. If so, stop running!

void stopIfFault() {

  if (md.getM1Fault()) {
    md.disableDrivers();
    delay(1);
    Serial.println("M1 fault");
    while (1);
  }

  if (md.getM2Fault()) {
    md.disableDrivers();
    delay(1);
    Serial.println("M2 fault");
    while (1);
  }

}
