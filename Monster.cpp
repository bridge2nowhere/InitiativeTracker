#include "Monster.h"

Monster::Monster(char* n, byte r,byte h, int i, bool p, char* d){
	mName = n;
	inCombat = false;
	initRoll = r;
	hp = h;
	currhp = hp;
	initMod = i;
	npc = p;
	details = d;
	
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