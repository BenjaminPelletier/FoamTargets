#include <Arduino.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#include "Hardware.h"
#include "AccelTarget.h"
#include "AccelerometerTargets.h"
#include "Endpoints.h"
#include "LEDs.h"
#include "Network.h"
#include "Comms.h"

void setup() {
  delay(1000);
  
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LED_ON);

  Serial.begin(57600);
  Serial.println("=====================================");
  Serial.println("Initializing...");

  setupLEDs();

  // Attempt to connect to existing AP
  showBootStatus(0, CRGB::Yellow);
  // TODO
  
  // This unit will act as the game master
  showBootStatus(0, CRGB::Cyan);
  Serial.println("Setting up access point...");
  setupAP();
  gameMaster = true;
  showBootStatus(0, CRGB::Blue);

  // Set up communications
  Serial.println("Setting up communications...");
  setupComms();

  // Set up endpoints
  showBootStatus(1, CRGB::Yellow);
  Serial.println("Setting up endpoints...");
  setupEndpoints();
  showBootStatus(1, CRGB::Green);

  Serial.println("Setting up accelerometer targets...");
  setupAccelerometerTargets();

  // Finish setup
  clearBootStatus();
  Serial.println("Initiating loop.");
  digitalWrite(PIN_LED, LED_OFF);
}

void loop() {
  watchAccelerometers();
  server.handleClient();
  pollUDP();
}

void onMPUActive(uint8_t m, unsigned long t) {
  digitalWrite(PIN_LED, LED_ON);
  showTargetActive(m);
  Serial.print(m);
  Serial.print(" active at ");
  Serial.print(t);
  Serial.print(" with ");
  Serial.println(mpuTargets[m].z);
}

void onMPUInactive(uint8_t m, unsigned long t) {
  digitalWrite(PIN_LED, LED_OFF);
  showTargetInactive(m);
  Serial.print(m);
  Serial.print(" inactive at ");
  Serial.println(t);
}
