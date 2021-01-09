#include <Arduino.h>

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#include "Hardware.h"
#include "AccelTarget.h"
#include "AccelerometerTargets.h"
#include "Endpoints.h"
#include "LEDs.h"
#include "Network.h"
#include "Comms.h"
#include "Game.h"

VictoryGame victoryGame;
SimpleGame simpleGame;
Game* currentGame;

void setup() {
  delay(1000);
  
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LED_ON);

  Serial.begin(57600);
  Serial.println("=====================================");
  Serial.println("Initializing...");

  setupLEDs();

  // Attempt to connect to existing AP
  showBootStatus(0, CRGB::Orange);
  for (int i = 0; i < 3; i++) {
    Serial.print("Attempt ");
    Serial.print(i);
    Serial.print(" to connect to existing access point...");
    gameMaster = !connectToAP();
    if (!gameMaster) {
      Serial.println("  Connected to AP.");
      break;
    }
    delay(1000);
  }

  if (gameMaster) {
    // This unit will be the access point
    showBootStatus(0, CRGB::Cyan);
    Serial.println("Setting up new access point...");
    setupAP();
    showBootStatus(0, CRGB::Blue);
  } else {
    showBootStatus(0, CRGB::Green);
  }

  // Set up communications
  showBootStatus(1, CRGB::Orange);
  Serial.println("Setting up communications...");
  setupComms();
  showBootStatus(1, CRGB::Green);

  // Set up endpoints
  showBootStatus(2, CRGB::Orange);
  Serial.println("Setting up endpoints...");
  setupEndpoints();
  showBootStatus(2, CRGB::Green);

  // Set up accelerometer targets
  showBootStatus(3, CRGB::Orange);
  Serial.println("Setting up accelerometer targets...");
  setupAccelerometerTargets();
  showBootStatus(3, CRGB::Green);

  // Finish setup
  clearBootStatus(4);
  Serial.println("Setup complete.");
  digitalWrite(PIN_LED, LED_OFF);

  // Set up games
  if (gameMaster) {
    simpleGame.nextGame = &victoryGame;
    victoryGame.nextGame = &simpleGame;
    victoryGame.idleStyle = TargetStyles::blank;
    currentGame = &victoryGame;
    currentGame->init();
  }
}

void loop() {
  watchAccelerometers();
  server.handleClient();
  pollUDP(onSlaveTargetHit);
  if (gameMaster) {
    checkGameChange(currentGame->heartbeat());
  }
  animateTargets(millis());
}

void onMPUActive(uint8_t m, unsigned long t) {
  targetDisplays[m].resetAnimation();
  drawTarget(m, true, millis());
  if (gameMaster) {
    checkGameChange(currentGame->hit(GameTargetID(0, m)));
  } else {
    notifyGameMasterOfHit(m);
  }
}

void onMPUInactive(uint8_t m, unsigned long t) {
  targetDisplays[m].resetAnimation();
  drawTarget(m, false, millis());
}

void onSlaveTargetHit(GameTargetID id) {
  checkGameChange(currentGame->hit(id));
}

void checkGameChange(Game* newGame) {
  if (newGame != NO_GAME) {
    currentGame = newGame;
    currentGame->init();
  }
}
