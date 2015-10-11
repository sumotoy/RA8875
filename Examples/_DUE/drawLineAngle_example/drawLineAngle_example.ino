/*
example of drawLineAngle
*/

#include <SPI.h>
#include <RA8875.h>


#define RA8875_CS 10

#define RA8875_RESET 9//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

void setup()
{

  tft.begin(RA8875_800x480);


  tft.drawCircle(tft.width() / 2, tft.height() / 2, 101, 0xFFFF);//draw round gauge
  tft.drawCircle(tft.width() / 2, tft.height() / 2, 103, 0xFFFF);//draw round gauge
}

void loop()
{
  for (int i = 0; i <= 360; i++) {
    if (i > 0) tft.drawLineAngle(tft.width() / 2, tft.height() / 2, i - 1, 100, 0x0000);//erase previous needle
    tft.drawLineAngle(tft.width() / 2, tft.height() / 2, i, 100, 0xFFFF);//draw needle
    delay(10);
  }
}