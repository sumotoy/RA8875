/*
A benchmark test - measure the speed of many drawing functions of RA8875 library at any rotation
Open your serial monitor for results
*/

#include <SPI.h>
#include <RA8875.h>

#define DELAY_BETWEEN 500

/* #if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif */

/*
Teensy3.x and Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//DUE refere to arduino site
 MISO:  12//DUE refere to arduino site)
 SCK:   13//DUE refere to arduino site)
 the rest of pin below:
 */
#define RA8875_CS 10
#define RA8875_RESET 9
/*
DUE: check arduino site
*/


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

void setup() {
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_800x480);

}

uint8_t rot = 0;
void loop(void) {
  test(rot);
  rot++;
  if (rot > 3) rot = 0;
}

