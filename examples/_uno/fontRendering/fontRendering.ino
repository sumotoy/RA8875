/*
This shows the capabilities of the library text engine that is
extremely flexible and simple! All commands are the same for all type of text
(INT,EXT or Render ones) and things like println works too.
You can use all the print() options (like print(var,HEX), etc. Take a look the print command 
in the arduino guide!) 
*/

#include <SPI.h>
#include <RA8875.h>
#include "fonts/akashi_36.c"//minipixel

#define RA8875_CS 10 
#define RA8875_RESET 9//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); 

void setup()
{
//  Serial.begin(38400);
//  long unsigned debug_start = millis ();
//  while (!Serial && ((millis () - debug_start) <= 5000)) ;
//  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  tft.setRotation(0);//works at any rotation as well
  tft.setFont(&akashi_36);
  tft.setFontScale(3);
  tft.setCursor(CENTER,0);
  tft.setTextColor(RA8875_GREEN);//notice that! After grandient text will be this color!
  //grandient it's one shot, text color will be reset as the one choosed by setTextColor
  tft.setTextGrandient(RA8875_RED,RA8875_CYAN);//works also with rendered text!
  tft.println("CD 0123");
  tft.println("ABCD");//notice that correctly goes in another line
  tft.setFont(INT);
  tft.setTextColor(RA8875_WHITE);//Force white
  tft.println("this is the internal text.");//notice that correctly goes in another line too!

}

void loop()
{

}