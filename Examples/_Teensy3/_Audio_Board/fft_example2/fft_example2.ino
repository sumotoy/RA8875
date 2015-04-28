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

//#define USEINTERNALSINE 1
#define USEMIKE 1
//#define USELINE 1
//PIN definitions ------------------
#define RA_CS      2  //out 2
#define RA_RST     29 //out/28
#define RA_INT     3  //in
#define SD_CS      10 //out 
#define MEM_CS     6  //out
//#define BUZ        5  //out - pwm
//#define VIB        32 //out

static inline uint8_t floatToInt(float x){
  return (int)(x * 255.0f);
}

//VARS ------------------------------
#define  STDBARWIDTH  1
#define  SCREENWIDE   480
const int _bands = 159;
const int _fullRangeBands = (SCREENWIDE/2)-1;
long fcounter = 50;
uint8_t bandRange = 4;
uint8_t oldbandRange = 255;
uint8_t _barWidth = STDBARWIDTH;
uint8_t _segHeight = 2;
const uint8_t _segments = 60;
/*
0: 50...5K (119 bands)
 1: 5K...10K (119 bands)
 2: 10K..15K (119 bands)
 3: 15K..20K (119 bands)
 4: 50...20K (119 bands interpolated)
 */
elapsedMillis chgMsec=0;
elapsedMillis chgFsec=0;
volatile uint8_t fftdata[(SCREENWIDE/2)];
//INSTANCES -------------------------

//this will instantiate RA8875 library with alternative mosi=11 and alternative SCLK=7
RA8875 tft = RA8875(RA_CS,RA_RST,11,7);
#if defined(USEINTERNALSINE) || defined(USELINE)
const int myInput = AUDIO_INPUT_LINEIN;
#else
const int myInput = AUDIO_INPUT_MIC;
#endif

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioSynthWaveformSine sinewave;
AudioAnalyzeFFT1024    myFFT;
AudioOutputI2S         audioOutput;        // audio shield: headphones & line-out

// Connect either the live input or synthesized sine wave
#if defined(USELINE) || defined(USEMIKE)
AudioConnection patchCord1(audioInput, 0, myFFT, 0);
#elif defined(USEINTERNALSINE) 
AudioConnection patchCord1(sinewave, 0, myFFT, 0);
#endif

AudioControlSGTL5000 audioShield;

void setup() {
  pinMode(SD_CS,OUTPUT);
  digitalWrite(SD_CS,HIGH);
  pinMode(RA_CS,OUTPUT);
  digitalWrite(RA_CS,HIGH);
  Serial.begin(9600);
  while (!Serial) ;
  delay(50);
  SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
  SPI.setSCK(14);  // Audio shield has SCK on pin 14

    // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);
  audioShield.inputLevel(0.9);
  tft.begin(RA8875_480x272);
  Serial.println("TFT inited!");
  //SD card init
  if (!(SD.begin(SD_CS))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  } 
  else {
    Serial.println("SD card ok");
  }
  
  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
  //myFFT.windowFunction(NULL);

  // Create a synthetic sine wave, for testing
  // To use this, edit the connections above
#if defined(USEINTERNALSINE)
  sinewave.amplitude(1.0);
  sinewave.frequency(10000.0);//11025 max
#endif

}
int startBand = 0;
int endBand = _bands;

void loop() {


  if (oldbandRange != bandRange){
    switch(bandRange){
    case 0://50...5K (119 bands)
      startBand = 0;      //0
      endBand = _bands;    //119
      _barWidth = STDBARWIDTH;
      _segHeight = 2;
      createBars(_bands,0,20);
      break;
    case 1://5K...10K (119 bands)
      startBand = _bands;  //119
      endBand = _bands*2;  //238
      _barWidth = STDBARWIDTH;
      _segHeight = 2;
      createBars(_bands,0,20);
      break;
    case 2://10K..15K (119 bands)
      startBand = _bands*2;//238
      endBand = _bands*3;  //357
      _barWidth = STDBARWIDTH;
      _segHeight = 2;
      createBars(_bands,0,20);
      break;
    case 3://15K..20K (119 bands)
      startBand = _bands*3;//357
      endBand = _bands*4;  //479
      _barWidth = STDBARWIDTH;
      _segHeight = 2;
      createBars(_bands,0,20);
      break;
    case 4://50...20K (119 bands interpolated)
      startBand = 0;   //0
      endBand = _fullRangeBands;    //51
      _barWidth = 1;
      _segHeight = 2;
      createBars(_fullRangeBands,0,20);
      break;
    default:
      bandRange = 0;
      startBand = 0;      //0
      endBand = _bands;    //119
      _barWidth = STDBARWIDTH;
      _segHeight = 2;
      createBars(_bands,0,20);
      break;
    } 
    oldbandRange = bandRange;
  }

  if (myFFT.available()) {//fill fft data
    float n = 0;
    //data read
    for (int i=startBand; i<endBand; i++) {//max 511 bands
      fftdata[i-startBand] = 0;
      if (bandRange != 4){
        n = myFFT.read(i);
      } 
      else {
        //n = myFFT.read(5*i,5+(5*i));//group 5 bands per time
        if (i < 231){
          //460 to 462 bands grouped by 2
          n = myFFT.read(2*i,2+(2*i));//group 2 bands per time
          //} else if (i > 230 && i < 237){
          //n = myFFT.read(6*i,6+(6*i));//group 2 bands per time
        } 
        else {
          if (i == 231){
            n = myFFT.read(463,468);
          } 
          else if (i == 232){
            n = myFFT.read(469,474);
          } 
          else if (i == 233){
            n = myFFT.read(475,480);
          } 
          else if (i == 234){
            n = myFFT.read(481,486);
          } 
          else if (i == 235){
            n = myFFT.read(486,491);
          } 
          else if (i == 236){
            n = myFFT.read(492,497);
          } 
          else if (i == 237){
            n = myFFT.read(498,503);
          }
          else {
            n = myFFT.read(504,511);
          }
        }
      }
      //fftdata[i-startBand] = (n*255)*10;
      fftdata[i-startBand] = floatToInt(n*10);
    }
  }//end fft
  //Update bargraps
  if (chgMsec > 3) {//10ms
    if (bandRange != 4){
      updateBars(_bands,0,20);
    } 
    else {
      updateBars(_fullRangeBands,0,20);
    }
    chgMsec = 0;
  }

#if defined(USEINTERNALSINE)
  if (chgFsec > 5){
    if (fcounter >=11000){
      fcounter = 50;
    } 
    else {
      fcounter = fcounter+100;
    }
    sinewave.frequency((float)fcounter);
    chgFsec = 0;
  }
#endif

}








