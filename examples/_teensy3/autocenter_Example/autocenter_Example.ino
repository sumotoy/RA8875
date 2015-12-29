/*
autocenter example:
setCursor has an optional parameter that let you precisely center
text regardless text lenght.
The autocenter flag, automatically reset after each use!!!
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy


void setup()
{
/*
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");
*/
  tft.begin(RA8875_800x480);//or whatever you have
  tft.setRotation(0);
  //normal method
  tft.drawRect(100, 100, 100, 100, RA8875_RED);//draw a rect
  tft.setCursor(150, 150);//set text cursor inside rect center
  tft.setFontScale(2);
  tft.print("AB");//print the text, will not appear in the center
  //because text has his lenght
  tft.setFontScale(0);
  tft.setCursor(CENTER, CENTER);
  tft.print("normal");
  delay(3000);
  tft.clearScreen();
  //autocenter method
  tft.drawRect(100, 100, 100, 100, RA8875_RED);//draw a rect
  tft.setCursor(150, 150, true);//set text cursor inside rect center and activate autocenter
  tft.setFontScale(2);
  tft.print("AB");//now text it's inside center regardless lenght!
  tft.setFontScale(0);
  tft.setCursor(CENTER, CENTER);
  tft.print("centered");
  //autocenter it's now automatically disabled
}

void loop()
{

}