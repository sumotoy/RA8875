/*
The slight modified adafruit test for his libraries
 adapted for this one, shows how simple is deal with it.
 */

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10

#define RA8875_RESET 9//any pin or nothing!

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);


float p = 3.1415926;

void setup()
{
  tft.begin(RA8875_800x480);
}

uint8_t rot = 0;
void loop() {
  tft.setRotation(rot);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", RA8875_WHITE);
  delay(1000);
  tftPrintTest();
  delay(1000);
  testlines(RA8875_YELLOW);
  delay(500);
  testfastlines(RA8875_RED, RA8875_BLUE);
  delay(500);
  testdrawrects(RA8875_GREEN);
  delay(500);
  testfillrects(RA8875_YELLOW, RA8875_MAGENTA);
  delay(500);
  tft.fillWindow();
  testfillcircles(10, RA8875_BLUE);
  testdrawcircles(10, RA8875_WHITE);
  delay(500);
  testroundrects();
  delay(500);
  testtriangles();
  delay(500);
  mediabuttons();
  delay(1000);
  if (rot > 3) rot = 0;
  rot++;
}

void mediabuttons() {
  // play
  tft.fillWindow();
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
  tft.fillWindow();
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
  tft.fillWindow();
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
}

void testroundrects() {
  tft.fillWindow();
  uint16_t color = 100;
  uint16_t i;
  uint8_t t;
  uint16_t x, y, w, h;

  for (t = 0 ; t <= 4; t += 1) {
    x = 0;
    y = 0;
    w = tft.width() - 1;
    h = tft.height() - 1;
    for (i = 0 ; i < tft.width(); i += 1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x += 2;
      y += 3;
      w -= 4;
      h -= 6;
      color += 1100;
    }
    color += 100;
  }
}

void testtriangles() {
  tft.fillWindow();
  uint16_t maxw = tft.width() - 1;
  uint16_t maxh = tft.height() - 1;
  uint16_t k, t;
  for (k = 0; k < 64; k++) {
    for (t = 0 ; t <= 30; t += 1) {
      tft.fillTriangle(random(0, maxw), random(0, maxh), random(0, maxw), random(0, maxh), random(0, maxw), random(0, maxh), random(0x0000, 0xFFFF));
    }
    tft.fillWindow();
  }
}



void testdrawcircles(uint8_t radius, uint16_t color) {
  uint16_t x, y;
  for (x = 0; x < tft.width() + radius; x += radius * 2) {
    for (y = 0; y < tft.height() + radius; y += radius * 2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  uint16_t x, y;
  for (x = radius; x < tft.width(); x += radius * 2) {
    for (y = radius; y < tft.height(); y += radius * 2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}


void testdrawrects(uint16_t color) {
  uint16_t x;
  uint8_t inc = 2;
  uint16_t side;
  if (tft.isPortrait()) {
    side = tft.width();
  } else {
    side = tft.height();
  }
  tft.fillWindow();
  for (x = 0; x < side; x += inc) {
    tft.drawRect((tft.width() / 2) - (x / 2), (tft.height() / 2) - (x / 2) , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  uint16_t x = 0;
  uint16_t inc = 8;
  uint16_t side;
  if (tft.isPortrait()) {
    side = tft.width();
  } else {
    side = tft.height();
  }
  tft.fillWindow();
  for (x = side - 1; x > inc; x -= inc) {
    tft.fillRect((tft.width() / 2) - (x / 2), (tft.height() / 2) - (x / 2) , x, x, color1);
    tft.drawRect((tft.width() / 2) - (x / 2), (tft.height() / 2) - (x / 2) , x, x, color2);
  }
}

void testfastlines(uint16_t color1, uint16_t color2) {
  uint16_t x, y;
  uint8_t inc = 5;
  tft.fillWindow(RA8875_BLACK);
  for (y = 0; y < tft.height(); y += inc) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (x = 0; x < tft.width(); x += inc) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawtext(const char *text, uint16_t color) {
  tft.fillWindow();
  tft.setFontScale(0);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.print(text);
}

void testlines(uint16_t color) {
  uint16_t maxw = tft.width() - 1;
  uint16_t maxh = tft.height() - 1;
  uint16_t x, y;
  uint8_t incr = 3;

  tft.fillWindow();
  for (x = 0; x < tft.width(); x += incr) {
    tft.drawLine(0, 0, x, maxh, color);
  }
  for (y = 0; y < tft.height(); y += incr) {
    tft.drawLine(0, 0, maxw, y, color);
  }

  tft.fillWindow();
  for (x = 0; x < tft.width(); x += incr) {
    tft.drawLine(maxw, 0, x, maxh, color);
  }
  for (y = 0; y < tft.height(); y += incr) {
    tft.drawLine(maxw, 0, 0, y, color);
  }

  tft.fillWindow();
  for (x = 0; x < tft.width(); x += incr) {
    tft.drawLine(0, maxh, x, 0, color);
  }
  for (y = 0; y < tft.height(); y += incr) {
    tft.drawLine(0, maxh, maxw, y, color);
  }

  tft.fillWindow();
  for (x = 0; x < tft.width(); x += incr) {
    tft.drawLine(maxw, maxh, x, 0, color);
  }
  for (y = 0; y < tft.height(); y += incr) {
    tft.drawLine(maxw, maxh, 0, y, color);
  }
}