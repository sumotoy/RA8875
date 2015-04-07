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
ISO_IEC_8859_1 (default), ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4
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
GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN/GREEK/ARABIC */
#define	_DFT_RA8875_EXTFONTROMCODING	GB2312

/* SPI MAX SPEED it's ONLY used in SPI Transaction mode +++++++++++++++++++
it ensure the max and correct speed for accessing RA8875 in Read/Write...
Datasheet it's clear:

System clock/3(only write cycle), System clock/6(with read cycle)

MAXSPISPEED parameters it's also related to MCU features so it probably need to be tuned.
Not all MCU are capable to work at those speeds. Those parameters should work fine.

SPI_MULT parameter it was just introduced and it's a multiplier, the purpose it's multiply
the MAXSPISPEED so you can have faster SPI performances, however this depends of many factors
like the lenght of cables, soldering, and (most important) the SysClock (in initialization function).
You cannot go over the max SPI speed supported by chip that is 20Mhz!
*/
#if defined(__MK20DX128__) || defined(__MK20DX256__) //teensy 3 , 3.1
	#define MAXSPISPEED 			8000000
	#define SPI_MULT				1
#elif defined(__MKL26Z64__)							 //teensy LC	 (12 or 24 Mhz max)
	#define MAXSPISPEED 			6000000
	#define SPI_MULT				1
#elif defined(__SAM3X8E__)							 // due
	#define MAXSPISPEED 			6600000
	#define SPI_MULT				1
#else												 //rest of the world
	#define MAXSPISPEED 			4000000
	#define SPI_MULT				1
#endif

#endif