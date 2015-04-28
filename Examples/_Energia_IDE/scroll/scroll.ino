/*
Basic Scroll example
 Tested and worked with:
 Stellaris
 Works with Energia 0013 IDE
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
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");
  //initialization routine
  tft.begin(RA8875_800x480);
  //tft.setRotation(1);

  //RA8875 it's capable to draw graphic but also Text
  //here we switch to TEXT mode
  //tft.changeMode(TEXT);
  //now set a text color, background transparent
  tft.setTextColor(RA8875_WHITE);
  //use the classic print an println command
  tft.print("Hello World");
  //by default the text location is set to 0,0
  //now set it at 50,20 pixels and different color
  tft.setCursor(50,20);//set cursor work in pixel!!!
  //this time we not using transparent background
  tft.setTextColor(RA8875_RED,RA8875_GREEN);
  tft.print("Hello World");
  //by default we using the internal font
  //so some manipulation it's possible
  tft.setFontScale(1);//font x2
  tft.setTextColor(RA8875_RED);
  tft.print("Hello World");
  //You notice that font location has been 
  //automatically increased by chip, unfortunatly not
  //tracked by library but we can use a command for that...
  uint16_t currentX,currentY;
  tft.getCursor(&currentX,&currentY);
  //now we have the location, lets draw a white pixel
  //tft.changeMode(GRAPHIC);//first we swith in graphic mode
  tft.drawPixel(currentX,currentY,RA8875_WHITE);
  //did you see the white dot?
  //tft.changeMode(TEXT);//go back to text mode
  tft.setFontScale(0);//font x1
  tft.setCursor(0,50);
  tft.setTextColor(RA8875_YELLOW);
  tft.println("ABCDEF 1 2 3 4");//this time println!
  tft.setFontSpacing(5);//now give 5 pix spacing
  tft.println("ABCDEF 1 2 3 4");
  tft.setFontSpacing(0);//reset
  tft.setFontScale(2);//font x1
  tft.setTextColor(RA8875_BLUE,RA8875_BLACK);
}

uint16_t i = 0;

void loop() 
{
  tft.setScrollWindow(0,320,0,60);	//Specifies scrolling activity area
  i=0; 
  while(i++<60){
    delay(10); 
    tft.scroll(i,i);
  } //Note:  scroll offset value must be less than  scroll setting range
  while(i-->0){
    delay(10); 
    tft.scroll(i,i);
  }       
  while(i++<60){
    delay(10); 
    tft.scroll(i,i);
  }
  while(i-->0){
    delay(10); 
    tft.scroll(i,i);
  }
}