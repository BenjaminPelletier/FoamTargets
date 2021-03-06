#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#ifdef ESP32
  const uint8_t PIN_RGB_DATA = 4;
#endif

#ifdef ESP8266
  const uint8_t PIN_RGB_DATA = 14; // D5=GPIO14, D7=GPIO13
#endif

const int NUM_LEDS = 74;
CRGB leds[NUM_LEDS];

const CRGB colors[] = { CRGB::Green, CRGB::Red, CRGB::Blue, CRGB::White };
const int NUM_COLORS = sizeof(colors) / sizeof(CRGB);

void setup() {
  delay(1000);

  Serial.begin(57600);
  Serial.println("===================");
  Serial.print("Initializing ");
  Serial.print(NUM_LEDS);
  Serial.print(" LEDs on pin ");
  Serial.print(PIN_RGB_DATA);
  Serial.println("...");
  
  FastLED.addLeds<WS2812, PIN_RGB_DATA, RGB>(leds, NUM_LEDS);
  leds[0] = CRGB::Yellow;
  FastLED.show();
}

void loop() {
  Serial.println("Starting loop");
  for (int c = 0; c < NUM_COLORS; c++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = colors[c];
      FastLED.show();
      delay(100);
      leds[i] = CRGB::Black;
    }
  }
}
