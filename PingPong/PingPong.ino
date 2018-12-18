#include "LedControl.h"
#include <LiquidCrystal.h>
#define V0_PIN 9
#define FIRST_PLAYER_PIN A0
#define SECOND_PLAYER_PIN A1


LiquidCrystal lcd(4, 5, 6, 7, 8, 13);


LedControl lc = LedControl(12, 11, 10, 1);

void lcdPrint(int i, int index) {
  lcd.setCursor(0,0);
  if(index == 1) {
    lcd.print("Player One: ");
  else lcd.print("Player Two: ");
  lcd.setCursor(i,1);
  lcd.print("->");
  lcd.setCursor(11,1);
  lcd.print("START"); 
}

class Tab{
 protected:
  byte pos;
  int PIN;
  
  public:
  Tab(int potPin) { 
      PIN = potPin;
      pos = B00111100;
    }
    
  Tab(byte x, int potPin) {
      pos = x; 
      PIN = potPin;
      pos = B00111100;
  }
  
  ~Tab() {}
  
  byte getPos() { 
    return pos;
  }
  
  int getPIN() { return PIN;}
  
  void setPos(byte x) {
    pos = x;
  }
  
  void setPIN(int pot) { PIN = pot; }
};

class Player : public Tab{
    int index;
    int score;
  public:
    Player(int pin):Tab(pin) {
    index = 0;
    score = 0;
  }
  int getScore() { return score; }
  
  bool startGame(long& previousMillis, long& previousValue, int& i) {
    long currentMillis = millis();
    long currentValue = analogRead(getPIN());
    if(currentMillis - previousMillis > 250) {
      if(currentValue > previousValue + 50) {
        i++;
        previousValue = currentValue;
       }
     lcdPrint(i);
     if(i > 11) {
     lcd.clear();
     return true;
      }
    }
    return false;
  }
  
  void moveTab(int previousMillis, int previousValue) {
    
    byte maxLeft = { B11110000 };
    byte maxRight = { B00001111 };
    long currentMillis = millis();
    long currentValue = analogRead(PIN);
    if(currentMillis - previousMillis > 250) {
      if(currentValue - previousValue > 150 && pos != maxLeft) {
        pos=pos<<1;
        previousValue = currentValue;
      }
      else if(previousValue - currentValue > 150 && pos != maxRight) {
        pos = pos>>1;
        previousValue = currentValue;
      }
      previousMillis = currentMillis;
      if(pos == maxLeft || pos == maxRight)
      previousValue = currentValue;
    }
  }
  
};

class Ball {
  byte position;
  
}

class Game {
  Player player1;
  Player player2;
  long previousValue, previousMillis;
  byte image[8];
  int arrow;
  int state; // 1-> start player1, 2->start player2, 3->play the game
  
  public:
    Game(): player1(FIRST_PLAYER_PIN),player2(SECOND_PLAYER_PIN) {
      state = 1;
      previousValue = previousMillis = 0;
      arrow = 0;
      byte img[8] = {
        B00000000,
        B01100110,
        B00000000,
        B00010000,
        B00000000,
        B10000001,
        B01000010,
        B00111100
      };
      for( int i = 0; i < 8; i++)
        image[i] = img[i];
      }
    bool start() {
      switch(state) {
        case 1: {
          bool done;
          done = player1.startGame(previousMillis, previousValue, arrow);
          if(done == true) {
            state = 2;
            previousMillis = previousValue = arrow = 0;
          }
          break;
        }
        case 2:{
          bool done;
          done = player2.startGame(previousMillis, previousValue, arrow);
          if(done == true) {
            state = 3;
            previousMillis = previousValue = arrow = 0;
            changeMatrix();
          }
          break;
        }
        case 3:{
          //afiseaza scorul
           break;
        }
      }
      if(state == 3) return true;
      return false;
    }
    
    void printMatrix() {
      for (int i = 0; i < 8; i++)
        lc.setRow(0, i, image[i]);
    }
    
    void phaseOne() {
     byte img[8] = {
        B00111100,
        B00000000,
        B00000000,
        B00010000,
        B00000000,
        B00000000,
        B00000000,
        B00111100
      }
      for( int i = 0; i < 8; i++)
        image[i] = img[i];
    }
    
    void updatePlayers() {
      player1.moveTab();
      player2.moveTab();
      image[0] = player1.getPos();
      image[7] = player2.getPos();
    }
    
    void updateBall() {
      
    }
    void updateScore() {
      
    }
    
    
};


void setup() {
  // put your setup code here, to run once:
  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
  lcd.begin(16, 12);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(V0_PIN, OUTPUT);
  analogWrite(V0_PIN, 120); 
}

Game pong;
bool play = false;

void loop() {
  pong.printMatrix();
  if(play == false)  {
    play = pong.start();
  }
  if(play == true) {
    pong.updatePlayers();
    pong.updateBall();
    pong.updateScore();
  }
}
  
