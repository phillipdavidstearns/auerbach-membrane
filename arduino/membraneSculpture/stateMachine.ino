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

  unsigned long currentTime = millis();

  switch (machineState) {

    case CALIBRATE: // calibration
      break;

    case OPEN: // open
      if (isOpen) {
        machineState = HOLD_OPEN;
      } else {
        target = targetOpen;
        holdOpenStart = currentTime;
        clearPositionFlags();
      }
      break;

    case HOLD_OPEN: // holding open
      if (currentTime - holdOpenStart > holdOpenEnd) {
        target = targetClosed;
        machineState = CLOSE;
        clearPositionFlags();
      } else {
        target = targetOpen;
      }
      break;

    case CLOSE: // close
      if (isClosed) {
        machineState = HOLD_CLOSED;
      } else {
        target = targetClosed;
        holdClosedStart = currentTime;
        clearPositionFlags();
      }
      break;

    case HOLD_CLOSED: // holding closed
      if (currentTime - holdClosedStart > holdClosedEnd) {
        target = targetOpen;
        machineState = OPEN;
        clearPositionFlags();
      } else {
        target = targetClosed;
      }
      break;

    case 5: // startup

        if (currentTime - startupStart > startupEnd) {
          setFlowRate(runFlow);
          machineState = OPEN;
          clearPositionFlags();
        }
       
      break;
  }
}
