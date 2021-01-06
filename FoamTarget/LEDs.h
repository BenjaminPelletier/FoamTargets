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
  } Style;
};

typedef void(*DrawTargetHandler)(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);

void bigdrawBlank(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void smalldrawBlank(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void bigdrawBlankHit(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void smalldrawBlankHit(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void bigdrawRed(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void smalldrawRed(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void bigdrawRedHit(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void smalldrawRedHit(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void bigdrawBlue(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void smalldrawBlue(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void bigdrawBlueHit(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);
void smalldrawBlueHit(const int* sides, uint16_t* f, unsigned long* t0, unsigned long t);

const DrawTargetHandler bigHandlers[] = {
  bigdrawBlank,
  bigdrawBlankHit,
  bigdrawRed,
  bigdrawRedHit,
  bigdrawBlue,
  bigdrawBlueHit,
};
const DrawTargetHandler smallHandlers[] = {
  smalldrawBlank,
  smalldrawBlankHit,
  smalldrawRed,
  smalldrawRedHit,
  smalldrawBlue,
  smalldrawBlueHit
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
