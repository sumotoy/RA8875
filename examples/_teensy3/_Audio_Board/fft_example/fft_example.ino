/*
  FFT EXAMPLE
 This uses an RA8875 TFT display and the PJRC Audio Shield
 with Teensy 3.1. The purpose it's demostrate how to use both
 in the same time. The sketch provide also support for SD card and
 W25Q128 flash memory, a buzzer, a vibration motor and full touch screen support.
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RA8875.h>


//PIN definitions ------------------
#define RA_CS      2  //out
#define RA_RST     28 //out
#define RA_INT     1  //in
#define SD_CS      10 //out 
#define SD_DEC     29 //in
#define MEM_CS     6  //out
#define BUZ        5  //out - pwm
#define VIB        32 //out
//VARS ------------------------------
long counter = 0;
long fcounter = 50;
const uint8_t bands = 119;
uint8_t fftdata[bands];
//INSTANCES -------------------------

//this will instantiate RA8875 library with alternative mosi=11 and alternative SCLK=7
RA8875 tft = RA8875(RA_CS,RA_RST,11,7);

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioSynthWaveformSine sinewave;
AudioAnalyzeFFT1024    myFFT;
AudioOutputI2S         audioOutput;        // audio shield: headphones & line-out

// Connect either the live input or synthesized sine wave
//AudioConnection patchCord1(audioInput, 0, myFFT, 0);
AudioConnection patchCord1(sinewave, 0, myFFT, 0);

AudioControlSGTL5000 audioShield;

volatile bool bandChanged = false;

uint8_t createBar(uint16_t origX,uint16_t origY,uint8_t segments,uint8_t barWidth,uint8_t segHeight){
  //the real bar height
  uint16_t barHeight = origY + (segHeight * segments) + (segments * 2);
  if (barHeight > 271) return 0;//siamo fuori misura, exit
  //with vertical bars the Y origin starts from the bottom
  uint16_t newOriginY = 271-barHeight;
  uint8_t rc,gc,bc;//container for colors
  bc = 1;//blue always 0
  for (uint8_t i=0;i<segments;i++){
    gc = map(i,0,segments-1,100,255);//calculate green
    rc = map(i,segments-1,0,200,255);//calculate red
    tft.fillRect(origX,newOriginY+((segHeight*i)+(i*2)),barWidth,segHeight,tft.Color565(bc,gc,rc));//bar
  }
  return barHeight;
}

void createBars(uint16_t origX,uint16_t origY){
  uint8_t barWidth = 2;
  uint8_t segHeight = 2;
  uint8_t segments = 60;
  uint16_t barsWidth = origX+(barWidth*bands)+(bands*2);
  if (barsWidth > 479) return;//too big! exit
  tft.displayOn(false);
  tft.useLayers(true);//turn on layers
  tft.writeTo(L1);//write colored bars to layer 1
  for (uint8_t i=0;i<bands;i++){//create the bars
    if(!createBar(origX+(barWidth*i)+(i*2),origY,segments,barWidth,segHeight)) return;
  }
  tft.writeTo(L2);//from this point we write on layer 2
  tft.layerEffect(AND);//apply AND effect between layer 1 and 2
  tft.displayOn(true);
}

void updateBar(uint8_t val,uint16_t origX,uint16_t origY,uint8_t segments,uint8_t barWidth,uint8_t segHeight){
  uint16_t barHeight = origY + (segHeight * segments) + (segments * 2);
  uint16_t newOriginY = 271-barHeight;
  uint16_t currentBarH = map(val,0,255,segHeight,(barHeight-origY));
  if (val < 255) tft.fillRect(origX,newOriginY,barWidth,(barHeight-origY)-currentBarH,0x0000);//bar negative
  tft.fillRect(origX,(barHeight-newOriginY)-(currentBarH),barWidth,currentBarH,0xFFFF);//bar positive
}

void updateBars(uint16_t origX,uint16_t origY){
  uint8_t barWidth = 2;
  uint8_t segHeight = 2;
  uint8_t segments = 60;
  for (uint8_t i=0;i<bands;i++){//update the bars
    updateBar(fftdata[i],origX+(barWidth*i)+(i*2),origY,segments,barWidth,segHeight);
  }
}



void setup() {
  Serial.begin(9600);
  tft.begin(RA8875_480x272);
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);
  /*
  //SD card init
  if (!(SD.begin(SD_CS))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  */
  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
  //myFFT.windowFunction(NULL);

  // Create a synthetic sine wave, for testing
  // To use this, edit the connections above
  sinewave.amplitude(1.0);
  sinewave.frequency(5000.0);

  //createBars(0,20);
  /*
  0...124:50...
  */
}

void loop() {
  float n;
  int i;
  if (!bandChanged){
    createBars(0,20);
    bandChanged = true;
  }
  if (myFFT.available()) {
    // each time new FFT data is available
    // print it all to the Arduino Serial Monitor
    Serial.print("FFT: ");
    for (i=0; i<bands; i++) {
      n = myFFT.read(i);
      if (n >= 0.01) {
        fftdata[i] = n*255;
        Serial.print(fftdata[i]);
        Serial.print(" ");
      } 
      else {
        fftdata[i] = 0;
        Serial.print("  -  "); // don't print "0.00"
      }
    }
    Serial.println();
    //updateBars(40,50,20);

  }
    if (counter >= 8000){
      counter = 0;
      updateBars(0,20);
  if (fcounter >=5000){
    fcounter = 50;
  } else {
    fcounter = fcounter+100;
  }
  sinewave.frequency((float)fcounter);
    }
  counter++;

}
