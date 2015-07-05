/*
Just a basic example, an hello world example.
*/

#include <SPI.h>
#include <RA8875.h>
#include "fonts/minipixel.h"

#define RA8875_CS 10 //see below...
#define RA8875_RESET 23//any pin or 255 to disable it!
uint8_t flags = 0b10000000;

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy

void setup()
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);

  tft.setRotation(0);//works at any rotation as well
  tft.setFont(&minipixel);
  tft.setFontScale(8);
  tft.setCursor(CENTER,0);
  tft.setTextColor(RA8875_RED);
  tft.println("CD 012345678");
  tft.setFont(INT);
  tft.println("this is the internal text.");


  //tft.drawFastVLine(tft.width() / 2, 0, tft.height(), RA8875_RED);
  //tft.drawFastHLine(0, tft.height() / 2, tft.width(), RA8875_RED);

}

void loop()
{

}

