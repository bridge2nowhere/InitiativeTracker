#include "Monster.h"

Monster::Monster(char n[25],byte h, int i, bool p, char d[25]){
  strcpy(mName , n);
  inCombat = false;
  initRoll = 1;
  hp = h;
  currhp = hp;
  initMod = i;
  npc = p;
  strcpy(details, d);
}


void Monster::roll(){
  initRoll = random(1,21) + initMod;
}

void Monster::engage(){
  inCombat = true;
}

void Monster::kill(){
  inCombat = false;
}

void Monster::takeHit(int howHard){
  currhp -= howHard;
  if (currhp <= 0){
    kill();
  }
}

