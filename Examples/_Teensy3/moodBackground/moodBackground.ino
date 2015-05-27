/*
  Modulate background color
 Works with Arduino 1.0.6 IDE, Arduino 1.6.x IDE
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
#define RA8875_RESET 9//any pin or nothing!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy3/arduino's


float angle;

void setup()
{
  tft.begin(RA8875_480x272);

}

// Translate a hue "angle" -120 to 120 degrees (ie -2PI/3 to 2PI/3) to
// a 6-bit R channel value
//
// This is very slow on a microcontroller, not a great example!
inline int angle_to_channel(float a) {
  if (a < -PI) a += 2*PI;
  if (a < -2*PI/3  || a > 2*PI/3) return 0;
  float f_channel = cos(a*3/4); // remap 120-degree 0-1.0 to 90 ??
  return ceil(f_channel * 255);//63
}

void loop() {
  uint16_t clr = (((angle_to_channel(angle-4*PI/3)>>1) & 0xF8) << 8) | (((angle_to_channel(angle-2*PI/3)) & 0xFC) << 3) | ((angle_to_channel(angle)>>1) >> 3);
  tft.fillWindow(clr);

  angle += 0.01;
  if(angle > PI)
    angle -= 2*PI;
}