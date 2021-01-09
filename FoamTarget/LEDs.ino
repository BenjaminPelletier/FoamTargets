#include <FastLED.h>
#include "Hardware.h"
#include "LEDs.h"
#include "AccelerometerTargets.h"

void setupLEDs() {
  FastLED.addLeds<WS2812, PIN_RGB_DATA, BRG>(leds, NUM_LEDS);
  for (uint8_t i = 0; i < NUM_TARGETS; i++) {
    targetDisplays[i].styleIdle = TargetStyles::blank;
    targetDisplays[i].styleHit = TargetStyles::blank;
    drawTarget(i, false, 0);
  }
}

void showBootStatus(uint8_t bootStep, CRGB rgb) {
  leds[bootStep] = rgb;
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
    leds[s[1] + i] = CRGB::Black;
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
  leds[s[0] + 0] = CRGB::White;
  leds[s[1] + 0] = CRGB::White;
  leds[s[0] + 4] = CRGB::White;
  leds[s[1] + 4] = CRGB::White;
  return true;
}

bool sDrawBlankHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Black;
  leds[s[1]] = CRGB::White;
  leds[s[2]] = CRGB::Black;
  leds[s[3]] = CRGB::White;
  return true;
}

void bDrawColor(CRGB c, const int* s) {
  for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
    leds[s[0] + i] = i == 1 || i == 3 ? c : CRGB::Black;
    leds[s[1] + i] = i == 1 || i == 3 ? c : CRGB::Black;
  }
}

void bDrawColor2(CRGB c, const int* s) {
  for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
    leds[s[0] + i] = i == 0 || i == 4 ? c : CRGB::Black;
    leds[s[1] + i] = i == 0 || i == 4 ? c : CRGB::Black;
  }
}

void sDrawColor(CRGB c, const int* s) {
  leds[s[0]] = c;
  leds[s[1]] = CRGB::Black;
  leds[s[2]] = c;
  leds[s[3]] = CRGB::Black;
}

void sDrawColor2(CRGB c, const int* s) {
  leds[s[0]] = CRGB::Black;
  leds[s[1]] = c;
  leds[s[2]] = CRGB::Black;
  leds[s[3]] = c;
}

bool bDrawRed(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawColor(CRGB2::VeryDarkRed, s);
  return true;
}

bool bDrawRed2(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawColor2(CRGB2::VeryDarkRed, s);
  return true;
}

bool sDrawRed(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  sDrawColor(CRGB2::VeryDarkRed, s);
  return true;
}

bool sDrawRed2(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  sDrawColor2(CRGB2::VeryDarkRed, s);
  return true;
}

bool bDrawRedHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
    leds[s[0] + i] = leds[s[1] + i] = i == 0 || i == 4 ? CRGB::White : CRGB::Red;
  }
  return true;
}

bool sDrawRedHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Red;
  leds[s[1]] = CRGB::White;
  leds[s[2]] = CRGB::Red;
  leds[s[3]] = CRGB::White;
  return true;
}

bool bDrawBlue(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawColor(CRGB2::VeryDarkBlue, s);
  return true;
}

bool bDrawBlue2(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawColor2(CRGB2::VeryDarkBlue, s);
  return true;
}

bool sDrawBlue(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  sDrawColor(CRGB2::VeryDarkBlue, s);
  return true;
}

bool sDrawBlue2(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  sDrawColor2(CRGB2::VeryDarkBlue, s);
  return true;
}

bool bDrawBlueHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
    leds[s[0] + i] = leds[s[1] + i] = i == 0 || i == 4 ? CRGB::White : CRGB::Blue;
  }
  return true;
}

bool sDrawBlueHit(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  leds[s[0]] = CRGB::Blue;
  leds[s[1]] = CRGB::White;
  leds[s[2]] = CRGB::Blue;
  leds[s[3]] = CRGB::White;
  return true;
}

bool bDrawGreen(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  bDrawColor(CRGB::DarkGreen, s);
  return true;
}

bool sDrawGreen(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  sDrawColor(CRGB::DarkGreen, s);
  return true;
}

bool bDrawRedSelected(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  return animAlternate(500, bDrawRed, bDrawRed2, s, f, t0, t);
}

bool sDrawRedSelected(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  return animAlternate(500, sDrawRed, sDrawRed2, s, f, t0, t);
}

bool bDrawBlueSelected(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  return animAlternate(500, bDrawBlue, bDrawBlue2, s, f, t0, t);
}

bool sDrawBlueSelected(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  return animAlternate(500, sDrawBlue, sDrawBlue2, s, f, t0, t);
}

bool bDrawRainbow(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  for (uint8_t side = 0; side < 2; side++) {
    for (uint8_t i = 0; i < BIG_SIDE_LENGTH; i++) {
      leds[s[side] + i] = SPECTRUM[random(SPECTRUM_LENGTH)];
    }
  }
  return true;
}

bool sDrawRainbow(const int* s, uint16_t* f, unsigned long* t0, unsigned long t) {
  for (uint8_t i = 0; i < 4; i++) {
    leds[s[i]] = SPECTRUM[random(SPECTRUM_LENGTH)];
  }
}
