#ifndef LEDs_h
#define LEDs_h

struct TargetStyles {
  typedef enum : uint8_t {
    blank = 0,
    blankHit,
    red,
    redHit,
    blue,
    blueHit,
    redSelected
  } Style;
};

typedef bool(*DrawTargetHandler)(const int* sides, uint16_t* animationFrame, unsigned long* tLastFrame, unsigned long tNow);
#define declDraw(handlerName) bool handlerName(const int* s, uint16_t* f, unsigned long* t0, unsigned long t)

declDraw(bDrawBlank); declDraw(sDrawBlank);
declDraw(bDrawBlankHit); declDraw(sDrawBlankHit);
declDraw(bDrawRed); declDraw(sDrawRed);
declDraw(bDrawRedHit); declDraw(sDrawRedHit);
declDraw(bDrawBlue); declDraw(sDrawBlue);
declDraw(bDrawBlueHit); declDraw(sDrawBlueHit);
declDraw(bDrawRedSelected); declDraw(sDrawRedSelected);

const DrawTargetHandler bigHandlers[] = {
  bDrawBlank,
  bDrawBlankHit,
  bDrawRed,
  bDrawRedHit,
  bDrawBlue,
  bDrawBlueHit,
  bDrawRedSelected
};
const DrawTargetHandler smallHandlers[] = {
  sDrawBlank,
  sDrawBlankHit,
  sDrawRed,
  sDrawRedHit,
  sDrawBlue,
  sDrawBlueHit,
  sDrawRedSelected
};

class TargetDisplay {
  public:
    TargetStyles::Style styleIdle;
    TargetStyles::Style styleHit;
    TargetStyles::Style styleNextHit;
    uint16_t animationFrame;
    unsigned long tLastFrame;

    void resetAnimation();
};

void TargetDisplay::resetAnimation() {
  animationFrame = 0;
  tLastFrame = 0;
}

const int NUM_LEDS = 54;

const uint8_t NUM_TARGETS = 5;
TargetDisplay targetDisplays[NUM_TARGETS];

const uint8_t NUM_SMALL_TARGET_SIDES = 4;
const int targetSides[][NUM_SMALL_TARGET_SIDES] = {
  { 2, 3, 0, 1 },
  { 17, 4, 0, 0 },
  { 25, 22, 23, 24 },
  { 49, 26, 0, 0 },
  { 44, 31, 0, 0 },
};

const uint8_t INDICATOR_SIDE_TOP = 9;
const uint8_t INDICATOR_SIDE_BOTTOM = 36;

const uint8_t BIG_SIDE_LENGTH = 5;

CRGB leds[NUM_LEDS];

#endif
