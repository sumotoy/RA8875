/*
This shows the capabilities of the library text engine that is
extremely flexible and simple! All commands are the same for all type of text
(INT,EXT or Render ones) and things like println works too.
You can use all the print() options (like print(var,HEX), etc. Take a look the print command 
in the arduino guide!) 
*/

#include <SPI.h>
#include <RA8875.h>
#include "fonts/heydings_36.c"//minipixel

#define RA8875_CS 20 
#define RA8875_RESET 21//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET,7,14); 

void setup()
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  tft.setRotation(0);//works at any rotation as well
  tft.setFont(&heydings_36);
  tft.setFontScale(2);
  tft.setCursor(0,0);
  //tft.setTextColor(RA8875_GREEN);//notice that! After grandient text will be this color!
  //grandient it's one shot, text color will be reset as the one choosed by setTextColor
  //tft.setTextGrandient(RA8875_RED,RA8875_CYAN);//works also with rendered text!
  tft.println("BSuRF");
  tft.print("_");


}

void loop()
{

}
