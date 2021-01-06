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

const uint8_t NUM_TARGETS = 5;
TargetDisplay targetDisplays[NUM_TARGETS];

const uint8_t NUM_TARGET_SIDES = 4;
const int targetSides[][NUM_TARGET_SIDES] = {
  { 0, 0, 0, 0 },
  { 9, 9, 9, 9 },
  { 18, 0, 0, 0 },
  { 26, 26, 26, 26 },
  { 36, 0, 0, 0 },
};

const uint8_t BIG_SIDE_LENGTH = 8;
const uint8_t SMALL_SIDE_LENGTH = 3;

CRGB leds[NUM_LEDS];

#endif
