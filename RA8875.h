/*
	--------------------------------------------------
	RA8875 LCD/TFT Graphic Controller Driver Library
	--------------------------------------------------
	Version:0.69b64
	This is the 0.70 preview!
	Added support for DUE SPI extended, faster AVR code, drawArc
	++++++++++++++++++++++++++++++++++++++++++++++++++
	Written by: Max MC Costa for s.u.m.o.t.o.y
	++++++++++++++++++++++++++++++++++++++++++++++++++
An attemp to create a full featured library support for RA8875 chip from RAiO
that is alternative to the buggy and incomplete Adafruit_RA8875 (but thanks Ada 
and PaintYourDragon to bring us something almost working for free!).

-------------------------------------------------------------------------------------
					>>>>>>>>>>>> About Copyrights <<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
I spent quite a lot of time to improve and bring to life all features so if you clone,
copy or modify please leave all my original notes intact!
-------------------------------------------------------------------------------------
					     >>>>>>>>>>>> Thanks to <<<<<<<<<<<<<<<
-------------------------------------------------------------------------------------
Teensy 3.1, a tiny MCU but monster of performances at tiny price, Arduino should learn...
Paul Stoffregen, the 'guru' behind many arduino magic, the father of Teensy
Bill Greyman, another 'maestro', greatly inspired many coders
Jnmattern & Marek Buriak for drawArc
Adafruit, not a great example of coding but thanks for sharing
Last but not less important the contributors and beta tester of this library:
M.Sandercrock, the experimentalist.
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


Screen fill              16635		16585		16561		8326
Test Pixel               60			77			63			53
Test Pixels              39780		52085		41885		35412
Text                     4500		4389		4634		3409
Lines                    96705		101951		97943		55986
Horiz/Vert Lines         66656		72470		67732		42703
Rectangles (outline)     269786		271251		269987		137691
Rectangles (filled)      269498		271415		270002		137858
Circles (filled)         110285		195011		181003		117279
Circles (outline)        109604		191746		177459		115584
Triangles (outline)      24521		26320		24886		14728
Triangles (filled)       64947		66801		65367		35226
Rounded rects (outline)  146391		167477		151142		103027
Rounded rects (filled)   935691		953280		940016		493179


PIN     UNO		MEGA       CD4050     RA8875
SCK      13		52           YES       SCK
MOSI     11		51           YES       MOSI
MISO     12		50            NO       MISO
RST      9		 5           YES       RST
CS       10		53           YES       CS

*/

#ifndef _RA8875MC_H_
#define _RA8875MC_H_

#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
	#include "Energia.h"
	#undef byte
	#define byte      uint8_t
	#if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)//tiva???
		#define NEEDS_SET_MODULE
		#define _FASTCPU
	#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)//stellaris first version
		#define NEEDS_SET_MODULE
		#define _FASTCPU
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

#include "_utility/RA8875Font.h"

//pgmspace fixup
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)//teensy 3 or 3.1 or LC
	#include <avr/pgmspace.h>//Teensy3 and AVR arduinos can use pgmspace.h
	#define _FASTCPU
	#ifdef PROGMEM
		#undef PROGMEM
		#define PROGMEM __attribute__((section(".progmem.data")))
	#endif
#elif defined(__32MX320F128H__) || defined(__32MX795F512L__) || (defined(ARDUINO) && defined(__arm__) && !defined(CORE_TEENSY))//chipkit uno, chipkit max, arduino DUE	
	#define _FASTCPU
	#ifndef __PGMSPACE_H_
	#define __PGMSPACE_H_ 1
	#define PROGMEM
	#define PGM_P  const char *
	#define PSTR(str) (str)
	#define pgm_read_byte_near(addr) pgm_read_byte(addr)
	#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	#define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
#else
	#include <avr/pgmspace.h>//Teensy3 and AVR arduinos can use pgmspace.h
#endif


#if !defined(swapvals)
	#define swapvals(a, b) { typeof(a) t = a; a = b; b = t; }
#endif

//enum RA8875sizes { RA8875_320x240, RA8875_480x272, RA8875_640x480, RA8875_800x480, RA8875_800x480ALT, Adafruit_480x272, Adafruit_640x480, Adafruit_800x480 };
enum RA8875sizes { RA8875_480x272, RA8875_800x480, RA8875_800x480ALT, Adafruit_480x272, Adafruit_800x480 };
enum RA8875tcursor { NOCURSOR=0,IBEAM,UNDER,BLOCK };//0,1,2,3
enum RA8875tsize { X16=0,X24,X32 };//0,1,2
enum RA8875fontSource { INT=0, EXT };//0,1
enum RA8875fontCoding { ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4 };
enum RA8875extRomType { GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, GT23L32S4W, GT30H32S4W, GT30L32S4W, ER3303_1, ER3304_1 };
enum RA8875extRomCoding { GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN };
enum RA8875extRomFamily { STANDARD, ARIAL, ROMAN, BOLD };
enum RA8875boolean { LAYER1, LAYER2, TRANSPARENT, LIGHTEN, OR, AND, FLOATING };
enum RA8875writes { L1=0, L2, CGRAM, PATTERN, CURSOR };
enum RA8875scrollMode{ SIMULTANEOUS, LAYER1ONLY, LAYER2ONLY, BUFFERED };
enum RA8875pattern{ P8X8, P16X16 };
enum RA8875btedatam{ CONT, RECT };
enum RA8875btelayer{ SOURCE, DEST };

/* ----------------------------DO NOT TOUCH ANITHING FROM HERE ------------------------*/
#include "_utility/RA8875Registers.h"
#include "_utility/RA8875ColorPresets.h"
#include "_utility/RA8875UserSettings.h"

//include the support for FT5206
#if defined (USE_FT5206_TOUCH)
	#if !defined(USE_EXTERNALTOUCH)
		#define USE_EXTERNALTOUCH//FT5206 doesn't use RA8875 stuff
	#endif
	#include "Wire.h"
#endif

#if !defined(USE_EXTERNALTOUCH)
	#include "_utility/RA8875Calibration.h"
#endif

#define CENTER 9998
#define ARC_ANGLE_MAX 360		
#define ARC_ANGLE_OFFSET -90	

#if defined(__MKL26Z64__)
	static bool _altSPI;
#endif
#ifdef SPI_HAS_TRANSACTION
	static volatile uint32_t _maxspeed;//holder for SPI speed
#endif

// Touch screen cal structs
typedef struct Point_TS { int32_t x; int32_t y; } tsPoint_t;//fix for DUE
typedef struct Matrix_TS { int32_t An,Bn,Cn,Dn,En,Fn,Divider ; } tsMatrix_t;//fix for DUE
//const
static const uint8_t _RA8875colorMask[6] = {11,5,0,13,8,3};//for color masking, first 3 byte for 65K

class RA8875 : public Print {
 public:
	//void 		debugData(uint16_t data,uint8_t len=8);
//------------- Instance -------------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__)//Teensy 3.0, Teensy 3.1
		#if defined (USE_FT5206_TOUCH)//with FT5206_TOUCH
			RA8875(const uint8_t CSp,const uint8_t RSTp=255,const uint8_t INTp=2,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);
		#else
			RA8875(const uint8_t CSp,const uint8_t RSTp=255,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);
		#endif
	#elif defined(__MKL26Z64__)//TeensyLC with FT5206_TOUCH
		#if defined (USE_FT5206_TOUCH)
			RA8875(const uint8_t CSp,const uint8_t RSTp=255,const uint8_t INTp=2,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);
		#else
			RA8875(const uint8_t CSp,const uint8_t RSTp=255,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);
		#endif
	#elif defined(__SAM3X8E__)//DUE
		#if defined (USE_FT5206_TOUCH)// with FT5206_TOUCH
			RA8875(const uint8_t CSp, const uint8_t RSTp=255,const uint8_t INTp=2);
		#else
			RA8875(const uint8_t CSp, const uint8_t RSTp=255);
		#endif
	#elif defined(NEEDS_SET_MODULE)//ENERGIA
		#if defined (USE_FT5206_TOUCH)//with FT5206_TOUCH
			RA8875(const uint8_t module, const uint8_t RSTp=255,const uint8_t INTp=2);
		#else
			RA8875::RA8875(const uint8_t module, const uint8_t RSTp=255);
		#endif
	#else//8 BIT ARDUINO's
		#if defined (USE_FT5206_TOUCH)//with FT5206_TOUCH
			RA8875(const uint8_t CSp, const uint8_t RSTp=255,const uint8_t INTp=2);
		#else
			RA8875(const uint8_t CSp, const uint8_t RSTp=255);
		#endif
	#endif
//------------- Setup -------------------------
	void 		begin(const enum RA8875sizes s,uint8_t colors=16);
	//(RA8875_320x240, RA8875_480x272, RA8875_640x480, RA8875_800x480, Adafruit_480x272, Adafruit_640x480, Adafruit_800x480) , (8/16 bit)
//------------- Hardware related -------------------------
	void 		backlight(boolean on);
	void    	displayOn(boolean on);//turn diplay on/off
	void    	sleep(boolean sleep);//put display in sleep or not
	void 		brightness(uint8_t val);//ok
	uint8_t 	readStatus(void);//used to verify when an operation has concluded
	void 		clearMemory(bool stop=false);
	//void		clearMemory(boolean full);//clear the RA8875 internal buffer (fully or current layer)
	uint8_t		errorCode(void);//0: no error,
	/*
	format: 1byte 0b00000000;, bit described below:
	nan,nan,nan,nan,nan,CS(out-of-range),MOSI/MISO/SCLK(out-of-range),display(not-recognized)
	*/
//------------ Low Level functions -----------------------
	void    	writeCommand(uint8_t d);
	void  		writeData16(uint16_t data);
//--------------area -------------------------------------
	void		setActiveWindow(int16_t XL,int16_t XR,int16_t YT,int16_t YB);//The working area where to draw on
	void		setActiveWindow(void);
	void 		getActiveWindow(int16_t &XL,int16_t &XR ,int16_t &YT ,int16_t &YB);
	void		clearActiveWindow(bool full=false);//it clears the active window
	uint16_t 	width(void) const;//the phisical display width
	uint16_t 	height(void) const;//the phisical display height
	void		setRotation(uint8_t rotation); //rotate text and graphics
	uint8_t		getRotation(); //return the current rotation 0-3
	boolean		isPortrait(void);
//--------------color -------------------------------------
	void		setForegroundColor(uint16_t color);//color of objects in 16bit
	void		setForegroundColor(uint8_t R,uint8_t G,uint8_t B);//color of objects in 8+8+8bit
	void		setBackgroundColor(uint16_t color);//color of objects background in 16bit
	void		setBackgroundColor(uint8_t R,uint8_t G,uint8_t B);//color of objects background in 8+8+8bit
	void 		setTransparentColor(uint16_t color);//the current transparent color in 16bit
	void 		setTransparentColor(uint8_t R,uint8_t G,uint8_t B);//the current transparent color in 8+8+8bit
	void 		setColor(uint16_t fcolor,uint16_t bcolor,bool bcolorTraspFlag=false);
	void 		setColorBpp(uint8_t colors);//set the display color space 8 or 16!
	uint8_t 	getColorBpp(void);//get the current display color space (return 8 or 16)
//--------------color conversion ----------------------------
	inline uint16_t Color565(uint8_t r,uint8_t g,uint8_t b) { return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3); }
	inline uint16_t Color24To565(int32_t color_) { return ((((color_ >> 16) & 0xFF) / 8) << 11) | ((((color_ >> 8) & 0xFF) / 4) << 5) | (((color_) &  0xFF) / 8);}
	inline uint16_t htmlTo565(int32_t color_) { return (uint16_t)(((color_ & 0xF80000) >> 8) | ((color_ & 0x00FC00) >> 5) | ((color_ & 0x0000F8) >> 3));}
//--------------Cursor Stuff---------------------------------
	void 		cursorIncrement(bool on);
	void 		setCursorBlinkRate(uint8_t rate);//set blink rate of the cursor 0...255 0:faster
	void 		showCursor(enum RA8875tcursor c,bool blink);//show cursor(NOCURSOR,IBEAM,UNDER,BLOCK), default blinking
	void    	setCursor(int16_t x, int16_t y,bool autocenter=false);//set cursor position to write text(pixels or CENTER)
	void 		getCursor(int16_t &x, int16_t &y);//from the RA8875 registers
	void 		getCursorFast(int16_t &x, int16_t &y);//from library (faster)
	int16_t		getCursorX(void);
	int16_t		getCursorY(void);
//--------------Text functions ------------------------------
	void 		uploadUserChar(const uint8_t symbol[],uint8_t address);//upload user defined char as array at the address 0..255
	void		showUserChar(uint8_t symbolAddrs,uint8_t wide=0);//show user uploaded char at the adrs 0...255
	void    	setTextColor(uint16_t fcolor, uint16_t bcolor);//set text color + text background color
	void 		setTextColor(uint16_t fcolor);//set text color (backgroud will be transparent)
	void    	setFontScale(uint8_t scale);//global font scale (w+h)
	void    	setFontScale(uint8_t vscale,uint8_t hscale);//font scale separated by w and h
	void    	setFontSize(enum RA8875tsize ts);//X16,X24,X32
	void 		setFontSpacing(uint8_t spc);//0:disabled ... 63:pix max
	void 		setFontInterline(uint8_t pix);//0...63 pix
	void 		setFontFullAlign(boolean align);//mmmm... doesn't do nothing! Have to investigate
	uint8_t 	getFontWidth(boolean inColums=false);
	uint8_t 	getFontHeight(boolean inRows=false);
	//----------Font Selection and related..............................
	void		setExternalFontRom(enum RA8875extRomType ert, enum RA8875extRomCoding erc,enum RA8875extRomFamily erf=STANDARD);
	void 		setFont(enum RA8875fontSource s);//INT,EXT (if you have a chip installed)
	void 		setIntFontCoding(enum RA8875fontCoding f);
	void		setExtFontFamily(enum RA8875extRomFamily erf,boolean setReg=true);
//--------------Font Rendering Engine (ALPHA!!!! only a test) -------------------------
	void 		gPrint(uint16_t x,uint16_t y,const char *in,uint16_t color,uint8_t scale,const struct FONT_DEF *strcut1);
	void 		gPrint(uint16_t x,uint16_t y,int num,uint16_t color,uint8_t scale,const struct FONT_DEF *strcut1);
	void 		gPrintEfx(uint16_t x,uint16_t y,const char *in,uint16_t color,uint8_t pixellation,const struct FONT_DEF *strcut1);
//--------------Graphic Funcions -------------------------
	void    	setXY(int16_t x, int16_t y);//graphic set location
	void 		setX(int16_t x);
	void 		setY(int16_t y) ;
	void 		setGraphicCursor(uint8_t cur);//0...7 Select a custom graphic cursor (you should upload first)
	void 		showGraphicCursor(boolean cur);//show graphic cursor
	//--------------- DRAW -------------------------
	void    	drawPixel(int16_t x, int16_t y, uint16_t color);
	void 		drawPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y);
	uint16_t 	getPixel(int16_t x, int16_t y);
	//void 		getPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y);
	void    	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void    	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void    	fillWindow(uint16_t color=_RA8875_DEFAULTBACKLIGHT);//fill the ActiveWindow with a color(default black)
	void		clearScreen(uint16_t color=_RA8875_DEFAULTBACKLIGHT);//fill the entire screen (regardless ActiveWindow) with a color(default black)
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
	inline __attribute__((always_inline)) void drawArc(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
		if (start == 0 && end == _arcAngleMax) {
			drawArcHelper(cx, cy, radius, thickness, 0, _arcAngleMax, color);
		} else {
			drawArcHelper(cx, cy, radius, thickness, start + (_arcAngleOffset / (float)360)*_arcAngleMax, end + (_arcAngleOffset / (float)360)*_arcAngleMax, color);
		}	
	}

//-------------- LAYERS -----------------------------------------
	void 		useLayers(boolean on);//mainly used to turn of layers!
	void		writeTo(enum RA8875writes d);//L1, L2, CGRAM, PATTERN, CURSOR
	void 		layerEffect(enum RA8875boolean efx);//LAYER1, LAYER2, TRANSPARENT, LIGHTEN, OR, AND, FLOATING
	void 		layerTransparency(uint8_t layer1,uint8_t layer2);
	uint8_t		getCurrentLayer(void); //return the current drawing layer. If layers are OFF, return 255
//--------------- SCROLL ----------------------------------------
	void        setScrollMode(enum RA8875scrollMode mode); // The experimentalist
	void 		setScrollWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB);
	void 		scroll(int16_t x,int16_t y);
//-------------- DMA -------------------------------
	void 		DMA_blockModeSize(int16_t BWR,int16_t BHR,int16_t SPWR);
	void 		DMA_startAddress(unsigned long adrs);
	void 		DMA_enable(void);
	void 		drawFlashImage(int16_t x,int16_t y,int16_t w,int16_t h,uint8_t picnum);
//-------------- BTE --------------------------------------------
	void 		BTE_size(int16_t w, int16_t h);
	//void	 	BTE_fromTo(int16_t SX,int16_t DX ,int16_t SY ,int16_t DY);
	void	 	BTE_moveFrom(int16_t SX,int16_t SY);
	void	 	BTE_moveTo(int16_t DX,int16_t DY);
	//void 		BTE_source_destination(uint16_t SX,uint16_t DX ,uint16_t SY ,uint16_t DY);
	void		BTE_ropcode(unsigned char setx);//
	void 		BTE_enable(bool on);//
	void 		BTE_dataMode(enum RA8875btedatam m);//CONT,RECT
	void 		BTE_layer(enum RA8875btelayer sd,uint8_t l);//SOURCE,DEST - 1 or 2
	void		BTE_move(int16_t SourceX, int16_t SourceY, int16_t Width, int16_t Height, int16_t DestX, int16_t DestY, uint8_t SourceLayer=0, uint8_t DestLayer=0, bool Transparent = false, uint8_t ROP=RA8875_BTEROP_SOURCE, bool Monochrome=false, bool ReverseDir = false);
	//---------- pattern --------------------------------------------
	void 		setPattern(uint8_t num, enum RA8875pattern p=P8X8);
	void 		writePattern(int16_t x,int16_t y,const uint8_t *data,uint8_t size,bool setAW=true);
//-------------- GPIO & PWM -------------------------
	void    	GPIOX(boolean on);
	void    	PWMout(uint8_t pw,uint8_t p);//1:backlight, 2:free
//-------------- Touch Screen -------------------------
#if !defined(USE_EXTERNALTOUCH)
	void 		touchBegin(uint8_t intPin);//prepare Touch Screen driver
	void    	touchEnable(boolean enabled);//enable/disable Touch Polling (disable INT)
	boolean 	touchDetect(boolean autoclear=false);//true=touch detected
	//Note:		must followed by touchReadxxx or use autoclear=true for standalone
	void 		touchReadAdc(uint16_t *x, uint16_t *y);//returns 10bit ADC data (0...1024)
	void 		touchReadPixel(uint16_t *x, uint16_t *y);//return pixels (0...width, 0...height)
	boolean		touchCalibrated(void);//true if screen calibration it's present
#endif
//--------------Capacitive Touch Screen FT5206 -------------------------
#if defined (USE_FT5206_TOUCH)
	void 		armTouchISR(bool force = false); 
	bool 		touched(bool safe=false);
	void 		setTouchLimit(uint8_t limit);
	uint8_t 	getTouchLimit(void);
	void	 	updateTS(void);
	uint8_t 	getGesture(void);
	uint8_t 	getTouches(void);
	uint8_t 	getTouchState(void);
	uint8_t 	getTScoordinates(uint16_t (*touch_coordinates)[2]);
#endif
//------------- Keyscan Matrix ------------------------------------------
/*
#if defined(USE_RA8875_KEYMATRIX)
	void 	keypadInit(
			bool scanEnable = true, 
			bool longDetect = false, 
			uint8_t sampleTime = 0, 		//0..3
			uint8_t scanFrequency = 0, 		//0..7
			uint8_t longTimeAdjustment = 0,	//0..3
			bool interruptEnable = false, 
			bool wakeupEnable = false
	);
	boolean keypadTouched(void);
	uint8_t getKey(void);
	
	void enableKeyScan(bool on);
	uint8_t getKeyValue(void);
	boolean isKeyPress(void);
#endif
*/
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
 protected:
	uint8_t _rst;
	#if defined (USE_FT5206_TOUCH)
	uint8_t _ctpInt;
	#endif
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
		uint8_t _cs;
		uint8_t _miso, _mosi, _sclk;
		#if defined(__MKL26Z64__)
			//bool _altSPI;
		#endif
	#elif defined(ENERGIA)
		uint8_t _cs;
	#else
		#if defined(__SAM3X8E__)
			#if defined(_FASTSSPORT)
				volatile uint32_t *csport;
				uint32_t _cs, cspinmask;
			#else
				uint8_t _cs;
			#endif
		#else
			#if defined(_FASTSSPORT)
				volatile uint8_t *csport;
				uint8_t _cs, cspinmask;
			#else
				uint8_t _cs;
			#endif
		#endif
	#endif
	
 private:
	volatile bool 			_currentMode;

	// Touch Screen vars ---------------------
	#if defined (USE_FT5206_TOUCH)//internal FT5206 driver
		uint8_t					_maxTouch;
		static void 		 	isr(void);
		uint8_t 				_cptRegisters[28];
		uint8_t					_gesture;
		uint8_t					_currentTouches;//0...5
		uint8_t					_currentTouchState;//0,1,2
		bool					_needISRrearm;
		void 					initFT5206(void);
		void 					regFT5206(uint8_t reg,uint8_t val);
	//const uint8_t coordRegStart[5] = {{0x03},{0x09},{0x0F},{0x15},{0x1B}};
	#endif
	#if !defined(USE_EXTERNALTOUCH)
	uint8_t					_touchPin;
	bool					_clearTInt;
	uint16_t 				_tsAdcMinX,_tsAdcMinY,_tsAdcMaxX,_tsAdcMaxY;
	bool					_touchEnabled;
	#endif
	#if defined(USE_RA8875_KEYMATRIX)
	bool					_keyMatrixEnabled;
	#endif
	//system vars -------------------------------------------
	bool					_inited;//true when init has been ended
	bool					_sleep;
	bool					_portrait;
	uint8_t					_rotation;
	uint8_t					_initIndex;
	int16_t 		 		_width, _height;//relative to rotation
	int16_t 		 		WIDTH, HEIGHT;//absolute
	int16_t					_activeWindowXL;
	int16_t					_activeWindowXR;
	int16_t					_activeWindowYT;
	int16_t					_activeWindowYB;
	uint8_t					_errorCode;
	//color vars ----------------------------------------------
	uint16_t				_foreColor;
	uint16_t				_backColor;
	bool					_backTransparent;
	uint8_t					_colorIndex;
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
	uint16_t				_textForeColor;
	uint16_t				_textBackColor;
	bool					_recoverTextColor;
	#endif
	//text vars-------------------------------------------------
	int16_t					_cursorX, _cursorY;//try to internally track text cursor...
	uint8_t 		 		_textHScale, _textVScale;	 		
	uint8_t					_fontSpacing;
	uint8_t					_fontInterline;
	/*
	bit			 parameter
	0	->		_extFontRom
	1	->		_autoAdvance
	2	->		_textWrap
	3	->		_fontFullAlig
	4	->		_fontRotation
	5	->		_alignXToCenter;
	6	->		_alignYToCenter;
	*/
	bool					_centerFlag;
	bool					_autocenter;
	uint8_t					_commonTextPar;
	enum RA8875extRomFamily _fontFamily;
	enum RA8875extRomType 	_fontRomType;
	enum RA8875extRomCoding _fontRomCoding;
	enum RA8875tsize		_textSize;
	//volatile bool 			_currentMode;
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
	uint8_t					_brightness;
	//various
	float 					_arcAngleMax;
	int 					_arcAngleOffset;
	// Register containers -----------------------------------------
	// this needed to  prevent readRegister from chip that it's slow.
	volatile uint8_t		_MWCR0Reg; //keep track of the register 		  [0x40]
	uint8_t		_DPCRReg;  ////Display Configuration		  	  [0x20]
	uint8_t		_FNCR0Reg; //Font Control Register 0 		  	  [0x21]
	uint8_t		_FNCR1Reg; //Font Control Register1 			  [0x22]
	uint8_t		_FWTSETReg; //Font Write Type Setting Register 	  [0x2E]
	uint8_t		_SFRSETReg; //Serial Font ROM Setting 		  	  [0x2F]
	uint8_t		_TPCR0Reg; //Touch Panel Control Register 0	  	  [0x70]
	uint8_t		_INTC1Reg; //Interrupt Control Register1		  [0xF0]
	//		functions --------------------------
	void 	initialize();
	void 	setSysClock(uint8_t pll1,uint8_t pll2,uint8_t pixclk);
	void    textWrite(const char* buffer, uint16_t len=0);//thanks to Paul Stoffregen for the initial version of this one
	void 	PWMsetup(uint8_t pw,boolean on, uint8_t clock);
	void 	updateActiveWindow(bool full);
	void 	changeMode(bool m);
	void 	scanDirection(boolean invertH,boolean invertV);
	// 		helpers-----------------------------
	void 	setTextPosition(int16_t x, int16_t y,bool update);
	//void 	checkLimitsHelper(int16_t &x,int16_t &y);//RA8875 it's prone to freeze with values out of range
	void 	circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled);
	void 	rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled);
	void 	triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled);
	void 	ellipseCurveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis,uint8_t curvePart, uint16_t color, bool filled);
	void 	lineAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	void 	curveAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
	void 	roundRectHelper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled);
	void 	drawArcHelper(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float startAngle, float endAngle, uint16_t color);
	float 	cosDegrees(float angle);
	float 	sinDegrees(float angle);
	void 	setArcParams(float arcAngleMax, int arcAngleOffset);
	#if !defined(USE_EXTERNALTOUCH)
	void	readTouchADC(uint16_t *x, uint16_t *y);
	void 	clearTouchInt(void);
	boolean touched(void);
	#endif
	inline __attribute__((always_inline)) 
		void checkLimitsHelper(int16_t &x,int16_t &y){
			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x >= WIDTH) x = WIDTH - 1;
			if (y >= HEIGHT) y = HEIGHT -1;
			x = x;
			y = y;
		}
    // Low level access  commands ----------------------
	//void 		startSend();
	//void 		endSend();
	inline __attribute__((always_inline)) 
	void startSend(){
		#if defined(SPI_HAS_TRANSACTION)
			#if defined(__MKL26Z64__)	
				_altSPI == true ? SPI1.beginTransaction(SPISettings(_maxspeed, MSBFIRST, SPI_MODE3)) : SPI.beginTransaction(SPISettings(_maxspeed, MSBFIRST, SPI_MODE3));
			#else
				SPI.beginTransaction(SPISettings(_maxspeed, MSBFIRST, SPI_MODE3));
			#endif
		#elif !defined(ENERGIA)
			cli();//protect from interrupts
		#endif
		#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
			digitalWriteFast(_cs, LOW);
		#else
			#if !defined(ENERGIA)//UNO,DUE,ETC.
				#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)//DUE extended SPI
					//nothing
				#else//DUE (normal),UNO,ETC.
					#if defined(_FASTSSPORT)
						*csport &= ~cspinmask;
					#else
						digitalWrite(_cs, LOW);
					#endif
				#endif
			#else//ENERGIA
				digitalWrite(_cs, LOW);
			#endif
		#endif
	}
	
	inline __attribute__((always_inline)) 
	void endSend(){
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
		digitalWriteFast(_cs, HIGH);
	#else
		#if !defined(ENERGIA)
			#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)//DUE extended SPI
				//nothing
			#else//DUE (normal),UNO,ETC.
				#if defined(_FASTSSPORT)
					*csport |= cspinmask;
				#else
					digitalWrite(_cs, HIGH);
				#endif
			#endif
		#else//ENERGIA
			digitalWrite(_cs, HIGH);
		#endif
	#endif
	#if defined(SPI_HAS_TRANSACTION)
		#if defined(__MKL26Z64__)	
			_altSPI == true ? SPI1.endTransaction() : SPI.endTransaction();
		#else
			SPI.endTransaction();
		#endif
	#elif !defined(ENERGIA)
		sei();//enable interrupts
	#endif
} 
	void    	writeReg(uint8_t reg, uint8_t val);
	uint8_t 	readReg(uint8_t reg);
	void    	writeData(uint8_t data);
	uint8_t 	readData(bool stat=false);
	#if defined(_FASTCPU)
		//void 		slowDownSPI(bool slow);
	#endif
	
	boolean 	waitPoll(uint8_t r, uint8_t f);//from adafruit
	void 		waitBusy(uint8_t res=0x80);//0x80, 0x40(BTE busy), 0x01(DMA busy)

	#if defined(NEEDS_SET_MODULE)//for Energia
	void 		selectCS(uint8_t module);
	#endif

#if defined(_FASTCPU)
inline __attribute__((always_inline)) 
void slowDownSPI(bool slow)
{
	#if defined(SPI_HAS_TRANSACTION)
		if (slow){
			_maxspeed = 10000000;
		} else {
			#if defined(__MKL26Z64__)	
				if (_altSPI){
					_maxspeed = 22000000;//TeensyLC max SPI speed on alternate SPI
				} else {
					_maxspeed = MAXSPISPEED;
				}
			#else
				_maxspeed = MAXSPISPEED;
			#endif
		}
	#else
		if (slow){
			#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_SAFE);
			#else
				SPI.setClockDivider(SPI_SPEED_SAFE);
			#endif
		} else {
			#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_WRITE);
			#else
				SPI.setClockDivider(SPI_SPEED_WRITE);
			#endif
		}
	#endif
}
#endif

#if defined(__AVR__)
	inline __attribute__((always_inline))
		void spiwrite16(uint16_t d) {
			SPDR = highByte(d);
			while (!(SPSR & _BV(SPIF)));
			SPDR = lowByte(d);
			while (!(SPSR & _BV(SPIF)));
		}
/*	
	inline __attribute__((always_inline))
		uint16_t spiread16(void) {
			uint16_t r = 0;
			SPDR = highByte(d);
			while (!(SPSR & _BV(SPIF)));
			SPDR = lowByte(d);
			while (!(SPSR & _BV(SPIF)));
		}	
*/
	inline __attribute__((always_inline))
		void spiwrite(uint8_t c) {
			SPDR = c;
			//asm volatile("nop");
			while (!(SPSR & _BV(SPIF)));
		}
		
	inline __attribute__((always_inline))
		uint8_t spiread(void) {
			uint8_t r = 0;
			SPDR = 0x00;
			//asm volatile("nop");
			while(!(SPSR & _BV(SPIF)));
			r = SPDR;
			return r;
		}
#endif


};

#endif
