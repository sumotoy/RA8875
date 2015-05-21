/*
Initialization test.
It uses Serial to help identify initialization issues...
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or nothing!
#define RA8875_MOSI 11
#define RA8875_SCLK 14
#define RA8875_MISO 12

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET, RA8875_MOSI, RA8875_SCLK, RA8875_MISO);

void setup() {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_480x272);//initialize library
  if (tft.errorCode() != 0) {
    Serial.println("Initializing error!\n");
    uint8_t error = tft.errorCode();
    if (bitRead(error, 0)) {
      Serial.println("display not recognized!");
    } else if (bitRead(error, 1)) {
      Serial.println("MOSI or MISO or SCLK out of permitted range!");
    } else if (bitRead(error, 2)) {
      Serial.println("CS out of permitted range!");
    }
  }
  Serial.println("RA8875 initialized!");
  tft.print("Hello World");
}

void loop() {

}