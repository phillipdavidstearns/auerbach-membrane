//////////////////////////////////////////////////////////////////
// initButtons()
//

void initButtons() {
  // 4x4 MATRIX SETUP

  pinMode(BUTTON_COL_0, INPUT_PULLUP);
  pinMode(BUTTON_COL_1, INPUT_PULLUP);
  pinMode(BUTTON_COL_2, INPUT_PULLUP);
  pinMode(BUTTON_COL_3, INPUT_PULLUP);
  pinMode(BUTTON_ROW_0, OUTPUT);
  pinMode(BUTTON_ROW_1, OUTPUT);
  pinMode(BUTTON_ROW_2, OUTPUT);
  pinMode(BUTTON_ROW_3, OUTPUT);
}

//////////////////////////////////////////////////////////////////
// readButtons()
//

int readButtons() {
  int button = -1;
  for (int row = 0 ; row < buttonRows ; row++) {

    // set the row pin low
    digitalWrite(BUTTON_ROW_0, row != 0);
    digitalWrite(BUTTON_ROW_1, row != 1);
    digitalWrite(BUTTON_ROW_2, row != 2);
    digitalWrite(BUTTON_ROW_3, row != 3);

    for (int col = 0 ; col < buttonCols ; col++) {
      buttonStates[col][row] = !boolean(digitalRead(BUTTON_COL_0 + col));

      // if the button was off but now is on, set button to the number corresponding to it
      if (buttonStates[col][row] && !lastButtonStates[col][row]) {
        button =  col + row * buttonRows;
      }

      // store the current state of the buttons for comparision next time around
      lastButtonStates[col][row] = buttonStates[col][row];

    }
  }
  return button;
}

//////////////////////////////////////////////////////////////////
// executeButtonAction()
//

void executeButtonAction(int _button) {

  if (machineState == CALIBRATE) { // if in calibration mode, buttons have these functions
    switch (_button) {
      case 0: // BUTTON 01 - decrease position by ~1 degree
        m1PosOffset -= 11;
        break;

      case 1:  // BUTTON 02 - increase position by ~1 degree
        m1PosOffset += 11;
        break;

      case 2: // BUTTON 03 - decrease position by ~10 degrees
        m2PosOffset -= 11;
        break;

      case 3: // BUTTON 04 - increase position by ~10 degrees
        m2PosOffset += 11;
        break;

      case 4: // BUTTON 05 - set open position
        targetOpen = target;
        break;

      case 5: // BUTTON 06 - reset open position
        targetOpen = initTargetOpen;
        break;

      case 6: // set close
        targetClosed = target;
        break;

      case 7: // reset close
        targetClosed = initTargetClose;
        break;

      case 8: // increase speed
        target = targetOpen;
        break;

      case 9: // decrease speed
        target = targetClosed;
        break;

      case 10: // reset speed
        target -= 110;
        break;

      case 11: // pause
        target += 110;
        break;

      case 12: // pump rate = startupFlow
        setFlowRate(startupFlow);
        break;

      case 13: // pump rate = runFlow
        setFlowRate(runFlow);
        break;

      case 14: // pause / resume pump
        pausePump();
        break;

      case 15: //
        machineState = lastMachineState;
        break;
    }
  } else { // otherwise the machine is in run mode and has these functions
    switch (_button) {
      case 0: // BUTTON 01 - set state to Open
        machineState = OPEN;
        clearPositionFlags();
        break;

      case 1:  // BUTTON 02 - set state to Close
        machineState = CLOSE;
        clearPositionFlags();
        break;

      case 2: // BUTTON 03 - set state to Startup
        machineState = STARTUP;
        setFlowRate(startupFlow);
        clearPositionFlags();
        break;

      case 3: // BUTTON 04 - enter calibration mode
        lastMachineState = machineState;
        machineState = CALIBRATE;
        clearPositionFlags();
        break;
    }
  }
}

void clearPositionFlags() {
  isOpen = false;
  isClosed = false;
}

//////////////////////////////////////////////////////////////////
// printButtonStates()
//
// not used. just here for diagnostics

void printButtonStates() {
  Serial.print("Button States: \n");
  for (int row = 0 ; row < buttonRows ; row++) {
    for (int col = 0 ; col < buttonCols ; col++) {
      Serial.print("Col: ");
      Serial.print(col);
      Serial.print(", Row: ");
      Serial.print(row);
      Serial.print(", State = ");
      Serial.print(buttonStates[col][row]);
      Serial.print("\n");
    }
  }
}
