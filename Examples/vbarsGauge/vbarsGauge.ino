/*
Just a basic example, it will do nothing, initialize screen and put black.
 Used as basic setup for experimenting with Arduino and Energia IDE's.
 Tested and worked with:
 Teensy3,Teensy3.1,Arduino UNO,Arduino YUN,Arduino Leonardo,Stellaris
 Works with Arduino 1.0.6 IDE, Arduino 1.5.8 IDE, Energia 0013 IDE
 */

#include <SPI.h>
#include <RA8875.h>

/*
Teensy3.x and Arduino's
 You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 MISO:  12//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 SCK:   13//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 the rest of pin below:
 */
#define RA8875_INT 3 //any pin
#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
 Arduino's 8 bit: any
 DUE: should be any but not sure
 */
#define RA8875_RESET 9//any pin or 255 to disable it!


#define WIDTH 480
const uint8_t _barWidth = 1;
const uint8_t _spacer = 1;
const uint8_t _segHeight = 2;
const uint8_t _segments = 60;
const unsigned int NBARS = WIDTH / 3;

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy


uint8_t fftdata[267];
//INSTANCES -------------------------

volatile bool started = false;




void setup()
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");

  tft.begin(RA8875_480x272);

}

void loop()
{
  if (!started) {
    createBars(0, 20);
    started = true;
  }
  for (unsigned int i = 0; i < NBARS; i++) {
    fftdata[i] = random(0, 255);
  }
  updateBars(fftdata, 0, 20);
}

uint8_t createBar(uint16_t origX, uint16_t origY) {

  //the real bar height
  uint16_t barHeight = origY + (_segHeight * _segments) + (_segments * 2);
  if (barHeight > tft.height()) return 0;//siamo fuori misura, exit
  //with vertical bars the Y origin starts from the bottom
  uint16_t newOriginY = tft.height() - barHeight;
  uint8_t rc, gc, bc; //container for colors
  bc = 1;//blue always 0
  for (int i = 0; i < _segments; i++) {
    gc = map(i, 0, _segments - 1, 100, 255); //calculate green
    rc = map(i, _segments - 1, 0, 200, 255); //calculate red
    tft.fillRect(origX, newOriginY + ((_segHeight * i) + (i * 2)), _barWidth, _segHeight, tft.Color565(bc, gc, rc)); //bar
  }
  return barHeight;
}

void createBars(uint16_t origX, uint16_t origY) {
  //uint16_t barsWidth = origX + (_barWidth * NBARS) + (NBARS * _spacer);
  //if (barsWidth > tft.width()) return;//too big! exit
  tft.useLayers(true);//turn on layers
  tft.writeTo(L1);//write colored bars to layer 1
  for (unsigned int i = 0; i < NBARS; i++) { //create the bars
    if (!createBar(origX + (_barWidth * i) + (i * _spacer), origY)) return;
  }
  tft.writeTo(L2);//from this point we write on layer 2
  tft.layerEffect(AND);//apply AND effect between layer 1 and 2
}

void updateBar(uint8_t val, uint16_t origX, uint16_t origY) {
  uint16_t barHeight = origY + (_segHeight * _segments) + (_segments * 2);
  uint16_t newOriginY = tft.height() - barHeight;
  uint16_t currentBarH = map(val, 0, 256, _segHeight, (barHeight - origY));
  if (val < 255) tft.fillRect(origX, newOriginY, _barWidth, (barHeight - origY) - currentBarH, 0x0000); //bar negative
  tft.fillRect(origX, (barHeight - newOriginY) - (currentBarH), _barWidth, currentBarH, 0xFFFF); //bar positive
}

void updateBars(uint8_t vals[], uint16_t origX, uint16_t origY) {
  //uint16_t barsWidth = origX+(barWidth*nBars)+(nBars*2);
  for (unsigned int i = 0; i < NBARS; i++) { //create the bars
    updateBar(vals[i], origX + (_barWidth * i) + (i * 2), origY);
  }
}


