//////////////////////////////////////////////////////////////////
// MOTION CONTROL FUNCTIONS
//
//
//////////////////////////////////////////////////////////////////
//  moveToTarget()

void moveToTarget(int _target) {

  m1PosComp = m1Pos + m1PosOffset;
  m2PosComp = m2Pos + m2PosOffset;

  float m1Distance = _target - m1PosComp;
  float m2Distance = _target - m2PosComp;

  if (m1Distance > targetWindow ) {
    if (m1PosComp < _target) {
      m1NextPos += posInc;
    }
  } else if (m1Distance < -targetWindow) {
    if (m1PosComp > _target) {
      m1NextPos -= posInc;
    }

    if (machineState == OPEN) {
      isOpen = false;
    } else if (machineState == CLOSE) {
      isClosed = false;
    }
  } else {
    m1Power = 0;
    if (machineState == OPEN) {
      isOpen = true;
    } else if (machineState == CLOSE) {
      isClosed = true;
    }
  }

  if (m2Distance > targetWindow) {
    if (m2PosComp < _target) {
      m2NextPos += posInc;
    }
  } else if ( m2Distance < -targetWindow) {

    if (m2PosComp > _target) {
      m2NextPos -= posInc;
    }

    if (machineState == OPEN) {
      isOpen = false;
    } else if (machineState == CLOSE) {
      isClosed = false;
    }
    
  } else {
    m2Power = 0;
    if (machineState == OPEN) {
      isOpen = true;
    } else if (machineState == CLOSE) {
      isClosed = true;
    }
  }

  m1Power = ease(m1Power, constrain(powerScalar * (m1NextPos - m1PosComp), -200, 200), powerEasing);
  //m1Power *= powerDampening;
  if ((abs(m1Power) < powerCutoff) && (abs(m1Speed) < speedCutoff) ) {
    m1Power = 0;
  }
  md.setM1Speed(m1Power);
  stopM1OnFault();

  m2Power = ease(m2Power, constrain(powerScalar * (m2NextPos - m2PosComp), -200, 200), powerEasing);
  //m2Power *= powerDampening;
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
