#include <Arduino.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiAP.h>
  #include <WebServer.h>
  
  WebServer server(80);
#endif

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>

  ESP8266WebServer server(80);
#endif

#include "AccelSwitch.h"

// Pin connections and hardware-specific definitions
#ifdef ESP32
  const int PIN_MPU_A0[] = {25, 26, 33};
  const int PIN_LED = 2;
  const uint8_t PIN_RGB_DATA = 4;
  // D22 -> SCL
  // D21 -> SDA

  const int LED_OFF = LOW;
  const int LED_ON = HIGH;
#endif

#ifdef ESP8266
  // https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
  const int PIN_MPU_A0[] = {16, 0, 12}; // D0=GPIO16, D3=GPIO0, D6=GPIO12, D7=GPIO13, D8=GPIO15
  const int PIN_LED = 2; // D4=GPIO2
  const uint8_t PIN_RGB_DATA = 14; // D5=GPIO14 (or D7=GPIO13)
  const uint8_t PIN_SCL = 5; // D1=GPIO5 -> SCL
  const uint8_t PIN_SDA = 4; // D2=GPIO4 -> SDA

  const int LED_OFF = HIGH;
  const int LED_ON = LOW;
#endif

// Accelerometer switches
const int MPU_COUNT = sizeof(PIN_MPU_A0) / sizeof(int);
AccelSwitch switches[MPU_COUNT];

// RGB strip
const int NUM_LEDS = 40;
CRGB leds[NUM_LEDS];

// Server
const char *ssid = "FoamTargets";
const char *password = "foamflinger";
bool isAP;
IPAddress myIP;

unsigned long last_report;

void setup() {
  delay(1000);
  
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LED_ON);

  Serial.begin(57600);
  Serial.println("=====================================");
  Serial.println("Initializing...");

  FastLED.addLeds<WS2812, PIN_RGB_DATA, RGB>(leds, NUM_LEDS);
  leds[0] = CRGB::Yellow;
  FastLED.show();

  // Attempt to connect to existing AP
  // TODO
  
  // This unit will act as the AP
  leds[0] = CRGB::Cyan;
  FastLED.show();

  WiFi.softAP(ssid, password);
  myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  leds[0] = CRGB::Blue;
  FastLED.show();

  // Set up web server
  leds[1] = CRGB::Yellow;
  FastLED.show();
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
  server.begin();
  leds[1] = CRGB::Green;
  FastLED.show();

  // Set up accelerometer switches
  Serial.println("Setting up accelerometer switches...");
  #ifdef ESP8266
    Wire.begin(PIN_SDA, PIN_SCL);
  #else
    Wire.begin();
  #endif
  for (uint8_t s = 0; s < MPU_COUNT; s++) {
    switches[s].configure(PIN_MPU_A0[s]);
  }
  for (uint8_t s = 0; s < MPU_COUNT; s++) {
    switches[s].init(millis());
  }
  for (uint8_t s = 0; s < MPU_COUNT; s++) {
    Serial.print("=== Debug ");
    Serial.print(s);
    Serial.println(switches[s].valid ? " [valid]" : " [not valid]");
    Serial.print(switches[s].debug());
  }

  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  FastLED.show();

  last_report = millis();

  Serial.println("Initiating loop.");

  digitalWrite(PIN_LED, LED_OFF);
}

void loop() {
  watchAccelerometers();
  server.handleClient();
}

void watchAccelerometers() {
  unsigned long t = millis();
  for (uint8_t m = 0; m < MPU_COUNT; m++) {
    if (!switches[m].valid) continue;
    uint8_t change = switches[m].measure(t);
    if (change == MEASURE_BECAME_ACTIVE) {
      onMPUActive(m, t);
    } else if (change == MEASURE_BECAME_INACTIVE) {
      onMPUInactive(m, t);
    }
  }
}

void onMPUActive(uint8_t m, unsigned long t) {
  digitalWrite(PIN_LED, LED_ON);
  leds[4 + 9 * m] = CRGB::Yellow;
  FastLED.show();
  Serial.print(m);
  Serial.print(" active at ");
  Serial.print(t);
  Serial.print(" with ");
  Serial.println(switches[m].z);
}

void onMPUInactive(uint8_t m, unsigned long t) {
  digitalWrite(PIN_LED, LED_OFF);
  leds[4 + 9 * m] = CRGB::Black;
  FastLED.show();
  Serial.print(m);
  Serial.print(" inactive at ");
  Serial.println(t);
}
