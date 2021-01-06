#include <FastLED.h>
#include "Hardware.h"
#include "LEDs.h"
#include "AccelerometerTargets.h"

void setupLEDs() {
  FastLED.addLeds<WS2812, PIN_RGB_DATA, RGB>(leds, NUM_LEDS);
  for (uint8_t i = 0; i < NUM_TARGETS; i++) {
    targetDisplays[i].styleIdle = TargetStyles::red;
    targetDisplays[i].styleHit = TargetStyles::blueHit;
    targetDisplays[i].styleNextHit = targetDisplays[i].styleHit;
    drawTarget(i, false, 0);
  }
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
  digitalWrite(PIN_LED, LED_ON);
}

void showHandledRequest() {
  digitalWrite(PIN_LED, LED_OFF);
}

void drawTarget(uint8_t target, bool hit, unsigned long t) {
  TargetStyles::Style style = hit ? targetDisplays[target].styleHit : targetDisplays[target].styleIdle;
  const int* sides = targetSides[target];
  if (target == 0 || target == 1 || target == 3) {
    bigHandlers[style](sides, &targetDisplays[target].animationFrame, &targetDisplays[target].tLastFrame, t);
  } else {
    smallHandlers[style](sides, &targetDisplays[target].animationFrame, &targetDisplays[target].tLastFrame, t);
  }
  FastLED.show();
}

void bigdrawBlank(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
    leds[s[0] + i] = CRGB::Black;
  }
}

void smalldrawBlank(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
    leds[s[0] + i] = CRGB::Black;
  }  
}

void bigdrawBlankHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bigdrawBlank(s, f, t0, t);
  leds[s[0] + 4] = CRGB::White;
}

void smalldrawBlankHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  smalldrawBlank(s, f, t0, t);
  leds[s[0] + 1] = CRGB::White;
}

void bigdrawRed(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bigdrawBlank(s, f, t0, t);
  leds[s[0] + 1] = CRGB::Green;
  leds[s[0] + 7] = CRGB::Green;
}

void smalldrawRed(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  smalldrawBlank(s, f, t0, t);
  leds[s[0] + 1] = CRGB::Green;
}

void bigdrawRedHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bigdrawRed(s, f, t0, t);
  leds[s[0] + 4] = CRGB::White;
}

void smalldrawRedHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  smalldrawRed(s, f, t0, t);
  leds[s[0] + 0] = CRGB::White;
  leds[s[0] + 2] = CRGB::White;
}

void bigdrawBlue(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bigdrawBlank(s, f, t0, t);
  leds[s[0] + 1] = CRGB::Blue;
  leds[s[0] + 7] = CRGB::Blue;
}

void smalldrawBlue(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  smalldrawBlank(s, f, t0, t);
  leds[s[0] + 1] = CRGB::Blue;
}

void bigdrawBlueHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bigdrawBlue(s, f, t0, t);
  leds[s[0] + 4] = CRGB::White;
}

void smalldrawBlueHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  smalldrawBlue(s, f, t0, t);
  leds[s[0] + 0] = CRGB::White;
  leds[s[0] + 2] = CRGB::White;
}
