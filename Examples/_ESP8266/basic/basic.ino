/*
Explain the minimal setup and how to use instances...
*/


#include <SPI.h>
#include <RA8875.h>
//#include "fonts/akashi_36.c"////akashi_36

/*
ESP8266----------------
 MOSI:      D7 (13)
 MISO:      D6 (12)
 SCK:       D5 (14)
 CS_DISP    D0 (16)
 SDA:       D2 (4)
 SCL:       D1 (3)
 */

#define RA8875_CS 16


RA8875 tft = RA8875(RA8875_CS);


void setup()
{
  Serial.begin(115200);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("\nRA8875 start");
  /*
      If you have an Adafruit RA8875 board + display choose:
  Adafruit_480x272 , Adafruit_800x480
      If you have any other display you have to choose:
  RA8875_480x272 , RA8875_800x480
  */
  tft.begin(RA8875_800x480);
  /*
  By default the library init display at 16bit color depth but
  you can optionally force the display to work at 8 bit:

  tft.begin(RA8875_480x272,8);//force 8bit color depth
  */
  //Now that board it's inited you can use any draw or text command:
  //tft.setFont(&akashi_36);//minipixel //fluide_caps
  //tft.setTextColor(RA8875_WHITE);//background transparent!!!
  //tft.setFontScale(3);
  tft.print("SUMOTOY");//NQW
}

void loop()
{

}