void testSequence() {
  md.enableDrivers();
  delay(1);  // The drivers require a maximum of 1ms to elapse when brought out of sleep mode.

  for (int i = 0; i <= 400; i++) {
    md.setM1Speed(i);
    stopIfFault();
    if (i % 200 == 100) {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 400; i >= -400; i--) {
    md.setM1Speed(i);
    stopIfFault();
    if (i % 200 == 100) {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = -400; i <= 0; i++) {
    md.setM1Speed(i);
    stopIfFault();
    if (i % 200 == 100) {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 0; i <= 400; i++) {
    md.setM2Speed(i);
    stopIfFault();
    if (i % 200 == 100) {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 400; i >= -400; i--) {
    md.setM2Speed(i);
    stopIfFault();
    if (i % 200 == 100) {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = -400; i <= 0; i++) {
    md.setM2Speed(i);
    stopIfFault();
    if (i % 200 == 100) {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }

  md.disableDrivers(); // Put the MOSFET drivers into sleep mode.
  delay(500);
}
