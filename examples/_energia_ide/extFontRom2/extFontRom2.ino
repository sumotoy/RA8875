/******************************************************************
An example of how use an external ROM chip with RA8875, this time
the ER3303_1 (same as GT23L24T3Y or GT30H24T3Y), an unified chinese
font rom. 
In short:

1) use setExternalFontRom to set ROM and it's correct encoding
2) use setFont(EXTFONT) to use the external font
3) to switch back to internal font ROM, use setFont(INTFONT)

Library can handle the following Font ROM:
GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, 
GT23L32S4W, GT30H32S4W, ER3303_1
Each font ROM has it's encoding so follow font rom datasheet!
Not working? You don't have the correct font rom installed!
Choose yours!


 ******************************************************************/
#include <SPI.h>
#include <RA8875.h>

#define RA8875_RESET 9//any pin or nothing!

#if defined(NEEDS_SET_MODULE)//Energia, this case is for stellaris/tiva

RA8875 tft = RA8875(3);//select SPI module 3
/*
for module 3 (stellaris)
 SCLK:  PD_0
 MOSI:  PD_3
 MISO:  PD_2
 SS:    PD_1
 */
#endif

void setup()
{
  //  Serial.begin(38400);
  //  long unsigned debug_start = millis ();
  //  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  //  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);

  tft.setRotation(0);//works at any rotation as well

  tft.setExternalFontRom(ER3304_1, GB2312);//we set the correct ROM
  tft.setFont(EXTFONT);//enable ROM
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
  tft.setFont(INTFONT);
  tft.println("Internal ascii");
  //so you can switch between INTFONT and EXTFONT by using setFont
  //without loosing the initial assignments!

}

void loop()
{

}