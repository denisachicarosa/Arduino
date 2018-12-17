#include "LedControl.h"
#include <LiquidCrystal.h>
#define V0_PIN 9
#define FIRST_PLAYER_PIN A0
#define SECOND_PLAYER_PIN A1


LiquidCrystal lcd(4, 5, 6, 7, 8, 13);


LedControl lc = LedControl(12, 11, 10, 1);

bool moved;
bool start;
bool gasit;
long interval;
long previousMillis;
byte maxLeft = { B11110000 };
byte maxRight = { B00001111 };

int xs,ys;

class 

class ball {
  int row,col;
  int dirX, dirY;
  int speed;
  public:
  ball(){
    row = col = 3;
    dirX = dirY = -1; 
    }
  ~ball() { row = col = 3; }
  void moveBall(){
    //right wall case
    

    //left wall case


    //up tab/floor


    //down tab

    
  }
  int getRow() { return row; }
  int getCol() { return col;}  
};

class tab{
  byte pos = { B00111100 };
  public:
  tab(){}
  tab(byte x) { pos = x;}
  ~tab(){}
  byte getPos(){ 
    return pos;
  }
  void setPos(byte x){
    pos = x;
  }
};

class player{
  int index;
  int score;
  tab paddle;
  public:
  player() {index = 0; score = 0; }
  
  ~player(){ index = 0; score = 0;}
  int getScore() { return score; }
  byte getPosition(){return paddle.getPos();}
  void setPosition(byte x) { paddle.setPos(x); }
};

byte startImage[8] = {
  B00000000,
  B01100110,
  B01100110,
  B00000000,
  B10000001,
  B01000010,
  B00100100,
  B00011000
};

byte play[8] = {
  B00111100,
  B00000000,
  B00000000,
  B00010000,
  B00000000,
  B00000000,
  B00000000,
  B00111100,
};

ball b;
player player1,player2;

int i,j,previousValue;
void setup() {
    Serial.begin(9600);
    lc.shutdown(0, false);
    lc.setIntensity(0, 1);
    lc.clearDisplay(0);    
    playersNumber = 0;
    lcd.begin(16, 12);
    moved = false;
    gasit  = false;
    previousMillis = 0;
    interval = 250;
    start = false;
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(V0_PIN, OUTPUT);
    analogWrite(V0_PIN, 120);
    i = 0; j = 1;
    previousValue = 0;
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome!");
    lcd.setCursor(i,j);
    xs=1; ys=0;
    lcd.print("->");
    lcd.setCursor(11,1);
    lcd.print("START");
}


int val;

void startGame1(){
  
  long currentMillis = millis();
  val = analogRead(FIRST_PLAYER_PIN);
  //Serial.println(val);
  if(gasit==false){
  if(currentMillis - previousMillis > 250){  
  if(val > previousValue+50)  {
    i++; previousValue=val;
  }

  lcd.setCursor(0,0);
  lcd.print("Player One: ");
  lcd.setCursor(i,j);
  xs=1; ys=0;
  lcd.print("->");
  lcd.setCursor(11,1);
  lcd.print("START");
  if(i>=11) {gasit = true; lcd.clear();
            i = 14;
            j = 1;
            previousMillis = 0;
            previousValue = 0;
            lcd.print("Player Two: ");
  lcd.setCursor(i,j);
   xs=1; ys=0;
  lcd.print("<-");
  lcd.setCursor(0,1);
  lcd.print("START");
            }
  }
  }
}

void startGame2(){
  long currentMillis = millis();
  val = analogRead(SECOND_PLAYER_PIN);
  Serial.println(val);
  if(start == false){
  if(currentMillis - previousMillis > 250){  
  if(val > previousValue+50)  {
    i--; previousValue=val;
  }
  lcd.setCursor(0,0);
  lcd.print("Player Two: ");
  lcd.setCursor(i,j);
   xs=1; ys=0;
  lcd.print("<-");
  lcd.setCursor(0,1);
  lcd.print("START");
  if(i<5) {
    start = true; 
    previousMillis = 0;
    previousValue = 0;
    lcd.clear();}
  }
  } 
}

void printScore(){
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Score: ");
  lcd.setCursor(2,1);
  lcd.print("P1     :     P2");
  lcd.setCursor(8,1);
  lcd.print(player1.getScore());
  lcd.clear();
  lcd.setCursor(10,1);
  lcd.print(player2.getScore());
  
}

int currentValue;

void moveTab(){
  long currentMillis = millis();
 // currentValue = analogRead(FIRST_PLAYER_PIN);
  if(currentMillis - previousMillis > interval){
      if(currentValue - previousValue>150 && player1.getPosition() != maxLeft){
        player1.setPosition(player1.getPosition()<<1);
        play[0] = player1.getPosition();
        previousValue=currentValue;
        moved = true;
      }
      //currentValue = analogRead(FIRST_PLAYER_PIN);
      else if(previousValue - currentValue>150&& player1.getPosition() != maxRight){
        player1.setPosition(player1.getPosition()>>1);
        play[0] = player1.getPosition();
        moved = true;
      }
      previousMillis = currentMillis;
      if(player1.getPosition() == maxLeft || player1.getPosition() == maxRight)
      previousValue = currentValue;
   }
  }
  


void loop() {
  if(start == false){
  for (int i = 0; i < 8; i++){
        lc.setRow(0, i, startImage[i]);}
  startGame1();
  if(gasit == true) {
      startGame2();
  }
  }
  else
  if(start == true){
    printScore();
    for (int i = 0; i < 8; i++){
     lc.setRow(0, i, play[i]);
    currentValue1 = analogRead(FIRST_PLAYER_PIN);
    moveTab();
   // previousValue=0;
    currentValue2 = analogRead(SECOND_PLAYER_PIN);
    Serial.println(currentValue);
    moveTab();
      for (int i = 0; i < 8; i++){
        lc.setRow(0, i, play[i]);
  
      }
  }
  }
  }




  
  
