void SimpleGame::init() {
  for (uint8_t i = 0; i < NUM_TARGETS; i++) {
    targetDisplays[i].styleIdle = TargetStyles::blank;
    targetDisplays[i].styleHit = TargetStyles::blank;
    targetDisplays[i].styleNextHit = targetDisplays[i].styleHit;
    targetDisplays[i].resetAnimation();
    drawTarget(i, false, 0);
  }
}

void SimpleGame::hit() {
  
}

void SimpleGame::heartbeat() {
  
}
