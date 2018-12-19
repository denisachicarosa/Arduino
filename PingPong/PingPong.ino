#include "LedControl.h"
#include <LiquidCrystal.h>
#define V0_PIN 9
#define FIRST_PLAYER_PIN A0
#define SECOND_PLAYER_PIN A5
#define interval 250
#define potSignal 40
#define ballMovingTime 450
LiquidCrystal lcd(4, 5, 6, 7, 8, 13);


LedControl lc = LedControl(12, 11, 10, 1);
bool stopp = false;

void lcdPrint(int i, int index) {
  lcd.setCursor(0,0);
  if(index == 1) 
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
    Player(int pin, int i):Tab(pin) {
    index = i;
    score = 0;
  }
  Player operator++() {
    score++;
    return *this;
  }
  int getScore() { return score; }
  void init() { score = 0; }
  bool startGame(long& previousMillis, long& previousValue, int& i) {
    long currentMillis = millis();
    long currentValue = analogRead(getPIN());
   // Serial.println(currentValue);
    if(currentMillis - previousMillis > interval) {
      if(currentValue > previousValue + 40) {
        i++;
        previousValue = currentValue;
       }
     lcdPrint(i, index);
     if(i > 11) {
     lcd.clear();
     return true;
      }
    }
    return false;
  }
  
  void moveTab(int& previousMillis, int& previousValue) {
    
    byte maxLeft = { B11110000 };
    byte maxRight = { B00001111 };
    long currentMillis = millis();
    long currentValue = analogRead(PIN);
    //Serial.print(currentValue);
    //Serial.print(" ");
   // Serial.println(previousValue);
    if(currentMillis - previousMillis > interval) {
      if(currentValue - previousValue > potSignal && pos != maxLeft) {
        pos=pos<<1;
        //Serial.println("here");
        previousValue = currentValue;
      }
      else if(previousValue - currentValue > potSignal && pos != maxRight) {
        pos = pos>>1;
        //Serial.println("and here");
        previousValue = currentValue;
      }
      previousMillis = currentMillis;
     // if(pos == maxLeft || pos == maxRight){
       //previousValue = currentValue;
      //}
    }
  }
  
};

class Ball {
  byte pos;
  int line,column;
  int dirL, dirC;
  long previousMillis;  
  byte maxLeft = { B10000000 };
  byte maxRight = { B00000001 };
  public:
  Ball() {
    line = 4;
    column = 3;
    pos = 1<<column;
    dirL = 1; 
    dirC = 1;
    previousMillis = 0;
  }
  
  void ballInit(){
    column = 4;
    pos = 1<<column;
    line = 4;
    int randomDirection;
    randomDirection = random(-1,2);
    if(randomDirection == 0) randomDirection = 1;
    dirL=randomDirection;
    randomDirection = random(-1,2);
    if(randomDirection == 0) randomDirection = 1;
    dirC=randomDirection;
    previousMillis = 0;
  }
  
  int getDirL(){
    return dirL;
  }
  int getDirC(){
    return dirC;
  }
  void moveBall() {
    long currentMillis = millis();
    if(currentMillis - previousMillis > ballMovingTime){
    line = line + dirL;
    column = column + dirC;
    pos = 1<<column;
    previousMillis = currentMillis;
    }
  }
  
  byte getPos() {
    return pos;
  }
  
  int getLine() {
    return line;
  }
  
  int getColumn() {
    return column;
  }
  
  void setDirL(int d) { dirL = d;}
  
  void setDirC(int d) { dirC = d;}
  
  void touchBall() {
    if(pos == maxLeft){
      dirC = -1;
      long randomDirection = random(-1,2);
      if(randomDirection == 0) randomDirection = 1;
      randomDirection = random(-1,2);
      dirL = randomDirection;
    }
    if(pos == maxRight){
     dirC = 1; 
     long randomDirection = random(-1,2);
     if(randomDirection == 0) randomDirection = -1;
     dirL = randomDirection;
     }
  } 
  
};

class Game {
  Player player1;
  Player player2;
  Ball ball;
  byte image[8];
  int arrow;
  int winner;
  int state; // 1-> start player1, 2->start player2, 3->play the game
  
  public:
    long previousValue1, previousMillis1;
    long previousValue2, previousMillis2;
    Game(): player1(FIRST_PLAYER_PIN,1),player2(SECOND_PLAYER_PIN,2) {
      state = 1;
      winner = 0;
      previousMillis1 = previousValue1 = 0;
      previousValue2 = previousMillis2 = 0;
      arrow = 0;
      byte img[8] = {
        B00000000,
        B01100110,
        B01100110,
        B00000000,
        B00000000,
        B10000001,
        B01000010,
        B00111100
      };
      for( int i = 0; i < 8; i++)
        image[i] = img[i];
      }
      void phaseZero(){
        byte img[8] = {
        B00000000,
        B01100110,
        B01100110,
        B00000000,
        B00000000,
        B10000001,
        B01000010,
        B00111100
      };
      for( int i = 0; i < 8; i++)
        image[i] = img[i];
        printMatrix();
      }
        
    bool start() {
      switch(state) {
        case 1: {
          bool done = false;
          done = player1.startGame(previousMillis1, previousValue1, arrow);
          if(done == true) {
            state = 2;
            previousMillis1 = previousValue1 = arrow = 0;
          }
          break;
        }
        case 2:{
          bool done = false;
          done = player2.startGame(previousMillis2, previousValue2, arrow);
          if(done == true) {
            state = 3;
            previousMillis2 = previousValue2 = arrow = 0;
            phaseOne();
          }
          break;
        }
        case 3:{
          //afiseaza scorul
          updateScore();
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
        B00000000,
        B00000000,
        B00000000,
        B00000000,
        B00111100
      };
      for( int i = 0; i < 8; i++)
        image[i] = img[i];
    }
    
    void updatePlayers() {
      int a,b;
      a =  previousMillis1;
      b = previousValue1;
      player1.moveTab(a,b);
      previousMillis1 = a;
      previousValue1 = b;
     int c, d;
      c =  previousMillis2;
      d = previousValue2;
      //player2.moveTab(previousMillis2, previousValue2);
      player2.moveTab(c,d);
      previousMillis2= c;
      previousValue2 = d;     
      image[0] = player1.getPos();
      printMatrix();
      image[7] = player2.getPos();
      printMatrix();
    }
    
    void updateBall() {
      int ballLine = ball.getLine();
      image[ballLine]= B00000000;
      ball.moveBall();
      verifyBall();
      byte ballPosition = ball.getPos();
      ballLine = ball.getLine();
      image[ballLine] = ballPosition;
      printMatrix();
    }
    
    void verifyBall(){
    ball.touchWall();
    //touch the paddle
    if(ball.getLine() == 0) {
      byte result;
      byte posb, posp; 
      posb = ball.getPos();
      posp = player1.getPos();
      result = posb | posp;
      if(result == posp) {
        // the ball touched the paddle -> change direction
        long randomDirection = random(-1,2);
        ball.setDirL(1);
        //ball.setDirL(ball.getDirL() * (-1));
        ball.setDirC(randomDirection);
      }
      else {
        //the ball is out of bounds
       player2++; 
       updateScore();
       reset();
      }
    }
    else if(ball.getLine() == 7) {
      byte result, posb, posp; 
      posb = ball.getPos();
      posp = player2.getPos();
      result = posb | posp;
      if(result == posp) {
        // the ball touched the paddle -> change direction
        ball.setDirL(-1);
        long randomDirection = random(-1,2);
        ball.setDirC(randomDirection);
      }
      else {
        //the ball is out of bounds
       player1++;
       updateScore(); 
       reset();
      }
    }
  }
    void updateScore() {
      lcd.setCursor(0,0);
      lcd.print("Player1 Player2");
      lcd.setCursor(3,1);
      lcd.print(player1.getScore());
      lcd.setCursor(12,1);
      lcd.print(player2.getScore());
    }
    void winn(){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("winner: ");
        lcd.setCursor(2,1);
        lcd.print(winner);
    }

    void reset(){
      int win = 0;
      if(player1.getScore() >= 6) {
        win = 1;
        winner = 1;
        lcd.print(1);
        lcd.clear();
        stopp = true;
      }
      if(player2.getScore() >= 6){
        win = 2;
        winner = 2;
        lcd.clear();
        stopp = true;
      }
//      winner(win);
      phaseOne();
      player1.setPos(B00111100);
      player2.setPos(B00111100);
      ball.ballInit();
//      if(win != 0){
//      phaseZero();
//      stopp = true;
//      }
      //replay();
    }

    void init() {
      state = 1;
      winner = 0;
      previousMillis1 = previousValue1 = 0;
      previousValue2 = previousMillis2 = 0;
      arrow = 0;
      player1.init();
      player2.init();
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
 // Serial.begin(9600);
}


Game pong;
bool play = false;

void replay() {
    int pot1, pot2;
     byte img[8] = {
        B00000000,
        B01100110,
        B01100110,
        B00000000,
        B00000000,
        B10000001,
        B01000010,
        B00111100
      };
   for(int i = 0; i <8; i++)
      lc.setRow(0, i, img[i]);
      pot1 = analogRead(FIRST_PLAYER_PIN);
      pot2 = analogRead(SECOND_PLAYER_PIN);
    if(pot1 < 15 && pot2 < 15) {
    play = false;
    stopp = false;
    pong.init();
    }
}

void loop() {
  if(stopp == false){
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
  else {
  //pong.phaseZero();
  pong.winn();
  replay();
  }
}