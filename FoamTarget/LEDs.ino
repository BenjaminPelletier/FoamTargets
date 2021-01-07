#include <FastLED.h>
#include "Hardware.h"
#include "LEDs.h"
#include "AccelerometerTargets.h"

void setupLEDs() {
  FastLED.addLeds<WS2812, PIN_RGB_DATA, RGB>(leds, NUM_LEDS);
  for (uint8_t i = 0; i < NUM_TARGETS; i++) {
    targetDisplays[i].styleIdle = TargetStyles::red;
    targetDisplays[i].styleHit = TargetStyles::redHit;
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

void animateTargets(unsigned long t) {
  for (uint8_t m = 0; m < MPU_COUNT; m++) {
    drawTarget(m, mpuTargets[m].active, t);
  }
}

void drawTarget(uint8_t target, bool hit, unsigned long t) {
  TargetStyles::Style style = hit ? targetDisplays[target].styleHit : targetDisplays[target].styleIdle;
  const int* sides = targetSides[target];
  bool draw;
  if (target == 0 || target == 2) {
    draw = smallHandlers[style](sides, &(targetDisplays[target].animationFrame), &(targetDisplays[target].tLastFrame), t);
  } else {
    draw = bigHandlers[style](sides, &(targetDisplays[target].animationFrame), &(targetDisplays[target].tLastFrame), t);
  }
  if (draw) {
    FastLED.show();
  }
}

bool animAlternate(uint16_t interval, DrawTargetHandler draw1, DrawTargetHandler draw2, const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  const uint16_t STEPS = 2;
  if (*t0 == 0 || t > *t0) {
    if (*t0 == 0) {
      *t0 = (t + interval) - (t % (STEPS * interval));
    } else {
      *t0 += interval;
    }
    if (*f == 0) {
      draw1(s, f, t0, t);
      *f = 1;
    } else {
      draw2(s, f, t0, t);
      *f = 0;
    }
    return true;
  }
  return false;
}

bool bDrawBlank(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
    leds[s[0] + i] = CRGB::Black;
  }
  return true;
}

bool sDrawBlank(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Black;
  leds[s[1]] = CRGB::Black;
  leds[s[2]] = CRGB::Black;
  leds[s[3]] = CRGB::Black;
  return true;
}

bool bDrawBlankHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawBlank(s, f, t0, t);
  leds[s[0] + 4] = CRGB::White;
  return true;
}

bool sDrawBlankHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Black;
  leds[s[1]] = CRGB::White;
  leds[s[2]] = CRGB::Black;
  leds[s[3]] = CRGB::White;
  return true;
}

bool bDrawRed(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawBlank(s, f, t0, t);
  leds[s[0] + 1] = CRGB::Green;
  leds[s[0] + 7] = CRGB::Green;
  return true;
}

bool bDrawRed2(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawBlank(s, f, t0, t);
  leds[s[0] + 2] = CRGB::Green;
  leds[s[0] + 6] = CRGB::Green;
  return true;
}

bool sDrawRed(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Green;
  leds[s[1]] = CRGB::Black;
  leds[s[2]] = CRGB::Green;
  leds[s[3]] = CRGB::Black;
  return true;
}

bool sDrawRed2(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Black;
  leds[s[1]] = CRGB::Green;
  leds[s[2]] = CRGB::Black;
  leds[s[3]] = CRGB::Green;
  return true;
}

bool bDrawRedHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawRed(s, f, t0, t);
  leds[s[0] + 4] = CRGB::White;
  return true;
}

bool sDrawRedHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Green;
  leds[s[1]] = CRGB::White;
  leds[s[2]] = CRGB::Green;
  leds[s[3]] = CRGB::White;
  return true;
}

bool bDrawBlue(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawBlank(s, f, t0, t);
  leds[s[0] + 1] = CRGB::Blue;
  leds[s[0] + 7] = CRGB::Blue;
  return true;
}

bool sDrawBlue(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Blue;
  leds[s[1]] = CRGB::Black;
  leds[s[2]] = CRGB::Blue;
  leds[s[3]] = CRGB::Black;
  return true;
}

bool bDrawBlueHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawBlue(s, f, t0, t);
  leds[s[0] + 4] = CRGB::White;
  return true;
}

bool sDrawBlueHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Blue;
  leds[s[1]] = CRGB::White;
  leds[s[2]] = CRGB::Blue;
  leds[s[3]] = CRGB::White;
  return true;
}

bool bDrawRedSelected(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  return animAlternate(500, bDrawRed, bDrawRed2, s, f, t0, t);
}

bool sDrawRedSelected(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  return animAlternate(500, sDrawRed, sDrawRed2, s, f, t0, t);
}
