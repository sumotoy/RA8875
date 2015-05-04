/*
----- Warning! HAve some bugs! - Preliminary---
Simple test of BTE block move with the colour-expansion set
(This allows a one-bit-per-pixel font to be stored on layer2 in a compact form)
BTEMoveMonochrome
*/

#include <SPI.h>
#include <RA8875.h>

#define RA8875_CS 10 //see below...
#define RA8875_RESET 9//any pin or 255 to disable it!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//arduino's

//copied from custom char demo
const uint8_t battery1[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint16_t smiley[16] = {
  0b0000000000000000,
  0b0000001111000000,
  0b0000110000110000,
  0b0001000000001000,
  0b0010000000000100,
  0b0010011001100100,
  0b0100011001100010,
  0b0100000000000010,
  0b0100000000000010,
  0b0100000000000010,
  0b0010010000100100,
  0b0010001111000100,
  0b0001000000001000,
  0b0000110000110000,
  0b0000001111000000,
  0b0000000000000000
};

void setup() 
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 400)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_800x480);
  
  //copy the custom char onto the 2nd layer so it's stored onboard the RA8875
  tft.writeTo(L2);
  tft.layerEffect(LAYER2); //not necessary, but useful to see what this layer looks like
  //drawPixels doesn't work, because the BTE uses every second 16-bit word
  //tft.drawPixels((uint16_t *)battery1, sizeof(battery1)/2, 0, 0); //drawpixels always transfers 16-bit words
  for(byte i=0;i<sizeof(battery1);i+=1) {
    tft.drawPixel(i*2, 0, battery1[i]);
    //because it skips every second word, we can interleave another character on the odd-numbered words
    //We can't interleave 4 8-wide chars in the space of one 16-wide because we can't select the appropriate start bit in the word
    tft.drawPixel(i*2+1, 0, smiley[i]);
  }
  //create a simple 32-wide character.
  for(byte i=0;i<40*2*2;i+=32) {
    tft.drawPixel(i   , 1, 0b0000000110000001);    tft.drawPixel(i+1 , 1, 0b0000000110000000);
    tft.drawPixel(i+4 , 1, 0b0000001111000001);    tft.drawPixel(i+5 , 1, 0b0000001111000000); //it uses 2 words, then skips 2 words
    tft.drawPixel(i+8 , 1, 0b0000011111100001);    tft.drawPixel(i+9 , 1, 0b0000011111100000);
    tft.drawPixel(i+12, 1, 0b0000111111110001);    tft.drawPixel(i+13, 1, 0b0000111111110000);
    tft.drawPixel(i+16, 1, 0b0001111111111001);    tft.drawPixel(i+17, 1, 0b0001111111111000);
    tft.drawPixel(i+20, 1, 0b0011111111111101);    tft.drawPixel(i+21, 1, 0b0011111111111100);
    tft.drawPixel(i+24, 1, 0b0111111111111111);    tft.drawPixel(i+25, 1, 0b0111111111111110);
    tft.drawPixel(i+28, 1, 0b0000000000000000);    tft.drawPixel(i+29, 1, 0b0000000000000000);
  }
  //delay(2000);  //uncomment this if you want to see what layer 2 looks like
  
  tft.writeTo(L1);
  tft.layerEffect(LAYER1);
  tft.setRotation(0); //This demo works in rotation 0 and 2 (landscape) Portrait will require some more work.
  //fill a gradient, so we can test BTEing stuff around the screen
  for(int i=0;i<tft.height();i++) tft.drawLine(0,i,tft.width()-1,i,tft.Color565(map(i,0,tft.height(),128,40), map(i,0,tft.height(),255,40), map(i,0,tft.height(),40,128)));
  

  tft.setTextColor(RA8875_PINK,RA8875_BLUE);
  tft.setCursor(30,30);
  tft.print("HELLO WORLD!");
  //The transparent-move option uses the foreground colour as the transparent colour.
  tft.setTextColor(RA8875_BLUE);
  tft.BTE_Move(20,20,30,30,200,200,0,0,true);
  delay(1); //Wait for the BTE move to finish

  //Demonstrate the monochrome colour-expansion mode of the BTE move
  tft.setTextColor(RA8875_WHITE,RA8875_DARK_ORANGE);
  //draw the monochrome bitmap in normal FG+BG mode
  tft.BTE_Move(0,1,32,40,10,145, 2, 1, false, 0xE0,true);
  //don't wait for the move to complete - the next BTE_Move will check the busy status before it carries out its orders
  //However other tft commands here with no delay may halt the move in progress
  
  //Changing the width does strange things:
  //8 wide means it only uses the least-significant 8 bits of every second word (ROP=0x70)
  //16 wide uses all the bits of every second word (ROP=0xF0)
  //32 wide uses word 0 and 1, then skips 2 and 3 before going to word 4 (ROP=0xE0)
  //Other widths are possible but you need to choose different ROP for each one
    

  //now try it with the transparent BG option
  tft.BTE_Move(0,0,8,16,30,190, 2, 1, true, 0x70, true);
  
  //This 8-wide character can also be rendered 16 wide, with a different ROP, because there's nothing in the upper half of each word
  tft.BTE_Move(0,0,16,16,40,190, 2, 1, true, 0xF0, true);
  
  //show the interleaved character (source starts from x coordinate=1) 
  tft.BTE_Move(1,0,16,16,58,190, 2, 1, true, 0xF0, true);
}

void loop() 
{

}