/*
Simple test of BTE block move
*/

#include <SPI.h>
#include <RA8875.h>

/*
Teensy3.x
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x
 MISO:  12//Teensy3.x
 SCK:   13//Teensy3.x
 the rest of pin below:
 */

#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
*/
#define RA8875_RESET 9//any pin or 255 to disable it!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);


void setup() 
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 400)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  
  //tft.setRotation(0);
  //fill a gradient, so we can test BTEing stuff around the screen
  for(int i=0;i<tft.height();i++) tft.drawLine(0,i,tft.width()-1,i,tft.Color565(map(i,0,tft.height(),128,40), map(i,0,tft.height(),255,40), map(i,0,tft.height(),40,128)));
  

  tft.setTextColor(RA8875_PINK,RA8875_BLUE);
  tft.setCursor(30,30);
  tft.print("HELLO WORLD!");
  //The transparent-move option uses the foreground colour as the transparent colour.
  tft.setTextColor(RA8875_BLUE);
  tft.BTE_move(20,20,30,30,200,200,0,0,true);
}

void loop() 
{

}