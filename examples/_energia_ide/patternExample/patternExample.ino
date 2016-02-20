/*
Just a basic example, an hello world example.
*/

#include <SPI.h>
#include <RA8875.h>


#include "pattern16x16.h"


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
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);

}

void loop()
{
  tft.clearScreen(RA8875_BLUE);
  tft.writePattern(100, 100, _pattern16x16, 16, false); //this write a pattern 16x16 at 100,100
  tft.BTE_move(100, 100, 16, 16, 200, 200, 1, 1, true, 0xC5);
  delay(1000);
  tft.clearScreen(RA8875_BLUE);
  tft.writePattern(200, 200, _pattern16x16, 16, false);
  tft.BTE_move(200, 200, 16, 16, 300, 300, 1, 1, true, 0xC4);
  delay(1000);
}