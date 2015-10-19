/*
This example create an hudge vertical bars gauge display with more than 230 bars!
Perfect for FFT, etc. 
The sketch shows the power of layers! 
First it will create the full bars background in a layer, then it draws
2 rectangles for each bar (one black will cover the background, the other white
will reveal the background) in the layer 2 by using AND function. The result its a really
fast hudge vertical bars gauge that it's not possible to do with a tiny microcontroller
and if you have a Teensy 3.1 you still have resources for a complete FFT and more!
Created by Max MC Costa as a demo for RA8875 library
For 800x480 see the 800x480 folder!
*/

#include <SPI.h>
#include <RA8875.h>



#define RA8875_CS 10

#define RA8875_RESET 9//any pin or 255 to disable it!


const uint8_t _barWidth = 2;           //width of single bar dot
const uint8_t _spacer = 1;             //space between bars
const uint8_t _segHeight = 2;          //height of every bar single dot
const uint8_t _segments = 60;          //how many dots per bar 
const unsigned long updateEvery = 40;  //time between screen updates, fom 0 ...xx (in ms)


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);


uint8_t fftdata[400];//depend of the display w and _barWidth this can be reduced.
// you can put maxbars value generated from createBars here but you need to run this
//sketch at list once!

volatile bool started = false;
unsigned int maxbars = 0;
unsigned long updateDelay = 0;

void setup()
{
//  Serial.begin(38400);
//  long unsigned debug_start = millis ();
//  while (!Serial && ((millis () - debug_start) <= 5000)) ;
//  Serial.println("RA8875 start");

  tft.begin(RA8875_480x272);

}

void loop()
{
  if (!started) {
    tft.useLayers(true);//turn on layers
    tft.writeTo(L1);//write colored bars to layer 1
    maxbars = createBars(0, 20);
    started = true;
    tft.writeTo(L2);//from this point we write on layer 2
    tft.layerEffect(AND);//apply AND effect between layer 1 and 2
  }
  
  //fill bars data with some random stuff
  for (uint16_t i = 0; i <= maxbars; i++) {
    fftdata[i] = random(0, 256);
  }
  
  
  if ((millis() - updateDelay) > updateEvery){//screen refresh every xx ms
    updateBars(fftdata, 0, 20, maxbars,false);
    updateDelay = millis();
  }
  
}

boolean createBar(uint16_t origX, uint16_t origY) {
  uint8_t i;
  //the real bar height
  uint16_t barHeight = origY + (_segHeight * _segments) + (_segments * 2);
  if (barHeight > tft.height()) return false;//too much, exit
  //with vertical bars the Y origin starts from the bottom
  uint16_t newOriginY = tft.height() - barHeight;
  uint8_t rc, gc, bc; //container for colors
  bc = 1;//blue always 0
  for (i = 0; i < _segments; i++) {
    gc = map(i, 0, _segments - 1, 100, 255); //calculate green
    rc = map(i, _segments - 1, 0, 200, 255); //calculate red
    if (i > 0 && i < (_segments-1)) {
      tft.fillRect(origX, newOriginY + ((_segHeight * i) + (i * 2)), _barWidth, _segHeight, tft.Color565(rc, gc, bc)); //MIDDLE
    } else if (i == 0){
      tft.fillRect(origX, newOriginY + ((_segHeight * i) + (i * 2)), _barWidth, _segHeight, RA8875_YELLOW);//TOP
    } else {
      tft.fillRect(origX, newOriginY + ((_segHeight * i) + (i * 2)), _barWidth, _segHeight, RA8875_WHITE);//BOTTOM
    }
  }
  return true;
}

uint16_t createBars(uint16_t origX, uint8_t origY) {
  uint16_t i;
  uint16_t currentX;
  uint16_t resBar;
  uint16_t maxW = tft.width()-(_spacer+_barWidth);
  for (i = 0; i < maxW; i++) { //create the bars
    currentX = origX + (i * _barWidth) + (i * _spacer);
    if (currentX >= maxW){
      createBar(currentX, origY);
      return resBar;
    } else {
      createBar(currentX, origY);
      resBar++;
    }
  }
  return resBar;
}

void updateBar(uint8_t val, uint16_t origX, uint16_t origY,bool single) {
  uint16_t barHeight = origY + (_segHeight * _segments) + (_segments * 2);
  uint16_t newOriginY = tft.height() - barHeight;
  uint16_t currentBarH = map(val, 0, 256, _segHeight, (barHeight - origY));
  if (single) {
      tft.fillRect(origX, newOriginY, _barWidth, barHeight, 0x0000); //bar negative
    if (_barWidth < 2){
      tft.drawPixel(origX,currentBarH,0xFFFF);
    } else {
      tft.fillRect(origX, currentBarH, _barWidth, _segHeight, 0xFFFF); //bar positive
    }
  } else {
    if (val < 255) tft.fillRect(origX, newOriginY, _barWidth, (barHeight - origY) - currentBarH, 0x0000); //bar negative
    tft.fillRect(origX, (barHeight - newOriginY) - (currentBarH - _segHeight), _barWidth, currentBarH, 0xFFFF); //bar positive
  }
}

void updateBars(uint8_t vals[], uint16_t origX, uint16_t origY,uint16_t maxbars,bool single) {
  for (uint16_t i = 0; i <= maxbars; i++) { //create the bars
    updateBar(vals[i], origX + (i * _barWidth) + (i * _spacer), origY,single);
  }
}
