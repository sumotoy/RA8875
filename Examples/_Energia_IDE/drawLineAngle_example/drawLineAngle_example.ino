/*
example of drawLineAngle
*/

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

  tft.begin(RA8875_800x480);


  tft.drawCircle(tft.width() / 2, tft.height() / 2, 101, 0xFFFF);//draw round gauge
  tft.drawCircle(tft.width() / 2, tft.height() / 2, 103, 0xFFFF);//draw round gauge
}

void loop()
{
  for (int i = 0; i <= 360; i++) {
    if (i > 0) tft.drawLineAngle(tft.width() / 2, tft.height() / 2, i - 1, 100, 0x0000);//erase previous needle
    tft.drawLineAngle(tft.width() / 2, tft.height() / 2, i, 100, 0xFFFF);//draw needle
    delay(10);
  }
}