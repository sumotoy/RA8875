/*
The simpliest way to draw a Round Gauge by using the RA8875 library
functions.
This will draw 3 round gauges
*/
#include <SPI.h>
#include <RA8875.h>


#define RA8875_CS 10
#define RA8875_RESET 9//any pin or nothing!

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

//gauge position x,y,radius
const int gaugePos1[3] = {64, 64, 63};
const int gaugePos2[3] = {gaugePos1[0] * 3 + 4 + 1, gaugePos1[1], gaugePos1[2]};
const int gaugePos3[3] = {gaugePos1[0] * 5 + 8 + 1, gaugePos1[1], gaugePos1[2]};

//Radians limit anglemin anglemax
const int gaugeLimits1[3] = {150, 390};
const int gaugeLimits2[3] = {150, 390};
const int gaugeLimits3[3] = {150, 390};

//value containers
volatile int16_t gaugeVal1[2] = {0, -1};
volatile int16_t gaugeVal2[2] = {0, -1};
volatile int16_t gaugeVal3[2] = {0, -1};

void setup() {
  tft.begin(RA8875_800x480);
  tft.brightness(160);
  drawGauge(gaugePos1, gaugeLimits1);
  drawGauge(gaugePos2, gaugeLimits2);
  drawGauge(gaugePos3, gaugeLimits3);
}

void loop(void) {
  gaugeVal1[0] = random(1023);
  gaugeVal2[0] = random(1023);
  gaugeVal3[0] = random(1023);
  gaugeVal1[1] = drawNeedle_(gaugeVal1, gaugePos1, gaugeLimits1, RA8875_RED);
  gaugeVal2[1] = drawNeedle_(gaugeVal2, gaugePos2, gaugeLimits2, RA8875_GREEN);
  gaugeVal3[1] = drawNeedle_(gaugeVal3, gaugePos3, gaugeLimits3, RA8875_BLUE);
  delay(150);
}

int16_t drawNeedle_(volatile int16_t val[], const int pos[], const int limits[], uint16_t color) {
  if (val[0] != val[1]) {
    uint16_t len = pos[2] / 1.35;
    int angleMax = limits[1] - limits[0];
    tft.drawLineAngle(pos[0], pos[1], map(val[1], 0, 1023, 0, angleMax), len, RA8875_BLACK, limits[0]);
    tft.drawLineAngle(pos[0], pos[1], map(val[0], 0, 1023, 0, angleMax), len, color, limits[0]);
    val[1] = val[0];
  }
  return val[1];
}

void drawGauge(const int pos[], const int limits[]) {
  tft.drawCircle(pos[0], pos[1], pos[2], RA8875_WHITE); //draw instrument container
  tft.drawCircle(pos[0], pos[1], pos[2] + 1, RA8875_WHITE); //draw instrument container
  tft.roundGaugeTicker(pos[0], pos[1], pos[2], limits[0], limits[1], 1.3, RA8875_WHITE);
  if (pos[2] > 15) tft.roundGaugeTicker(pos[0], pos[1], pos[2], limits[0] + 15, limits[1] - 15, 1.1, RA8875_WHITE); //draw minor ticks
}