/*
Just a basic example, it will do nothing, initialize screen and put black.
Used as basic setup for experimenting with Arduino and Energia IDE's.
 Tested and worked with:

 Works with Arduino 1.0.6 IDE, Arduino 1.6.x IDE
*/

#include <SPI.h>
#include <RA8875.h>

/*
Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//DUE refere to arduino site
 MISO:  12//DUE refere to arduino site
 SCK:   13//DUE refere to arduino site
 the rest of pin below:
 */

#define RA8875_CS 10 //see below...
/*
DUE: should be any but not sure
*/
#define RA8875_RESET 9//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's
//RA8875 tft = RA8875(RA8875_CS);//arduino's no rst


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