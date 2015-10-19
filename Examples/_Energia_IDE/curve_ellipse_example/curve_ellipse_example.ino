/*
curve and ellipse test
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
  //  Serial.begin(38400);
  //  long unsigned debug_start = millis ();
  //  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  //  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
}

uint8_t b = 0;
void loop()
{
  tft.setRotation(b);
  tft.drawEllipse(CENTER, CENTER, 100, 60, RA8875_PURPLE);
  tft.fillCurve(CENTER, CENTER, 80, 30, 0, RA8875_CYAN);
  tft.fillCurve(CENTER, CENTER, 80, 30, 1, RA8875_MAGENTA);
  tft.fillCurve(CENTER, CENTER, 80, 30, 2, RA8875_YELLOW);
  tft.fillCurve(CENTER, CENTER, 80, 30, 3, RA8875_RED);

  tft.drawCurve(CENTER, CENTER, 90, 50, 0, RA8875_CYAN);
  tft.drawCurve(CENTER, CENTER, 90, 50, 1, RA8875_MAGENTA);
  tft.drawCurve(CENTER, CENTER, 90, 50, 2, RA8875_YELLOW);
  tft.drawCurve(CENTER, CENTER, 90, 50, 3, RA8875_RED);
  tft.fillCircle(CENTER, CENTER, 30, RA8875_BLUE);
  delay(100);
  if (b > 2) {
    b = 0;
    tft.fillWindow();
  } else {
    b++;
  }
}