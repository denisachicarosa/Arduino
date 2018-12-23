#include "LedControl.h"
#include <LiquidCrystal.h>
#define V0_PIN 9
#define FIRST_PLAYER_PIN A0 
#define SECOND_PLAYER_PIN A5
#define interval 250                                      
#define minPotSignal 40                                      
#define ballMovingTime 450                               

LiquidCrystal lcd(4, 5, 6, 7, 8, 13);
LedControl lc = LedControl(12, 11, 10, 1);

bool stopp = false;                                       //false -> in timpul jocului  true -> afisez castigator+astept semnal pentru replay


void lcdPrint(int i, int index){                          //functie pentru afisarea START-ului pe LCD. Primeste i pentru pozitia sagetii si index pentru numarul jucatorului
  lcd.setCursor(0,0);
  if (index == 1) 
    lcd.print("Player One: ");
  else 
    lcd.print("Player Two: ");
  lcd.setCursor(i, 1);
  lcd.print("->");
  lcd.setCursor(11, 1);
  lcd.print("START");
  }

class Tab{                                                //clasa pentru paleta
protected:
  byte pos;                                               //configuratia paletei care se modifica in functie de pozitie
  int PIN;                                                //camp care ne ajuta sa stim la ce player aducem modificarile
public:
  Tab(int potPin){                                        //constructor parametrizat. Pune initial paleta la mijlocul liniei 
    PIN = potPin;
    pos = B00111100;
    }
    
  Tab(byte x, int potPin){
    pos = x; 
    PIN = potPin;
    pos = B00111100;
    }
  
  ~Tab(){}
  
  byte getPos(){ 
    return pos;
    }
  
  int getPIN(){ 
    return PIN;
    }
  
  void setPos(byte x){
    pos = x;
  }
  
  void setPIN(int pot){ PIN = pot; }
};

class Player : public Tab{                                //clasa Player. jucatorul contine o paleta, scor si index.
    int index;
    int score;
  public:
    Player(int pin, int i):Tab(pin){
    index = i;
    score = 0;
  }
  
  Player operator++(){
    score++;
    return *this;
  }
  
  int getScore(){ 
    return score; 
    }
    
  void init(){                                           // la replay, scorul devine iar 0
    score = 0; 
    }
  
  bool startGame(long& previousMillis, long& previousValue, int& i){ 
    long currentMillis = millis();                         
    long currentValue = analogRead(getPIN());             
    if (currentMillis - previousMillis > interval){     
      if (currentValue > previousValue + minPotSignal){      
        i++;                                              // Pentru a da start, jucatorul trebuie sa invarta potentiometrul pentru a misca sageata de pe LCD spre START.
        previousValue = currentValue;                     // Sageata se muta cu un caracter, cand diferenta dintre valorile citite la potentiometru este mai mare decat minPotSignal
        }
      lcdPrint(i, index);                                 // trimit pozitia sagetii pentru a fi actualizata pe LCD
      if (i > 11){                                        // 11 este valoarea de care trebuie sa treaca sageata. 
        lcd.clear();
        return true;                                      //true -> playerul a dat START
        }
        }
    return false;                                         //false -> sageata inca nu a ajuns la start
    }
  
  void moveTab(int& previousMillis, int& previousValue){  //functie care controleaza miscarea paletei
    byte maxLeft = B11110000;                             //marginea din stanga -> paleta nu se poate deplasa mai la stanga, pentru a nu iesi din suprafata matricei 
    byte maxRight = B00001111;                            //marginea din dreapta -> paleta nu se poate deplasa mai la dreapta, pentru a nu iesi din suprafata matricei
    long currentMillis = millis();
    long currentValue = analogRead(PIN);
    if (currentMillis - previousMillis > interval){       
      if (currentValue - previousValue > minPotSignal && pos != maxLeft){ // daca valoarea trimisa de potentiometru este mai mare decat cea anterioara si nu am ajuns in margine, paleta se misca la stanga
        pos=pos<<1;                                                       // miscarea se face printr-o siftare la stanga cu o pozitie   
        previousValue = currentValue;                                     // salvam modificarea
        }
      else 
        if (previousValue - currentValue > minPotSignal && pos != maxRight){ //idem mai sus, pentru partea dreapta
        pos = pos>>1;
        previousValue = currentValue;
        }
      previousMillis = currentMillis;
      }
    }
  
};

class Ball{                                               
  byte pos;                                               //mingea este afisata ca un punct pe matrice, iar in pos retinem pozitia pe coloana a acesteia
  int line, column;                                       //coordonatele
  int dirL, dirC;                                         //directia pentru coloana si linie. Acestea pot lua valorile -1 (scade), 1(creste), 0(nu isi schimba directia)
  long previousMillis;  
  byte maxLeft = B10000000;                               //cea mai din stanga pozitie
  byte maxRight = B00000001;                              //cea mai din dreapta pozitie
public:
  Ball(){
    line = 4;                                             //la inceput mingea este pozitionata aproximativ la mijlocul tablei de joc
    column = 3;
    pos = 1<<column;
    dirL = 1;                                             //ambele coordonate cresc
    dirC = 1;
    previousMillis = 0;
  }
  
  void ballInit(){                                        //la fiecare meci nou, mingea este pozitionata la mijlocul tablei
    int randomDirection;
    column = 4;
    pos = 1<<column;
    line = 4;
    randomDirection = random(-1, 2);                      //directia de deplasare este aleasa random
    if (randomDirection == 0)                             //evitam deplasarea pe aceeasi linie/coloana prin modificarea directiei in cazul =0
      randomDirection = 1;
    dirL=randomDirection;
    randomDirection = random(-1, 2);
    if (randomDirection == 0) 
      randomDirection = 1;
    dirC = randomDirection;
    previousMillis = 0;
  }
  
  int getDirL(){
    return dirL;
  }
  int getDirC(){
    return dirC;
  }
  void moveBall(){                                         
    long currentMillis = millis();
    if (currentMillis - previousMillis > ballMovingTime){  //la un anumit interval de timp, mingea isi schimba pozitia
      line = line + dirL;                                  //linia creste/scade/ramane constanta in functie de directie
      column = column + dirC;                              //coloana creste/scade/ramane constanta in functie de directie
      pos = 1<<column;
      previousMillis = currentMillis;
      }
  }
  
  byte getPos(){
    return pos;
  }
  
  int getLine(){
    return line;
  }
  
  int getColumn(){
    return column;
  }
  
  void setDirL(int d){
    dirL = d;
    }
  
  void setDirC(int d){ 
    dirC = d;
    }
  
  void touchWall(){                                         //verificam daca mingea a atins peretii laterali
    if (pos == maxLeft){                                    //daca a atins peretele din stanga
      dirC = -1;                                            //coloana trebuie sa scada. Mingea nu trebuie sa iasa dim suprafata matricei
      long randomDirection = random(-1, 2);                 //directia liniei va fi aleasa random
      if (randomDirection == 0) 
        randomDirection = 1;
      dirL = randomDirection;
      }
    if (pos == maxRight){                                   //daca a atins peretele din dreapta
      dirC = 1;                                             //coloana trebuie sa creasca, pentru a nu iesi mingea din suprafata matricei
      long randomDirection = random(-1,2);                  //directia liniei este aleasa random
      if (randomDirection == 0) 
        randomDirection = -1;
      dirL = randomDirection;
      }
    } 
};

class Game{                                                 //jocul va contine:
  Player player1;                                           //doi playeri
  Player player2;
  Ball ball;                                                //o minge
  byte image[8];                                            //configuratia matricei
  int arrow;                                                //sageata pentru start
  int winner;                                               //castigatorul
  int state;                                                //1-> start player1, 2->start player2, 3->play the game
public:
  long previousValue1, previousMillis1;
  long previousValue2, previousMillis2;
  
  Game(): player1(FIRST_PLAYER_PIN, 1), player2(SECOND_PLAYER_PIN, 2){
      state = 1;                                            //partea de start
      winner = 0;
      previousMillis1 = previousValue1 = 0;
      previousValue2 = previousMillis2 = 0;
      arrow = 0;
      byte img[8] ={                                        //in partea de start game, pe matrice apare un smiley face
        B00000000,
        B01100110,
        B01100110,
        B00000000,
        B00000000,
        B10000001,
        B01000010,
        B00111100
      };
      for (int i = 0; i < 8; i++)
        image[i] = img[i];
      }
      
  void phaseZero(){                                         //smiley-face pentru momentele in care nu se foloseste matricea
    byte img[8] ={
     B00000000,
     B01100110,
     B01100110,
     B00000000,
     B00000000,
     B10000001,
     B01000010,
     B00111100
    };
    for (int i = 0; i < 8; i++)
      image[i] = img[i];
    printMatrix();
    }
      
  void phaseOne(){                                           //imaginea default a jocului. Contine paletele care apoi vor fi shiftate, iar linia pe care se adauga mingea va fi modificata
    byte img[8] ={
      B00111100,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00111100
      };
    for (int i = 0; i < 8; i++)
      image[i] = img[i];
    }
      
  bool start(){                                                       
    switch (state){
      case 1:{                                                            //partea de start pentru player1
        bool done = false;
        done = player1.startGame(previousMillis1, previousValue1, arrow); //apelez functia startGame din clasa Player care intoarce true-> a ajuns la start sau false -> inca nu a ajuns la start
        if (done == true){                                                //daca s-a dat start, se trece la player2
          state = 2;
          previousMillis1 = previousValue1 = arrow = 0;
          }
        break;
        }
      case 2:{
        bool done = false;                                                //partea de start pentru player2
        done = player2.startGame(previousMillis2, previousValue2, arrow);
        if (done == true){
          state = 3;                                                      //daca s-a dat start, se trece la state 3, se incepe jocul
          previousMillis2 = previousValue2 = arrow = 0;
          phaseOne();
          }
        break;
        }
      case 3:{
        updateScore();                                                    //daca jocul a inceput, la state 3 doar actualizam scorul
        break;
        }
      }
    if (state == 3) 
      return true;
    return false;
    }
    
  void printMatrix(){                                                     //afisarea jocului pe matrice
    for (int i = 0; i < 8; i++)
      lc.setRow(0, i, image[i]);
      }
    
  void updatePlayers(){                                                   //miscarea paletelor
    int pm1, pv1, pm2, pv2;                                               //in aceste variabile retin valorile previousMillis si previousValue, pentru a putea fi trimise ca parametru in functii si modificate
    pm1 =  previousMillis1;
    pv1 = previousValue1;
    player1.moveTab(pm1, pv1);                                            //mutam paleta playerului1
    previousMillis1 = pm1;      
    previousValue1 = pv1;
    pm2 =  previousMillis2;                                           
    pv2 = previousValue2;
    player2.moveTab(pm2, pv2);                                            //mutam paleta playerului2
    previousMillis2= pm2;
    previousValue2 = pv2;     
    image[0] = player1.getPos();                                          //modificam in matrice paletele
    printMatrix();
    image[7] = player2.getPos();                                      
    printMatrix();                                                        //actualizam matricea
    }
    
  void updateBall(){                                                      //modific mingea        
    int ballLine = ball.getLine();                                        //ii salvez pozitia initiala
    image[ballLine]= B00000000;                                           //sterg mingea de pe pozitia respectiva 
    ball.moveBall();                                                      //apelez functia care calculeaza noua pozitie a acesteia
    verifyBall();                                                         //verific daca a atins peretii sau daca s-a ciocnit de una dintre palete
    byte ballPosition = ball.getPos();                                    //ii salvez noua pozitie
    ballLine = ball.getLine();
    image[ballLine] = ballPosition;                                       //adaug in matrice noua pozitie 
    printMatrix();                                                        //actualizez matricea
    }
    
  void verifyBall(){                                                     
    ball.touchWall();                                                     //verific daca mingea a atins peretii laterali
    if (ball.getLine() == 0){                                             //daca a ajuns pe linia 0, a ajuns pe linia playerului 1                     
      byte result;
      byte posb, posp; 
      posb = ball.getPos();                                               //salvez pozitia pe care se afla mingea sub forma de byte
      posp = player1.getPos();                                            //salvez pozitia pe care se afla paleta playerului1 sub forma de byte
      result = posb | posp;                                               //aplic operatia sau pe cele doua pozitii
      if (result == posp){                                                //daca rezultatul este acelasi byte ca si pozitia paletei, inseamna ca mingea a atins paleta
        long randomDirection = random(-1, 2);                             //schimb directia in care merge mingea, directia este random
        ball.setDirL(1);
        ball.setDirC(randomDirection);
      }
      else{                                                               //daca rezultatul nu este acelasi ca pozitia playerului, inseamna ca mingea nu a atins paleta
        player2++;                                                        //celalalt jucator primeste un punct
        updateScore();                                                    //actualizam scorul pe LCD
        reset();                                                          //resetam jocul
      }
    }
    else 
      if (ball.getLine() == 7){                                           //daca mingea a ajuns pe linia 7, a ajuns pe linia playerului 2
        byte result, posb, posp;                                          //reiau aceeasi pasi ca si pentru linia 0
        posb = ball.getPos();
        posp = player2.getPos();
        result = posb | posp;
        if(result == posp){
        // the ball touched the paddle -> change direction
          ball.setDirL(-1);
          long randomDirection = random(-1, 2);
          ball.setDirC(randomDirection);
          }
      else{
        //the ball is out of bounds
        player1++;
        updateScore(); 
        reset();
        }
    }
  }
  
  void updateScore(){                                           //functie care afiseaza pe LCD scorul celor 2 jucatori
    lcd.setCursor(0, 0);
    lcd.print("Player1 Player2");
    lcd.setCursor(3, 1);
    lcd.print(player1.getScore());
    lcd.setCursor(12, 1);
    lcd.print(player2.getScore());
    }
 
  void winn(){                                                  //functie care afiseaza pe LCD castigatorul
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("winner: ");
    lcd.setCursor(2, 1);
    lcd.print(winner);
    }


  void reset(){                                                 //cand un jucator scapa mingea, jocul se reseteaza
    int win = 0;
    if (player1.getScore() >= 6){                               //daca unul dintre jucatori a ajuns la scor = 6, jocul s-a terminat
      win = 1;
      winner = 1;
      lcd.print(1);                                            //afisez castigator
      lcd.clear();
      stopp = true;                                             //stop joc
      }
    if (player2.getScore() >= 6){
      win = 2;
      winner = 2;
      lcd.clear();
      stopp = true;
    }
    phaseOne();                                               //se pune configuratia default pe matrice
    player1.setPos(B00111100);
    player2.setPos(B00111100);
    ball.ballInit();                                          //initializez din nou mingea
    }

  void init(){                                                //initializare pentru un joc nou
    state = 1;                                                //clasa Game, primeste state = 1, se reincepe cu START
    winner = 0;
    previousMillis1 = previousValue1 = 0;
    previousValue2 = previousMillis2 = 0;
    arrow = 0;                      
    player1.init();                                           //initializez ambii jucatori (scor = 0)
    player2.init();
    }
};



void setup(){
  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
  lcd.begin(16, 12);
  pinMode(A0, INPUT);                                         //potentiometru player1
  pinMode(A1, INPUT);                                         //potentiometru player2
  pinMode(V0_PIN, OUTPUT);
  analogWrite(V0_PIN, 120); 
}

Game pong;
bool play = false;

void replay(){
  int pot1, pot2;
  byte img[8] ={
    B00000000,
    B01100110,
    B01100110,
    B00000000,
    B00000000,
    B10000001,
    B01000010,
    B00111100
  };
  for (int i = 0; i <8; i++)
    lc.setRow(0, i, img[i]);
  pot1 = analogRead(FIRST_PLAYER_PIN);
  pot2 = analogRead(SECOND_PLAYER_PIN);
  if (pot1 < 15 && pot2 < 15){                              //pentru a da replay, ambele potentiometre trebuie sa se indrepte spre valoarea 0
    play = false;
    stopp = false;
    pong.init();
   }
}

void loop(){
  if (stopp == false){
    pong.printMatrix();
    if (play == false) {
    play = pong.start();
      }
  
  if(play == true){
    pong.updatePlayers();
    pong.updateBall();
    pong.updateScore();
  }
  }
  else{
    pong.winn();
    replay();
  }
}
