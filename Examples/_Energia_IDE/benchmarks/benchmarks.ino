#include <SPI.h>
#include <RA8875.h>

#define DELAY_BETWEEN 500

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
#endif

void setup() {
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_800x480);

  Serial.println(F("Benchmark                Time (microseconds)"));
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(DELAY_BETWEEN);
  
  Serial.print(F("Test Pixel               "));
  Serial.println(testPixel());
  delay(DELAY_BETWEEN);
  
  Serial.print(F("Test Pixels              "));
  Serial.println(testPixels());
  delay(DELAY_BETWEEN);
  
  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(DELAY_BETWEEN);

  Serial.print(F("Lines                    "));
  Serial.println(testLines(RA8875_CYAN));
  delay(DELAY_BETWEEN);

  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(RA8875_RED, RA8875_BLUE));
  delay(DELAY_BETWEEN);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(RA8875_GREEN));
  delay(DELAY_BETWEEN);

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(RA8875_YELLOW,RA8875_MAGENTA));
  delay(DELAY_BETWEEN);

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10,RA8875_MAGENTA,RA8875_YELLOW));
  delay(DELAY_BETWEEN);
  
  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10,RA8875_WHITE));
  delay(DELAY_BETWEEN);

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles(RA8875_CYAN));
  delay(DELAY_BETWEEN);

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles(RA8875_RED,RA8875_CYAN));
  delay(DELAY_BETWEEN);

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(DELAY_BETWEEN);

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(DELAY_BETWEEN);


  Serial.println(F("--------------------------------"));
}

void loop(void) {

}

