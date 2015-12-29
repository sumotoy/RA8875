/*
autocenter example:
setCursor has an optional parameter that let you precisely center
text regardless text lenght.
The autocenter flag, automatically reset after each use!!!
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_RESET 9//any pin or nothing!

#if defined(NEEDS_SET_MODULE)//Energia, this case is for stellaris/tiva

RA8875 tft = RA8875(3);//select SPI module 3
/*
for module 3 (stellaris)
 SCLK:  PD_0
 MOSI:  PD_3
 MISO:  PD_2
 SS:    PD_1
 */
#endif


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