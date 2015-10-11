/*
fillWindow will fill the active window (set by setActiveWindow) with a choosed color. 
When you first start up, the active window it's the whole screen so it acts as clearSceen.
This is the fast way to clear the whole screen or a part of it!
The clearScreen command it's a combination of setActiveWindow(whole)->fillScreen->setActiveWindow(as it was)
so it's much slower, use it when you are using setActiveWindow.
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
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  tft.clearScreen(RA8875_RED);
  tft.setActiveWindow(100,tft.width()-100,100,tft.height()-100);
  tft.fillWindow();
}

void loop()
{

}