#include "Monster.h"

Monster::Monster(char* n, byte r,byte h, byte i, bool p){
	mName = n;
	inCombat = false;
	initRoll = r;
	hp = h;
	currhp = hp;
	initMod = i;
	npc = p;
	
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