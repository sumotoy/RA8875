/* Simply Touch screen library calibration V2:
 this will help you to calibrate your touch screen by modify
 4 parameters inside RA8875/_utility/RA8875Calibration.h file:

 TOUCSRCAL_XLOW  //min value of x you can get
 TOUCSRCAL_XHIGH  //max value of x you can get
 TOUCSRCAL_YLOW //min value of y you can get
 TOUCSRCAL_YHIGH  //max value of y you can get

 Normally those parameters are set as 0.

  Only for RESISTIVE TOUCH SCREEN!

 It's not a bullet-proof scientist alghorithm but calibrate
 using this method will be fast and enough accurate for basic
 touch screen operations like buttons, etc. If you need more
 precision and you don't want waste resources consider a capacitive touch!

 Now run this program and open Serial Monitor or follow screen instrunctions.

 Works with Arduino 1.0.6 IDE, Arduino 1.6.x IDE
*/


#include <SPI.h>
#include <RA8875.h>


#define RA8875_INT PF_4 //any pin (this case I'm using stellaris)
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

const uint8_t samples = 20;
uint16_t tempData[samples][2];
volatile int count = 0;
uint16_t tx, ty;//used as temp
uint16_t _XLOW_VAR;
uint16_t _YLOW_VAR;
uint16_t _XHIGH_VAR;
uint16_t _YHIGH_VAR;
bool proceed;

int scount = 0;


void setup() {
  Serial.begin(38400);
  //long unsigned debug_start = millis ();
  //while (!Serial && ((millis () - debug_start) <= 5000)) ;
  tft.begin(RA8875_800x480);//initialize RA8875
/* Adafruit_480x272 Adafruit_800x480 RA8875_480x272 RA8875_800x480 */
  tft.useINT(RA8875_INT);//We use generic int helper for Internal Resistive Touch
  tft.touchBegin();//enable touch support for RA8875
  if (tft.touchCalibrated()) {//already calibrated?
    if (tft.width() > 480)  tft.setFontScale(1);
    Serial.println("You have old calibration data present!\n");
    Serial.println("\nPlease open RA8875/_utility/RA8875calibration.h and put zero on the following:\n");
    Serial.println("#define TOUCSRCAL_XLOW  0");
    Serial.println("#define TOUCSRCAL_YLOW  0");
    Serial.println("#define TOUCSRCAL_XHIGH 0");
    Serial.println("#define TOUCSRCAL_YHIGH 0\n");
    Serial.println("Then save and try to run this again!");
    tft.setCursor(0, 0);
    tft.setTextColor(RA8875_RED);
    tft.println("---> You have old calibration data present! <---");
    tft.setTextColor(RA8875_WHITE);
    tft.println("Please open RA8875/_utility/RA8875calibration.h");
    tft.println("and put zero on the following:");
    tft.println(" ");
    tft.println("   #define TOUCSRCAL_XLOW   0");
    tft.println("   #define TOUCSRCAL_YLOW   0");
    tft.println("   #define TOUCSRCAL_XHIGH  0");
    tft.println("   #define TOUCSRCAL_YHIGH  0");
    tft.println(" ");
    tft.print("Then save and try to run this again!");
    proceed = false;
  } else {
    Serial.println("Start calibration, please follow indications...\n");
    Serial.println("\nPlease press FIRMLY the TOP/LEFT angle of your screen now!\n");
    if (tft.width() > 480)  tft.setFontScale(1);
    tft.setCursor(CENTER, CENTER);
    tft.setTextColor(RA8875_WHITE);
    tft.print("Please press the TOP/LEFT angle now!");
    tft.fillCircle(5, 5, 5, RA8875_RED);
    proceed = true;
    //this enable an ISR on CPU and RA8875 INT
    tft.enableISR(true);
    //You can avoid ISR by simple ignore the line above
    //it will use the slower digitalRead(pin) alternative internally
  }
}

void loop() {
  if (proceed) {
    if (tft.touched()) {
      if (count >= samples) {
        tft.touchEnable(false);
        count = 0;//reset counts
        uint32_t valx = 0;
        uint32_t valy = 0;

        if (scount < 1) {
          for (uint8_t i = 0; i < samples; i++) {
            valx += tempData[i][0];
            valy += tempData[i][1];
          }
          valx = valx / samples;
          valy = valy / samples;
          tft.fillWindow();
          if (tft.width() > 480)  tft.setFontScale(1);
          tft.setCursor(CENTER, CENTER);
          tft.setTextColor(RA8875_WHITE);
          tft.println("Top/Left done. Please do not touch screen...");
          tft.setTextColor(RA8875_RED);
          tft.setFontScale(1);
          tft.print("Please do not touch screen!");
          tft.setFontScale(0);
          Serial.print("Top/Left done...");
          Serial.print("Please do not touch screen...");
          _XLOW_VAR = valx;
          _YLOW_VAR = valy;
          delay(3000);
          tft.fillWindow();
          tft.fillCircle(tft.width() - 5, tft.height() - 5, 5, RA8875_RED);
          if (tft.width() > 480)  tft.setFontScale(1);
          tft.setCursor(CENTER, CENTER);
          tft.setTextColor(RA8875_WHITE);
          tft.print("ok, Now Touch Bottom/Right angle!");
          Serial.println("\n...done, Now Touch Bottom/Right angle!");
          delay(2000);
          tft.touchEnable(true);
          scount++;
        } else if (scount >= 1) {
          for (uint8_t i = 0; i < samples; i++) {
            valx += tempData[i][0];
            valy += tempData[i][1];
          }
          valx = valx / samples;
          valy = valy / samples;
          tft.fillWindow();
          tft.setCursor(0, 20);
          tft.setTextColor(RA8875_WHITE);
          tft.println("Calibration done...watch results!");
          Serial.println("\nCalibration done...watch results");
          _XHIGH_VAR = valx;
          _YHIGH_VAR = valy;
          tft.println("Now open file:");
          tft.setTextColor(RA8875_YELLOW);
          tft.println("RA8875/_utility/RA8875Calibration.h");
          tft.setTextColor(RA8875_WHITE);
          Serial.println("\nNow open file RA8875/_utility/RA8875Calibration.h\n");
          Serial.println("Change the following:\n");
          tft.println("Change the following:");
          tft.println(" ");
          tft.setTextColor(RA8875_GREEN);
          tft.print("#define TOUCSRCAL_XLOW   ");
          tft.setTextColor(RA8875_YELLOW);
          tft.println(_XLOW_VAR, DEC);
          tft.setTextColor(RA8875_GREEN);
          tft.print("#define TOUCSRCAL_YLOW   ");
          tft.setTextColor(RA8875_YELLOW);
          tft.println(_YLOW_VAR, DEC);
          tft.setTextColor(RA8875_GREEN);
          tft.print("#define TOUCSRCAL_XHIGH  ");
          tft.setTextColor(RA8875_YELLOW);
          tft.println(_XHIGH_VAR, DEC);
          tft.setTextColor(RA8875_GREEN);
          tft.print("#define TOUCSRCAL_YHIGH  ");
          tft.setTextColor(RA8875_YELLOW);
          tft.println(_YHIGH_VAR, DEC);
          tft.setTextColor(RA8875_WHITE);
          tft.println(" ");
          tft.println("...then save file and you are calibrated!");
          Serial.println("#define TOUCSRCAL_XLOW  0");
          Serial.println("#define TOUCSRCAL_YLOW  0");
          Serial.println("#define TOUCSRCAL_XHIGH 0");
          Serial.println("#define TOUCSRCAL_YHIGH 0");
          Serial.println("\nInto:\n");
          Serial.print("#define TOUCSRCAL_XLOW  ");
          Serial.println(_XLOW_VAR, DEC);
          Serial.print("#define TOUCSRCAL_YLOW  ");
          Serial.println(_YLOW_VAR, DEC);
          Serial.print("#define TOUCSRCAL_XHIGH ");
          Serial.println(_XHIGH_VAR, DEC);
          Serial.print("#define TOUCSRCAL_YHIGH ");
          Serial.println(_YHIGH_VAR, DEC);
          Serial.println("\nSave and Have a nice day!");
          proceed = false;
          tft.touchEnable(false);
        }
      } else {//continue get samples
        delay(1);
        tft.touchReadAdc(&tx, &ty);//we using 10bit adc data here
        if (count >= 0) {
          tempData[count][0] = tx;
          tempData[count][1] = ty;
        }
        count++;
      }
    }
  }//proceed
}