/*
Basic Text Functions (only internal fonts)
 Here's an example of how to use text commands on RA8875
 It shows many commands and all commented so you can figure out the library ability
*/

#include <SPI.h>
#include <RA8875.h>


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

void setup()
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");
  //initialization routine
  tft.begin(RA8875_800x480);//or whatever you have
  //following it's already by begin function but
  //if you like another background color....
  tft.fillWindow(RA8875_BLACK);//fill window black
  //now set a text color, background transparent
  tft.setTextColor(RA8875_WHITE);
  Serial.println(tft.getFontWidth(true));
  Serial.println(tft.getFontHeight(true));
  //use the classic print an println command
  tft.print("Hello World");
  //by default the text location is set to 0,0
  //now set it at 50,20 pixels and different color
  tft.setCursor(50, 20); //set cursor work in pixel!!!
  //this time we not using transparent background
  tft.setTextColor(RA8875_RED, RA8875_GREEN);
  tft.print("Hello World");
  //by default we using the internal font
  //so some manipulation it's possible
  tft.setFontScale(1);//font x1
  Serial.println(tft.getFontWidth());
  Serial.println(tft.getFontHeight());
  tft.setTextColor(RA8875_RED);
  tft.print("Hello World");
  //We can use current cursor location for some job...
  int16_t currentX, currentY;//we need this for store current cursor location
  tft.getCursor(currentX, currentY);//now we transfer to the currentX,currentY vars the location
  //now we have the location, lets draw a white pixel
  tft.drawPixel(currentX, currentY, RA8875_WHITE); //did you see the white dot?
  tft.setFontScale(0);//reset font scale
  //here's an alternative...
  //tft.setFontScale(0,1);xscale,yscale arbitrary x & y scaling
  tft.setCursor(0, 50);
  tft.setTextColor(RA8875_YELLOW);
  tft.println("ABCDEF 1 2 3 4");//this time println!
  tft.setFontSpacing(5);//now give 5 pix extra spacing between chars
  tft.println("ABCDEF 1 2 3 4");
  tft.setFontSpacing(0);//reset spacing
  //here's a useful feature of setCursor
  tft.setCursor(CENTER, CENTER); //this set text exact in the screen regardless his lenght and scale
  tft.setFontScale(1);//font x2
  tft.setTextGrandient(RA8875_YELLOW,RA8875_MAGENTA);
  tft.print("I'm in the center");
  //now use the autocenter feature
  tft.setFontScale(0);//reset font scale
  tft.drawRect((tft.width() / 2) - 25, (tft.height() / 2) - 100, 50, 50, RA8875_RED);//draw a box above the center
  tft.setTextColor(RA8875_YELLOW);
  //this will center the text inside the box!!!
  tft.setCursor((tft.width() / 2), (tft.height() / 2) - 75, true);
  tft.print("AB");
  tft.setFontScale(0);//reset font scale
  tft.setTextColor(RA8875_CYAN, RA8875_BLACK);
  tft.setCursor(50, 100);
  // Now play with cursor styles
  tft.print("Cursor Example: IBEAM");
  tft.showCursor(IBEAM, true); //activate cursor iBeam blinking
  delay(3000);
  tft.setCursor(50, 100);
  tft.print("Cursor Example: UNDER");
  tft.showCursor(UNDER, true); //activate cursor iBeam blinking
  delay(3000);
  tft.setCursor(50, 100);
  tft.print("Cursor Example: BLOCK");
  tft.showCursor(BLOCK, true); //activate cursor iBeam blinking
  delay(3000);
  tft.setCursor(50, 100);
  tft.print("                        ");
  tft.showCursor(NOCURSOR, false); //deactivate cursor
  delay(1000);
  tft.setFontScale(0, 3); //font x0,x4
  tft.setTextColor(RA8875_CYAN, RA8875_BLACK);
  //here's another unusual command, setFontAdvance enable/disable font advance
  //so you don't have to use setCursor a lot when you need to update numbers on screen
  tft.cursorIncrement(false);//turn off autoAdvance
  tft.setCursor(100, 100);
  for (uint8_t i = 0; i < 10; i++) {
    tft.print(i, DEC);
    delay(200);
  }
  tft.print(" ");
  tft.cursorIncrement(true);//back to normal
  tft.setFontScale(2);//font x3
  tft.setTextColor(RA8875_BLUE, RA8875_BLACK);
}

unsigned long i = 0;

void loop()
{
  tft.setCursor(50, 100);
  if (i > 999) {
    tft.setTextColor(RA8875_CYAN, RA8875_BLACK);
  }
  if (i > 9999) {
    tft.setTextColor(RA8875_MAGENTA, RA8875_BLACK);
  }
  if (i > 99999) {
    tft.setTextColor(RA8875_RED, RA8875_BLACK);
  }
  if (i > 999999) i = 0;
  tft.print(i, DEC);
  delay(1);
  i++;
}