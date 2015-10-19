/*
  An old school example of how to use layers and graphic boolean operation
 for fast display several vu meter bars.
 The Colored bars are created once all together, they are masked by an AND
 operation by 2 different rectangles, one white act as passTrough from the origin point to the value,
 another black rectangle starting from the value to the end of the colored bar.
 As result the operation it's really fast even on a slow SPI.
 Created by Max MC Costa for s.u.m.o.t.o.y
 */

#include <SPI.h>
#include <RA8875.h>


#define RA8875_CS 10 
#define RA8875_RESET 9//any pin or nothing!

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);


uint8_t  segW = 2;
uint8_t  barH = 15;

//this draw the masking bar(s)
void drawHbarVal(uint16_t originX, uint16_t originY, uint8_t segments, uint8_t maxVal) {
  //tft.fillRect(originX+segW*(2*segments-1)+2,originY+2,segW,barH-4,RA8875_WHITE);//single dot
  tft.fillRect(originX + 2, originY + 2, segW * (2 * segments - 1), barH - 4, RA8875_WHITE);
  tft.fillRect(originX + 2 + segW * (2 * segments - 1), originY + 2, (2 + segW * (2 * maxVal - 1)) - (segW * (2 * segments - 1) + 2), barH - 4, RA8875_BLACK);
  tft.drawRect(originX, originY, segW * (2 * maxVal - 1) + 4, barH, tft.Color565(255, 255, 255));
}

//draw the colored bar
void drawHbar(uint16_t originX, uint16_t originY, uint8_t segments, uint8_t maxVal) {
  uint16_t spacer = 0;
  uint8_t div = maxVal / 4;
  uint8_t rc, gc, bc;
  for (uint8_t i = 0; i < segments; i++) {
    if (i > 0) spacer = segW * (2 * i);
    rc = map(i, 0, maxVal, 100, 255);
    gc = map(i, maxVal, 0, 100, 255);
    if (i >= div && i <= div * 2) {
      bc = map(i, 0, div * 2, 0, 180);
    }
    else if (i >= div * 2 && i <= div * 3) {
      bc = map(i, div * 2, div * 3, 180, 0);
    }
    else {
      bc = 0;
    }
    tft.fillRect(originX + spacer + 2, originY + 2, segW, barH - 4, tft.Color565(rc, gc, bc));
  }
  tft.drawRect(originX, originY, segW * (2 * maxVal - 1) + 4, barH, tft.Color565(100, 100, 100));
}



void setup()
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);

  tft.writeTo(L1);//write colored bars to layer 1
  //create ONCE all colored bars
  drawHbar(10, 10, 100, 100);
  drawHbar(10, 30, 50, 50);
  drawHbar(10, 50, 80, 80);
  drawHbar(10, 70, 80, 80);
  drawHbar(10, 90, 80, 80);
  drawHbar(10, 110, 80, 80);
  drawHbar(10, 130, 80, 80);
  drawHbar(10, 150, 80, 80);
  drawHbar(10, 170, 80, 80);
  drawHbar(10, 190, 80, 80);
  drawHbar(10, 210, 80, 80);
  drawHbar(10, 230, 80, 80);
  tft.writeTo(L2);//from this point we write on layer 2
  tft.layerEffect(AND);//apply AND effect between layer 1 and 2
}

void loop()
{
  //now draw the masking bars on layer 2
  for (int i = 0; i <= 100; i++) {
    drawHbarVal(10, 10, i, 100);
    drawHbarVal(10, 30, random(50), 50);
    drawHbarVal(10, 50, random(80), 80);
    drawHbarVal(10, 70, random(80), 80);
    drawHbarVal(10, 90, random(80), 80);
    drawHbarVal(10, 110, random(80), 80);
    drawHbarVal(10, 130, random(80), 80);
    drawHbarVal(10, 150, random(80), 80);
    drawHbarVal(10, 170, random(80), 80);
    drawHbarVal(10, 190, random(80), 80);
    drawHbarVal(10, 210, random(80), 80);
    drawHbarVal(10, 230, random(80), 80);
    delay(50);
  }
  for (int i = 100; i >= 0; i--) {
    drawHbarVal(10, 10, i, 100);
    drawHbarVal(10, 30, random(50), 50);
    drawHbarVal(10, 50, random(80), 80);
    drawHbarVal(10, 70, random(80), 80);
    drawHbarVal(10, 90, random(80), 80);
    drawHbarVal(10, 110, random(80), 80);
    drawHbarVal(10, 130, random(80), 80);
    drawHbarVal(10, 150, random(80), 80);
    drawHbarVal(10, 170, random(80), 80);
    drawHbarVal(10, 190, random(80), 80);
    drawHbarVal(10, 210, random(80), 80);
    drawHbarVal(10, 230, random(80), 80);
    delay(50);
  }

}