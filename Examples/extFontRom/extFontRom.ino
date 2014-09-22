/******************************************************************

 ******************************************************************/
#include <SPI.h>
#include <RA8875.h>


#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;

void setup() 
{
  Serial.begin(9600);
  Serial.println("RA8875 start");

 
  if (!tft.begin(RA8875_480x272)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  tft.fillScreen(RA8875_BLACK);
  tft.changeMode(TEXT);
  tft.setTextColor(RA8875_WHITE);
  //now set the external rom font
  tft.setExternalFontRom(GT23L24T3Y,GB12345);
  //switch to external rom
  tft.setFont(EXT);
  //write some chinese....
  tft.print("ÉîÛÚÐñÈÕ¶«·½¿Æ¼¼ÓÐÏÞ¹«Ë¾"); 
  //switch to internal rom
  tft.setFont(INT);
  tft.setCursor(0,100);
  tft.print("ABCD 1234567890"); 

}

void loop() 
{
}
