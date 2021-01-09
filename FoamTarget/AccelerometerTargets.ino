#include "AccelerometerTargets.h"
#include "Hardware.h"

void setupAccelerometerTargets() {
  #ifdef ESP8266
    Wire.begin(PIN_SDA, PIN_SCL);
  #else
    Wire.begin();
  #endif
  for (uint8_t m = 0; m < MPU_COUNT; m++) {
    mpuTargets[m].configure(PIN_MPU_A0[m]);
  }
  for (uint8_t m = 0; m < MPU_COUNT; m++) {
    mpuTargets[m].init(millis());
  }
  for (uint8_t m = 0; m < MPU_COUNT; m++) {
    Serial.print("=== Target ");
    Serial.print(m);
    Serial.println(mpuTargets[m].valid ? " [valid]" : " [not valid]");
    Serial.print(mpuTargets[m].debug());
  }
}

void watchAccelerometers() {
  unsigned long t = millis();
  tMinus2 = tMinus1;
  tMinus1 = t;
  if (t - tMinus2 > tMaxDelta) {
    tMaxDelta = t - tMinus2;
  }
  for (uint8_t m = 0; m < MPU_COUNT; m++) {
    if (!mpuTargets[m].valid) continue;
    uint8_t change = mpuTargets[m].measure(t);
    if (change == MEASURE_BECAME_ACTIVE) {
      onMPUActive(m, t);
    } else if (change == MEASURE_BECAME_INACTIVE) {
      onMPUInactive(m, t);
    }
  }
}
