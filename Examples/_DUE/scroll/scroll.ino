/*
Basic Scroll example

 Works with Arduino 1.0.6 IDE, Arduino 1.6.x IDE
 */


#include <SPI.h>
#include <RA8875.h>

/*
Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//DUE refere to arduino site
 MISO:  12//DUE refere to arduino site
 SCK:   13//DUE refere to arduino site
 the rest of pin below:
 */

#define RA8875_CS 10 //see below...
/*
DUE: should be any but not sure
*/
#define RA8875_RESET 9//any pin or nothing!

RA8875 tft = RA8875(RA8875_CS,RA8875_RESET);//Teensy3/arduino's



void setup() 
{
  Serial.begin(9600);
  //while (!Serial) {;}
  Serial.println("RA8875 start");
  //initialization routine
  tft.begin(RA8875_800x480);
  //tft.setRotation(1);

tft.println("Once upon a midnight dreary, while I pondered, weak and weary,");
tft.println("Over many a quaint and curious volume of forgotten lore,");
tft.println("While I nodded, nearly napping, suddenly there came a tapping,");
tft.println("As of some one gently rapping, rapping at my chamber door.");
tft.println("'Tis some visitor,' I muttered, 'tapping at my chamber door Only this, and nothing more.'");
tft.println("");
tft.println("Ah, distinctly I remember it was in the bleak December,");
tft.println("And each separate dying ember wrought its ghost upon the floor.");
tft.println("Eagerly I wished the morrow;- vainly I had sought to borrow");
tft.println("From my books surcease of sorrow- sorrow for the lost Lenore-");
tft.println("For the rare and radiant maiden whom the angels name Lenore-");
tft.println("Nameless here for evermore.");
}



void loop()
{
  tft.setScrollWindow(0, tft.width()-1, 0, 80);	//Specifies scrolling activity area
  uint16_t i;
  for (i = 0;i < 80; i++){
    tft.scroll(0, i);
    delay(10);
  }
  delay(500);
  for (i = 79;i >0; i--){
    tft.scroll(0, i);
    delay(10);
  }
  delay(500);
  for (i = 0;i < tft.width(); i++){
    tft.scroll(i, 0);
    delay(5);
  }
  delay(500);
  for (i = tft.width()-1;i > 0; i--){
    tft.scroll(i, 0);
    delay(5);
  }
  delay(500);
}