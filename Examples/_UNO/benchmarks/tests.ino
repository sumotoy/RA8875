unsigned long testPixel() {
  tft.fillWindow();
  unsigned long start = micros();
  tft.drawPixel(0, 0, RA8875_WHITE);
  return micros() - start;
}

unsigned long testPixels() {
  int green = 0;
  tft.fillWindow();
  unsigned long start = micros();
  for (uint16_t i = 0; i < tft.width(); i++) {
    if (green > 255) green = 0;
    tft.drawPixel(i, 0, tft.Color565(0, green, 0));
    green++;
  }
  return micros() - start;
}

unsigned long testFillScreen() {
  tft.fillWindow();
  unsigned long start = micros();
  tft.fillWindow(RA8875_RED);
  return micros() - start;
}

unsigned long testText() {
  tft.clearScreen();
  tft.setTextColor(RA8875_WHITE);
  tft.setFontScale(0);
  unsigned long start = micros();
  tft.println("Hello World!");
  tft.setTextColor(RA8875_YELLOW);
  tft.setFontScale(1);
  tft.println(1234.56);
  tft.setTextColor(RA8875_RED);
  tft.println(0xFFFF, HEX);
  tft.println();
  tft.setTextColor(RA8875_GREEN);
  tft.setFontScale(3);
  tft.println("Hello");
  tft.setFontScale(0);
  tft.println("I implore thee, my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles, Or I will rend thee");
  tft.println("in the gobberwartswith my blurglecruncheon,");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  unsigned long result = micros() - start;
  return result;
}

unsigned long testLines(uint16_t color) {
  unsigned long start = 0;
  unsigned long t = 0;
  uint16_t i;

  tft.fillWindow();

  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(0, 0, i, tft.height() - 1, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(0, 0, tft.width() - 1, i, color);
  t = micros() - start;//exclude clear screen from count
  tft.fillWindow();
  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(tft.width() - 1, 0, i, tft.height() - 1, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(tft.width() - 1, 0, 0, i, color);
  t += micros() - start;
  tft.fillWindow();
  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(0, tft.height() - 1, i, 0, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(0, tft.height() - 1, tft.width() - 1, i, color);
  t += micros() - start;
  tft.fillWindow();
  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(tft.width() - 1, tft.height() - 1, i, 0, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(tft.width() - 1, tft.height() - 1, 0, i, color);
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  uint16_t i;
  tft.clearScreen();
  start = micros();
  for (i = 0; i < tft.height(); i += 5) tft.drawFastHLine(0, i, tft.width() - 1, color1);
  for (i = 0; i < tft.width(); i += 5) tft.drawFastVLine(i, 0, tft.height() - 1, color2);
  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  uint16_t i;
  uint16_t i2;
  uint16_t cx = tft.width() / 2;
  uint16_t cy = tft.height() / 2;
  uint16_t n = min(tft.width(), tft.height());
  tft.fillWindow();
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color);
  }
  return micros() - start;
}


unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start;
  unsigned long t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;
  tft.fillWindow();
  n     = min(tft.width(), tft.height());
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.fillRect(cx - i2, cy - i2, i, i, color1);
  }
  t = micros() - start;
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color2);
  }
  return t;
}


unsigned long testFilledCircles(uint8_t radius, uint16_t color1, uint16_t color2) {
  unsigned long start;
  //unsigned long t = 0;
  uint16_t x;
  uint16_t y;
  uint16_t r2 = radius * 2;
  tft.fillWindow();
  start = micros();
  for (x = radius; x < tft.width(); x += r2) {
    for (y = radius; y < tft.height(); y += r2) {
      tft.fillCircle(x, y, radius, color1);
      //t    += micros() - start;
      //tft.drawCircle(x, y, radius, color2);
    }
  }
  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  uint16_t x;
  uint16_t y;
  uint16_t r2 = radius * 2;
  start = micros();
  for (x = 0; x < (tft.width() - radius); x += r2) {
    for (y = 0; y < (tft.height() - radius); y += r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
  return micros() - start;
}

unsigned long testTriangles(uint16_t color) {
  unsigned long start;
  uint16_t i;
  uint16_t cx = (tft.width() / 2) - 1;
  uint16_t cy = (tft.height() / 2) - 1;
  uint16_t n = min(cx, cy);
  tft.fillWindow();
  start = micros();
  for (i = 0; i < n; i += 5) {
    // peak,bottom left,bottom right
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color);
  }
  return micros() - start;
}

unsigned long testFilledTriangles(uint16_t color1, uint16_t color2) {
  unsigned long start;
  unsigned long t = 0;
  uint16_t i;
  uint16_t cx = (tft.width() / 2) - 1;
  uint16_t cy = (tft.height() / 2) - 1;
  uint16_t n = min(cx, cy);
  tft.fillWindow();
  start = micros();
  for (i = n; i > 10; i -= 5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color1);
    //tft.Color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color2);
    //tft.Color565(i, i, 0));
  }
  return t;
}



unsigned long testFilledRoundRects() {
  unsigned long start;
  uint16_t i, d;
  tft.fillWindow();
  if (tft.getRotation() != 1 && tft.getRotation() != 3) {
    d = tft.height() - 1;
  } else {
    d = tft.width() - 1;
  }
  start = micros();
  for (i = d; i >= 10; i--) {
    tft.fillRoundRect(tft.width() / 2 - (i / 2), tft.height() / 2 - (i / 2), i, i, i / 4, tft.htmlTo565(0xFF0000 + i));
  }
  return micros() - start;
}

unsigned long testRoundRects() {
  unsigned long start;
  uint16_t i, d;
  tft.fillWindow();
  if (tft.getRotation() != 1 && tft.getRotation() != 3) {
    d = tft.height() - 1;
  } else {
    d = tft.width() - 1;
  }
  start = micros();
  for (i = d; i >= 10; i--) {
    tft.drawRoundRect(tft.width() / 2 - (i / 2), tft.height() / 2 - (i / 2), i, i, i / 4, random(0xFFFF));
  }
  return micros() - start;
}

void test(uint8_t rot) {
  tft.clearScreen();
  tft.setRotation(rot);
  tft.setCursor(CENTER,CENTER);
  tft.setTextColor(RA8875_WHITE);
  tft.setFontScale(2);
  tft.print("Rotation:");
  tft.print(rot);
  tft.setFontScale(2);
  tft.setCursor(0,0);
  delay(1000);
  Serial.print(F("screen:"));
  Serial.print(tft.width());
  Serial.print("x");
  Serial.print(tft.height());
  Serial.print(" - rotation:");
  Serial.print(rot);
  Serial.println(F("\nBenchmark                Time (microseconds)"));
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
  Serial.println(testFilledRects(RA8875_YELLOW, RA8875_MAGENTA));
  delay(DELAY_BETWEEN);

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, RA8875_MAGENTA, RA8875_YELLOW));
  delay(DELAY_BETWEEN);

  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, RA8875_WHITE));
  delay(DELAY_BETWEEN);

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles(RA8875_CYAN));
  delay(DELAY_BETWEEN);

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles(RA8875_RED, RA8875_CYAN));
  delay(DELAY_BETWEEN);

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(DELAY_BETWEEN);

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(DELAY_BETWEEN);


  Serial.println(F("--------------------------------\n"));
}