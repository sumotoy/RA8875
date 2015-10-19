/*
Explain the minimal setup and how to use instances...
*/

#include <SPI.h>
#include <RA8875.h>

/*
Teensy3.x
You are using 4 wire SPI here, so:
 MOSI:  11
 MISO:  12
 SCK:   13
 */
#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use as CS: 2,6,9,10,15,20,21,22,23
*/
#define RA8875_RESET 9//any pin, if you wnat to disable just set at 255 or not use at all

/*
Teensy3.x has small difference since it can use alternative SPI pins,
essential if you want to use this library with Audio Shield.
You can set alternative SPI pins:
RA8875(CSpin,RSTpin=255,MOSI=11,SCLK=13,MISO=12);//the default configuration
To use with Audio shield:-------------------------------
CS: 2,20,21 because are not used by audio shield
MOSI:7
SCLK:14
RA8875(2,255,7,14);//will not interfere with Audio Shield
Teensy LC
This cpu can use slight different configuration, it can be instanced
with the CS pin only so it will use SPI0 but you can use this:
//RA8875(CSp,RSTp,mosi_pin,sclk_pin,miso_pin);
RA8875 tft = RA8875(RA8875_CS,RA8875_RESET,0,20,1);
This will use the SPI1 (24Mhz)
--------------------------------------------------------
To omit Reset pin, set as 255
For CS you CANNOT use arbitrary pin!
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
The shorter instance can be: RA8875(CSpin);
*/
RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy
//or
//RA8875 tft = RA8875(RA8875_CS);//Teensy (not using rst pin)
//or (for audio board)
//RA8875 tft = RA8875(RA8875_CS,RA8875_RESET,RA8875_MOSI,RA8875_SCLK);//Teensy using alternative SPI pin
//or
//RA8875 tft = RA8875(RA8875_CS,RA8875_RESET,RA8875_MOSI,RA8875_SCLK,RA8875_MISO);//Teensy using alternative SPI pin + alt miso


void setup() 
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");
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
tft.print("Hello World!");//did you see it?
}

void loop() 
{

}