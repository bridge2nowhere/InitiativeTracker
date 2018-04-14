#ifndef Monster_H
#define Monster_H

#include <Arduino.h>

class Monster{
	
	public:
		char* 	mName;
		bool	inCombat;
		byte 	initRoll;
		byte	currhp;
		
		int		hp;
		byte	initMod;
		bool	npc;
		char*	details;
		
		
		Monster(char* n, byte r,byte h, int i, bool p, char* d);
		void roll();
		void engage();
		void kill();
		void takeHit(int howHard);
	
		

};

#endif
		
		