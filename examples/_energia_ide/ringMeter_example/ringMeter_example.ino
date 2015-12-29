/*
This is a test for the ringMeter function created by Alan Senior
and adapted for this library...
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

uint32_t runTime = -99999;       // time for next update

int reading = 0; // Value to be displayed
int d = 0; // Variable used for the sinewave test waveform

void setup()
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
}


void loop()
{
  if (millis() - runTime >= 100L) { // Execute every 2s
    runTime = millis();

    // Test with a slowly changing value from a Sine function
    d += 5;
    if (d >= 360) d = 0;

    // Set the the position, gap between meters, and inner radius of the meters
    int16_t xpos = 0;
    int16_t ypos = 0;
    uint16_t radius = 52;
    int16_t distance = 4;

    // Draw meter and get back x position of next meter

    // Test with Sine wave function, normally reading will be from a sensor
    reading = 250 + 250 * sineWave(d);
    tft.ringMeter(
      reading,
      0,
      500,
      xpos,
      ypos,
      radius,
      "mA",
      4,
      RA8875_BLACK,
      150,
      10); // Draw analogue meter

    reading = 20 + 30 * sineWave(d + 60);
    tft.ringMeter(
      reading,
      -10,
      50,
      xpos + (radius * 2) + distance,
      ypos,
      radius,
      "C",
      3,
      tft.htmlTo565(0x4B0082),
      150,
      10); // Draw analogue meter

    reading = 1000 + 150 * sineWave(d + 90);
    tft.ringMeter(
      reading,
      850,
      1150,
      xpos + (radius * 4) + distance * 2,
      ypos,
      radius,
      "rH",
      3,
      RA8875_BLACK,
      150,
      10); // Draw analogue meter

    reading = 15 + 15 * sineWave(d + 150);
    tft.ringMeter(
      reading,
      0,
      30,
      0,
      ypos + (radius * 2) + distance,
      72,
      "V",
      6,
      tft.htmlTo565(0x848482),
      90,
      10); // Draw analogue meter

    // Draw a large meter
    reading = 20 + 10 * sineWave(d + 180);
    tft.ringMeter(
      reading,
      0,
      40,
      150,
      120,
      120,
      "W",
      7,
      RA8875_BLACK,
      150,
      5); // Draw analogue meter
  }
}




float sineWave(int phase) {
  return sin(phase * 0.0174532925);
}