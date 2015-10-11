/* Touch screen super-easy paint!
  This version use the new touch screen functions, much easier!
  Did you have already calibrated your screen? Better do as soon you can
  Open TouchScreenCalibration example and follow instructions.
 Works with Arduino 1.0.6 IDE, Arduino 1.6.x IDE
*/

/*
You are using 4 wire SPI here, so:
 MOSI:11
 MISO:12
 SCK:13

 */

#include <SPI.h>
#include <RA8875.h>

/*
Teensy3.x and Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x
 MISO:  12//Teensy3.x
 SCK:   13//Teensy3.x
 the rest of pin below:
 */

#define RA8875_INT 2 //any pin
#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
*/
#define RA8875_RESET 9//any pin or nothing!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy3/arduino's


uint16_t tx, ty;


void interface(){
  tft.fillRect(10,10,40,40,RA8875_WHITE);
  tft.fillRect(10+(40*1)+(10*1),10,40,40,RA8875_BLUE);
  tft.fillRect(10+(40*2)+(10*2),10,40,40,RA8875_RED);
  tft.fillRect(10+(40*3)+(10*3),10,40,40,RA8875_GREEN);
  tft.fillRect(10+(40*4)+(10*4),10,40,40,RA8875_CYAN);
  tft.fillRect(10+(40*5)+(10*5),10,40,40,RA8875_MAGENTA);
  tft.fillRect(10+(40*6)+(10*6),10,40,40,RA8875_YELLOW);
  tft.drawRect(10+(40*7)+(10*7),10,40,40,RA8875_WHITE);
}

void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");

  tft.begin(RA8875_480x272);//initialize library

  tft.touchBegin(RA8875_INT);//enable Touch support!
  interface();
}

uint16_t choosenColor = 0;

void loop() 
{
  if (tft.touchDetect()){//easy!
      tft.touchReadPixel(&tx, &ty);//read directly in pixel!
      if (ty >= 0 && ty <= 55){ //interface area
        if ((tx > 10 && tx < (10+40))){
          choosenColor = RA8875_WHITE;
          interface();
          tft.fillRect(10,10,40,40,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*1)+(10*1) && tx < 10+(40*2)+(10*1))){
          choosenColor = RA8875_BLUE;
          interface();
          tft.fillRect(10+(40*1)+(10*1),10,40,40,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*2)+(10*2) && tx < 10+(40*3)+(10*2))){
          choosenColor = RA8875_RED;  
          interface();
          tft.fillRect(10+(40*2)+(10*2),10,40,40,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*3)+(10*3) && tx < 10+(40*4)+(10*3))){
          choosenColor = RA8875_GREEN;  
          interface();
          tft.fillRect(10+(40*3)+(10*3),10,40,40,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*4)+(10*4) && tx < 10+(40*5)+(10*4))){
          choosenColor = RA8875_CYAN;  
          interface();
          tft.fillRect(10+(40*4)+(10*4),10,40,40,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*5)+(10*5) && tx < 10+(40*6)+(10*5))){
          choosenColor = RA8875_MAGENTA;  
          interface();
          tft.fillRect(10+(40*5)+(10*5),10,40,40,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*6)+(10*6) && tx < 10+(40*7)+(10*6))){
          choosenColor = RA8875_YELLOW;  
          interface();
          tft.fillRect(10+(40*6)+(10*6),10,40,40,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*7)+(10*7) && tx < 10+(40*8)+(10*7))){
          choosenColor = 0;  
          interface();
          tft.fillRect(0,52,tft.width()-1,tft.height()-53,RA8875_BLACK);
          tft.fillCircle(tft.width()-10,10,5,RA8875_BLACK);
        }
      } else { //paint
        //if (choosenColor != 0) tft.fillCircle(tx,ty,1,choosenColor);
        if (choosenColor != 0) tft.drawPixel(tx,ty,choosenColor);
      }
  }
}
