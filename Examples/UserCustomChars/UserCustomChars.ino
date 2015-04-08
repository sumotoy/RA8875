/*
An example of how easy it's upload custom 8x16 symbols.
 RA8875 has 255 locations reserved for user custom designed symbols.
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
#define RA8875_INT 3 //any pin
#define RA8875_CS 2 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
Arduino's 8 bit: any
DUE: should be any but not sure
*/
#define RA8875_RESET 29//any pin or nothing!

#if defined(NEEDS_SET_MODULE)//Energia, this case is for stellaris/tiva

RA8875 tft = RA8875(3);//select SPI module 3
/*
for module 3 (stellaris)
SCLK:  PD_0
MOSI:  PD_3
MISO:  PD_2
SS:    PD_1
*/
#else

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy3/arduino's

#endif

//define some custom char (8x16)
const uint8_t battery1[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery2[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery3[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery4[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery5[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b11111111
};

//next 3 are part of the same char
const uint8_t battery6a[16] = {
  0b00000111,
  0b00000111,
  0b11111111,
  0b11000000,
  0b11000000,
  0b11000000,
  0b11110000,
  0b11111100,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b00000000,
  0b00000000
};

const uint8_t battery6b[16] = {
  0b11100000,
  0b11100000,
  0b11111111,
  0b00000011,
  0b00000011,
  0b00000011,
  0b00000011,
  0b00000011,
  0b00000011,
  0b11000011,
  0b11110011,
  0b11111111,
  0b11111111,
  0b11111111,
  0b00000000,
  0b00000000
};

const uint8_t battery6c[16] = {
  0b00011111,
  0b00000011,
  0b00001111,
  0b00000011,
  0b00011111,
  0b00000000,
  0b00011111,
  0b00000011,
  0b00001111,
  0b00000011,
  0b00011111,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};

void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 online");

  tft.begin(RA8875_480x272);
  //upload chars in address 0x00 to 0x04
  tft.uploadUserChar(battery1,0);//0x00
  tft.uploadUserChar(battery2,1);
  tft.uploadUserChar(battery3,2);
  tft.uploadUserChar(battery4,3);
  tft.uploadUserChar(battery5,4);//0x04
  tft.uploadUserChar(battery6a,5);//
  tft.uploadUserChar(battery6b,6);//
  tft.uploadUserChar(battery6c,7);//
  //now custom char are stored in CGRAM
  //to recall, first switch to TEXT
  //tft.changeMode(TEXT);
  
  tft.setTextColor(RA8875_WHITE,RA8875_BLACK);
  //custom char are managed as text but need to be called
  //by a special function
  //the only text parameter that is not accepted is setFontScale!
  for (int i=0;i<5;i++){
    tft.setCursor(10,10);
    tft.showUserChar(i);//retrieve from 0x00 to 0x04
    delay(200);
  }
  
  delay(500);
  
  tft.setTextColor(RA8875_RED,RA8875_BLACK);
  for (int i=4;i>=0;i--){
    tft.setCursor(10,10);
    tft.showUserChar(i);//retrieve from 0x00 to 0x04
    delay(200);
  }
  delay(500);
  
  //now another feature, you can design a char wider
  //by using many char slot combined.
  //below an example, 3 chars wider
  tft.setCursor(100,100);
  tft.showUserChar(5,2);
}

void loop() 
{

}