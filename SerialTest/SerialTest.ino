void setup() {
  Serial.begin(57600);
  Serial.println("================");
  Serial.println("Initialized.");
}

void loop() {
  Serial.println(millis());
  delay(1000);
}
