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
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
/*
Teensy3.x has small difference since it can use alternative
SPI pins, very useful if you want to use this library with
Audio Shield.
To omit Reset pin, set as 255!
So the full Teensy 3 instance is:
RA8875 tft = RA8875(CS,RST,SCLK,MOSI,MISO);
CS can be selected between compatible CS pin of Teensy 3
RST can literally be any pin or set to 255 to omit it.
SCLK,MOSI,MISO are the alternative pin triggers, normally are set
to 'false' but you can reassign any of those pins to his alternative
position! 
Set SCLK to true will switch SCLK from pin 13 to 14
Set MOSI to true will switch MOSI from pin 11 to 7
Set MISO to true will switch MISO from pin 12 to 8
*/
RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy
//or
//RA8875 tft = RA8875(RA8875_CS,255);//Teensy (not using rst pin)
/*
an example for using it with Audio Shield...
CS: set to 2,20,21 because are not used by audio shield
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
rst: not used
SCLK:14 (alternative pin selected)
MOSI:7 (alternative pin selected)
MISO:12 (classic pin)
*/
//RA8875 tft = RA8875(2,255,true,true);
#else

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's
//RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's no rst
#endif


void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");

  tft.begin(RA8875_480x272);


}

void loop() 
{

}