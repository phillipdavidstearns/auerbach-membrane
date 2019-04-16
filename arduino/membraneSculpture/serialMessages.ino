void verboseOutput() {
  Serial.print("Machine State: ");
  Serial.print(machineState);
  Serial.print("\tCurrent Target: ");
  Serial.print(target);
  Serial.print("\tTargetOpen: ");
  Serial.print(targetOpen);
  Serial.print("\tTargetClose: ");
  Serial.print(targetClosed);
  Serial.print("\tMotor 1 Power: ");
  Serial.print(m1Power);
  Serial.print("\tMotor 1 Speed: ");
  Serial.print(m1Speed);
  Serial.print("\tMotor 1 Position: ");
  Serial.print(m1PosComp);
  Serial.print("\tMotor 2 Power: ");
  Serial.print(m2Power);
  Serial.print("\tMotor 2 Speed: ");
  Serial.print(m2Speed);
  Serial.print("\tMotor 2 Position: ");
  Serial.print(m2PosComp);
  Serial.println();
}
