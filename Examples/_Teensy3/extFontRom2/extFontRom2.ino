/*
Just a basic example, an hello world example.
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10 //see below...
#define RA8875_RESET 23//any pin or 255 to disable it!


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy

void setup()
{
  //  Serial.begin(38400);
  //  long unsigned debug_start = millis ();
  //  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  //  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);

  tft.setRotation(0);//works at any rotation as well

  tft.setExternalFontRom(ER3304_1, GB2312);//we set the correct ROM
  tft.setFont(EXT);//enable ROM
  tft.setFontSize(X16);//not all ROM have X16,X24,X32 so check your ROM datasheet!
  tft.setFontScale(1);//with ROM font we can use scaling as well
  //Font ROM uses same commads as internal fonts....
  tft.setTextColor(0xFFFF);

  tft.setCursor(CENTER, CENTER);
  tft.print("CENTER");
  tft.setCursor(0, 0);
  tft.println("ROM ascii");
  tft.println("«·½¿Æ¼¼ÓÐÏÞ¹«Ë¾");
  //tft.println("ÉîÛÚÐñÈÕ¶");
  //now switch to internal font
  tft.setFont(INT);
  tft.println("Internal ascii");
  //so you can switch between INT and EXT by using setFont
  //without loosing the initial assignments!


  //tft.drawFastVLine(tft.width() / 2, 0, tft.height(), RA8875_RED);
  //tft.drawFastHLine(0, tft.height() / 2, tft.width(), RA8875_RED);

}

void loop()
{

}