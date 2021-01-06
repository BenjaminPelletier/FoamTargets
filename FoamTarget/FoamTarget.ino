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
  Serial.print("Attempting to connect to existing access point...");
  gameMaster = !connectToAP();

  if (gameMaster) {
    // This unit will be the access point
    showBootStatus(0, CRGB::Cyan);
    Serial.println("Setting up new access point...");
    setupAP();
    showBootStatus(0, CRGB::Blue);
  } else {
    showBootStatus(0, CRGB::Green);
  }

  // Set up communications
  showBootStatus(1, CRGB::Yellow);
  Serial.println("Setting up communications...");
  setupComms();
  showBootStatus(1, CRGB::Green);

  // Set up endpoints
  showBootStatus(2, CRGB::Yellow);
  Serial.println("Setting up endpoints...");
  setupEndpoints();
  showBootStatus(2, CRGB::Green);

  showBootStatus(3, CRGB::Yellow);
  Serial.println("Setting up accelerometer targets...");
  setupAccelerometerTargets();
  showBootStatus(3, CRGB::Green);

  // Finish setup
  clearBootStatus(4);
  Serial.println("Setup complete.");
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
