#include <FastLED.h>
#include "Hardware.h"
#include "LEDs.h"

void setupLEDs() {
  FastLED.addLeds<WS2812, PIN_RGB_DATA, RGB>(leds, NUM_LEDS);
}

void showBootStatus(uint8_t bootStep, CRGB color) {
  leds[bootStep] = color;
  FastLED.show();
}

void clearBootStatus(uint8_t nSteps) {
  for (uint8_t i = 0; i < nSteps; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void showHandlingRequest(bool isNormal) {
  leds[36] = isNormal ? CRGB::Green : CRGB::Red;
  FastLED.show();
}

void showHandledRequest() {
  leds[36] = CRGB::Black;
  FastLED.show();
}

void showTargetActive(uint8_t t) {
  leds[4 + 9 * t] = CRGB::Yellow;
  FastLED.show();
}

void showTargetInactive(uint8_t t) {
  leds[4 + 9 * t] = CRGB::Black;
  FastLED.show();
}
