/*
autocenter example:
setCursor has an optional parameter that let you precisely center
text regardless text lenght.
The autocenter flag automatically reset after each use.
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy


void setup()
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  tft.setRotation(0);
  //normal methood
  tft.drawRect(100, 100, 100, 100, RA8875_RED);
  tft.setCursor(150, 150);
  tft.setFontScale(2);
  tft.print("AB");
  tft.setFontScale(0);
  tft.setCursor(CENTER, CENTER);
  tft.print("normal");
  delay(3000);
  tft.clearScreen();
  //autocenter method
  tft.drawRect(100, 100, 100, 100, RA8875_RED);
  tft.setCursor(150, 150, true);//activate autocenter
  tft.setFontScale(2);
  tft.print("AB");
  tft.setFontScale(0);
  tft.setCursor(CENTER, CENTER);
  tft.print("centered");
  //autocenter now automatically disabled
}

void loop()
{

}