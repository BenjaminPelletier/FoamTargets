#ifndef LEDs_h
#define LEDs_h

// TODO: https://github.com/FastLED/FastLED/issues/507 https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/

struct TargetStyles {
  typedef enum : uint8_t {
    blank = 0,
    blankHit,
    red,
    redHit,
    blue,
    blueHit,
    green,
    redSelected,
    blueSelected,
    rainbow
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
declDraw(bDrawGreen); declDraw(sDrawGreen);
declDraw(bDrawRedSelected); declDraw(sDrawRedSelected);
declDraw(bDrawBlueSelected); declDraw(sDrawBlueSelected);
declDraw(bDrawRainbow); declDraw(sDrawRainbow);

const DrawTargetHandler bigHandlers[] = {
  bDrawBlank,
  bDrawBlankHit,
  bDrawRed,
  bDrawRedHit,
  bDrawBlue,
  bDrawBlueHit,
  bDrawGreen,
  bDrawRedSelected,
  bDrawBlueSelected,
  bDrawRainbow,
};
const DrawTargetHandler smallHandlers[] = {
  sDrawBlank,
  sDrawBlankHit,
  sDrawRed,
  sDrawRedHit,
  sDrawBlue,
  sDrawBlueHit,
  sDrawGreen,
  sDrawRedSelected,
  sDrawBlueSelected,
  sDrawRainbow
};

class TargetDisplay {
  public:
    TargetStyles::Style styleIdle;
    TargetStyles::Style styleHit;
    uint16_t animationFrame;
    unsigned long tLastFrame;

    void resetAnimation();
};

void TargetDisplay::resetAnimation() {
  animationFrame = 0;
  tLastFrame = 0;
}

const int NUM_LEDS = 74;

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

const uint8_t INDICATOR_TOP_INDEX = 9;
const uint8_t INDICATOR_TOP_LENGTH = 8;
const uint8_t INDICATOR_BOTTOM_INDEX = 36;
const uint8_t INDICATOR_BOTTOM_LENGTH = 8;
const uint8_t INDICATOR_SIDE_INDEX = 54;
const uint8_t INDICATOR_SIDE_LENGTH = 20;

const uint8_t BIG_SIDE_LENGTH = 5;

CRGB leds[NUM_LEDS];

class CRGB2 {
  public:
    enum {
      VeryDarkRed = 0x200000,
      VeryDarkBlue = 0x000020
    } ExtendedColors;
};

const CRGB SPECTRUM[] = { CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Cyan, CRGB::Blue, CRGB::Indigo, CRGB::Violet, CRGB::Magenta };
const uint8_t SPECTRUM_LENGTH = sizeof(SPECTRUM) / sizeof(CRGB);

#endif
