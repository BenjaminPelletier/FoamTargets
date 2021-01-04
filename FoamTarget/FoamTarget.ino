#include <Arduino.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#include "Hardware.h"
#include "AccelTarget.h"
#include "AccelerometerTargets.h"
#include "Endpoints.h"
#include "LEDs.h"

// Server
const char *ssid = "FoamTargets";
const char *password = "foamflinger";
bool isAP;
IPAddress myIP;

void setup() {
  delay(1000);
  
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LED_ON);

  Serial.begin(57600);
  Serial.println("=====================================");
  Serial.println("Initializing...");

  FastLED.addLeds<WS2812, PIN_RGB_DATA, RGB>(leds, NUM_LEDS);

  // Attempt to connect to existing AP
  showBootStatus(0, CRGB::Yellow);
  // TODO
  
  // This unit will act as the AP
  showBootStatus(0, CRGB::Cyan);
  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  showBootStatus(0, CRGB::Blue);

  // Set up endpoints
  showBootStatus(1, CRGB::Yellow);
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
