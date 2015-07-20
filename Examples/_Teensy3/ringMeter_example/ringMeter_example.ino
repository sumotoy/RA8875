/*
This is a test for the ringMeter function created by Alan Senior
and adapted for this library...
*/

#include <SPI.h>
#include <RA8875.h>


/*
Teensy3.x
You are using 4 wire SPI here, so:
 MOSI:  11
 MISO:  12
 SCK:   13
 the rest of pin below:
 */
#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
*/
#define RA8875_RESET 9//any pin or 255 to disable it!

/*
Teensy3.x has small difference since it can use alternative SPI pins,
essential if you want to use this library with Audio Shield.
You can set alternative SPI pins:
RA8875(CSpin,RSTpin=255,MOSI=11,SCLK=13,MISO=12);//the default configuration
To use with Audio shield:-------------------------------
CS: 2,20,21 because are not used by audio shield
MOSI:7
SCLK:14
RA8875(2,255,7,14);//will not interfere with Audio Shield
Teensy LC
This cpu can use slight different configuration, it can be instanced
with the CS pin only so it will use SPI0 but you can use this:
//RA8875(CSp,RSTp,mosi_pin,sclk_pin,miso_pin);
RA8875 tft = RA8875(RA8875_CS,RA8875_RESET,0,20,1);
This will use the SPI1 (24Mhz)
--------------------------------------------------------
To omit Reset pin, set as 255
For CS you CANNOT use any pin!
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
The smallest instance can be:
RA8875(CSpin);
*/
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy
//or
//RA8875 tft = RA8875(RA8875_CS);//Teensy (not using rst pin)

uint32_t runTime = -99999;       // time for next update

int reading = 0; // Value to be displayed
int d = 0; // Variable used for the sinewave test waveform

void setup()
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");
  /*
  If you have an Adafruit RA8875 board + display choose:
  Adafruit_480x272
  Adafruit_640x480
  Adafruit_800x480
  If you have any other display you have to choose:
  RA8875_320x240
  RA8875_480x272
  RA8875_800x480
  RA8875_640x480
  */
  tft.begin(RA8875_800x480);
  tft.setRotation(0);
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
