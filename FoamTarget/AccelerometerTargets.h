#ifndef AccelerometerTargets_h
#define AccelerometerTargets_h

#include "AccelTarget.h"
#include "Hardware.h"

const int MPU_COUNT = sizeof(PIN_MPU_A0) / sizeof(int);
AccelTarget mpuTargets[MPU_COUNT];

#endif
