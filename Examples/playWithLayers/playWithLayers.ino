/*
Play with layers!
*/

#include <SPI.h>
#include <RA8875.h>

/*
You are using 4 wire SPI here, so:
 MOSI:11
 MISO:12
 SCK:13
 if you are using DUE or 1280 or 2560 plese refere to correct pin
 since they are different!
 
 the rest of pin below:
 */
#define RA8875_INT 2
#define RA8875_CS 10
#define RA8875_RESET 9

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");
  //initialization routine
  tft.begin(RA8875_480x272);
  
  //now we turn on layers
  if (tft.useLayers(true)){
    Serial.println("layers");
  } 
  else {
    Serial.println("not possible");
  }
  //layer support it's possible only if configuration
  //let use 2 layers, if not the layers operation will do nothing.
  
  tft.setActiveLayer(1); //set layer 1
  tft.fillCircle(100,100,20,RA8875_BLUE);
  tft.layerEffect(LAYER1);//show only layer 1
  delay(1000);
  tft.setActiveLayer(2); //set layer 2
  tft.fillCircle(100,100,20,RA8875_RED);
  tft.layerEffect(LAYER2);//show only layer 2
  delay(1000);
  tft.layerEffect(OR);//perform OR
  delay(1000);
  tft.layerEffect(LIGHTEN);//perform LIGHTEN
  delay(1000);
  //0:disable layer,1...7 trasparent,8:full layer
  tft.layerTransparency(3,3);
}

void loop() 
{
  
}