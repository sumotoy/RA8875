/*
Basic Text Functions
*/

#include <SPI.h>
#include "RA8875.h"

#define RA8875_INT 2
#define RA8875_CS 10
#define RA8875_RESET 9

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);


void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");
  //initialization routine
  if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }
  Serial.println("RA8875 ready");

  //following it's already by begin function but
  //if you like another background color....
  tft.fillScreen(RA8875_BLACK);//fill screen black

  //RA8875 it's capable to draw graphic but also Text
  //here we switch to TEXT mode
  tft.changeMode(TEXT);
  //now set a text color, background transparent
  tft.setTextColor(RA8875_WHITE);
  //use the classic print an println command
  tft.print("Hello World");
  //by default the text location is set to 0,0
  //now set it at 50,20 pixels and different color
  tft.setCursor(50,20);//set cursor work in pixel!!!
  //this time we not using transparent background
  tft.setTextColor(RA8875_RED,RA8875_GREEN);
  tft.print("Hello World");
  //by default we using the internal font
  //so some manipulation it's possible
  tft.setFontScale(1);//font x2
  tft.setTextColor(RA8875_RED);
  tft.print("Hello World");
  //You notice that font location has been 
  //automatically increased by chip, unfortunatly not
  //tracked by library but we can use a command for that...
  uint16_t currentX,currentY;
  tft.getCursor(&currentX,&currentY);
  //now we have the location, lets draw a white pixel
  tft.changeMode(GRAPHIC);//first we swith in graphic mode
  tft.drawPixel(currentX,currentY,RA8875_WHITE);
  //did you see the white dot?
  tft.changeMode(TEXT);//go back to text mode
  tft.setFontScale(0);//font x1
  tft.setCursor(0,50);
  tft.setTextColor(RA8875_YELLOW);
  tft.println("ABCDEF 1 2 3 4");//this time println!
  tft.setFontSpacing(5);//now give 5 pix spacing
  tft.println("ABCDEF 1 2 3 4");
  tft.setFontSpacing(0);//reset
  tft.setFontRotate(true);
  tft.println("ABCDEF 1 2 3 4");
  tft.setFontRotate(false);
  tft.setFontScale(2);//font x1
  tft.setTextColor(RA8875_BLUE,RA8875_BLACK);
}

unsigned long i = 0;

void loop() 
{
  tft.setCursor(50,100);
  if (i > 99) tft.setTextColor(RA8875_CYAN,RA8875_BLACK);
  if (i > 999) tft.setTextColor(RA8875_MAGENTA,RA8875_BLACK);
  tft.print(i,DEC);
  delay(10);
  i++;
}

