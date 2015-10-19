/*
A similar clock example but this time uses some RA8875 gauges library function!
 */

#include <SPI.h>
#include <RA8875.h>

#define RA8875_RESET 9//any pin or nothing!

#if defined(NEEDS_SET_MODULE)//Energia, this case is for stellaris/tiva

RA8875 tft = RA8875(3);//select SPI module 3
/*
for module 3 (stellaris)
 SCLK:  PD_0
 MOSI:  PD_3
 MISO:  PD_2
 SS:    PD_1
 */
#endif


int16_t clockPos[3] = {0, 0, 50}; //x,y,r
uint8_t currentTime[3] = {0, 0, 0}; //hh,mm,ss

//background,face,hh,mm,ss
const uint16_t clockColors[5] = {RA8875_BLACK, RA8875_BLACK, RA8875_CYAN, RA8875_BLUE, RA8875_RED};

//houX,houY,minX,minY,secX,secY
uint16_t oldPos[6] = {0, 0, 0, 0, 0, 0};
unsigned long targetTime = 0;


void setup() {
  //  Serial.begin(38400);
  //  long unsigned debug_start = millis ();
  //  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_800x480);
  clockPos[0] = tft.width() / 2;
  clockPos[1] = tft.height() / 2;
  if (tft.isPortrait()){
    clockPos[2] = (tft.width()/2) - 20;
  } else {
    clockPos[2] = (tft.height()/2) - 20;
  }
  //clockPos[2] = 128;
  oldPos[0] = clockPos[0]; oldPos[1] = clockPos[1];
  oldPos[2] = clockPos[0]; oldPos[3] = clockPos[1];
  oldPos[4] = clockPos[0]; oldPos[5] = clockPos[1];
  tft.brightness(150);
  drawGauge(clockPos, 0, 360);
  //get current time from compiler
  currentTime[0] = conv2d(__TIME__);
  currentTime[1] = conv2d(__TIME__ + 3);
  currentTime[2] = conv2d(__TIME__ + 6);
  targetTime = millis() + 1000;
}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis() + 1000;
    currentTime[2]++;
    if (currentTime[2] == 60) {
      currentTime[2] = 0;
      currentTime[1]++;
      if (currentTime[1] > 59) {
        currentTime[1] = 0;
        currentTime[0]++;
        if (currentTime[0] > 23) currentTime[0] = 0;
      }
    }
    drawClockHands(clockPos, currentTime, clockColors);
  }
}


void drawGauge(int16_t pos[], uint16_t from, uint16_t to) {
  tft.drawCircle(pos[0], pos[1], pos[2], RA8875_WHITE); //draw instrument container
  tft.drawCircle(pos[0], pos[1], pos[2] + 1, RA8875_WHITE); //draw instrument container
  tft.roundGaugeTicker(pos[0], pos[1], pos[2], from, to, 1.3, RA8875_WHITE);
  if (pos[2] > 15) tft.roundGaugeTicker(pos[0], pos[1], pos[2], from + 15, to - 15, 1.1, RA8875_WHITE); //draw minor ticks
}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9') v = *p - '0';
  return 10 * v + *++p - '0';
}

void drawClockHands(int16_t pos[], uint8_t curTime[], const uint16_t colors[]) {

  uint8_t hlen = (pos[2] / 2 - pos[2] / 12);
  uint8_t mlen = pos[2] / 2 - pos[2] / 4;
  uint8_t slen = pos[2] / 2 - pos[2] / 4;
  float hx = cos(((curTime[0] * 30 + (curTime[1] * 6 + (curTime[2] * 6) * 0.01666667) * 0.0833333) - 90) * 0.0174532925);
  float hy = sin(((curTime[0] * 30 + (curTime[1] * 6 + (curTime[2] * 6) * 0.01666667) * 0.0833333) - 90) * 0.0174532925);
  float mx = cos(((curTime[1] * 6 + (curTime[2] * 6) * 0.01666667) - 90) * 0.0174532925);
  float my = sin(((curTime[1] * 6 + (curTime[2] * 6) * 0.01666667) - 90) * 0.0174532925);
  float sx = cos(((curTime[2] * 6) - 90) * 0.0174532925);
  float sy = sin(((curTime[2] * 6) - 90) * 0.0174532925);

  // Erase just old hand positions
  tft.drawLine(oldPos[0], oldPos[1], pos[0] + 1, pos[1] + 1, colors[1]);
  tft.drawLine(oldPos[2], oldPos[3], pos[0] + 1, pos[1] + 1, colors[1]);
  tft.drawLine(oldPos[4], oldPos[5], pos[0] + 1, pos[1] + 1, colors[1]);
  // Draw new hand positions
  tft.drawLine(hx * (pos[2] - hlen) + pos[0] + 1, hy * (pos[2] - hlen) + pos[1] + 1, pos[0] + 1, pos[1] + 1, colors[2]);
  tft.drawLine(mx * (pos[2] - mlen) + pos[0] + 1, my * (pos[2] - mlen) + pos[1] + 1, pos[0] + 1, pos[1] + 1, colors[3]);
  tft.drawLine(sx * (pos[2] - slen) + pos[0] + 1, sy * (pos[2] - slen) + pos[1] + 1, pos[0] + 1, pos[1] + 1, colors[4]);
  tft.fillCircle(pos[0] + 1, pos[1] + 1, 3, colors[4]);

  // Update old x&y coords
  oldPos[4] = sx * (pos[2] - slen) + pos[0] + 1;
  oldPos[5] = sy * (pos[2] - slen) + pos[1] + 1;
  oldPos[2] = mx * (pos[2] - mlen) + pos[0] + 1;
  oldPos[3] = my * (pos[2] - mlen) + pos[1] + 1;
  oldPos[0] = hx * (pos[2] - hlen) + pos[0] + 1;
  oldPos[1] = hy * (pos[2] - hlen) + pos[1] + 1;
}