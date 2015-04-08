#ifndef _RA8875USERSETTINGS_H_
#define _RA8875USERSETTINGS_H_
/* ---------------------------- USER SETTINGS ---------------------*/

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
#define DEFAULTCURSORBLINKRATE			10

/* DEFAULT INTERNAL FONT ENCODING ++++++++++++++++++++++++++++++++++++++++++++
RA8875 has 4 different font set, same shape but suitable for most languages
please look at RA8875 datasheet and choose the correct one for your language!
ISO_IEC_8859_1 (default), 
ISO_IEC_8859_2, 
ISO_IEC_8859_3, 
ISO_IEC_8859_4
The default one it's the most common one and should work in most situations */
#define DEFAULTINTENCODING				ISO_IEC_8859_1

/* STARTUP SETTINGS ++++++++++++++++++++++++++++++++++++++++++++ */
#define _DFT_RA8875_TEXTWRAP			true//

/* DEFAULT EXTERNAL FONT-ROM TYPE ++++++++++++++++++++++++++++++++++++++++++++
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
ER3303_1 	(Eastrising)
ER3304_1 	(Eastrising) */
#define	_DFT_RA8875_EXTFONTROMTYPE		GT21L16T1W

/* DEFAULT EXTERNAL FONT-ROM ENCODING ++++++++++++++++++++++++++++++++++++++++++++
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

/* SPI MAX SPEED it's ONLY used in SPI Transaction mode +++++++++++++++++++
it ensure the max and correct speed for accessing RA8875 in Read/Write...
Datasheet it's clear:

System clock/3(only write cycle), System clock/6(with read cycle)

MAXSPISPEED parameters it's also related to MCU features so it probably need to be tuned.
Not all MCU are capable to work at those speeds. Those parameters should work fine.
*/
#if defined(SPI_HAS_TRANSACTION)
	#if defined(__MK20DX128__) || defined(__MK20DX256__) //teensy 3 , 3.1
		#define MAXSPISPEED 			14000000//14000000 it's the max SPI freq without artifacts
	#elif defined(__MKL26Z64__)							 //teensy LC	 (12 or 24 Mhz max)
		#define MAXSPISPEED 			12000000
	#elif defined(__SAM3X8E__)							 // due
		#define MAXSPISPEED 			6600000
	#else												 //rest of the world
		#define MAXSPISPEED 			4000000
	#endif
#else
/* SPI MAX SPEED (old legacy SPI) +++++++++++++++++++
	it ensure the max and correct speed for accessing RA8875 in Read/Write...
*/
	#if defined(__SAM3X8E__)							 // due
		#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
		#define SPI_SPEED_READ SPI_CLOCK_DIV8
	#else												 //rest of the world
		#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
		#define SPI_SPEED_READ SPI_CLOCK_DIV8
	#endif
#endif

/* SPI MAX SPEED (Energia IDE doesn't have SPI Transaction) +++++++++++++++++++
	it ensure the max and correct speed for accessing RA8875 in Read/Write...
*/
#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
	#if defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)//tiva???
		#define SPI_SPEED_WRITE SPI_CLOCK_DIV4
		#define SPI_SPEED_READ SPI_CLOCK_DIV8
	#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__)//stellaris first version
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
#endif


#endif