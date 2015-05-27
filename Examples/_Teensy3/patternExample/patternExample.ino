/*
Just a basic example, an hello world example.
*/

#include <SPI.h>
#include <RA8875.h>


#include "pattern16x16.h"

/*
Teensy3.x
You are using 4 wire SPI here, so:
 MOSI:  11
 MISO:  12
 SCK:   13
 the rest of pin below:
 */
#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy



void setup() 
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  tft.fillWindow(RA8875_BLUE);
  tft.writePattern(100,100,_pattern16x16,16,false);//this write a pattern 16x16 at 100,100
  tft.BTE_layer(SOURCE,1);
  tft.BTE_layer(DEST,1);
  tft.BTE_moveFrom(100,100);
  tft.BTE_moveTo(200,200);
  tft.BTE_size(16,16);
  tft.setTransparentColor(0xF800);
  tft.BTE_ropcode(0xC5);
  tft.BTE_enable(true);
  
  tft.BTE_layer(SOURCE,1);
  tft.BTE_layer(DEST,1);
  tft.BTE_moveFrom(0,0);
  tft.BTE_moveTo(300,300);
  tft.BTE_size(16,16);
  tft.setTransparentColor(0xF800);
  tft.BTE_ropcode(0xC4);
  tft.BTE_enable(true);
}

void loop() 
{

}