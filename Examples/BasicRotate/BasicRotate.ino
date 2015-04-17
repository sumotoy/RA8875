/*
Basic Rotated Text Functions
 Demonstrate rotation of the screen with text and a custom char.
 
 Rotation can be 
   0 = connector to bottom
   1 = connector to right  
   2 = connector to top
   3 = connector to left
   
 This shows an intentional glitch on the first rotation - the current
 contents of the screen are rotated when going from 0 to 1 but the text
 will be mirrored. Look carefully at the yellow custom char to see
 how its top-left corner is in the same place when the un-mirrored text
 is put onto the screen.
 Because of this mirror effect you probably always want to clear the 
 screen when rotating, but it might be useful to somebody.
 
 X and Y coordinates are flipped so that (0,0) is always the top-left 
 corner, but X is always the long axis of the screen and Y is the
 short axis. In rotation 1 and 3, increasing Y moves over to the
 right. In 0 and 2, increasing Y moves down.
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
#define RA8875_CS 4 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
 Arduino's 8 bit: any
 DUE: should be any but not sure
 */
#define RA8875_RESET 3//any pin or nothing!

#if defined(NEEDS_SET_MODULE)//Energia, this case is for stellaris/tiva

RA8875 tft = RA8875(3);//select SPI mode 3
/*
for mode 3 (stellaris)
 SCLK:  PD_0
 MOSI:  PD_3
 MISO:  PD_2
 SS:    PD_1
 */
#else

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy3/arduino's

#endif

//define some custom char (8x16)
const uint8_t P2[16] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b11110000,
  0b10001000,
  0b10000100,
  0b10000100,
  0b10000100,
  0b10001000,
  0b11110000,
  0b10000110,
  0b10001001,
  0b10000001,
  0b10000010,
  0b00000100,
  0b00001111
};

void setup() 
{
  //initialization routine
  tft.begin(Adafruit_480x272);
  
  //Load up the custom char
  tft.uploadUserChar(P2,0);//0x00

  //following it's already by begin function but
  //if you like another background color....
  tft.fillScreen(RA8875_BLACK);//fill screen black
}

void writeText(char* msg) {
  //RA8875 it's capable to draw graphic but also Text
  //here we switch to TEXT mode
  //tft.changeMode(TEXT);//from version 0.96b12 it's automatic!
  //now set a text color, background transparent
  tft.setFontScale(0);
  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  tft.setCursor(0,0);
  //use the classic print an println command
  tft.print(msg);
  //by default the text location is set to 0,0
  //now set it at 50,20 pixels and different color
  tft.setCursor(50,20);//set cursor work in pixel!!!
  //this time we not using transparent background
  tft.setTextColor(RA8875_RED,RA8875_GREEN);
  tft.setFontScale(3); //large text, string will be wider than the portrait screen
  tft.print("Hello World");
  tft.println();
  tft.println();
  tft.setTextColor(RA8875_BLUE,RA8875_BLACK);
  tft.setFontScale(2);
  tft.print("Line 3");
  
  //try printing a custom char, to test the rotation of the bitmaps
  //The custom chars can only be shown at font scale 0
  tft.setTextColor(RA8875_YELLOW,RA8875_BLACK);
  tft.setFontScale(0);
  tft.setCursor(50,90);
  tft.showUserChar(0);
}

void loop() {
  tft.setRotation(0);
  writeText("Rotation test");  
  delay(3000);
  
  //Now try to rotate: show the mirrored text
  tft.setRotation(1);
  delay(1000);
  tft.fillScreen(RA8875_BLACK);//fill screen black  
  writeText("Rotation 1   ");
  
  delay(3000);
  tft.fillScreen(RA8875_BLACK);//fill screen black  
  tft.setRotation(2);
  writeText("Rotation 2   ");
  
  delay(3000);
  tft.fillScreen(RA8875_BLACK);//fill screen black  
  tft.setRotation(3);
  writeText("Rotation 3   ");
  
  delay(3000);
  tft.fillScreen(RA8875_BLACK);//fill screen black  

}
