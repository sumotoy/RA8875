/*
Preliminary font rendering engine *ALPHA* test
*/

#include <SPI.h>
#include <RA8875.h>
#include "fonts/Square_Pixel7__14.h"//
#include "fonts/Imagine_FontFixed__15.h"
/* 
Check the font folder inside the library if you want to test other fonts
*/

#define RA8875_INT 2 //any pin
#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's



void setup() 
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  
  /*
  x,y,text,color,pixellation,font
  pixellation it's an experimental efx that write font by using aligned pixels and affect the size, try it!
  */
  tft.gPrint(30,30,"Hello World",RA8875_CYAN,3,&Imagine_FontFixed__15);
}

void loop() 
{

}
