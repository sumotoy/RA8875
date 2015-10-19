/*
Another small example for render text.
This time shows the ability to use transparency.
*/

#include <SPI.h>
#include <RA8875.h>
#include "fonts/akashi_36.c"

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
  // Serial.begin(38400);
  // long unsigned debug_start = millis ();
  // while (!Serial && ((millis () - debug_start) <= 5000)) ;
  // Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);

  for (int i = 0; i < tft.width(); i++) {
    tft.drawFastVLine(i, 0, 110, tft.colorInterpolation(RA8875_RED, RA8875_PINK, i, tft.width()));
  }
  tft.setFont(&akashi_36);
  tft.setTextColor(RA8875_WHITE);//background transparent!!!
  tft.setCursor(CENTER, 0);
  if (tft.width(true) > 400){
    tft.setFontScale(2);
  } else {
    tft.setFontScale(1);
  }
  tft.println("SUMOTOY");
}

void loop()
{

}