
#include <SPI.h>
#include <RA8875.h>


#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!




RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy


uint16_t colorLightGray = tft.Color565(192,192,192);
uint16_t colorGray = tft.Color565(127,127,127);
uint16_t colorDarkGray = tft.Color565(64,64,64);

void setup()
{
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  //tft.setRotation(1);
}

void loop()
{
  screenLoading();
  screenClock();
  screenPie();
  screenSensors();
}


void screenClock()
{
  const uint16_t x = tft.width() / 2;
  const uint16_t y = tft.height() / 2;
  tft.clearScreen();
  tft.setCursor(x,y,true);
  tft.setFontScale(2);
  tft.setTextColor(RA8875_WHITE,RA8875_BLACK);
  tft.print("15:06");
  tft.drawArc(x, y, 102, 11, 0, 225, colorLightGray);	// 15 hours
  tft.drawArc(x, y, 113, 8, 0, 36, colorGray);	// 6 minutes
  tft.drawArc(x, y, 120, 5, 0, 360, colorDarkGray);	// seconds

  for (uint16_t d = 324; d < 372; d++)
  {
    tft.drawArc(x, y, 120, 5, d - 1, d + 1, RA8875_RED);
    tft.drawArc(x, y, 120, 5, d - 2, d - 1, colorDarkGray);	// to erase the red
    if (d == 360)
    {
      tft.drawArc(x, y, 113, 8, 0, 42, colorGray);	// 7 minutes
    }
    delay(166);
  }
}

void screenLoading()
{
  const uint16_t x = tft.width() / 2;
  const uint16_t y = tft.height() / 2;
  tft.clearScreen();
  tft.drawArc(x, y, 10, 5, 0, 360, colorLightGray);

  for (int i = 0; i < 2880; i += 4)
  {
    tft.drawArc(x, y, 10, 5, (i >> 1) - 45, (i >> 1) + 45, colorDarkGray);
    tft.drawArc(x, y, 10, 5, (i >> 1) - 45 - 4, (i >> 1) - 45, colorLightGray);

    tft.drawArc(x, y, 20, 5, 1440 - i - 45, 1440 - i + 45, colorDarkGray);
    tft.drawArc(x, y, 20, 5, 1440 - i + 45, 1440 - i + 45 + 4, colorLightGray);
  }
}

void screenPie()
{
  const uint16_t x = 159;
  const uint16_t y = 119;
  const uint16_t radius = 80;

  tft.clearScreen();
  tft.drawArc(x + 3, y - 2, radius, radius, 0, 60, tft.Color565(198, 255, 13));
  tft.drawArc(x - 3, y + 7, radius + 10, radius + 10, 60, 340, tft.Color565(255, 0, 54));
  tft.drawArc(x - 2, y - 3, radius, radius, 340, 360, tft.Color565(0, 255, 241));
  tft.setFontScale(0);
  tft.setTextColor(RA8875_BLACK);
  tft.setCursor(175,70);
  tft.print("16%");
  tft.setCursor(140,150);
  tft.print("78%");
  tft.setTextColor(colorLightGray);
  tft.setCursor(132,18);
  tft.print("6%");
  delay(2000);
}


void screenSensors()
{
  const uint16_t radius = 55;
  float temp = 22.4;
  uint16_t hum = 73, lux = 1154;

  const uint16_t s1x = 0;
  const uint16_t s1y = 10;

  const uint16_t s2x = 110;
  const uint16_t s2y = 70;

  const uint16_t s3x = 210;
  const uint16_t s3y = 130;

  tft.clearScreen();
  tft.setFontScale(0);
  tft.drawArc(s1x + radius, s1y + radius, radius, 10, -3, 3, tft.Color565(127, 0, 27));
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(0,25);
  tft.print("C ");
  tft.print(temp);
  tft.print(" f");
  //tft.fillRect(s1x + radius - 7, s1y + radius + 17, 2, 2, RA8875_WHITE);// degrees symbol

  for (uint16_t d = 1; d < temp * 10; d++)
  {
    tft.drawArc(s1x + radius, s1y + radius, radius, 10, d, d + 3, tft.Color565(127, 0, 27));
    tft.drawArc(s1x + radius, s1y + radius, radius - 3, 4, d - 1, d, tft.Color565(255, 0, 54));
  }

  tft.drawArc(s2x + radius, s2y + radius, radius, 10, -3, 3, tft.Color565(0, 43, 127));
  
  tft.setTextColor(RA8875_WHITE);
  tft.setCursor(0,25);
  tft.print(hum);
  tft.print(" %");

  for (uint16_t d = 1; d < 263; d++)
  {
    tft.drawArc(s2x + radius, s2y + radius, radius, 10, d, d + 3, tft.Color565(0, 43, 127));
    tft.drawArc(s2x + radius, s2y + radius, radius - 3, 4, d - 1, d, tft.Color565(0, 86, 255));
  }

  tft.drawArc(s3x + radius, s3y + radius, radius, 10, -3, 3, tft.Color565(127, 103, 6));


  for (uint16_t d = 1; d < 42; d++)
  {
    tft.drawArc(s3x + radius, s3y + radius, radius, 10, d, d + 3, tft.Color565(127, 103, 6));
    tft.drawArc(s3x + radius, s3y + radius, radius - 3, 4, d - 1, d,  tft.Color565(255, 206, 13));
  }

  delay(1000);
  for (uint16_t d = 1; d < 660; d++)
  {
    if (d < 220 )
    {
      lux += 21;

      tft.drawArc(s3x + radius, s3y + radius, radius, 10, ((float)lux / (float)10000) * 360, ((float)lux / (float)10000) * 360 + 3, tft.Color565(127, 103, 6));
      tft.drawArc(s3x + radius, s3y + radius, radius - 3, 4, ((float)lux / (float)10000) * 360 - 4, ((float)lux / (float)10000) * 360, tft.Color565(255, 206, 13));
    }

    if (d == 220)
      delay(1000);

    if (d > 220 && d % 15 == 0)
    {
      temp += 0.1;
      tft.drawArc(s1x + radius, s1y + radius, radius, 10, temp * 10, temp * 10 + 3, tft.Color565(127, 0, 27));
      tft.drawArc(s1x + radius, s1y + radius, radius - 3, 4, temp * 10 - 2, temp * 10, tft.Color565(255, 0, 54));
      delay(random(350, 700));
    }
    if (d > 300 && d < 600 && d % 38 == 0)
    {
      hum += 1;
      tft.drawArc(s2x + radius, s2y + radius, radius, 10, (float)hum * 3.6 - 4, (float)hum * 3.6 + 3, tft.Color565(0, 43, 127));
      tft.drawArc(s2x + radius, s2y + radius, radius - 3, 4, (float)hum * 3.6 - 5, (float)hum * 3.6, tft.Color565(0, 86, 255));
    }
  }
}
