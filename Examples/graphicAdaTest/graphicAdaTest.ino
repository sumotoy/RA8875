/*
The slight modified adafruit test for his libraries
adapted (easily) for this one, shows how simple is deal with it.
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
#define RA8875_INT 2 //any pin
#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
Arduino's 8 bit: any
DUE: should be any but not sure
*/
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
#else

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy3/arduino's

#endif

float p = 3.1415926;


void mediabuttons() {
  // play
  tft.fillScreen(RA8875_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, RA8875_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, RA8875_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, RA8875_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, RA8875_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, RA8875_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, RA8875_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, RA8875_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, RA8875_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, RA8875_GREEN);
}


void tftPrintTest() {
  //tft.setTextWrap(false);
  //tft.changeMode(TEXT);
  tft.fillScreen(RA8875_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(RA8875_RED);
  tft.setFontScale(1);
  tft.println("Hello World!");
  tft.setTextColor(RA8875_YELLOW);
  tft.setFontScale(2);
  tft.println("Hello World!");
  tft.setTextColor(RA8875_GREEN);
  tft.setFontScale(3);
  tft.println("Hello World!");
  tft.setTextColor(RA8875_BLUE);
  tft.setFontScale(2);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(RA8875_BLACK);
  tft.setTextColor(RA8875_WHITE);
  tft.setFontScale(0);
  tft.println("Hello World!");
  tft.setFontScale(1);
  tft.setTextColor(RA8875_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(RA8875_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(RA8875_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(RA8875_WHITE);
  tft.print(" seconds.");
  //tft.changeMode(GRAPHIC);
}

void testroundrects() {
  tft.fillScreen(RA8875_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = 127;
    int h = 159;
    for(i = 0 ; i <= 24; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void testtriangles() {
  tft.fillScreen(RA8875_BLACK);
  uint16_t p1x,p1y, p2x,p2y, p3x,p3y;
  uint16_t colour;
  for (uint16_t k = 0; k < 128; k++) {
    for(uint16_t t = 0 ; t <= 30; t+=1) {
      p1x=random(0,tft.width()-1);        //get a random number 0-319
      p1y=random(0,tft.height()-1);       //get a random number 0-239
      p2x=random(0,tft.width()-1);        //get a random number 0-319
      p2y=random(0,tft.height()-1);       //get a random number 0-239       
      p3x=random(0,tft.width()-1);        //get a random number 0-319
      p3y=random(0,tft.height()-1);       //get a random number 0-239     
      colour=random(0,65536);          //get a random number 0-65535
      //draw the triangle
      tft.fillTriangle(p1x, p1y, p2x, p2y, p3x, p3y, colour);
    }
    tft.fillScreen(RA8875_BLACK);
  }
}



void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

//bug!
void testdrawrects(uint16_t color) {
  tft.fillScreen(RA8875_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x+1, x+1, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(RA8875_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(RA8875_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setFontScale(0);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  //tft.setTextWrap(true);
  tft.print(text);
}

void testlines(uint16_t color) {
  tft.fillScreen(RA8875_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(RA8875_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(RA8875_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(RA8875_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}

void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");

  tft.begin(RA8875_480x272);

  uint16_t time = millis();
  tft.fillScreen(RA8875_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  tft.fillScreen(RA8875_BLACK);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", RA8875_WHITE);
  delay(1000);

  // tft print function!
  tftPrintTest();
  delay(1000);

  // line draw test
  testlines(RA8875_YELLOW);
  delay(500);

  testfastlines(RA8875_RED, RA8875_BLUE);
  delay(500);

  testdrawrects(RA8875_GREEN);
  delay(500);

  testfillrects(RA8875_YELLOW, RA8875_MAGENTA);
  delay(500);
  tft.fillScreen(RA8875_BLACK);
  testfillcircles(10, RA8875_BLUE);
  testdrawcircles(10, RA8875_WHITE);
  delay(500);

  testroundrects();
  delay(500);

  testtriangles();
  delay(500);


  mediabuttons();
  delay(500);
  Serial.println("done");
  delay(1000);
}

void loop(){
}