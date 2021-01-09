#ifndef Game_h
#define Game_h

#include "Comms.h"
#include "LEDs.h"

class GameTargetID {
  public:
    GameTargetID() : data(0) {}
    GameTargetID(uint8_t client, uint8_t target) : data((client << 4) | target) {}
    GameTargetID(char c) : data((uint8_t)c) {}

    uint8_t data;
    
    uint8_t client() { return data >> 4; }
    uint8_t target() { return data & 0x0F; }
};

typedef void(*SlaveHitHandler)(GameTargetID id);

bool operator==(const GameTargetID& lhs, const GameTargetID& rhs)
{
    return lhs.data == rhs.data;
}

class ClientTarget {
  public:
    TargetStyles::Style styleIdle;
    TargetStyles::Style styleHit;
};

ClientTarget clientTargets[MAX_CLIENTS][NUM_TARGETS];

class Game {
  public:
    virtual void init();
    virtual Game* hit(GameTargetID hit);
    virtual Game* heartbeat();

    Game* nextGame;
};

Game* NO_GAME = 0;

class SimpleGame : public Game {
  // Note: nextGame must be set to the address of a VictoryGame before using this class
  
  private:
    const int ANIMATION_PERIOD = 150;
    const int NUM_PHASES = 4;

    void drawScore(unsigned long t);
    unsigned long tDrawScore;
    uint8_t fDrawScore;
    bool flashBlue;
    bool flashRed;
    
  public:
    void init() override;
    Game* hit(GameTargetID id) override;
    Game* heartbeat() override;
    
    int maxScore;
    int redScore;
    int blueScore;
};

class VictoryGame : public Game {
  private:
    const int ANIMATION_PERIOD = 100;
    
  public:
    void init() override;
    Game* hit(GameTargetID id) override;
    Game* heartbeat() override;

    TargetStyles::Style idleStyle;

    unsigned long tDraw;
    uint8_t fIndicatorSide;
};

#endif
