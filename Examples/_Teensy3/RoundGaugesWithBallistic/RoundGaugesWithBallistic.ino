/*
ROUND GAUGE EXAMPLE with ballistic!
This example show how to create 3 round gauge that react like the real one with (almost) correct ballistic
The 3 gauges read analog values from A0,A1 & A2
Created by S.U.M.O.T.O.Y - Max MC Costa
If you modify or get better result please let me know
*/
#include <SPI.h>
#include <RA8875.h>

volatile int16_t curVal1 = 0;
volatile int16_t oldVal1 = 0;
volatile int16_t curVal2 = 0;
volatile int16_t oldVal2 = 0;
volatile int16_t curVal3 = 0;
volatile int16_t oldVal3 = 0;


#define RA8875_CS 10
#define RA8875_RESET 9//any pin or nothing!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);


void setup() {
  //Serial.begin(9600);
  tft.begin(RA8875_800x480);
  drawGauge(63, 63, 63);
  drawGauge(63 * 3 + 4, 63, 63);
  drawGauge(63 * 5 + 8, 63, 63);
}

void loop(void) {

  curVal1 = map(analogRead(A0), 0, 1023, 1, 254);
  curVal2 = map(analogRead(A1), 0, 1023, 1, 254);
  curVal3 = map(analogRead(A2), 0, 1023, 1, 254);

  if (oldVal1 != curVal1) {
    drawNeedle(curVal1, oldVal1, 63, 63, 63, RA8875_GREEN, RA8875_BLACK);
    oldVal1 = curVal1;
  }
  if (oldVal2 != curVal2) {
    drawNeedle(curVal2, oldVal2, 63 * 3 + 4, 63, 63, RA8875_CYAN, RA8875_BLACK);
    oldVal2 = curVal2;
  }
  if (oldVal3 != curVal3) {
    drawNeedle(curVal3, oldVal3, 63 * 5 + 8, 63, 63, RA8875_MAGENTA, RA8875_BLACK);
    oldVal3 = curVal3;
  }
}

void drawGauge(uint16_t x, uint16_t y, uint16_t r) {
  tft.drawCircle(x, y, r, RA8875_WHITE); //draw instrument container
  tft.roundGaugeTicker(x, y, r, 150, 390, 1.3, RA8875_WHITE); //draw major ticks
  if (r > 15) tft.roundGaugeTicker(x, y, r, 165, 375, 1.1, RA8875_WHITE); //draw minor ticks

}



void drawNeedle(int16_t val, int16_t oval, uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint16_t bcolor) {
  uint16_t i;
  if (val > oval) {
    for (i = oval; i <= val; i++) {
      drawPointerHelper(i - 1, x, y, r, bcolor);
      drawPointerHelper(i, x, y, r, color);
      if ((val - oval) < (128)) delay(1);//ballistic
    }
  }
  else {
    for (i = oval; i > val; i--) {
      drawPointerHelper(i + 1, x, y, r, bcolor);
      drawPointerHelper(i, x, y, r, color);
      //ballistic
      if ((oval - val) >= 128) {
        delay(1);
      } else {
        delay(3);
      }
    }
  }
}

void drawPointerHelper(int16_t val, uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
  float dsec, toSecX, toSecY;
  int16_t minValue = 0;
  int16_t maxValue = 255;
  float fromDegree = 150.0;//start
  float toDegree = 240.0;//end
  if (val > maxValue) val = maxValue;
  if (val < minValue) val = minValue;
  dsec = (((float)(val - minValue) / (float)(maxValue - minValue) * toDegree) + fromDegree) * (PI / 180.0);
  toSecX = cos(dsec) * (r / 1.35);
  toSecY = sin(dsec) * (r / 1.35);
  tft.drawLine(x, y, 1 + x + (int16_t)toSecX, 1 + y + (int16_t)toSecY, color);
  tft.fillCircle(x, y, 2, color);
}