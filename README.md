# InitiativeTracker
An Arduino based DnD intiative tracker

This is my first large scale project.

## Bill of Materials

* Arduino Uno  
* 2 128x64 OLED Displays  
* 4x4 Matrix Keypad (or serial monitor) for input  
* a 3D printed DM station  
* optional printed player pyramid  

## Tome

The tome is where you define the monster and pc data. Monsters are engaged into combat with their index in the array.

The sameple data is from the <a href="http://www.dmsguild.com/product/219159/Sons-of-Ignis">Son of Igus Adventure</a> created by my friend.
```
0  {"Thio Ki",25,4, false, "PC Human Wizard"},
1  {"Zenwan",27,2, false, "PC Halfing Rogue"},
2  {"Balkas",33,4, false, "PC Elf Monk"},
3  {"Fire Wolf",7,2, true, "AC=13 FlmBte M +4 2d4+1"},   
4  {"Fire Sprite",7,0, true, "AC=9 M-Bite +4 2d4"},  
5  {"Son of Ignis",10,1, true, "AC=11 Spell Caster Melee +3 1d4"},   
6  {"Aiden",14,1, true, "AC=13 Spell Caster Melee +3 1d4"},
7  {"Monster",5,0, true, "Generic with +0 init"},
8  {"Monster",5,1, true, "Generic with +1 init"},
9  {"Monster",5,2, true, "Generic with +2 init"},
10  {"Monster",5,3, true, "Generic with +3 init"},
11  {"Monster",5,4, true, "Generic with +4 init"},
12  {"Monster",5,5, true, "Generic with +5 init"},
```

## Operation

### Keypad layout:
```
7 8 9 H
4 5 6 K
1 2 3 G
N 0 @ E

Enter Key, type after every command
Hit 	ccHddd where <cc> is Tome Index, <ddd> is damage NOT YET IMPLEMENTED
Kill 	Kcc where <cc> is the Engaged Index
Go initiate the characters that are engaged
Next turn will go to the next engaged player
@ 	cc@ii where <cc> is Tome Index and <ii> is their init roll
```
### Bring in Characters:
* Auto Roll Initiative - Type the index of the characters you want to bring in, press enter
* Table Roll Iniative - Type the two digit (01,13 etc) index, press @, then the two digit iniative, then press enter.
* If there are multiples, they will be noted with a 1,2,3,etc after their name

### Start Combat:
* Press Go, Press Enter
* To go to the next player - Press next, then Enter

### To Kill a Character (or remove them from combat)
* Press K, then the engage number (precedes their name on the screen), Press enter
* The character will not fall off the list until the beginning of the next round
