/*
this sketch demostrate the ability to rotate screen .
since his introduction, setRotation created some problems in library
because RA8875 does most of his stuff internally so from library version 0.69b21
code has many changes to fix the rotation issues.

 */

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's



void setup() 
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  //Serial.println("RA8875 start");
  tft.begin(RA8875_800x480);
}


uint8_t rot = 0;

void loop() 
{
  tft.fillWindow();
  tft.setRotation(rot);
  tft.setCursor(0,0);
  tft.print("[rotation ");
  tft.print(rot);
  tft.print("]");
  tft.setCursor(100,0);
  tft.print("txt shifted to X100");
  tft.setCursor(0,20);
  tft.print("txt shifted to Y20");
  tft.drawCircle(tft.width()/2,tft.height()/2,50,RA8875_GREEN);
  tft.drawRect((tft.width()/2)-50,(tft.height()/2)-25,100,50,RA8875_YELLOW);
  tft.drawEllipse(tft.width()/2,tft.height()/2,50,25,RA8875_CYAN);
  tft.drawTriangle((tft.width()/2)-50,(tft.height()/2)-25,tft.width()/2,(tft.height()/2)+25,(tft.width()/2)+50,(tft.height()/2)-25,RA8875_RED);
  tft.drawPixel(tft.width()/2,tft.height()/2,RA8875_WHITE);
  tft.drawFastVLine(tft.width()/2,(tft.height()/2)-60,60,RA8875_RED);
  tft.drawFastHLine((tft.width()/2)-60,tft.height()/2,60,RA8875_RED);
  delay(3000);
  if (rot > 3) rot = 0;
  rot++;
}


