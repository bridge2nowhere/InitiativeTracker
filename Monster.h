#ifndef Monster_H
#define Monster_H

#include <Arduino.h>

class Monster{
  
  public:
    char  mName[20];
    bool  inCombat;
    byte  initRoll;
    byte  currhp;
    
    int   hp;
    byte  initMod;
    bool  npc;
    char  details[25];
    
    
    Monster(char n[20],byte h, int i, bool p, char d[25]);
    void roll();
    void engage();
    void kill();
    void takeHit(int howHard);
};

#endif
    
    
