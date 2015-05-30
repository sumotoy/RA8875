/*
A Ping Pong game for 1 user, you can use the capacitive touch screen or a pot connected to A0
If you have a buzzer connected to pin 8 you will have sound as well.
 */

#include <SPI.h>
#include <RA8875.h>
#if defined(USE_FT5206_TOUCH)
#include <Wire.h>
#endif

#define RA8875_CS 10//53 for MEGA2560
#define RA8875_RESET 9
#if defined(USE_FT5206_TOUCH)
#define RA8875_INT        2
#endif
#define BEEPER_PIN 8

uint8_t    ballSize = 8;
uint16_t   ballColor = RA8875_YELLOW;
uint8_t    ballSpeed = 10; // lower numbers are faster
uint8_t    paddleH = 5;
uint8_t    paddleW = 30;
int16_t    paddleX = 20;


int16_t paddleY = 0;
int16_t oldPaddleY;


int16_t scoreHumanTextX = 0;
int16_t scoreCpuTextX = 0;
int16_t ballTextX = 0;
int16_t scoreTextY = 0;

bool launchBall = true;

int humanScore = 0;
int cpuScore = 0;
uint8_t maxBalls = 10;
bool restart = true;

#if defined(USE_FT5206_TOUCH)
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET, RA8875_INT);
#else
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
#endif

int16_t ballDirX = 1;
int16_t ballDirY = 1;
int16_t ballX = 0;
int16_t ballY = 0;
int16_t oldBallX = -1;
int16_t oldBallY = -1;
uint16_t grey = tft.Color565(100, 100, 100);


void setup() {
  tft.begin(RA8875_800x480);
#if defined(USE_FT5206_TOUCH)
  tft.setTouchLimit(1);
  tft.armTouchISR(true);//touch screen interrupt it's armed
#endif
  scoreTextY = tft.height() - (30 + 1 + (ballSize + 1)) + 15;
}



void loop() {
  if (restart) {
    restart = false;
    restartGame();
  }
  paddleRead();

  // update the ball's position and draw it on screen
  if (millis() % ballSpeed < 2)  moveBall();

}



void moveBall() {
  bool started = false;
  if (launchBall) {
    launchBall = false;
    ballDirX =  random(1, 5);
    ballDirY =  random(1, 5);
    ballX = (tft.width() - 1) - (ballSize + ballDirY) ;
    ballY = (tft.height() / 2) + random(-30, 30);
    oldBallX = -1;
    oldBallY = -1;
    started = true;
  } else {
    bool reverseDirX = false;
    bool reverseDirY = false;
    if (ballX > tft.width() - (ballSize + 2)) reverseDirX = true;//RIGHT

    if (ballX < 0) {//missed ball!--------------------------------------
      tft.fillRect(oldBallX, oldBallY, ballSize, ballSize, RA8875_BLACK);
      tft.drawFastVLine(tft.width() - 1, 0, tft.height() - (30 + 1), grey);
      tone(BEEPER_PIN, 800, 100);
      launchBall = true;
      //delay(1000);
      maxBalls--;
      cpuScore += 100;
      setScore();
      if (maxBalls < 1) {
        tft.setCursor(CENTER, CENTER);
        tft.setFontScale(2);
        if (cpuScore > humanScore) {
          tft.print("YOU LOOSE!");
        } else if (cpuScore == humanScore) {
          tft.print("PARITY!");
        } else {
          tft.print("YOU WIN!");
        }
        setBallScore(0);
        setScore();
        delay(5000);
        tft.fillWindow();
        restart = true;
        return;
      } else {
        setBallScore(maxBalls);
        delay(1000);
        return;
      }
    }

    if (reverseDirX) {
      ballDirX = -ballDirX;
      beeper(false);
    }

    if ((ballY > tft.height() - (30 + 1 + (ballSize + 5)))) reverseDirY = true;//BOTTOM
    if (ballY < (ballSize)) reverseDirY = true;//TOP
    if (reverseDirY) {
      ballDirY = -ballDirY;
      beeper(false);
    }

    if (inPaddle(ballX, ballY, paddleX, paddleY, paddleH, paddleW)) {
      beeper(true);
      humanScore += 100;
      setScore();
      ballDirX = -ballDirX;
      ballDirY = -ballDirY;
    }

    ballX += ballDirX;
    ballY += ballDirY;
  }

  if (oldBallX != ballX || oldBallY != ballY) tft.fillRect(oldBallX, oldBallY, ballSize, ballSize, RA8875_BLACK);
  tft.fillRect(ballX, ballY, ballSize, ballSize, ballColor);
  if (started)tft.drawFastVLine(tft.width() - 1, 0, tft.height() - (30 + 1), grey);
  oldBallX = ballX;
  oldBallY = ballY;
}


void restartGame() {
  humanScore = 0;
  cpuScore = 0;
  maxBalls = 10;
  launchBall = true;
  //tft.clearScreen();

  tft.setFontScale(0);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.setCursor(30, scoreTextY);
  tft.print("Human: ");
  scoreHumanTextX = tft.getCursorX();
  tft.setCursor(120, scoreTextY);
  tft.print("CPU: ");
  scoreCpuTextX = tft.getCursorX();
  tft.setCursor(200, scoreTextY);
  tft.print("BALL: ");
  ballTextX = tft.getCursorX();
  tft.setCursor(tft.width() - 120, scoreTextY);
  tft.print("[ Damn Pong! ] ");
  tft.drawFastHLine(paddleX, tft.height() - (30 + 1), tft.width() - 1, grey);//bottom
  tft.drawFastHLine(paddleX, 0, tft.width() - 1, grey);//top
  tft.drawFastVLine(tft.width() - 1, 0, tft.height() - (30 + 1), grey);
  setScore();
  setBallScore(maxBalls);
}



// this function checks the position of the ball
// to see if it intersects with the paddle
boolean inPaddle(int16_t x, int16_t y, int16_t rectX, int16_t rectY, uint8_t rectWidth, uint8_t rectHeight) {
  if ((x >= rectX && x <= (rectX + rectWidth)) && (y >= rectY && y <= (rectY + rectHeight))) return true;
  return false;
}

void paddleRead(void) {
  //uint16_t test = random(100,1024);
  uint16_t test = 700;
#if defined(USE_FT5206_TOUCH)
  if (tft.touched()) {
    tft.updateTS();
    uint16_t coordinates[1][2];
    tft.getTScoordinates(coordinates);
    paddleY =  map(coordinates[0][1], 0, tft.width(), (tft.height() - (30 + 1) - (paddleW)), 4);
    tft.armTouchISR();
  }
#else
  int temp = analogRead(A0);
  paddleY =  map(temp, 0, 1024, (tft.height() - (30 + 1) - (paddleW)), 4);
#endif
  //paddleY =  map(test, 0, 1024, (tft.height() - (30 + 1) - (paddleW)), 4);
  if (oldPaddleY != paddleY) tft.fillRect(paddleX, oldPaddleY, paddleH, paddleW, RA8875_BLACK);

  tft.fillRect(paddleX, paddleY, paddleH, paddleW, RA8875_WHITE);
  oldPaddleY = paddleY;
}

void setScore() {
  tft.setFontScale(0);
  tft.setCursor(scoreHumanTextX, scoreTextY);
  tft.print("    ");
  tft.setCursor(scoreHumanTextX, scoreTextY);
  tft.print(humanScore);
  tft.setCursor(scoreCpuTextX, scoreTextY);
  tft.print("    ");
  tft.setCursor(scoreCpuTextX, scoreTextY);
  tft.print(cpuScore);
}

void setBallScore(uint8_t ball) {
  tft.setFontScale(0);
  tft.setCursor(ballTextX, scoreTextY);
  tft.print("    ");
  tft.setCursor(ballTextX, scoreTextY);
  tft.print(ball);
}

void beeper(bool padHit) {
  if (padHit) {
    tone(BEEPER_PIN, 2000, 20);
  } else {
    tone(BEEPER_PIN, 3000, 20);
  }
}



