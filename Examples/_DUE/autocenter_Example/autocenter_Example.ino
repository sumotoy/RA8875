/*
autocenter example:
setCursor has an optional parameter that let you precisely center
text regardless text lenght.
The autocenter flag, automatically reset after each use!!!
*/

#include <SPI.h>
#include <RA8875.h>

/*
Teensy3.x and Arduino's
 You are using 4 wire SPI here, so:
 MOSI:  11//(for MEGA/DUE refere to arduino site)
 MISO:  12//(for MEGA/DUE refere to arduino site)
 SCK:   13//(for MEGA/DUE refere to arduino site)
 the rest of pin below:
*/
#define RA8875_CS 10 //see below...
/* Arduino's 8 bit: any DUE: should use 4,10,53 */
#define RA8875_RESET 9//any pin or nothing!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);


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