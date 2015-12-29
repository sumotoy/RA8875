/*
fillWindow will fill the active window (set by setActiveWindow) with a choosed color. 
When you first start up, the active window it's the whole screen so it acts as clearSceen.
This is the fast way to clear the whole screen or a part of it!
The clearScreen command it's a combination of setActiveWindow(whole)->fillScreen->setActiveWindow(as it was)
so it's much slower, use it when you are using setActiveWindow.
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10
#define RA8875_RESET 9//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy


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