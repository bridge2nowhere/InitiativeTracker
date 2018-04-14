#include <SSD1306Ascii.h>
#include <SSD1306AsciiSoftSpi.h>
#include <SSD1306AsciiSpi.h>
#include <SSD1306init.h>
#include <CircularBuffer.h>
#include <Keypad.h>
#include "Monster.h"
//Monster(char* mName,bool inCombat, int initRoll,int hp, int initMod)
//.roll() updates initRoll with D20+initMod
//.engage() move to combat
//.kill() remove from combat


//-----------------------------------------------------------------------
//OLED Setup

#define CS_PIN 12
#define RST_PIN 13

#define DC_PIN 11
#define MOSI_PIN 9
#define CLK_PIN 10

SSD1306AsciiSoftSpi oled;



//----------------------------------------------------------------------------------------------------
//Keypad Details
const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1','2','3','H'},  //H hit
  {'4','5','6','K'},  //K kill
  {'7','8','9','G'},  //G go/stop
  {'N','0','@','E'}   //N next, @ at, E enter
};
byte rowPins[rows] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[cols] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );




//----------------------------------------------------------------------------------------------
//Variables
const unsigned int MAX_INPUT = 10;
const int TOME_LENGTH = 11;     //the length of the master tome of players and monsters
CircularBuffer<byte,TOME_LENGTH> turnOrderC;
//int turnOrder[TOME_LENGTH];    //holds the turn order
byte turnPosition = 0;           //holds the turn position
byte turnCounter = 0;
byte roundCounter = 1;

Monster tome[TOME_LENGTH] = {
  {"vek",18,3,4, false, "PC"},   //0
  {"bob",21,3,4, false, "PC"},
  {"jeff",15,3,4, true, "AC=15"},
  {"dale",17,3,4, true, "AC=15"},   
  {"keith",8,3,4, true,"AC=15"},   
  {"evan",23,3,4, true, "AC=15"},  //5
  {"goblin1",14,3,4, true, "AC=15"},   
  {"goblin2",1,3,4, true, "AC=15"},
  {"goblin2",1,3,4, true, "AC=15"},
  {"goblin3",1,3,4, true, "AC=15"},
  {"goblin4",1,3,4, true, "AC=15"}, //10
};

//-----------------------------------------------------------------------------------------------
//General Functions
void determineTurnOrder(); //updates array turn order with currently engaged beasts
void nextTurn();
void setPCinitiative (const char * data);
void killMonster (const char * data);
void takeHit (const char * data);

//Keypad Fucntions
void processKeypad();

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
  Serial.println("Enter number to engage a character");
  Serial.println("Enter cc@ii to engage and set initiative for a PC");
  Serial.println("Enter Kcc to kill off a character");
  Serial.println("Enter G to Get initiative list");

  
  //OLED screen setup
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, CLK_PIN, MOSI_PIN, RST_PIN);
  oled.setScroll(true);
  oled.clear();
  oled.set2X();
  oled.setFont(font8x8);
  oled.println("Iniative");
  oled.println("Tracker");
  oled.set1X();
  
}

void loop() {
   while (Serial.available () > 0)
    processIncomingByte (Serial.read ());
   processKeypad();
}

//-------------------------------------------------------------------
void determineTurnOrder(){
  turnOrderC.clear();
  for(int i = 40; i >= 0; i --){
    for(int a = 0; a < TOME_LENGTH; a++){
      if(tome[a].initRoll == i && tome[a].inCombat == true){
        turnOrderC.push(a);
      }
    }
  }
}

void serialPrintEngage() {
  Serial.print("Round");
  Serial.print("\t");
  Serial.print(roundCounter);
  Serial.println("");
  for(byte a = 0; a < turnOrderC.size(); a++){
      Serial.print(tome[turnOrderC[a]].mName);
      Serial.print("\t");
      Serial.print("\t");
      Serial.print(tome[turnOrderC[a]].initRoll);
      //Serial.print("\t");
      //Serial.print(tome[turnOrderC[a]].hp);
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
      oled.print(tome[turnOrderC[a]].mName);
      oled.print("--");
      oled.print(tome[turnOrderC[a]].initRoll);
      oled.println("");
      delay(1000);
  }
  oled.clear();
  oled.setFont(Callibri11_bold);
  oled.print("Round: ");
  oled.print(roundCounter);
  oled.println("");

  oled.setFont(Callibri11);
  for(byte a = 0; a < 2; a++){
      oled.print(tome[turnOrderC[a]].mName);
      oled.print("--");
      oled.print(tome[turnOrderC[a]].initRoll);
      oled.println("");
      
  }
     
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
    int dataNumType = atoi(data);
    tome[dataNumType].engage();
    if(tome[dataNumType].npc == true){
      tome[dataNumType].roll();
    }
  }
}  // end of process_data

void processKeypad(){
/*
    byte inputCounter = 0;
    static char data[MAX_INPUT];
    char keypress = keypad.getKey();
    if(keypress == 'E'){
      for(byte i = 0; i < keypadBuffer.size(); i ++){
        data[i] = keypadBuffer[i];
        inputCounter = i;
      }
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
  tome[pcMonsterInt].initRoll = pcInitInt;
  tome[pcMonsterInt].engage();
}

void killMonster (const char * data){
  char cMon[3] = {data[1],data[2],'\0'};
  int cMonInt = atoi(cMon);
  tome[cMonInt].kill();
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
  tome[npcMonsterInt].takeHit(npcHitInt);
}

