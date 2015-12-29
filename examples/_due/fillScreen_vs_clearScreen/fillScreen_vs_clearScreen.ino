/*
This sketch demonstrate the difference between fillScreen and clearScreen
fillScreen depends directly from the activeWindow settings, after startup
these settings are automatically set as the entire window visible so using
clearScreen instead fillScreen doesn't make sense (clearScreen it's slower).
BUT if you set different parameters for the active window (setActiveWindow)
you will notice that fillScreen clears the image INSIDE the activeWindow only!
In that case it's useful to have a command that clear the entire window in a command.

 */

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's

void setup() 
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  //Serial.println("RA8875 start");
  tft.begin(RA8875_800x480);
  
}


uint8_t alt = 0;

void loop() 
{
  tft.setActiveWindow(0,tft.width()-1,0,tft.height()-1);//this set the active window
  testdrawrects(RA8875_GREEN);
  delay(500);
  testfillrects(RA8875_YELLOW, RA8875_MAGENTA);
  delay(500);
  tft.setActiveWindow(80,tft.width()-80,80,tft.height()-80);//this set the active window
    tft.setCursor(CENTER,CENTER);
    tft.setTextColor(RA8875_WHITE,RA8875_BLUE);
  if (alt > 0){
    tft.print("[clearScreen]");
    delay(1000);
    tft.clearScreen();
  } else {
    tft.print("[fillScreen]");
    delay(1000);
    tft.fillScreen();
  }
  alt++;
  if (alt > 1) alt = 0;
  
}

void testdrawrects(uint16_t color) {
  for (int16_t x=0; x < tft.width(); x+=4) {
    tft.drawRect((tft.width()/2) - (x/2), (tft.height()/2) - (x/2) , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  for (int16_t x=tft.width()-1; x > 4; x-=4) {
    tft.fillRect((tft.width()/2) - (x/2), (tft.height()/2) - (x/2) , x, x, color1);
    tft.drawRect((tft.width()/2) - (x/2), (tft.height()/2) - (x/2) , x, x, color2);
  }
}

