/*
	--------------------------------------------------
	RA8875 LCD/TFT Graphic Controller Driver Library
	--------------------------------------------------

	Version:0.68 compile with teensyduino 1.22b
	High Optimizations for Teensy 3 SPI & Drawings
	Fixed a typo for DUE. (Thanks DrewJaworskiRIS)
	++++++++++++++++++++++++++++++++++++++++++++++++++
	Written by: Max MC Costa for s.u.m.o.t.o.y
	++++++++++++++++++++++++++++++++++++++++++++++++++
An attemp to create a full featured library support for RA8875 chip from RAiO
that is alternative to the buggy and incomplete Adafruit_RA8875 (but thanks Ada 
and PaintYourDragon, great work aniway).

-------------------------------------------------------------------------------------
					>>>>>>>>>>>> About Copyrights <<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
Even if similar to Adafruit_RA8875 in some parts this library was builded originated
(as the Adafruit one) from the application note and datasheet from RAiO, however
I spent quite a lot of time to improve and bring to life all features so if you clone,
copy or modify please leave all my original notes intact!

-------------------------------------------------------------------------------------
				>>>>>>>>>>>>>>>>>>>>> Wiring <<<<<<<<<<<<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
TFT side	Teensy/Uno
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
CLK:		pin 13 UNO
MOSI:		pin 11 UNO
MISO:		pin 12 UNO
CS:			pin 10 (selectable but note 1*!)
INT:		pin 2  (selectable 2*)
RESET:		pin 9  (selectable and optional 0*)
*(0) In some modules you can leave unconnected but check if it's needed!
*(1) On Teensy3.x not all pin are usable for CS! Read the printed paper
that come with your Teensy3.x for more informations!
*(2) Arduino cannot use any pin for interrupts! In my examples I don't use
interrupts but just reading pin state so you can use any pin :)
-------------------------------------------------------------------------------------
TFT side	Stellaris (LM4F120XL) module=0 (still not checked)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
CLK:		xxx
MOSI:		xxx
MISO:		xxx
CS:			xxx  (selectable)
INT:		xxx  (selectable)
RESET:		xxx  (selectable and optional)

*if you use an [optional] SD card here's connections...
-------------------------------------------------------------------------------------
TFT side	Teensy2/Teensy3/Uno
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SD CLK:		pin 13 [shared with RA]
SD MOSI:	pin 11 [shared with RA]
SD MISO:	pin 12 [shared with RA]
SD CS:		pin 4  (selectable 3*)
SD CARD ID: pin xx (selectable and optional)
*(3) On Teensy3.x not all pin are usable for CS! 
can be used: 2,6,9,10,15,20,21,22,23
-------------------------------------------------------------------------------------
TFT side	Stellaris (LM4F120XL) module=0 (still not checked)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SD CLK:		xxx shared
SD MOSI:	xxx shared
SD MISO:	xxx shared
SD CS:		xxx  (selectable)
SD CARD ID: pin xx (selectable and optional)
-------------------------------------------------------------------------------------
TFT side	Teensy3 alternative setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
There are special cases on Teensy 3.x since it can be used for audio shield that remap
some pin. Using SPI in that situation will be impossible but you can use the alternative
pins definition feature of Teensy 3!
SD CLK:		pin 13(normal),pin 14(alt)  [shared with RA]
SD MOSI:	pin 11(normal),pin 7(alt) 	[shared with RA]
SD MISO:	pin 12(normal),pin 8(alt) 	[shared with RA]
SD CS:		pin 2  (selectable 3*)
SD CARD ID: pin xx (selectable and optional)
*(3) On Teensy3.x not all pin are usable for CS's! 
can be used: 2,6,9,10,15,20,21,22,23
-------------------------------------------------------------------------------------
				 >>>>>> SUPPORTED EXTERNAL FONT CHIP <<<<<<<<<
Optional!
-------------------------------------------------------------------------------------
East Rising
	ER3300-1
	ER3302-1
	ER3303-1 <--- tested
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
-------------------------------------------------------------------------------------
			   >>>>> SUPPORTED EXTERNAL FONT ENCODING <<<<<<
Optional!
-------------------------------------------------------------------------------------
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





#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"

#undef byte
#define byte      uint8_t


  #if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)//tiva???
    #define NEEDS_SET_MODULE
	#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
	#define SPI_SPEED_READ SPI_CLOCK_DIV8
  #elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)//stellaris first version
    #define NEEDS_SET_MODULE
	#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
	#define SPI_SPEED_READ SPI_CLOCK_DIV8
  #elif defined(__MSP430MCU__)//MSP430???
	#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
	#define SPI_SPEED_READ SPI_CLOCK_DIV4
  #elif defined(TMS320F28069)//C2000???
	#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
	#define SPI_SPEED_READ SPI_CLOCK_DIV4
  #elif defined(__CC3200R1M1RGC__)//CC3200???
	#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
	#define SPI_SPEED_READ SPI_CLOCK_DIV4
  #endif

	static uint8_t SPImodule;
	static uint8_t SPDR;
	
#else
#include "Arduino.h"
#endif
#include "Print.h"

#ifdef __AVR__
  #include <math.h>
#endif

/* ---------------------------- USER SETTINGS ---------------------*/

/* SPI TRANSACTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++
If you have an SPI library that support SPI TRANSACTIONS
this library will use it, however if you vant to use the old one for compatibility
just comment the following line */
#define USESPITRANSACTIONS//uncomment will force to use the standard SPI library

/* EXTERNAL TOUCH CONTROLLER ++++++++++++++++++++++++++++++++++++++++++
Some TFT come with capacitive touch screen or you may decide to use a better
controller for that, decomment the following line to save resources */
//#define USE_EXTERNALTOUCH


/* INTERNAL KEY MATRIX ++++++++++++++++++++++++++++++++++++++++++
RA8875 has a 5x6 Key Matrix controller onboard, if you are not plan to use it
better leave commented the following define since it will share some registers
with several functions, otherwise de-comment it! */
//#define USE_RA8875_KEYMATRIX
/* DEFAULT CURSOR BLINK RATE ++++++++++++++++++++++++++++++++++++++++++++
Nothing special here, you can set the default blink rate */
#define DEFAULTCURSORBLINKRATE		10
/* DEFAULT INTERNAL FONT ENCODING ++++++++++++++++++++++++++++++++++++++++++++
RA8875 has 4 different font set, same shape but suitable for most languages
please look at RA8875 datasheet and choose the correct one for your language!
The default one it's the most common one and should work in most situations */
#define DEFAULTINTENCODING			ISO_IEC_8859_1//ISO_IEC_8859_2,ISO_IEC_8859_3,ISO_IEC_8859_4

/* SPI MAX SPEED it's ONLY used in SPI Transaction mode +++++++++++++++++++
it ensure the max and correct speed for accessing RA8875 in Read/Write...
Datasheet it's clear:

System clock/3(only write cycle), System clock/6(with read cycle)

My TFT uses a 20Mhz xtal so...
Write:	6.67Mhz, Read: 	3.34Mhz

MAXSPISPEED parameters it's also related to MCU features so it probably need to be tuned.
Not all MCU are capable to work at those speeds. Those parameters should work fine.

_SPI_HYPERDRIVE it's a parameter for Teensy3 and it's experimental. Try comment it if you have troubles.


*/
#if defined(__MK20DX128__) || defined(__MK20DX256__) //teensy 3, 3.1
	//#define _SPI_HYPERDRIVE			//experimental and works only with Teensy3!!!!!
	#define MAXSPISPEED 			6600000//3300000 in READ
#elif defined(__MKL26Z64__)							 //teensy LC
	#define MAXSPISPEED 			6600000//3300000 in READ
#elif defined(__SAM3X8E__)							 // due
	#define MAXSPISPEED 			6600000
#else												 //rest of the world
	#define MAXSPISPEED 			4000000//2000000 in READ
#endif
/* ----------------------------DO NOT TOUCH ANITHING FROM HERE ------------------------*/
#include "_utility/RA8875Registers.h"
#if !defined(USE_EXTERNALTOUCH)
#include "_utility/RA8875Calibration.h"
#endif

// Colors (RGB565)
#define	RA8875_BLACK            0x0000
#define	RA8875_BLUE             0x001F
#define	RA8875_RED              0xF800
#define	RA8875_GREEN            0x07E0
#define RA8875_CYAN             0x07FF
#define RA8875_MAGENTA          0xF81F
#define RA8875_YELLOW           0xFFE0  
#define RA8875_WHITE            0xFFFF


enum RA8875sizes { RA8875_320x240, RA8875_480x272, RA8875_800x480, Adafruit_480x272, Adafruit_640x480, Adafruit_800x480,RA8875_640x480 };
enum RA8875modes { GRAPHIC,TEXT };
enum RA8875tcursor { NORMAL,BLINK };
enum RA8875tsize { X16,X24,X32 };
enum RA8875fontSource { INT, EXT };
enum RA8875fontCoding { ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4 };
enum RA8875extRomType { GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, GT23L32S4W, GT30H32S4W, ER3303_1 };
enum RA8875extRomCoding { GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN };
enum RA8875extRomFamily { STANDARD, ARIAL, ROMAN, BOLD };
enum RA8875boolean { LAYER1, LAYER2, TRANSPARENT, LIGHTEN, OR, AND, FLOATING };//for LTPR0
enum RA8875writes { L1, L2, CGRAM, PATTERN, CURSOR };//TESTING

// Touch screen cal structs
typedef struct Point_TS { int32_t x; int32_t y; } tsPoint_t;//fix for DUE
typedef struct Matrix_TS { int32_t An,Bn,Cn,Dn,En,Fn,Divider ; } tsMatrix_t;//fix for DUE


class RA8875 : public Print {
 public:
//------------- Instance -------------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
	RA8875(const uint8_t CS,const uint8_t RST=255,const boolean altSCLK=false,const boolean altMOSI=false,const boolean altMISO=false);
	#else
	RA8875(const uint8_t CS, const uint8_t RST=255);
	#endif
//------------- Setup -------------------------
	void 		begin(const enum RA8875sizes s);
//------------- Hardware related -------------------------
	//void    	softReset(void);
	void    	displayOn(boolean on);
	void    	sleep(boolean sleep);
	void 		brightness(uint8_t val);//ok
	void 		changeMode(enum RA8875modes m);//GRAPHIC,TEXT
	uint8_t 	readStatus(void);
	void		clearMemory(boolean full);
	void 		scanDirection(boolean invertH,boolean invertV);
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
	//to calculate max column. (screenWidth/fontWidth)-1
	//to calculate max row.    (screenHeight/fontHeight)-1
	void		showCursor(boolean cur,enum RA8875tcursor c=BLINK);//show text cursor, select cursor typ (NORMAL,BLINK)
	void 		setCursorBlinkRate(uint8_t rate);//0...255 0:faster
	void    	setCursor(uint16_t x, uint16_t y);
	void 		getCursor(uint16_t *x, uint16_t *y);//update the library _cursorX,_cursorY internally
				//and get the current data, this is useful sometime because the chip track cursor internally only
	void    	setTextColor(uint16_t fColor, uint16_t bColor);
	void 		setTextColor(uint16_t fColor);//transparent background
	void 		uploadUserChar(const uint8_t symbol[],uint8_t address);
	void		showUserChar(uint8_t symbolAddrs,uint8_t wide=0);//0...255
	void    	setFontScale(uint8_t scale);//0..3
	void    	setFontSize(enum RA8875tsize ts,boolean halfSize=false);//X16,X24,X32
	void 		setFontSpacing(uint8_t spc);//0:disabled ... 63:pix max
	void 		setFontRotate(boolean rot);//true = 90 degrees
	void 		setFontInterline(uint8_t pix);//0...63 pix
	void 		setFontFullAlign(boolean align);//mmmm... doesn't do nothing! Have to investigate
	//----------Font Selection and related..............................
	void		setExternalFontRom(enum RA8875extRomType ert, enum RA8875extRomCoding erc,enum RA8875extRomFamily erf=STANDARD);
	void 		setFont(enum RA8875fontSource s);//INT,EXT (if you have a chip installed)
	void 		setIntFontCoding(enum RA8875fontCoding f);
	void		setExtFontFamily(enum RA8875extRomFamily erf,boolean setReg=true);
//--------------Graphic Funcions -------------------------
	void    	setXY(int16_t x, int16_t y);//graphic set location
	void 		setX(uint16_t x);
	void 		setY(uint16_t y) ;
	void 		setGraphicCursor(uint8_t cur);//0...7 Select a custom graphic cursor (you should upload first)
	void 		showGraphicCursor(boolean cur);//show graphic cursor
	//--------------- DRAW -------------------------
	//void    	pushPixels(uint32_t num, uint16_t p);//push large number of pixels
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
//--------------- SCROLL ----------------------------------------
	void 		setScrollWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB);
	void 		scroll(uint16_t x,uint16_t y);
//-------------- DMA -------------------------------
	void 		drawFlashImage(int16_t x,int16_t y,int16_t w,int16_t h,uint8_t picnum);
//-------------- BTE --------------------------------------------
	void 		BTE_size(uint16_t w, uint16_t h);
	void	 	BTE_source(uint16_t SX,uint16_t DX ,uint16_t SY ,uint16_t DY);
	void		BTE_ROP_code(unsigned char setx);//TESTING
	void 		BTE_enable(void);//TESTING
	
//-------------- LAYERS -----------------------------------------
	boolean 	useLayers(boolean on);
	void		writeTo(enum RA8875writes d);//TESTING
	void 		layerEffect(enum RA8875boolean efx);
	void 		layerTransparency(uint8_t layer1,uint8_t layer2);
//--------------GPIO & PWM -------------------------
	void    	GPIOX(boolean on);
	void    	PWMout(uint8_t pw,uint8_t p);//1:backlight, 2:free
//--------------Touch Screen -------------------------
#if !defined(USE_EXTERNALTOUCH)
	void 		touchBegin(uint8_t intPin);//prepare Touch Screen driver
	void    	touchEnable(boolean enabled);//enable/disable Touch Polling (disable INT)
	boolean 	touchDetect(boolean autoclear=false);//true=touch detected
	//Note:		must followed by touchReadxxx or use autoclear=true for standalone
	void 		touchReadRaw(uint16_t *x, uint16_t *y);//returns 10bit ADC data (0...1024)
	void 		touchReadPixel(uint16_t *x, uint16_t *y);//return pixels (0...width, 0...height)
	boolean		touchCalibrated(void);//true if screen calibration it's present
#endif
//----------------------------------------------------
	//thanks to Adafruit for this!
    inline uint16_t Color565(uint8_t r,uint8_t g,uint8_t b) { return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3); }
	void    	writeCommand(uint8_t d);
	//void    	writeData(uint8_t data);
	void  		writeData16(uint16_t data);
	//void 		waitBusy(uint8_t res=0x80);//0x80, 0x40(BTE busy), 0x01(DMA busy)
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
	volatile uint32_t		_spiSpeed;//for SPI transactions
	uint8_t 		 		_cs, _rst;
	// Touch Screen vars ---------------------
	#if !defined(USE_EXTERNALTOUCH)
	uint8_t					_touchPin;
	bool					_clearTInt;
	uint16_t 				_tsAdcMinX,_tsAdcMinY,_tsAdcMaxX,_tsAdcMaxY;
	bool					_touchEnabled;
	#endif
	#if defined(USE_RA8875_KEYMATRIX)
	bool					_keyMatrixEnabled;
	#endif
	//----------------------------------------
	uint16_t 		 		_width, _height;
	uint16_t				_cursorX, _cursorY;//try to internally track text cursor...
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
	//layer vars -----------------------------
	uint8_t					_maxLayers;
	bool					_useMultiLayers;
	uint8_t					_currentLayer;
	//scroll vars ----------------------------
	int16_t					_scrollXL,_scrollXR,_scrollYT,_scrollYB;

	//		functions --------------------------
	void 	initialize(uint8_t initIndex);
	void    textWrite(const char* buffer, uint16_t len=0);//thanks to Paul Stoffregen for the initial version of this one
	void 	PWMsetup(uint8_t pw,boolean on, uint8_t clock);
	// 		helpers-----------------------------
	void 	checkLimitsHelper(int16_t &x,int16_t &y);//RA8875 it's prone to freeze with values out of range
	void 	circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled);
	void 	rectHelper  (int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled);
	void 	triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled);
	void 	ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled);
	void 	curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled);
	void 	lineAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	void 	curveAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	void 	roundRectHelper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled);
	#if !defined(USE_EXTERNALTOUCH)
	void	readTouchADC(uint16_t *x, uint16_t *y);
	void 	clearTouchInt(void);
	boolean touched(void);
	#endif
	void 	DMA_blockModeSize(int16_t BWR,int16_t BHR,int16_t SPWR);
	void 	DMA_startAddress(unsigned long adrs);
	//---------------------------------------------------------
    // Low level access  commands ----------------------
	void    	writeReg(uint8_t reg, uint8_t val);
	uint8_t 	readReg(uint8_t reg);
	//void    	writeCommand(uint8_t d);
	void    	writeData(uint8_t data);
	//void  		writeData16(uint16_t data);
	uint8_t 	readData(bool stat=false);
	
#if defined _SPI_HYPERDRIVE && (defined(__MK20DX128__) || defined(__MK20DX256__))
	uint8_t pcs_command;
	
	void waitFifoNotFull(void) {
		uint32_t sr;
		uint32_t tmp __attribute__((unused));
		do {
			#if ARDUINO >= 160 || TEENSYDUINO > 120
			sr = KINETISK_SPI0.SR;
			#else
			sr = SPI0.SR;
			#endif
			if (sr & 0xF0) tmp = SPI0_POPR;  // drain RX FIFO
		} while ((sr & (15 << 12)) > (3 << 12));
	}

	void waitFifoEmpty(void) {
		uint32_t sr;
		uint32_t tmp __attribute__((unused));
		do {
			#if ARDUINO >= 160 || TEENSYDUINO > 120
			sr = KINETISK_SPI0.SR;
			#else
			sr = SPI0.SR;
			#endif
			if (sr & 0xF0) tmp = SPI0_POPR;  // drain RX FIFO
		} while ((sr & 0xF0F0) > 0);             // wait both RX & TX empty
	}
	
	void waitTransmitComplete(void) __attribute__((always_inline)) {
		uint32_t tmp __attribute__((unused));
		#if ARDUINO >= 160 || TEENSYDUINO > 120
		while (!(KINETISK_SPI0.SR & SPI_SR_TCF)) ; // wait until final output done
		#else
		while (!(SPI0.SR & SPI_SR_TCF)) ; // wait until final output done
		#endif
		tmp = SPI0_POPR;                  // drain the final RX FIFO word
	}
	
	void writecommand_cont(uint8_t c) __attribute__((always_inline)) {
		#if ARDUINO >= 160 || TEENSYDUINO > 120
		KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		#else
		SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		#endif
		waitFifoNotFull();
	}
	void writecommand16_cont(uint16_t c) __attribute__((always_inline)) {
		#if ARDUINO >= 160 || TEENSYDUINO > 120
		KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		#else
		SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		#endif
		waitFifoNotFull();
	}
	void writecommand16_last(uint16_t d) __attribute__((always_inline)) {
		waitFifoEmpty();
		#if ARDUINO >= 160 || TEENSYDUINO > 120
		KINETISK_SPI0.SR = SPI_SR_TCF;
		KINETISK_SPI0.PUSHR = d | (pcs_command << 16) | SPI_PUSHR_CTAS(1);
		#else
		SPI0.SR = SPI_SR_TCF;
		SPI0.PUSHR = d | (pcs_command << 16) | SPI_PUSHR_CTAS(1);
		#endif
		waitTransmitComplete();
	}
	void writecommand_last(uint8_t c) __attribute__((always_inline)) {
		waitFifoEmpty();
		#if ARDUINO >= 160 || TEENSYDUINO > 120
		KINETISK_SPI0.SR = SPI_SR_TCF;
		KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
		#else
		SPI0.SR = SPI_SR_TCF;
		SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
		#endif
		waitTransmitComplete();
	}
	void setMultipleRegisters(uint8_t reg[],uint8_t data[],uint8_t len);
	
#endif
	
	boolean 	waitPoll(uint8_t r, uint8_t f);//from adafruit
	void 		waitBusy(uint8_t res=0x80);//0x80, 0x40(BTE busy), 0x01(DMA busy)
	#if !defined _SPI_HYPERDRIVE
	void 		startSend();
	void 		endSend();
	uint8_t 	SPItranfer(uint8_t data);
	#endif
	#if defined(NEEDS_SET_MODULE)
	void 		selectCS(uint8_t module);
	#endif
	// Register containers -----------------------------------------
	uint8_t		_MWCR0Reg; //keep track of the register 		  [0x40]
	uint8_t		_DPCRReg;  ////Display Configuration		  	  [0x20]
	uint8_t		_FNCR0Reg; //Font Control Register 0 		  	  [0x21]
	uint8_t		_FNCR1Reg; //Font Control Register1 			  [0x22]
	uint8_t		_FWTSETReg; //Font Write Type Setting Register 	  [0x2E]
	uint8_t		_SFRSETReg; //Serial Font ROM Setting 		  	  [0x2F]
	uint8_t		_TPCR0Reg; //Touch Panel Control Register 0	  	  [0x70]
	uint8_t		_INTC1Reg; //Interrupt Control Register1		  [0xF0]
	// test -----------------------------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
	boolean altMosiPin;
	boolean altMisoPin;
	boolean altSclkPin;
	#endif
};

#endif
