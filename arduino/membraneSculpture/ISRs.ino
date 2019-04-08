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
