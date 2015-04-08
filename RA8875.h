/*
	--------------------------------------------------
	RA8875 LCD/TFT Graphic Controller Driver Library
	--------------------------------------------------
	Version:0.69b11 Added a function...
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

	-----------------------------------------------
	in ms
Screen fill              37380 	25140
Text                     36828	36626
Lines                    54186	40914
Horiz/Vert Lines         39910	32280
Rectangles (outline)     66329	45956
Rectangles (filled)      70788	48744
Circles (filled)         90466	67567
Circles (outline)        96677	72260
Triangles (outline)      12743	9848
Triangles (filled)       22655	16359
Rounded rects (outline)  10198	9045
Rounded rects (filled)   42062	30125


Benchmark                Time (microseconds)
Screen fill              4949
Test Pixel               42
Test Pixels              19779
Text                     20717 4696
Lines                    39317
Horiz/Vert Lines         30758
Rectangles (outline)     45341
Rectangles (filled)      213002
Circles (filled)         66797
Circles (outline)        66202
Triangles (outline)      9335
Triangles (filled)       15968
Rounded rects (outline)  8298
Rounded rects (filled)   29468
--------------------------------


*/

#ifndef _RA8875MC_H_
#define _RA8875MC_H_

#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
	#include "Energia.h"

	#undef byte
	#define byte      uint8_t

	#if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)//tiva???
		#define NEEDS_SET_MODULE
	#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)//stellaris first version
		#define NEEDS_SET_MODULE
	#elif defined(__MSP430MCU__)//MSP430???
		// don't know
	#elif defined(TMS320F28069)//C2000???
		// don't know
	#elif defined(__CC3200R1M1RGC__)//CC3200???
		// don't know
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

enum RA8875sizes { RA8875_320x240, RA8875_480x272, RA8875_800x480, Adafruit_480x272, Adafruit_640x480, Adafruit_800x480,RA8875_640x480 };
enum RA8875modes { GRAPHIC,TEXT };
enum RA8875tcursor { NOCURSOR,IBEAM,UNDER,BLOCK };
enum RA8875tsize { X16,X24,X32 };
enum RA8875fontSource { INT, EXT };
enum RA8875fontCoding { ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4 };
enum RA8875extRomType { GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, GT23L32S4W, GT30H32S4W, GT30L32S4W, ER3303_1, ER3304_1 };
enum RA8875extRomCoding { GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN };
enum RA8875extRomFamily { STANDARD, ARIAL, ROMAN, BOLD };
enum RA8875boolean { LAYER1, LAYER2, TRANSPARENT, LIGHTEN, OR, AND, FLOATING };//for LTPR0
enum RA8875writes { L1, L2, CGRAM, PATTERN, CURSOR };//TESTING
enum RA8875scrollMode{ SIMULTANEOUS, LAYER1ONLY, LAYER2ONLY, BUFFERED };

/* ----------------------------DO NOT TOUCH ANITHING FROM HERE ------------------------*/
#include "_utility/RA8875Registers.h"
#include "_utility/RA8875ColorPresets.h"
#include "_utility/RA8875UserSettings.h"
#if !defined(USE_EXTERNALTOUCH)
#include "_utility/RA8875Calibration.h"
#endif

// Touch screen cal structs
typedef struct Point_TS { int32_t x; int32_t y; } tsPoint_t;//fix for DUE
typedef struct Matrix_TS { int32_t An,Bn,Cn,Dn,En,Fn,Divider ; } tsMatrix_t;//fix for DUE
static const uint8_t _RA8875colorMask[4] = {11,5,13,8};//for color masking, first 2 byte for 65K

class RA8875 : public Print {
 public:
//------------- Instance -------------------------
	#if defined(__MKL26Z64__)
		RA8875(const uint8_t CS,const uint8_t RST=255,uint8_t spiInterface=0);//only Teensy LC
	#else	
		RA8875(const uint8_t CS, const uint8_t RST=255);//all the others
	#endif
//------------- Setup -------------------------
	void 		begin(const enum RA8875sizes s,uint8_t colors=16);
//------------- Hardware related -------------------------
	void    	displayOn(boolean on);
	void    	sleep(boolean sleep);
	void 		brightness(uint8_t val);//ok
	void 		changeMode(enum RA8875modes m);//GRAPHIC,TEXT
	uint8_t 	readStatus(void);
	void		clearMemory(boolean full);
	void 		scanDirection(boolean invertH,boolean invertV);
	void 		setColorBpp(uint8_t colors);//set the display color space 8 or 16!
	uint8_t 	getColorBpp();//get the current display color space (return 8 or 16)
//--------------area & color -------------------------
	void		setActiveWindow(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB);
	uint16_t 	width(void);
	uint16_t 	height(void);
	void		setForegroundColor(uint16_t color);//OK
	void		setForegroundColor(uint8_t R,uint8_t G,uint8_t B);
	void		setBackgroundColor(uint16_t color);//OK
	void		setBackgroundColor(uint8_t R,uint8_t G,uint8_t B);
	void 		setTrasparentColor(uint16_t color);//OK
	void 		setTrasparentColor(uint8_t R,uint8_t G,uint8_t B);
//--------------Text functions ------------------------- 
	//----------cursor stuff................
	//void		showCursor(boolean cur,enum RA8875tcursor c=BLINK);//show text cursor, select cursor typ (NORMAL,BLINK)
	void 		setCursorBlinkRate(uint8_t rate);//0...255 0:faster
	void 		showCursor(enum RA8875tcursor c,bool blink);
	void    	setCursor(uint16_t x, uint16_t y);
	void 		getCursor(uint16_t *x, uint16_t *y);//update the library _cursorX,_cursorY internally
				//and get the current data, this is useful sometime because the chip track cursor internally only
	void    	setTextColor(uint16_t fColor, uint16_t bColor);
	void 		setTextColor(uint16_t fColor);//transparent background
	void 		uploadUserChar(const uint8_t symbol[],uint8_t address);
	void		showUserChar(uint8_t symbolAddrs,uint8_t wide=0);//0...255
	void    	setFontScale(uint8_t scale);//0..3
	//void    	setFontScale(uint8_t vscale,uint8_t hscale);//0..3
	void    	setFontSize(enum RA8875tsize ts,boolean halfSize=false);//X16,X24,X32
	void 		setFontSpacing(uint8_t spc);//0:disabled ... 63:pix max
	void 		setFontRotate(boolean rot);//true = 90 degrees
	void 		setFontInterline(uint8_t pix);//0...63 pix
	void 		setFontFullAlign(boolean align);//mmmm... doesn't do nothing! Have to investigate
	uint8_t 	getFontWidth(boolean inColums=false);
	uint8_t 	getFontHeight(boolean inRows=false);
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
	void    	drawPixel(int16_t x, int16_t y, uint16_t color);
	void    	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);//ok
	void    	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);//ok
	void    	fillScreen(uint16_t color);//ok
	void		clearScreen(uint16_t color=RA8875_BLACK);
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
	void 		drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);//ok
	void 		fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
//--------------- SCROLL ----------------------------------------
	void        setScrollMode(enum RA8875scrollMode mode); // The experimentalist
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
	void 		useLayers(boolean on);
	void		writeTo(enum RA8875writes d);//L1, L2, CGRAM, PATTERN, CURSOR
	void 		layerEffect(enum RA8875boolean efx);//LAYER1, LAYER2, TRANSPARENT, LIGHTEN, OR, AND, FLOATING
	void 		layerTransparency(uint8_t layer1,uint8_t layer2);
	uint8_t		getCurrentLayer(void); //return the current drawing layer. If layers are OFF, return 255
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
	void  		writeData16(uint16_t data);
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

	#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)
		uint8_t _SPIint;
	#endif
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
	bool					_unsupported;//if true, not supported board
	bool					_inited;//true when init has been ended
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
	bool 					_hasLayerLimits;//helper
	//scroll vars ----------------------------
	int16_t					_scrollXL,_scrollXR,_scrollYT,_scrollYB;
	//color space-----------------------------
	uint8_t					_color_bpp;//8=256, 16=64K colors

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
	void    	writeData(uint8_t data);
	uint8_t 	readData(bool stat=false);
	

	boolean 	waitPoll(uint8_t r, uint8_t f);//from adafruit
	void 		waitBusy(uint8_t res=0x80);//0x80, 0x40(BTE busy), 0x01(DMA busy)

	void 		startSend();
	void 		endSend();

	#if defined(NEEDS_SET_MODULE)//for Energia
	void 		selectCS(uint8_t module);
	#endif
	// Register containers -----------------------------------------
	// this needed to  prevent readRegister from chip that it's slow.
	uint8_t		_MWCR0Reg; //keep track of the register 		  [0x40]
	uint8_t		_DPCRReg;  ////Display Configuration		  	  [0x20]
	uint8_t		_FNCR0Reg; //Font Control Register 0 		  	  [0x21]
	uint8_t		_FNCR1Reg; //Font Control Register1 			  [0x22]
	uint8_t		_FWTSETReg; //Font Write Type Setting Register 	  [0x2E]
	uint8_t		_SFRSETReg; //Serial Font ROM Setting 		  	  [0x2F]
	uint8_t		_TPCR0Reg; //Touch Panel Control Register 0	  	  [0x70]
	uint8_t		_INTC1Reg; //Interrupt Control Register1		  [0xF0]

};

#endif
/*
Register affected by color
REG[10h] System Configuration Register (SYSR) bit 3,2//OK
REG[41h] Memory Write Control Register1 (MWCR1) bit 0//OK
REG[60h] Background Color Register 0 (BGCR0) bit 4..0//OK
REG[61h] Background Color Register 1 (BGCR1) bit 5..0//OK
REG[62h] Background Color Register 2 (BGCR2) bit 4..0//OK
REG[63h] Foreground Color Register 0 (FGCR0) bit 4..0//OK
REG[64h] Foreground Color Register 1 (FGCR1) bit 5..0//OK
REG[65h] Foreground Color Register 2 (FGCR2) bit 4..0//OK
REG[67h] Background Color Register for Transparent 0 (BGTR0) bit 4..0//OK
REG[68h] Background Color Register for Transparent 1 (BGTR1) bit 4..0//OK
REG[69h] Background Color Register for Transparent 2 (BGTR2) bit 4..0//OK
REG[84h] Graphic Cursor Color 0 (GCC0) bit 7..0
REG[85h] Graphic Cursor Color 1 (GCC1) bit 7..0

*/