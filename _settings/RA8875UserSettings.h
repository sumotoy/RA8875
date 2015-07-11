#ifndef _RA8875USERSETTINGS_H_
#define _RA8875USERSETTINGS_H_
/* ---------------------------- USER SETTINGS --------------------------------------*/
/* [USE_RA8875_SEPARATE_TEXT_COLOR] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
RA8875 set color globally, this mean that if you set text color white and after you create
a red circle, every text after that will result red text. Enabling this feature your text
will always maintain the choosed color. */
#define USE_RA8875_SEPARATE_TEXT_COLOR

/* [RENDERED FONTS FOLLOWS INTERNAL FONTS CURSORS] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The library internal tracking the x,y position of the text cursors but not the rendered fonts, by
enabling this option you will force library to inform RA8875 to keep track of the x,y text cursor.
This option will slow down a bit the text rendering (just a bit) */
#define FORCE_RA8875_TXTREND_FOLLOW_CURS

/* [AVOID ALL TOUCH SCREEN LIBRARY CAPABILITIES] ++++++++++++++++++++++++++++++++++++++++++
If your project don't need Touch Screen or you choosed to use an external library you can comment out
the following line and ALL resources related will not loaded at all. */
//#define _AVOID_TOUCHSCREEN

/* [CHOOSE YOUR TOUCH SCREEN TYPE] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
This library supports the RA8875 internal resistive touch screen driver or a FT5206
based capacitive touch screen driver. Below, you have to choose one of them.
The Resistive Touch screen it's drived directly by RA8875.
The Capacitive Touch Screen use a I2C chip called FT5206 (hardwired to address 0x38).
Please choose at list one (NOT both), if you comment both it's the same as _AVOID_TOUCHSCREEN*/

//#define USE_RA8875_TOUCH
//#define USE_FT5206_TOUCH

/* [Default Interrupt Number for Touchscreen] [RA8875 & FT5206] ++++++++++++++++++++++++++++++++++
Some microcontroller like Teensy 3,3.1 and LC automatically assign ISR number (since they
can virtually assign ISR to any pin) but some old one like Arduino UNO have ISR hardcoded */
#if !defined(_AVOID_TOUCHSCREEN)
	#define __RA8875ISRINT 0//pin 2 on Arduino UNO
#endif

/* [INTERNAL KEY MATRIX] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
RA8875 has a 5x6 Key Matrix controller onboard, if you are not plan to use it
better leave commented the following define since it will share some registers
with several functions, otherwise de-comment it! */
//#define USE_RA8875_KEYMATRIX

/* [DEFAULT CURSOR BLINK RATE] +++++++++++++++++++++++++++++++++++++++++++++++++++++++
Nothing special here, you can set the default blink rate */
#define DEFAULTCURSORBLINKRATE			10

/* [DEFAULT INTERNAL FONT ENCODING] ++++++++++++++++++++++++++++++++++++++++++++++++++
RA8875 has 4 different font set, same shape but suitable for most languages
please look at RA8875 datasheet and choose the correct one for your language!
ISO_IEC_8859_1 (default), 
ISO_IEC_8859_2, 
ISO_IEC_8859_3, 
ISO_IEC_8859_4
The default one it's the most common one and should work in most situations */
#define DEFAULTINTENCODING				ISO_IEC_8859_1

/* [STARTUP SETTINGS] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#define _DFT_RA8875_TEXTWRAP			true

/* [DEFAULT EXTERNAL FONT-ROM TYPE] ++++++++++++++++++++++++++++++++++++++++++++
If your card has a FONT-ROM installed, here you have to write wich one!
I support both Genicomp and Eastrising since they are the same chip named differently!
Possible solutions:
GT21L16T1W	(Genicomp)
GT21H16T1W	(Genicomp)
GT23L16U2W	(Genicomp)
GT30H24T3Y	(Genicomp)
GT23L24T3Y	(Genicomp)
GT23L24M1Z	(Genicomp)
GT23L32S4W	(Genicomp)
GT30H32S4W	(Genicomp)
GT30L32S4W	(Genicomp)
ER3303_1 	(Eastrising) *tested
ER3304_1 	(Eastrising) *tested
*/
#define	_DFT_RA8875_EXTFONTROMTYPE		GT21L16T1W

/* [DEFAULT EXTERNAL FONT-ROM ENCODING] ++++++++++++++++++++++++++++++++++++++++++++
Having an external FONT-ROM mean choose the desidered encoding (supported by ROM-CHIP!)
Possible solutions:
GB2312, 
GB12345, 
BIG5, 
UNICODE, 
ASCII, 
UNIJIS, 
JIS0208, 
LATIN/GREEK/ARABIC */
#define	_DFT_RA8875_EXTFONTROMCODING	GB2312

/* [Default Screen Rotation] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTSCRROT			0
/* [Default Backlight Color] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTBACKLIGHT		RA8875_BLACK

/* [Default foreground Text Color] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTTXTFRGRND		RA8875_WHITE

/* [Default background Text Color] ++++++++++++++++++++++++++++++++++++++++++++
*/
#define	_RA8875_DEFAULTTXTBKGRND		RA8875_BLACK

/* [ARDUINO DUE SPI MODE] ++++++++++++++++++++++++++++++++++++++++++++
This library support DUE SPI Extended mode, by decommenting the line below
you have to choose from pin 4,10,52 for CS pin in DUE or you will get an error!
*/
//#define SPI_DUE_MODE_EXTENDED

/*----------------------------------------------------------------------------------
									SPI SPEED
----------------------------------------------------------------------------------*/
/*

On Arduino DUE and other 8 bit Arduino MCU you can disable the fast CS port 
by commenting #define _FASTSSPORT !!!

*/
/* Accordly RA8875 datasheet the READ cycles and WRITE cycles have different speed:
System clock/3(only write cycle), System clock/6(with read cycle)
MAXSPISPEED parameters it's also related to MCU features so it probably need to be tuned.
Not all MCU are capable to work at those speeds. Following parameters worked with both board I have.
After som mail exchange with RAiO I solved the dilemma behind SPI speed limit:
The RA8875 has limitation of 12Mhz SPI but this has been set because not all internal macros
can run over that speed, the library automatically deal with this  so I was able to go over 20Mhz!
At that speed you need to short cables as much you can, provide clean supply and good decoupling!
DO NOT Exceed 23Mhz for RA8875! It will result in garbage on screen or run very slow.
*/

#if defined(SPI_HAS_TRANSACTION)//SPI transaction enabled library----------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__) //Teensy 3.0 , 3.1
		const static uint32_t MAXSPISPEED	= 22000000;//22000000;
	#elif defined(__MKL26Z64__)							 //Teensy LC (12 or 24 Mhz max)
		const static uint32_t MAXSPISPEED	= 12000000;	 //default SPI main speed TeensyLC
		const static uint32_t MAXSPISPEED2	= 22000000;
	#elif defined(__SAM3X8E__)							 //DUE
		const static uint32_t MAXSPISPEED	= 22000000;
		//#define _FASTSSPORT
	#else												 //rest of the world (UNO, etc)
		const static uint32_t MAXSPISPEED	= 10000000;
		#define _FASTSSPORT
	#endif
#else// legacy SPI library-------------------------------------------------------------
	#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
		#if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)//tiva???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)//stellaris first version
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(__MSP430MCU__)//MSP430???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(TMS320F28069)//C2000???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#elif defined(__CC3200R1M1RGC__)//CC3200???
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV8
		#endif
	#else
		#if defined(__SAM3X8E__)// DUE
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV4	//84 divided by 4 = 21Mhz
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV8
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV6	//10.5Mhz
			//#define _FASTSSPORT
		#else//rest of the world included UNO, etc.
			#define SPI_SPEED_WRITE 	SPI_CLOCK_DIV2	//UNO = 4Mhz
			#define SPI_SPEED_READ 		SPI_CLOCK_DIV4
			#define SPI_SPEED_SAFE 		SPI_CLOCK_DIV2
			#define _FASTSSPORT
		#endif
	#endif
#endif

//Touch Screen stuff (do not touch)
#if defined(_AVOID_TOUCHSCREEN)
	#undef USE_RA8875_TOUCH
	#undef USE_FT5206_TOUCH
#else
	#if defined(USE_RA8875_TOUCH) && defined(USE_FT5206_TOUCH)
		#error you have to choose between USE_RA8875_TOUCH & USE_FT5206_TOUCH!
	#elif !defined(USE_RA8875_TOUCH) && !defined(USE_FT5206_TOUCH)
		#define _AVOID_TOUCHSCREEN
	#elif defined(USE_FT5206_TOUCH) && !defined(USE_RA8875_TOUCH)
		#include "Wire.h"//include the support for FT5206
		static const uint8_t _FT5206REgisters[9] = {
			0x16,0x3C,0xE9,0x01,0x01,0xA0,0x0A,0x06,0x28
		};
	#elif !defined(USE_FT5206_TOUCH) && defined(USE_RA8875_TOUCH)
		#include "_settings/RA8875Calibration.h"
	#endif
#endif

#if defined(__AVR__)
	#if !defined(_FORCE_PROGMEM__)
		#define _FORCE_PROGMEM__
	#endif
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
	#endif
#endif

#endif