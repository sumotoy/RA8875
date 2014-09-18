/* Touch screen library calibration:
this will help you to calibrate your touch screen by modify
4 parameters inside RA8875.h file:

TOUCSRCAL_XLOW	//min value of x you can get
TOUCSRCAL_XHIGH	//max value of x you can get
TOUCSRCAL_YLOW	//min value of y you can get
TOUCSRCAL_YHIGH	//max value of y you can get

Normally those parameters are set as 0.
So open the RA8875.h file with your preferred text editor.
To perform the calibration you need to comment #define TOUCHINPIXELS
inside RA8875.h just for calibration, normally it's already disabled but
because it's useful maybe you enabled, in that case we need the pure ADC values
so we need off, after calibration you can pull back as it was.

Now run this program and open Serial Monitor:

1) Touch the TOP/LEFT side of your tft and watch the screen to identify the lowest
X and lowest Y values, put x in TOUCSRCAL_XLOW and y in TOUCSRCAL_YLOW.

2) now touch the BOTTOM/RIGHT side of your tft and find the max x and y.
Put those values inside TOUCSRCAL_XHIGH and TOUCSRCAL_YHIGH.

3) save RA8875.h file.

If everithing fine, send again this sketch and open Serial Monitor,
touch again TOP/LEFT and you should have values near 0 for both axists, in counterpart
you should have values around 1024 for the BOTTOM/RIGHT.

If everithing is acceptable, you got calibration!!
Do not forget to enable #define TOUCHINPIXELS if you are using it!
*/



#include <SPI.h>
#include <RA8875.h>
/*
You are using 4 wire SPI here, so:
MOSI:11
MISO:12
SCK:13
the rest of pin below:
*/
#define RA8875_INT 2//for touch screen
#define RA8875_CS 10//SS
#define RA8875_RESET 9//reset for the screen

uint16_t tx, ty;

#if defined (TOUCHINPIXELS)
  #error please comment TOUCHINPIXELS in RA8875.h file!
#endif

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

void setup() 
{
  Serial.begin(9600);
  while (!Serial) {;}
  Serial.println("RA8875 start");

  if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 Not ready!?!?!");
    while (1);
  }
  Serial.println("RA8875 ready");
  
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  
  tft.touchEnable(true);

}

void loop() 
{

  if (!digitalRead(RA8875_INT)) 
  {
    if (tft.touched()) 
    {
      Serial.print("Touch: "); 
      tft.touchRead(&tx, &ty);
      Serial.print("x: "); 
      Serial.print(tx,DEC); 
      Serial.print(" / y: "); 
      Serial.println(ty,DEC); 
    } 
  }
}
