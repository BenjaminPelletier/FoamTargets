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
  const uint8_t PIN_RGB_DATA = 14; // D5=GPIO14
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
    switches[s].debug();
  }

  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  FastLED.show();

  last_report = millis();

  Serial.println("Initiating loop.");

  digitalWrite(PIN_LED, LED_OFF);
}

void loop() {
  server.handleClient();
  watchAccelerometers();
}

void watchAccelerometers() {
  bool reported =false;
  unsigned long t = millis();
  if (t - last_report > 1000) {
    reported = true;
    Serial.print(t);
  }
  
  for (uint8_t s = 0; s < MPU_COUNT; s++) {
    if (!switches[s].valid) continue;
    uint8_t change = switches[s].measure(t);
    if (reported) {
      Serial.print(", <");
      Serial.print(switches[s].x);
      Serial.print(", ");
      Serial.print(switches[s].y);
      Serial.print(", ");
      Serial.print(switches[s].z);
      Serial.print(">");
      reported = true;
    }
    if (change == MEASURE_BECAME_ACTIVE) {
      digitalWrite(PIN_LED, LED_ON);
      leds[4 + 9 * s] = CRGB::Yellow;
      FastLED.show();
      Serial.print(s);
      Serial.print(" active at ");
      Serial.print(t);
      Serial.print(" with ");
      Serial.println(switches[s].z);
    } else if (change == MEASURE_BECAME_INACTIVE) {
      digitalWrite(PIN_LED, LED_OFF);
      leds[4 + 9 * s] = CRGB::Black;
      FastLED.show();
      Serial.print(s);
      Serial.print(" inactive at ");
      Serial.println(t);
    }
  }
  if (reported) {
    Serial.println();
    while (t - last_report > 1000) {
      last_report += 1000;
    }
  }
}

void handleRoot() {
  leds[36] = CRGB::Green;
  FastLED.show();
  server.send(200, "text/plain", "Hello from FoamTarget!");
  leds[36] = CRGB::Black;
  FastLED.show();
}

void handleNotFound() {
  leds[36] = CRGB::Red;
  FastLED.show();
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  leds[36] = CRGB::Black;
  FastLED.show();
}
