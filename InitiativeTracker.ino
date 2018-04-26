#include <SSD1306Ascii.h>
#include <SSD1306AsciiSoftSpi.h>
#include <SSD1306AsciiSpi.h>
#include <SSD1306init.h>
#include <CircularBuffer.h>
#include <Keypad.h>
#include "Monster.h"



//-----------------------------------------------------------------------
//OLED Setup

#define CS_PIN 9
#define RST_PIN 10

#define DC_PIN 11
#define MOSI_PIN 13
#define CLK_PIN 12

SSD1306AsciiSoftSpi oled;



//----------------------------------------------------------------------------------------------------
//Keypad Details
const byte ROWS = 4;
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'7','8','9','H'},  //H hit
  {'4','5','6','K'},  //K kill
  {'1','2','3','G'},  //G go/stop
  {'N','0','@','E'}   //N next, @ at, E enter
};
byte rowPins[ROWS] = {A3, A2, A4, A5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
CircularBuffer<char,10> keypadBuffer;



//----------------------------------------------------------------------------------------------
//Variables
const unsigned int MAX_INPUT = 10;
const int TOME_LENGTH = 13;     //the length of the master tome of players and monsters
CircularBuffer<byte,TOME_LENGTH> turnOrderC;
//int turnOrder[TOME_LENGTH];    //holds the turn order
byte turnPosition = 0;           //holds the turn position
byte turnCounter = 0;
byte roundCounter = 1;
byte lastBuilt = 1;

const Monster PROGMEM masterTome[TOME_LENGTH] = {
  {"Thio Ki",25,4, false, "PC Human Wizard"},
  {"Zenwan",27,2, false, "PC Halfing Rogue"},
  {"Balkas",33,4, false, "PC Elf Monk"},
  {"Fire Wolf",7,2, true, "AC=13 FlmBte M +4 2d4+1"},   
  {"Fire Sprite",7,0, true, "AC=9 M-Bite +4 2d4"},  
  {"Son of Ignis",10,1, true, "AC=11 Spell Caster Melee +3 1d4"},   
  {"Aiden",14,1, true, "AC=13 Spell Caster Melee +3 1d4"},
  {"Monster",5,0, true, "Generic with +0 init"},
  {"Monster",5,1, true, "Generic with +1 init"},
  {"Monster",5,2, true, "Generic with +2 init"},
  {"Monster",5,3, true, "Generic with +3 init"},
  {"Monster",5,4, true, "Generic with +4 init"},
  {"Monster",5,5, true, "Generic with +5 init"},
};
  

CircularBuffer<Monster*,20> engagedBuffer;

//-----------------------------------------------------------------------------------------------
//General Functions
void determineTurnOrder(); //updates array turn order with currently engaged beasts
void nextTurn();
void setPCinitiative (const char * data);
void killMonster (const char * data);
void takeHit (const char * data);
void makeMonster(int i); //this version auto rolls 
void makeMoster(int i,byte r); //this version uses r as the roll
void multiMonster();


//Keypad Fucntions
void processKeypad(char keypress);

//OLED Functions
void oledPrintEngage();

//Serial Functions
void process_data (const char * data);
void processIncomingByte (const byte inByte);
void serialPrintEngage(); //Prints out the initiative list in order of initiative

//----------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  //Serial Screen Setup
  Serial.println(F("Enter number to engage a character"));
  Serial.println(F("Enter cc@ii to engage and set initiative for a PC"));
  Serial.println(F("Enter Kcc to kill off a character"));
  Serial.println(F("Enter G to Get initiative list"));

  
  //OLED screen setup
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, CLK_PIN, MOSI_PIN, RST_PIN);
  oled.setScroll(true);
  oled.clear();
  oled.set2X();
  oled.setFont(font8x8);
  oled.println(F("Iniative"));
  oled.println(F("Tracker"));
  oled.set1X();
  
}

void loop() {
   while (Serial.available () > 0)
    processIncomingByte (Serial.read ());
   //processKeypad();
   char key = keypad.getKey();

   if (key){
    processKeypad(key);
   }
}

//-------------------------------------------------------------------


void makeMoster(int i){
	Monster* m = new Monster(masterTome[i].mName,  masterTome[i].hp,  masterTome[i].initMod,  masterTome[i].npc,  masterTome[i].details);
	m->inCombat = true;
  m->roll();
	engagedBuffer.push(m);
}

void makeMoster(int i,byte r){
  Monster* m = new Monster(masterTome[i].mName,  masterTome[i].hp,  masterTome[i].initMod,  masterTome[i].npc,  masterTome[i].details);
  m->inCombat = true;
  m->initRoll = r;
  engagedBuffer.push(m);
}

void determineTurnOrder(){
  turnOrderC.clear();
  multiMonster();
  for(int i = 40; i >= 0; i --){
    for(int a = 0; a < engagedBuffer.size(); a++){
      if(engagedBuffer[a]->initRoll == i && engagedBuffer[a]->inCombat == true){
        turnOrderC.push(a);
      }
    }
  }
}

void serialPrintEngage() {
  Serial.print(F("Round"));
  Serial.print("\t");
  Serial.print(roundCounter);
  Serial.println("");
  for(byte a = 0; a < turnOrderC.size(); a++){
      Serial.print(engagedBuffer[turnOrderC[a]]->mName);
      Serial.print("\t");
      Serial.print("\t");
      Serial.print(engagedBuffer[turnOrderC[a]]->initRoll);
      Serial.println("");
  }
}

void oledPrintEngage(){
  oled.clear();
  oled.setFont(Callibri11_bold);
  oled.print("Round: ");
  oled.print(roundCounter);
  oled.println("");
  delay(1000);
  oled.setFont(Callibri11);
  for(byte a = 0; a < turnOrderC.size(); a++){
      oled.print(turnOrderC[a]);
	    oled.print("--");
	    oled.print(engagedBuffer[turnOrderC[a]]->mName);
      oled.print("--");
      oled.print(engagedBuffer[turnOrderC[a]]->initRoll);
      oled.println("");
      delay(1000);
  }
  oled.clear();
  oled.setFont(Callibri11_bold);
  oled.print("Round: ");
  oled.print(roundCounter);
  oled.println("");

  oled.setFont(Callibri11);
  oled.print(engagedBuffer[turnOrderC[0]]->mName);
  oled.print("--");
  oled.print(engagedBuffer[turnOrderC[0]]->initRoll);
  oled.println("");
  oled.setFont(Stang5x7);
  oled.println(engagedBuffer[turnOrderC[0]]->details);
  
  
     
}

void process_data (const char * data){
  if(data[0] == 'G'){
    determineTurnOrder();
    serialPrintEngage(); 
    oledPrintEngage();   
  }
  else if(data[0] == 'K'){
    killMonster(data);
  }
  else if(data[0] == 'N'){
    nextTurn();
  }
  else if(data[2] == '@'){
    setPCinitiative(data);
  }
  else if(data[2] == 'H'){
    takeHit(data);
  }
  else{
    makeMoster(atoi(data));
  }
}  // end of process_data

void processKeypad(char keypress){
  if(keypress == 'E'){
    char inputLine[keypadBuffer.size()+1];
    for(byte i = 0; i < keypadBuffer.size(); i++){
      Serial.print(keypadBuffer[i]);
      inputLine[i] = keypadBuffer[i];
    }
    Serial.println("");
    inputLine[keypadBuffer.size()+1] = '\0';
    process_data (inputLine);
    keypadBuffer.clear();
    
  }
  
  else{
    keypadBuffer.push(keypress);
  }

/*
    byte inputCounter = 0;
    static char data[MAX_INPUT];
    char keypress = keypad.getKey();
    if(keypress == 'E'){
      for(byte i = 0; i < keypadBuffer.size(); i ++){
        data[i] = keypadBuffer[i];
        inputCounter = i;
      }
    data[inputCounter+1] = '\0';
    process_data(data);
    keypadBuffer.clear();
    }
    else{
      keypadBuffer.push(keypress);
    }
    */
}

void processIncomingByte (const byte inByte){
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte)
    {

    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte
      
      // terminator reached! process input_line here ...
      process_data (input_line);
      
      // reset buffer for next time
      input_pos = 0;  
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;

    }  // end of switch
   
  } // end of processIncomingByte

void setPCinitiative (const char * data){
  char pcMonster[3] = {data[0],data[1],'\0'};
  int pcMonsterInt = atoi(pcMonster);
  char pcInit[3] = {data[3],data[4],'\0'};
  int pcInitInt = atoi(pcInit);

  makeMoster(pcMonsterInt,pcInitInt);
  
  
  //engagedBuffer[pcMonsterInt]->initRoll = pcInitInt;
  //engagedBuffer[pcMonsterInt]->engage();
}

void multiMonster(){
    for(byte active = 0; active < engagedBuffer.size(); active++){
      CircularBuffer<byte,20> monsterMatch;
      monsterMatch.push(active);                         //Adds the active index to monsterMatch
      for(byte compare = 0; compare < engagedBuffer.size(); compare++){
      if(strcmp(engagedBuffer[active]->mName, engagedBuffer[compare]->mName) == 0 &&  active !=  compare){
        monsterMatch.push(compare);
      }
    }
    
    byte apendCounter = 1;
    if (monsterMatch.size() > 1){
      for(byte u = 0; u < monsterMatch.size(); u++){
        char monsterBuffer[4];
        sprintf(monsterBuffer, " %d", apendCounter);
        strcat(engagedBuffer[monsterMatch[u]]->mName, monsterBuffer);
        apendCounter++;
      }
    }
  }
}

void killMonster (const char * data){
  char cMon[3] = {data[1],data[2],'\0'};
  int cMonInt = atoi(cMon);
  engagedBuffer[cMonInt]->kill();
  // engagedBuffer[cMonInt] = NULLPTR;
}

void nextTurn(){
  turnOrderC.push(turnOrderC.shift());
  turnCounter ++;
  if(turnCounter == turnOrderC.size()){
    turnCounter = 0;
    roundCounter ++;
    determineTurnOrder();
  }  
  else{
    serialPrintEngage();
    oledPrintEngage();
  }
}

void takeHit (const char * data){
  char npcMonster[3] = {data[0],data[1],'\0'};
  int npcMonsterInt = atoi(npcMonster);
  char npcHit[3] = {data[3],data[4],'\0'};
  int npcHitInt = atoi(npcHit);
  engagedBuffer[npcMonsterInt]->takeHit(npcHitInt);
}

