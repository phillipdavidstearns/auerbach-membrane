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
