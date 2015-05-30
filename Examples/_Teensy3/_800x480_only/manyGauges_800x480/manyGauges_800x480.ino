/*
ROUND GAUGE EXAMPLE with ballistic!
This example show how to create 3 round gauge that react like the real one with (almost) correct ballistic
The 3 gauges read analog values from A0,A1 & A2
Created by S.U.M.O.T.O.Y - Max MC Costa
If you modify or get better result please let me know
*/
#include <SPI.h>
#include <RA8875.h>
#include <math.h>


volatile int16_t curVal[6] = {-1, -1, -1, -1, -1, -1};
volatile int16_t oldVal[6] = {0, 0, 0, 0, 0, 0};
const int16_t posx[6] = {63, 193, 323, 453, 583, 713};
const int16_t posy[6] = {63, 63, 63, 63, 63, 63};
const int16_t radius[6] = {63, 63, 63, 63, 63, 63};
const uint8_t analogIn[6] = {A0, A1, A2, A3, A8, A9};
const uint16_t needleColors[6] = {RA8875_GREEN, RA8875_CYAN, RA8875_MAGENTA, RA8875_YELLOW, RA8875_LIGHT_ORANGE, RA8875_RED};
const uint8_t degreesVal[6][2] = {
{150,240},
{150,240},
{150,240},
{150,240},
{150,240},
{150,240},
};

/*
Teensy3.x
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x
 MISO:  12//Teensy3.x
 SCK:   13//Teensy3.x
 the rest of pin below:
 */
#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or nothing!
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
*/



RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);


void setup() {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_800x480);
  for (uint8_t i = 0; i < 6; i++) {
    drawGauge(posx[i], posy[i], radius[i]);
  }
}

void loop(void) {
  for (uint8_t i = 0; i < 6; i++) {
    curVal[i] = random(255);
    //curVal[i] =255;
    drawNeedle(i,RA8875_BLACK);
    //delay(1000);
    /*
    curVal[i] = map(analogRead(analogIn[i]), 0, 1024, 1, 255);
    drawNeedle(i,RA8875_BLACK);
    */
  }
}


void drawGauge(uint16_t x, uint16_t y, uint16_t r) {
  tft.drawCircle(x, y, r, RA8875_WHITE); //draw instrument container
  faceHelper(x, y, r, 150, 390, 1.3); //draw major ticks
  if (r > 15) faceHelper(x, y, r, 165, 375, 1.1); //draw minor ticks

}

void faceHelper(uint16_t x, uint16_t y, uint16_t r, int from, int to, float dev) {
  float dsec,rdev;
  uint16_t w,h,nx,ny;
  int i;
  rdev = r / dev;
  for (i = from; i <= to; i += 30) {
    dsec = i * (PI / 180.0);
    nx = (uint16_t)(1 + x + (cos(dsec) * rdev));
    ny = (uint16_t)(1 + y + (sin(dsec) * rdev));
    w =  (uint16_t)(1 + x + (cos(dsec) * r));
    h =  (uint16_t)(1 + y + (sin(dsec) * r));
    tft.drawLine(nx, ny, w, h, RA8875_WHITE);
  }
}

void drawNeedle(uint8_t index, uint16_t bcolor) {
  uint16_t i;
  if (oldVal[index] != curVal[index]) {
  if (curVal[index] > oldVal[index]) {
    for (i = oldVal[index]; i <= curVal[index]; i++) {
      if (i > 0) drawPointerHelper(index,i - 1, posx[index], posy[index], radius[index], bcolor);
      drawPointerHelper(index,i, posx[index], posy[index], radius[index], needleColors[index]);
      if ((curVal[index] - oldVal[index]) < (128)) delay(1);//ballistic
    }
  }
  else {
    for (i = oldVal[index]; i >= curVal[index]; i--) {
      drawPointerHelper(index,i + 1, posx[index], posy[index], radius[index], bcolor);
      drawPointerHelper(index,i, posx[index], posy[index], radius[index], needleColors[index]);
      //ballistic
      if ((oldVal[index] - curVal[index]) >= 128) {
        delay(1);
      } else {
        delay(3);
      }
    }
  }
  oldVal[index] = curVal[index];
  }
  //oldVal[index] = curVal[index];
}

void drawPointerHelper(uint8_t index,int16_t val, uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
  float dsec;
  const int16_t minValue = 0;
  const int16_t maxValue = 255;
  if (val > maxValue) val = maxValue;
  if (val < minValue) val = minValue;
  dsec = (((float)(uint16_t)(val - minValue) / (float)(uint16_t)(maxValue - minValue) * degreesVal[index][1]) + degreesVal[index][0]) * (PI / 180);
  uint16_t w = (uint16_t)(1 + x + (cos(dsec) * (r / 1.35)));
  uint16_t h = (uint16_t)(1 + y + (sin(dsec) * (r / 1.35)));
  /*
  min: x:713 | y:63 | w:673 | h:87
  min: x:713 | y:63 | w:754 | h:87
  */
  /*
  if (index == 5){
  Serial.print("x:");
  Serial.print(x);
  Serial.print(" | y:");
  Serial.print(y);
  Serial.print(" | w:");
  Serial.print(w);
  Serial.print(" | h:");
  Serial.print(h);
  Serial.print("\n");
  }
  */
  tft.drawLine(x, y, w, h, color);
  tft.fillCircle(x, y, 2, color);
}
