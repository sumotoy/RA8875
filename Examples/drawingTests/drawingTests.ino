/*
  A collection of hardware accellerated drawings
  to demostrate how fast is the RA8875
*/


#include <SPI.h>
#include <RA8875.h>

/*
You are using 4 wire SPI here, so:
 MOSI:11
 MISO:12
 SCK:13
 the rest of pin below:
 */
#define RA8875_INT 2
#define RA8875_CS 10
#define RA8875_RESET 9

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");

  if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 not ready!");
    while (1);
  }
  Serial.println("RA8875 ready");

}

void loop(){

  testdrawrects();
  testtriangles(false);
  testtriangles(true);
  randomRect(false);
  randomRect(true);
  randomCircles(false);
  randomCircles(true);
  randomLines();
  randomPoints();
}

void testdrawrects() {
  tft.fillScreen(RA8875_BLACK);
  for(int k=0;k<64;k++){
    for (uint16_t x=1; x < tft.height(); x+=6) {
      uint16_t c1 = random(0x00FF,0xFFFF);
      tft.drawRect((uint16_t)((tft.width()/2) - (x/2)), (uint16_t)((tft.height()/2) - (x/2)), x, x, c1);
    }
  }
}

void randomCircles(bool fill){
  tft.fillScreen(RA8875_BLACK);
  uint8_t k,c;
  for (k = 0; k < 32; k++) {
    for (c = 0; c < 32; c++) {
      //  coordinates
      uint16_t x = random(0,tft.width()-1);
      uint16_t y = random(0,tft.height()-1);

      uint16_t r = random(1,tft.height()/2);

      if (x - r <  0) r = x;
      if (x + r > (tft.width()-1)) r = (tft.width()-1) - x;
      if (y - r <  0) r = y;
      if (y + r > (tft.height()-1)) r = (tft.height()-1) - y;

      if (fill){
        tft.fillCircle(x, y, r,random(0x0010,0xFFFF));
      } 
      else {
        tft.drawCircle(x, y, r,random(0x00FF,0xFFFF));
      }
    }
    if (!fill) tft.fillScreen(RA8875_BLACK);
  }
}

void randomRect(bool fill){
  tft.fillScreen(RA8875_BLACK);
  uint16_t k,c;
  for (k = 0; k < 128; k++) {
    for (c = 0; c < 32; c++) {
      uint16_t cx, cy, x, y, w, h;
      //  center
      cx = random(0,tft.width()-1);
      cy = random(0,tft.height()-1);
      //  size
      w = random(1,tft.width()/3);
      h = random(1,tft.height()/3);
      //  upper-left
      x = cx - w / 2;
      y = cy - h / 2;
      if (x < 0) x = 0;
      if (y < 0) y = 0;
      //  adjust size
      if (x + w >= tft.width()) w = tft.width()-1 - x;
      if (y + h >= tft.height()) h = tft.height()-1 - y;
      if (fill){
        tft.fillRect(x, y, w, h,random(0x0010,0xFFFF));
      } 
      else {
        tft.drawRect(x, y, w, h,random(0x0010,0xFFFF));
      }

    }
    tft.fillScreen(RA8875_BLACK);
  }
}


void randomLines(){
  tft.fillScreen(RA8875_BLACK);
  uint16_t k,c;
  for (k = 0; k < tft.height(); k++) {
    for (c = 0; c < 32; c++) {
      uint16_t x1 = random(0,tft.width()-2);
      uint16_t y1 = random(0,tft.height()-2);
      uint16_t x2 = random(0,tft.width()-1);
      uint16_t y2 = random(0,tft.height()-1);
      tft.drawLine(x1, y1, x2, y2,random(0x0010,0xFFFF));
    }
    tft.fillScreen(RA8875_BLACK);
  }
}


void randomPoints(){
  tft.fillScreen(RA8875_BLACK);
  uint16_t k,c;
  for (k = 0; k < 62; k++) {
    for (c = 0; c < 1000; c++) {
      uint16_t x = random(0,tft.width()-1);
      uint16_t y = random(0,tft.height()-1);
      tft.drawPixel(x, y,random(0x0010,0xFFFF));
    }
    tft.fillScreen(RA8875_BLACK);
  }
}

void testtriangles(bool fill) {
  tft.fillScreen(RA8875_BLACK);
  uint16_t color = 0xF800;
  tft.fillScreen(RA8875_BLACK);
  uint16_t k,c;
  for (k = 1; k < tft.width()-1; k++) {
    for (c = 1; c < 40; c++) {
      if (fill){
        tft.fillTriangle(random(1,tft.width()),random(1,tft.height()),k,c,k-c,k,color);
      } 
      else {
        tft.drawTriangle(random(1,tft.width()),random(1,tft.height()),k,c,c,k,color);
      }
      color+=100;
    }
    tft.fillScreen(RA8875_BLACK);
  }
}

uint16_t halveColor(uint16_t rgb){
  return (((rgb & 0b1111100000000000) >> 12) << 11 | ((rgb & 0b0000011111100000) >>  6) <<  5 | ((rgb & 0b0000000000011111) >>  1));
}


