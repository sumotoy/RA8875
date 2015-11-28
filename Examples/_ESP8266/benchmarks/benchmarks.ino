/*
A benchmark test - measure the speed of many drawing functions of RA8875 library at any rotation
Open your serial monitor for results
*/

#include <SPI.h>
#include <RA8875.h>

#define DELAY_BETWEEN 500


/*
ESP8266----------------
 MOSI:      D7 (13)
 MISO:      D6 (12)
 SCK:       D5 (14)
 CS_DISP    D0 (16)
 SDA:       D2 (4)
 SCL:       D1 (3)
 */
 
#define RA8875_CS 16


RA8875 tft = RA8875(RA8875_CS);

void setup() {
  Serial.begin(115200);
  tft.begin(RA8875_800x480);
}

uint8_t rot = 0;
void loop(void) {
  test(rot);
  rot++;
  if (rot > 3) rot = 0;
}
