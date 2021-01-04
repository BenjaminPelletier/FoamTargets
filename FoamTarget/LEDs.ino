#include <FastLED.h>
#include "Hardware.h"
#include "LEDs.h"

void showBootStatus(uint8_t bootStep, CRGB color) {
  leds[bootStep] = color;
  FastLED.show();
}

void clearBootStatus() {
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
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
