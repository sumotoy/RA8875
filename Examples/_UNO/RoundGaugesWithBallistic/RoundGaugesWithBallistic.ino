/*
ROUND GAUGE EXAMPLE with ballistic!
This example show how to create 3 round gauge that react like the real one with (almost) correct ballistic
The 3 gauges read analog values from A0,A1 & A2
Created by S.U.M.O.T.O.Y - Max MC Costa
If you modify or get better result please let me know
*/
#include <SPI.h>
#include <RA8875.h>



// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF


volatile int16_t curVal1 = 0;
volatile int16_t oldVal1 = 0;
volatile int16_t curVal2 = 0;
volatile int16_t oldVal2 = 0;
volatile int16_t curVal3 = 0;
volatile int16_t oldVal3 = 0;

/*
Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//Arduino UNO
 MISO:  12//Arduino UNO
 SCK:   13//Arduino UNO
 the rest of pin below:
 */
#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or nothing!
/*
Arduino's 8 bit: any
*/

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);


void setup() {
  //Serial.begin(9600);
  tft.begin(RA8875_480x272);
  drawGauge(63,63,63);
  drawGauge(63*3+4,63,63);
  drawGauge(63*5+8,63,63);
}

void loop(void) {

  curVal1 = map(analogRead(A0),0,1023,1,254);
  curVal2 = map(analogRead(A1),0,1023,1,254);
  curVal3 = map(analogRead(A2),0,1023,1,254);
  
  if (oldVal1 != curVal1){
    drawNeedle(curVal1,oldVal1,63,63,63,GREEN,BLACK);
    oldVal1 = curVal1;
  }
  if (oldVal2 != curVal2){
    drawNeedle(curVal2,oldVal2,63*3+4,63,63,CYAN,BLACK);
    oldVal2 = curVal2;
  }
  if (oldVal3 != curVal3){
    drawNeedle(curVal3,oldVal3,63*5+8,63,63,MAGENTA,BLACK);
    oldVal3 = curVal3;
  }
}

void drawGauge(uint16_t x,uint16_t y,uint16_t r) {
  tft.drawCircle(x, y, r,WHITE);//draw instrument container
  faceHelper(x,y,r,150,390,1.3);//draw major ticks
  if (r > 15) faceHelper(x,y,r,165,375,1.1);//draw minor ticks

}

void faceHelper(uint16_t x,uint16_t y,uint16_t r,int from,int to,float dev){
  float dsec,fromSecX,fromSecY,toSecX,toSecY;
  int i;
  for (i = from;i <= to;i += 30) {
    dsec = i * (PI / 180);
    fromSecX = cos(dsec) * (r / dev);
    fromSecY = sin(dsec) * (r / dev);
    toSecX = cos(dsec) * r;
    toSecY = sin(dsec) * r;
    tft.drawLine(1 + x + fromSecX,1 + y + fromSecY,1 + x + toSecX,1 + y + toSecY,WHITE);
  }
}

void drawNeedle(int16_t val,int16_t oval,uint16_t x,uint16_t y,uint16_t r,uint16_t color,uint16_t bcolor){
  uint16_t i;
  if (val > oval){
    for (i = oval; i <= val; i++){
      drawPointerHelper(i-1,x,y,r,bcolor);
      drawPointerHelper(i,x,y,r,color);
      if ((val - oval) < (128)) delay(1);//ballistic
    }
  } 
  else {
    for (i = oval; i >= val; i--){
      drawPointerHelper(i+1,x,y,r,bcolor);
      drawPointerHelper(i,x,y,r,color);
      //ballistic
      if ((oval - val) >= 128){
        delay(1);
      } else {
        delay(3);
      }
    }
  }
}

void drawPointerHelper(int16_t val,uint16_t x,uint16_t y,uint16_t r,uint16_t color) {
  float dsec, toSecX, toSecY;
  int16_t minValue = 0;
  int16_t maxValue = 255;
  int fromDegree = 150;//start
  int toDegree = 240;//end
  if (val > maxValue) val = maxValue;
  if (val < minValue) val = minValue;
  dsec = (((float)(uint16_t)(val - minValue) / (float)(uint16_t)(maxValue - minValue) * toDegree) + fromDegree) * (PI / 180);
  toSecX = cos(dsec) * (r / 1.35);
  toSecY = sin(dsec) * (r / 1.35);
  tft.drawLine(x, y, 1 + x + toSecX, 1 + y + toSecY, color);
  tft.fillCircle(x,y,2,color);
}

