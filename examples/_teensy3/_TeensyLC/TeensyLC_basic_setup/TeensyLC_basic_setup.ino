/*
Just a basic example, an hello world example.
*/

#include <SPI.h>
#include <RA8875.h>


#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
*/
#define RA8875_RESET 9//any pin or 255 to disable it!

/*
TeensyLC can use a second SPI channel

*/
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //TeensyLC
//or
//RA8875 tft = RA8875(RA8875_CS,RA8875_RESET,true);//TeensyLC use second SPI bus (24Mhz) and RST
//RA8875 tft = RA8875(RA8875_CS);//TeensyLC (not using rst pin)



void setup()
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");
  /*
  If you have an Adafruit RA8875 board + display choose:
  Adafruit_480x272
  Adafruit_640x480
  Adafruit_800x480
  If you have any other display you have to choose:
  RA8875_320x240
  RA8875_480x272
  RA8875_800x480
  RA8875_640x480
  */
  tft.begin(RA8875_800x480);
  /*
  By default the library init display at 16bit color depth but
  you can optionally force the display to work at 8 bit:

  tft.begin(RA8875_480x272,8);//force obit color depth
  */
  /*
  Now that board it's inited you can use any draw or text command:
  */
  tft.print("Hello World!");
}

void loop()
{

}