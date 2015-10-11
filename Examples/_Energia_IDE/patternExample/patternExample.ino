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
  tft.fillWindow(RA8875_BLUE);
  tft.writePattern(100, 100, _pattern16x16, 16, false); //this write a pattern 16x16 at 100,100
  tft.BTE_layer(SOURCE, 1);
  tft.BTE_layer(DEST, 1);
  tft.BTE_moveFrom(100, 100);
  tft.BTE_moveTo(200, 200);
  tft.BTE_size(16, 16);
  tft.setTransparentColor(0xF800);
  tft.BTE_ropcode(0xC5);
  tft.BTE_enable(true);

  tft.BTE_layer(SOURCE, 1);
  tft.BTE_layer(DEST, 1);
  tft.BTE_moveFrom(0, 0);
  tft.BTE_moveTo(300, 300);
  tft.BTE_size(16, 16);
  tft.setTransparentColor(0xF800);
  tft.BTE_ropcode(0xC4);
  tft.BTE_enable(true);
}

void loop()
{

}