// Button Matrix Test Code
// Used for quickly setting up a button interface with
// 4x4 button matrix TS-792 from Tinkersphere.com

// pin definitions for button columns
#define BUTTON_COL_0 22
#define BUTTON_COL_1 23
#define BUTTON_COL_2 24
#define BUTTON_COL_3 25

// pin difinitions for button rows
#define BUTTON_ROW_0 26
#define BUTTON_ROW_1 27
#define BUTTON_ROW_2 28
#define BUTTON_ROW_3 29

boolean debug = true;

const int buttonRows = 4;
const int buttonCols = 4;
boolean buttonStates[buttonRows][buttonCols];
boolean lastButtonStates[buttonRows][buttonCols];

// variables for timing of the scan vs display of status
unsigned long buttonScanInterval = 33;
unsigned long printButtonInterval = 1000;
unsigned long lastButtonScan = 0;
unsigned long lastButtonPrint = 0;

void setup() {

  pinMode(BUTTON_COL_0, INPUT_PULLUP);
  pinMode(BUTTON_COL_1, INPUT_PULLUP);
  pinMode(BUTTON_COL_2, INPUT_PULLUP);
  pinMode(BUTTON_COL_3, INPUT_PULLUP);
  pinMode(BUTTON_ROW_0, OUTPUT);
  pinMode(BUTTON_ROW_1, OUTPUT);
  pinMode(BUTTON_ROW_2, OUTPUT);
  pinMode(BUTTON_ROW_3, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  readButtons();
}

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
    checkButtonChanges();
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
