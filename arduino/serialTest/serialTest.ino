#include <ArduinoJson.h>
#include <TimerOne.h>
#define CALIBRATE   0 //  pause and allow for mid routine adjustments
#define OPEN        1 //  move to targetOpen
#define HOLD_OPEN   2 //  hold on targetOpen
#define CLOSE       3 //  move to targetClosed
#define HOLD_CLOSED 4 //  hold on targetClosed
#define STARTUP     5 //  run EZO-PMP until wires are coated

DynamicJsonDocument doc(2048);
String serialReceived = "";


void serialEvent() {
  serialReceived = Serial.readStringUntil('\n');
}

void parseSerial() {
  deserializeJson(doc, serialReceived);
  serializeJson(doc, Serial);
  serialReceived = "";
}

void doNothing(){
  return;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) continue; // wait for serial port to connect.
  Serial.setTimeout(100);
  doc["message"] = "Hello World";
  serializeJson(doc, Serial);
  // TimerOne SETUP for regular sampling of speed
  Timer1.initialize(1e5); // period in micro seconds (1e5 = 100ms)
  Timer1.attachInterrupt(doNothing);
}

//////////////////////////////////////////////////////////////
// main loop()

void loop() {
  if (serialReceived != "") {
    parseSerial();
  }
}
