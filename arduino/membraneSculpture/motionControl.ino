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
