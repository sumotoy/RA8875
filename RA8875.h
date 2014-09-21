/*
	----------------------------------------
	RA8875 LCD/TFT Graphic Controller Driver
	----------------------------------------
	Version:0.2(early beta)
	++++++++++++++++++++++++++++++++++++++++
RA8875 it's an amazing device, it can drive LCD/TFT till 800x480 in 2,3,4 wire
or 8/16 bit parallel (6800/8080). It's different from other chip because most graphic
functions are hardware accellerated so you can use it with slow MCU with impressive
results. In addition the chip controls directly a resistive Touch Screen and can have an external
keypad, also optional external fonts and external flash chip can be installed and directly controlled.
It also have 2 PWM generators and 2 GPIO (one PWM can be used to control the backligh of the display)
and can use DMA and BTE (Block Transfer Engine), can handle directly Text by using internal (or external)
font library multiple scroll,patterns, custom pointers and much more. It's a really full loaded device!
Basic use it's really easy to implement but go inside all features it's a small nightmare.
In contrast it has some issue such as the SLOW SPI speed, some slow down when uploading big images
and since it use internal hardware accellerated routines and commands are short it's possible than MCU
it's faster than chip so you have to check if the job it's done before send another command.
RAiO has a massive datasheet for it and I got an application paper from the company that was used
by Adafruit for his version of this driver.
This is why I managed to create another library, the Adafruit has several issues and builded over
the RAiO application note that it's just a basic example.

Some Features of the chip

	- Hardware accellerated graphic functions
	- Build-in resistive Touch Screen controller
	- DMA
	- BTE
	- Build-In keypad controller
	- Internal Font library
	- External Font Chip directly drive (Genicomp)
	- External Directly drive Flash chip
	- Mixed graphic / Text environment
	- Many communication protocols
	- Interrupts and Wait pins
	- Internal memory
	- Patterns
	- Multiple Scrolling
	- Using relative big size screens with tiny MCU
	- Advanced sleep mode with multiple wake-ups
	_ 2 PWM
	- 2 GPIO
	
The goals of this library:

	- Compatible with most RA8875 based TFT's and even Adafruit piggyBack
	- All resolution supported
	- All functions supported (included keypad, ext. flash, ext. font chip, etc.)
	- All communication protocol supported

NOTES:
I'm normally use the excellent Adaruit_GFX library for all my tft,oled stuff but this time it's
totally useless here since this chip use it's graphic functions so no other library it's needed apart SPI.
The SPI code uses a new feature called SPI Transitions that it's brand new and not official, it's 
included with Teensy3 1.0.5R2 and hopefully will be included by Arduino as well so check for it!
Using SPI Transitions will force you to check if the other libraries that use SPI in your project (if you have)
are compatible, fortunately it's easy stuff to do but better say in advance. If you use a non SPI transitions
library the code will automatically compile with standard one but speed will suffer so it's highly raccomanded.

Now an early beta, only SPI supported, it's has been tested ONLY with Teensy3 and Teensy3.1

Wiring

chip		Teensy
-------------------------
CLK:		pin 13
MOSI:		pin 11
MISO:		pin 12
SS:			pin 10 (selectable)
INT:		pin 3  (selectable)
RESET:		pin 9  (selectable and optional)
In some modules you can leave unconnected but check if it's needed

-------------SUPPORTED EXTERNAL FONT CHIP
East Rising
	ER3300-1
	ER3302-1
	ER3303-1 <--- 8x16, 16,24 Arial
	ER3304-1
	ER3301-1
	ER3303-1
Genicomp
	GT21L16T1W
	GT23L16U2W
	GT23L24T3Y
	GT23L24M1Z
	GT23L32S4W
Note: East Rising and Genicomp looks the same chip! Just named different
-------------SUPPORTED EXTERNAL FONT ENCODING
	GB2312
	GB12345
	BIG5
	UNICODE
	ASCII
	UNIJIS
	JIS0208
	LATIN/GREEK/ARABIC
*/

#ifndef _RA8875MC_H_
#define _RA8875MC_H_


#include "Arduino.h"
#include "Print.h"

#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif

#include "_utility/RA8875Registers.h"
/* ---------------------------- USER SETTINGS ---------------------*/

/* SPI TRANSACTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++
If you have an SPI library that support SPI TRANSACTIONS
this library will use it, however if you vant to use the old one for compatibility
just comment the following line */
#define USESPITRANSACTIONS//uncomment will force to use the standard SPI library
/* TOUCH SCREEN AXIS INVERSION +++++++++++++++++++++++++++++++++++++++
Some chinese product have both axis inverted (EastRising),
you can uncomment INVERTETOUCH_Y or INVERTETOUCH_X or both
if your display fall in that case! */
#define INVERTETOUCH_Y
#define INVERTETOUCH_X
/* TOUCH SCREEN IN PIXELS ++++++++++++++++++++++++++++++++++++++++++++
Normally Touch Screen return ADC values (0...1024) but decomment
the following it will return values in pixels */
#define TOUCHINPIXELS
/* ------------- TOUCH SCREEN CALIBRATION (how to) -----------
Touch Screen are not all the same and needs a calibration, you will see
yourself by load touchPaint.ino example, try to paint all over the screen!
If you have space on one or more side you prolly need to calibrate values  
    ---- perform calibration ----
To perform the touch screen calibration, load libTouchSCalibration.ino and open serial terminal:
1) the lowest value of x by touch the top/left angle of your tft, put value in TOUCSRCAL_XLOW
2) you can get the lowest value of y in the same time, put value in TOUCSRCAL_YLOW
3) the highest value of x by touching the lower/bottom corner of your tft, put the value in TOUCSRCAL_XHIGH
4) in the same manner you get the max value of y, put that value in TOUCSRCAL_XHIGH
*/
#define TOUCSRCAL_XLOW	62//44
#define TOUCSRCAL_YLOW	153//147
#define TOUCSRCAL_XHIGH	924//945
#define TOUCSRCAL_YHIGH	917//934
/* Max Speed it's used only in SPI Transaction mode,
it ensure the max and correct speed for accessing RA8875 in Read/Write...
Datasheet it's clear:

System clock/3(only write cycle)
System clock/6(with read cycle)

My TFT uses a 20Mhz xtal so...
Write:	6.67Mhz
Read: 	3.34Mhz

MAXSPISPEED parameters it's also related to MCU features so it probably need to be tuned.
Not all MCU are capable to work at those speeds. Those parameters should work fine.
*/
#if defined(__MK20DX128__) || defined(__MK20DX256__) //teensy 3
	#define MAXSPISPEED 			6600000
#elif defined(__SAM3X8E__)							 // due
	#define MAXSPISPEED 			6600000
#else												 //rest of the world
	#define MAXSPISPEED 			4000000
#endif
/* DEFAULT CURSOR BLINK RATE ++++++++++++++++++++++++++++++++++++++++++++
Nothing special here, you can set the default blink rate */
#define DEFAULTCURSORBLINKRATE		10
/* DEFAULT INTERNAL FONT ENCODING ++++++++++++++++++++++++++++++++++++++++++++
RA8875 has 4 different font set, same shape but suitable for most languages
please look at RA8875 datasheet and choose the correct one for your language!
The default one it's the most common one and should work in most situations */
#define DEFAULTINTENCODING			ISO_IEC_8859_1//ISO_IEC_8859_2,ISO_IEC_8859_3,ISO_IEC_8859_4
/* ----------------------------DO NOT TOUCH ANITHING FROM HERE ------------------------*/

// Colors (RGB565)
#define	RA8875_BLACK            0x0000
#define	RA8875_BLUE             0x001F
#define	RA8875_RED              0xF800
#define	RA8875_GREEN            0x07E0
#define RA8875_CYAN             0x07FF
#define RA8875_MAGENTA          0xF81F
#define RA8875_YELLOW           0xFFE0  
#define RA8875_WHITE            0xFFFF


enum RA8875sizes { RA8875_480x272, RA8875_800x480, Adafruit_480x272, Adafruit_800x480 };
enum RA8875modes { GRAPHIC,TEXT };
enum RA8875tcursor { NORMAL,BLINK };
enum RA8875tsize { X16,X24,X32 };
enum RA8875fontSource { INT, EXT };
enum RA8875fontCoding { ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4 };
enum RA8875extRomType { GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, GT23L32S4W, GT30H32S4W };
enum RA8875extRomCoding { GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN };
enum RA8875extRomFamily { STANDARD, ARIAL, ROMAN, BOLD };
// Touch screen cal structs
typedef struct Point 
{
  int32_t x;
  int32_t y;
} tsPoint_t;

typedef struct Matrix 
{
  int32_t An,
          Bn,
          Cn,
          Dn,
          En,
          Fn,
          Divider ;
} tsMatrix_t;

class RA8875 : public Print {
 public:
//------------- Instance -------------------------
	RA8875(uint8_t cs, uint8_t rst);
	RA8875(uint8_t cs);
//------------- Setup -------------------------
	boolean 	begin(enum RA8875sizes s);
//------------- Hardware related -------------------------
	bool		useExternalFontRom(enum RA8875extRomType ert, enum RA8875extRomCoding erc,enum RA8875extRomFamily erf=STANDARD);
	void    	softReset(void);
	void    	displayOn(boolean on);
	void    	sleep(boolean sleep);
	void 		brightness(uint8_t val);//ok
	void 		changeMode(enum RA8875modes m);//GRAPHIC,TEXT
	uint8_t 	readStatus(void);
//--------------area & color -------------------------
	void		setActiveWindow(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB);
	uint16_t 	width(void);
	uint16_t 	height(void);
	void		setForegroundColor(uint16_t color);
	void		setForegroundColor(uint8_t R,uint8_t G,uint8_t B);
	void		setBackgroundColor(uint16_t color);
	void		setBackgroundColor(uint8_t R,uint8_t G,uint8_t B);
	void 		setTrasparentColor(uint16_t color);
	void 		setTrasparentColor(uint8_t R,uint8_t G,uint8_t B);
//--------------Text functions ------------------------- 
	//----------cursor stuff................
	void		showCursor(boolean cur,enum RA8875tcursor c=BLINK);//show text cursor, select cursor typ (NORMAL,BLINK)
	void 		setCursorBlinkRate(uint8_t rate);//0...255 0:faster
	void    	setCursor(uint16_t x, uint16_t y);
	void 		getCursor(uint16_t *x, uint16_t *y);//update the library _cursorX,_cursorY internally
				//and get the current data, this is useful sometime because the chip track cursor internally only
	void    	setTextColor(uint16_t fColor, uint16_t bColor);
	void 		setTextColor(uint16_t fColor);//transparent background
	void    	setFontScale(uint8_t scale);//0..3
	void    	setFontSize(enum RA8875tsize ts,boolean halfSize=false);//X16,X24,X32
	void 		setFontSpacing(uint8_t spc);//0:disabled ... 63:pix max
	void 		setFontRotate(boolean rot);//true = 90 degrees
	void 		setFontInterline(uint8_t pix);//0...63 pix
	void 		setFontFullAlign(boolean align);//mmmm... doesn't do nothing! Have to investigate
	//----------Font Selection and related..............................
	void 		setFont(enum RA8875fontSource s);//INT,EXT (if you have a chip installed)
	void 		setIntFontCoding(enum RA8875fontCoding f);
//--------------Graphic Funcions -------------------------
	void    	setXY(int16_t x, int16_t y);//graphic set location
	void 		setGraphicCursor(uint8_t cur);//0...7 Select a custom graphic cursor (you should upload first)
	void 		showGraphicCursor(boolean cur);//show graphic cursor
	//--------------- DRAW -------------------------
	void    	pushPixels(uint32_t num, uint16_t p);//push large number of pixels
	//void    	fillRect(void);
	void    	drawPixel(int16_t x, int16_t y, uint16_t color);
	void    	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void    	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void    	fillScreen(uint16_t color);
	void    	drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void    	drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void    	fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void    	drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void    	fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void    	drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void    	fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void    	drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
	void    	fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color);
	void    	drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);
	void    	fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color);
	void 		drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
	void 		fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
//--------------GPIO & PWM -------------------------
	void    	GPIOX(boolean on);
	void    	PWMout(uint8_t pw,uint8_t p);//1:backlight, 2:free
//--------------Touch Screen -------------------------
	void    	touchEnable(boolean on);
	boolean 	touched(void);
	boolean 	touchRead(uint16_t *x, uint16_t *y);

//--------------Text Write -------------------------
virtual size_t write(uint8_t b) {
	textWrite((const char *)&b, 1);
	return 1;
}

virtual size_t write(const uint8_t *buffer, size_t size) {
	textWrite((const char *)buffer, size);
	return size;
}
using Print::write;

 private:
	//------------- VARS ----------------------------
	uint8_t 		 		_cs, _rst;
	uint16_t 		 		_width, _height;
	uint8_t 		 		_textScale;
	bool					_textWrap;
	uint8_t					_fontSpacing;
	bool					_fontFullAlig;
	bool					_fontRotation;
	bool					_extFontRom;
	uint8_t					_fontInterline;
	enum RA8875extRomFamily _fontFamily;
	enum RA8875extRomType 	_fontRomType;
	enum RA8875extRomCoding _fontRomCoding;
	enum RA8875tsize		_textSize;
	enum RA8875modes 		_currentMode;
	enum RA8875sizes 		_size;
	enum RA8875fontSource 	_fontSource;
	enum RA8875tcursor		_textCursorStyle;
	
	
	volatile uint32_t		_spiSpeed;//for SPI transactions
	uint16_t				_cursorX, _cursorY;
	//		functions --------------------------
	void 	initialize(void);
	void    textWrite(const char* buffer, uint16_t len=0);
	void 	PWMsetup(uint8_t pw,boolean on, uint8_t clock);
	// 		helpers-----------------------------
	void 	checkLimitsHelper(int16_t &x,int16_t &y);
	void 	circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled);
	void 	rectHelper  (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled);
	void 	triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled);
	void 	ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled);
	void 	curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled);
	void 	lineAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	void 	curveAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	void 	roundRectHelper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled);
	//---------------------------------------------------------
    // Low level access  commands ----------------------
	void    	writeReg(uint8_t reg, uint8_t val);
	uint8_t 	readReg(uint8_t reg);
	void    	writeData(uint8_t d);
	void  		writeData16(uint16_t d);
	uint8_t 	readData(void);
	void    	writeCommand(uint8_t d);
	
	boolean 	waitPoll(uint8_t r, uint8_t f);
	void 		startSend();
	void 		endSend();
	// Register containers -----------------------------------------
	uint8_t		_MWCR0Reg;//keep track of the register 		  [0x40]
	uint8_t		_FNCR0Reg;//Font Control Register 0 		  [0x21]
	uint8_t		_FNCR1Reg;//Font Control Register1 			  [0x22]
	uint8_t		_FWTSETReg;//Font Write Type Setting Register [0x2E]
	uint8_t		_SFRSETReg;//Serial Font ROM Setting 		  [0x2F]
};

#endif