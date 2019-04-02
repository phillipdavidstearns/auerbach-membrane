

void readButtons() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonScan >= buttonScanInterval) {
    for (int row = 0 ; row < buttonRows ; row++) {
      if (row == 0) {
        digitalWrite(BUTTON_ROW_0, LOW);
      } else {
        digitalWrite(BUTTON_ROW_0, HIGH);
      }
      if (row == 1) {
        digitalWrite(BUTTON_ROW_1, LOW);
      } else {
        digitalWrite(BUTTON_ROW_1, HIGH);
      }
      if (row == 2) {
        digitalWrite(BUTTON_ROW_2, LOW);
      } else {
        digitalWrite(BUTTON_ROW_2, HIGH);
      }
      if (row == 3) {
        digitalWrite(BUTTON_ROW_3, LOW);
      } else {
        digitalWrite(BUTTON_ROW_3, HIGH);
      }
      for (int col = 0 ; col < buttonCols ; col++) {
        buttonStates[col][row] = !boolean(digitalRead(BUTTON_COL_0 + col));
      }
    }
    updateMotorSpeeds(checkButtonChanges());
    lastButtonScan = currentTime;
  }
}

int checkButtonChanges() {
  int button=-1;
  boolean buttonsPressed[buttonRows][buttonCols];
  for (int row = 0 ; row < buttonRows ; row++) {
    for (int col = 0 ; col < buttonCols ; col++) {
      buttonsPressed[col][row] = buttonStates[col][row] && !lastButtonStates[col][row];
      lastButtonStates[col][row] = buttonStates[col][row];
      if (buttonsPressed[col][row]) {
        button = col + row * buttonRows;
        if (debug) {
          Serial.print("Col: ");
          Serial.print(col);
          Serial.print(", Row: ");
          Serial.print(row);
          Serial.print(", Number: ");
          Serial.print(col + row * buttonRows);
          Serial.print(" was Pressed!\n");
        }
      }
    }
  }
  return button;
}

void printButtonStates() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonPrint >= printButtonInterval) {
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
    lastButtonPrint = currentTime;
  }
}
