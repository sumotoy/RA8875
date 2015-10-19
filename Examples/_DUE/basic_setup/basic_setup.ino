/*
Just a basic example, it will do nothing, initialize screen, blank it and write 'hello world'.
Used as basic setup for experimenting with Arduino and Energia IDE's, it shows how to wire library.

The library support DUE EXTENDED SPI and Fast Port (see RA8875UserSettings.h file)
It's also SPI Transaction compatible!!!

 Works with Arduino 1.0.6 IDE, Arduino 1.6.x IDE but 1.6.5 or better raccomanded.
*/

#include <SPI.h>
#include <RA8875.h>

/*
Use the 6 pin SPI connector on arduino DUE board (NOT the ICSP!)
The pin 1 has a white dot behind it.
	MOSI:	SPI CONNECTOR 4
	MISO:	SPI CONNECTOR 1
	SCLK:	SPI CONNECTOR 3
	CS: on SPI extended mode can use only PIN 4,10,52 !!!
*/

#define RA8875_CS 10 //see below...
/*
DUE can use: 4,10,52(o 53? not remember)
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