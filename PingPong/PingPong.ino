#include "LedControl.h"
#include <LiquidCrystal.h>
#define V0_PIN 9
#define FIRST_PLAYER_PIN A0
#define SECOND_PLAYER_PIN A5
#define interval 250
#define potSignal 100
#define ballMovingTime 2000
LiquidCrystal lcd(4, 5, 6, 7, 8, 13);


LedControl lc = LedControl(12, 11, 10, 1);

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
      if(pos == maxLeft || pos == maxRight){
       previousValue = currentValue;
      }
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
    pos = {B00001000};
    line = 4;
    dirL = 1; 
    dirC = 1;
    previousMillis = 0;
  }
  void ballInit(){
    pos = {B00001000};
    line = 4;
    dirL = 1; 
    dirC = 1;
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
    if(dirC > 0) pos = pos>>1;
    else pos = pos<<1;
    previousMillis = currentMillis;
    }
  }
  byte getPos(){
    return pos;
  }
  byte getLine(){
    return line;
  }
  void setDirL(int d){ dirL = d;}
  void setDirC(int d){ dirC = d;}
  void touchWall(){
    if(pos == maxLeft){
      if(dirL < 0)  dirC = -1;
      else
      if(dirC < 0)  dirL = 1;
    }
    if(pos == maxRight){
      if(dirL < 0) {
        dirL = -1;
        dirC = -1;
      }
      else if(dirL > 0) 
      dirC = 1;
      }
  }
  
};

class Game {
  Player player1;
  Player player2;
  Ball ball;
  byte image[8];
  int arrow;
  int state; // 1-> start player1, 2->start player2, 3->play the game
  
  public:
    long previousValue1, previousMillis1;
    long previousValue2, previousMillis2;
    Game(): player1(FIRST_PLAYER_PIN,1),player2(SECOND_PLAYER_PIN,2) {
      state = 1;
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
    bool start() {
      switch(state) {
        case 1: {
          bool done;
          done = player1.startGame(previousMillis1, previousValue1, arrow);
          if(done == true) {
            state = 2;
            previousMillis1 = previousValue1 = arrow = 0;
          }
          break;
        }
        case 2:{
          bool done;
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
      byte ballLine = ball.getLine();
      image[ballLine]<<9;
      ball.moveBall();
      verifyBall();
      byte ballPosition = ball.getPos();
      ballLine = ball.getLine();
      image[ballLine] = ballPosition;
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
        ball.setDirL(1);
        ball.setDirC(ball.getDirC() * (-1));
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
        ball.setDirC(ball.getDirC() * (-1));
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
    
    void reset(){
      int win;
      if(player1.getScore() >= 3)
        win = 1;
      if(player2.getScore() >= 3);
        win = 2;
        
      winner(win);
      phaseOne();
      player1.setPos(B00111100);
      player2.setPos(B00111100);
      ball.ballInit();
      
      //replay();
    }
    void winner(int win){
      lcd.clear();
      delay(50);
      if(win == 1) {
      lcd.setCursor(1,0);
      lcd.print("Winner: First");
      lcd.setCursor(0,1);
      lcd.print("Rotate=replay");
      }
      else 
        if(win == 2) {
          lcd.setCursor(1,0);
          lcd.print("Winner: Second");
          lcd.setCursor(0,1);
          lcd.print("Rotate=replay");
      }
    }
//    void replay() {
//      lcd.setCursor(0,0);
//      if(index == 1) 
//      lcd.print("Player One: ");
//      else lcd.print("Player Two: ");
//      lcd.setCursor(i,1);
//      lcd.print("->");
//      lcd.setCursor(10,1);
//      lcd.print("REPLAY"); 
//      
//    }
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
  
