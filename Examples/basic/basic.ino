/*
Just a basic example, it will do nothing, initialize screen and put black.
Used as basic setup for experimenting with Arduino and Energia IDE's.
 Tested and worked with:
 Teensy3,Teensy3.1,Arduino UNO,Arduino YUN,Arduino Leonardo,Stellaris
 Works with Arduino 1.0.6 IDE, Arduino 1.5.8 IDE, Energia 0013 IDE
*/

#include <SPI.h>
#include <RA8875.h>

/*
Teensy3.x and Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 MISO:  12//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 SCK:   13//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 the rest of pin below:
 */
#define RA8875_INT 2 //any pin
#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
Arduino's 8 bit: any
DUE: should be any but not sure
*/
#define RA8875_RESET 9//any pin or 255 to disable it!

#if defined(NEEDS_SET_MODULE)//Energia, this case is for stellaris/tiva

RA8875 tft = RA8875(3);//select SPI module 3
/*
for module 3 (stellaris)
SCLK:  PD_0
MOSI:  PD_3
MISO:  PD_2
SS:    PD_1
*/
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
/* Teensy3.x has small difference since it can use alternative
 SPI pins, very useful if you want to use this library with Audio Shield.
So the full Teensy 3 instance is:
RA8875 tft = RA8875(CS,RST,MOSI,SCLK,MISO);
CS can be selected between compatible CS pin of Teensy 3
RST can literally be any pin or set to 255 to omit it (if you use alternative SPI pins you have to set at 255)
MOSI,SCLK,MISO are the alternative pin (see Teensy 3, 3.1 and LC docs or read wiki)
*/
RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy
//or
//RA8875 tft = RA8875(RA8875_CS);//Teensy (not using RST pin)
/*
an example for using it with Audio Shield...
CS: set to 2,20,21 because are not used by audio shield
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
rst: not used
SCLK:14 (alternative pin selected)
MOSI:7 (alternative pin selected)
*/
//RA8875 tft = RA8875(2,255,7,14);

#else

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's
//RA8875 tft = RA8875(RA8875_CS);//arduino's no rst
#endif


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

tft.begin(RA8875_480x272,8);//force 8bit color depth
*/
/*
Now that board it's inited you can use any draw or text command:
*/
tft.print("Hello World!");
}

void loop() 
{

}