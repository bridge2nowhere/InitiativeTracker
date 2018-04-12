#ifndef Monster_H
#define Monster_H

#include <Arduino.h>

class Monster{
	
	public:
		char* 	mName;
		bool	inCombat;
		byte 	initRoll;
		byte		currhp;
		
		byte		hp;
		byte		initMod;
		bool	npc;
		
		
		Monster(char* n, byte r,byte h, byte i, bool p);
		void roll();
		void engage();
		void kill();
	
		

};

#endif
		
		