/*
An ultra-simple sketch to test Layers.
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10
#define RA8875_RESET 9


RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);

void setup() {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_800x480);
  Serial.println(F("Turn ON layers (automatically)"));
  tft.writeTo(L1);//If layers off it will turn on
  //remember to turn off or layers remain active forever!
  Serial.println(F("Writing on Layer1 a magenta rect"));
  tft.fillRect(0, 0 , 100, 100, RA8875_MAGENTA);
  Serial.println(F("Tells that magenta it's a Transparent color"));
  tft.setTransparentColor(RA8875_MAGENTA);
  Serial.println(F("Writing on Layer2 a circle..."));
  tft.writeTo(L2);
  tft.drawCircle(100, 100, 100, RA8875_GREEN);
  Serial.println(F("Show LAYER1"));
  tft.layerEffect(LAYER1);
  delay(1000);
  Serial.println(F("Show LAYER2"));
  tft.layerEffect(LAYER2);
  delay(1000);
  Serial.println(F("Apply Lighten"));
  tft.layerEffect(LIGHTEN);
  delay(1000);
  Serial.println(F("Apply OR"));
  tft.layerEffect(OR);
  delay(1000);
  Serial.println(F("Apply Transparent"));
  tft.layerEffect(TRANSPARENT);
  delay(1000);
  Serial.println(F("Clear current layer"));
  tft.clearMemory();
  delay(1000);
  Serial.println(F("Turn OFF layers"));
  tft.useLayers(false);//turn off layers
  delay(1000);
  Serial.println(F("Clear the remaining memory"));
  tft.clearMemory();
}

void loop() {
}