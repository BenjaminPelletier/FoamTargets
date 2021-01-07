#ifndef Game_h
#define Game_h

class Game {
  public:
    virtual void init();
    virtual void hit();
    virtual void heartbeat();
};

class SimpleGame : public Game {
  public:
    void init();
    void hit();
    void heartbeat();
};

#endif
