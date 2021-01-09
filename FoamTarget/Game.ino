#include "Game.h"
#include "AccelerometerTargets.h"
#include "Comms.h"

uint8_t targetIDCount;
GameTargetID targetIDs[NUM_TARGETS * MAX_CLIENTS];

uint8_t idIndices[NUM_TARGETS * MAX_CLIENTS];
unsigned long sortBuffer[NUM_TARGETS * MAX_CLIENTS];
unsigned long sortBufferTemp;

void enumerateAvailableTargets() {
  targetIDCount = 0;
  for (uint8_t t = 0; t < NUM_TARGETS; t++) {
    if (mpuTargets[t].valid) {
      targetIDs[targetIDCount] = GameTargetID(CLIENT_SELF, t);
      targetIDCount++;
    }
  }
  for (uint8_t c = 1; c < MAX_CLIENTS; c++) {
    if (clients[c].ok) {
      for (uint8_t t = 0; t < NUM_TARGETS; t++) {
        // TODO: check whether each target is working for client
        targetIDs[targetIDCount] = GameTargetID(c, t);
        targetIDCount++;
      }
    }
  }
}

void shuffleIndices(uint8_t maxIndex) {
  for (uint8_t i = 0; i < maxIndex; i++) {
    sortBuffer[i] = (random(0x10000000) & ~(long)0xFF) | i;
  }

  // Bubble sort
  unsigned long temp;
  bool swapped = true;
  while (swapped) {
    swapped = false;
    for (uint8_t i = 0; i < maxIndex - 1; i++) {
      if (sortBuffer[i + 1] < sortBuffer[i]) {
        temp = sortBuffer[i];
        sortBuffer[i] = sortBuffer[i + 1];
        sortBuffer[i + 1] = temp;
        swapped = true;
      }
    }
  }
  
  for (uint8_t i = 0; i < maxIndex; i++) {
    idIndices[i] = (uint8_t)(sortBuffer[i] & 0xFF);
  }
}

void setTarget(GameTargetID id, TargetStyles::Style idle, TargetStyles::Style hit) {
  uint8_t client = id.client();
  uint8_t t = id.target();
  Serial.print("setTarget(<");
  Serial.print(client);
  Serial.print(", ");
  Serial.print(t);
  Serial.print(">, ");
  Serial.print(idle);
  Serial.print(", ");
  Serial.print(hit);
  Serial.println(")");
  if (client == 0) {
    targetDisplays[t].styleIdle = idle;
    targetDisplays[t].styleHit = hit;
    targetDisplays[t].resetAnimation();
  } else {
    changeSlaveTargetStyle(id, idle, hit);
    clientTargets[client][t].styleIdle = idle;
    clientTargets[client][t].styleHit = hit;
  }
}

TargetStyles::Style getTargetIdleStyle(GameTargetID id) {
  uint8_t client = id.client();
  if (client == 0) {
    return targetDisplays[id.target()].styleIdle;
  } else {
    return clientTargets[client][id.target()].styleIdle;
  }
}

TargetStyles::Style getTargetHitStyle(GameTargetID id) {
  uint8_t client = id.client();
  if (client == 0) {
    return targetDisplays[id.target()].styleHit;
  } else {
    return clientTargets[client][id.target()].styleHit;
  }
}

void SimpleGame::init() {
  Serial.println("Beginning new Simple game...");

  maxScore = 10;
  
  enumerateAvailableTargets();
  Serial.print("Found ");
  Serial.print(targetIDCount);
  Serial.println(" targets");

  shuffleIndices(targetIDCount);
  
  uint8_t perSide = targetIDCount / 2;
  for (uint8_t i = 0; i < perSide; i++) {
    setTarget(targetIDs[idIndices[i]], TargetStyles::red, TargetStyles::redHit);
    setTarget(targetIDs[idIndices[perSide + i]], TargetStyles::blue, TargetStyles::blueHit);
  }
  if (targetIDCount % 2 == 1) {
    bool tieRed = random(2) == 0;
    setTarget(targetIDs[idIndices[targetIDCount - 1]], tieRed ? TargetStyles::red : TargetStyles::blue, tieRed ? TargetStyles::redHit : TargetStyles::blueHit);
  }

  blueScore = 0;
  redScore = 0;

  for (uint8_t i = 0; i < INDICATOR_TOP_LENGTH; i++) {
    leds[INDICATOR_TOP_INDEX + i] = CRGB::Black;
  }

  //for (uint8_t i = 0; i < INDICATOR_BOTTOM_LENGTH; i++) {
  //  leds[INDICATOR_BOTTOM_INDEX + i] = CRGB::Black;
  //}

  for (uint8_t i = 0; i < INDICATOR_SIDE_LENGTH; i++) {
    leds[INDICATOR_SIDE_INDEX + i] = CRGB::Black;
  }
}

Game* SimpleGame::hit(GameTargetID hit) {
  switch (getTargetHitStyle(hit)) {
    case TargetStyles::redHit:
      redScore++;
      setTarget(hit, TargetStyles::blue, TargetStyles::blueHit);
      flashRed = true;
      break;
    case TargetStyles::blueHit:
      blueScore++;
      setTarget(hit, TargetStyles::red, TargetStyles::redHit);
      flashBlue = true;
      break;
    default:
      break;
  }

  uint8_t nRed = 0;
  uint8_t nBlue = 0;
  for (uint8_t t = 0; t < targetIDCount; t++) {
    TargetStyles::Style s = getTargetHitStyle(targetIDs[t]);
    if (s == TargetStyles::redHit) nRed++;
    if (s == TargetStyles::blueHit) nBlue++;
  }

  if (nRed == 0 || (nRed == 1 && random(2) == 1)) {
    setTarget(targetIDs[random(targetIDCount)], TargetStyles::red, TargetStyles::redHit);
  }
  if (nBlue == 0 || (nBlue == 1 && random(2) == 1)) {
    setTarget(targetIDs[random(targetIDCount)], TargetStyles::blue, TargetStyles::blueHit);
  }

  // Display new score on side indicator
  Serial.print("SimpleGame.hit drawing blue ");
  Serial.print(blueScore);
  Serial.print(" to red ");
  Serial.println(redScore);
  fDrawScore = 0;
  tDrawScore = millis();
  drawScore(tDrawScore);

  if (blueScore >= maxScore || redScore >= maxScore) {
    ((VictoryGame*)nextGame)->idleStyle = redScore >= maxScore ? TargetStyles::redSelected : TargetStyles::blueSelected;
    return nextGame;
  } else {
    return NO_GAME;
  }
}

Game* SimpleGame::heartbeat() {
  drawScore(millis());
  return NO_GAME;
}

void SimpleGame::drawScore(unsigned long t) {
  if (t >= tDrawScore) {
    int state, index;
    uint8_t phase;
    
    state = NUM_PHASES * blueScore * (INDICATOR_SIDE_LENGTH / 2) / maxScore;
    index = state / NUM_PHASES;
    phase = (uint8_t)(state % NUM_PHASES);
    CRGB c;
    for (uint8_t i = 0; i < INDICATOR_SIDE_LENGTH / 2; i++) {
      if (i < index) {
        c = CRGB2::VeryDarkBlue;
      } else if (i > index) {
        c = CRGB::Black;
      } else {
        switch(phase) {
          case 0: c = CRGB::Black; break;
          case 1: c = fDrawScore / 2 == 0 ? CRGB2::VeryDarkBlue : CRGB::Black; break;
          case 2: c = fDrawScore % 2 == 0 ? CRGB2::VeryDarkBlue : CRGB::Black; break;
          default: c = CRGB2::VeryDarkBlue; break;
        }
      }
      leds[INDICATOR_SIDE_INDEX + (INDICATOR_SIDE_LENGTH / 2) + i] = c;
    }
    if (flashBlue) {
      leds[INDICATOR_SIDE_INDEX + (INDICATOR_SIDE_LENGTH / 2) + index] = CRGB::White;
      flashBlue = false;
    }

    state = NUM_PHASES * redScore * (INDICATOR_SIDE_LENGTH / 2) / maxScore;
    index = state / NUM_PHASES;
    phase = (uint8_t)(state % NUM_PHASES);
    for (uint8_t i = 0; i < INDICATOR_SIDE_LENGTH / 2; i++) {
      if (i < index) {
        c = CRGB2::VeryDarkRed;
      } else if (i > index) {
        c = CRGB::Black;
      } else {
        switch(phase) {
          case 0: c = CRGB::Black; break;
          case 1: c = fDrawScore / 2 == 0 ? CRGB2::VeryDarkRed : CRGB::Black; break;
          case 2: c = fDrawScore % 2 == 0 ? CRGB2::VeryDarkRed : CRGB::Black; break;
          default: c = CRGB2::VeryDarkRed; break;
        }
      }
      leds[INDICATOR_SIDE_INDEX + (INDICATOR_SIDE_LENGTH / 2) - 1 - i] = c;
    }
    if (flashRed) {
      leds[INDICATOR_SIDE_INDEX + (INDICATOR_SIDE_LENGTH / 2) - 1 - index] = CRGB::White;
      flashRed = false;
    }

    FastLED.show();
    while (tDrawScore <= t) {
      tDrawScore += ANIMATION_PERIOD;
    }
    fDrawScore = (fDrawScore + 1) % NUM_PHASES;
  }
}

void VictoryGame::init() {
  Serial.println("Beginning new Victory game...");

  enumerateAvailableTargets();
  Serial.print("Found ");
  Serial.print(targetIDCount);
  Serial.println(" targets");

  setTarget(targetIDs[0], TargetStyles::green, TargetStyles::rainbow);
  for (uint8_t i = 1; i < targetIDCount; i++) {
    setTarget(targetIDs[i], idleStyle, TargetStyles::rainbow);
  }
  
  for (uint8_t i = 0; i < INDICATOR_TOP_LENGTH; i++) {
    leds[INDICATOR_TOP_INDEX + i] = CRGB::Black;
  }

  //for (uint8_t i = 0; i < INDICATOR_BOTTOM_LENGTH; i++) {
  //  leds[INDICATOR_BOTTOM_INDEX + i] = CRGB::Black;
  //}

  leds[INDICATOR_SIDE_INDEX] = CRGB::White;
  for (uint8_t i = 1; i < INDICATOR_SIDE_LENGTH; i++) {
    leds[INDICATOR_SIDE_INDEX + i] = CRGB::Black;
  }
  fIndicatorSide = 0;
  tDraw = millis() + ANIMATION_PERIOD;
  FastLED.show();
}

Game* VictoryGame::hit(GameTargetID hit) {
  if (hit == targetIDs[0]) {
    return nextGame;
  }

  return NO_GAME;
}

Game* VictoryGame::heartbeat() {
  unsigned long t = millis();
  if (t >= tDraw) {
    leds[INDICATOR_SIDE_INDEX + fIndicatorSide] = CRGB::Black;
    fIndicatorSide = (fIndicatorSide + 1) % INDICATOR_SIDE_LENGTH;
    leds[INDICATOR_SIDE_INDEX + fIndicatorSide] = CRGB::White;
    FastLED.show();
    while (tDraw <= t) {
      tDraw += ANIMATION_PERIOD;
    }
  }
  return NO_GAME;
}
