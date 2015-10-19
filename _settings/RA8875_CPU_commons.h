#ifndef _RA8875_CPU_H_
#define _RA8875_CPU_H_
/*
CPU based Preprocessor directives file.
RA8875 Library support many MCU's so I wroted this to simplify the overall code.

Part of RA8875 library from https://github.com/sumotoy/RA8875

    RA8875 fast SPI library for RAiO SPI RA8875 drived TFT
    Copyright (C) 2014  egidio massimo costa sumotoy (a t) gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* 
--------------------------------------------------------------
			ENERGIA BASED BOARDS
	Partially supported and actually never tested
--------------------------------------------------------------
*/
#if defined(ENERGIA)
	#include "Energia.h"
	#undef byte
	#define byte uint8_t
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
#elif defined(__AVR__)
/* 
--------------------------------------------------------------
			8 BIT AVR BOARDS (UNO,YUN,LEONARDO,ETC.)
	Fully supported (tested)
--------------------------------------------------------------
*/
	#if !defined(_FORCE_PROGMEM__)
		#define _FORCE_PROGMEM__
	#endif
	#define __PRGMTAG_	PROGMEM
	#include "Arduino.h"
	#include <math.h>
	#include <avr/pgmspace.h>
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
/* 
--------------------------------------------------------------
			TEENSY 3, TEENSY 3.1, TEENSY 3.2, TEENSY LC
	Fully supported (tested)
--------------------------------------------------------------
*/
	#define ___TEENSYES
	#define _FASTCPU
	#include "Arduino.h"
	#include <avr/pgmspace.h>//Teensy3 and AVR arduinos can use pgmspace.h
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
		#define PROGMEM __attribute__((section(".progmem.data")))
	#endif
	#define __PRGMTAG_	
#elif defined(__32MX320F128H__) || defined(__32MX795F512L__) //chipkit uno, chipkit max
/* 
--------------------------------------------------------------
			CHIPKIT UNO, CHIPKIT MAX
	Partially supported and never tested
--------------------------------------------------------------
*/
	#define ___CHIPKIT
	#define _FASTCPU
	#include "Arduino.h"
	#ifndef __PGMSPACE_H_
	  #define __PGMSPACE_H_ 1
	  #define PROGMEM
	  #define PGM_P  const char *
	  #define PSTR(str) (str)
	  #define pgm_read_byte_near(addr) pgm_read_byte(addr)
	  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
	#define __PRGMTAG_	
#elif defined (__arm__) && defined(_VARIANT_ARDUINO_DUE_X_)
/* 
--------------------------------------------------------------
			ARDUINO DUE
	Fully supported (tested)
--------------------------------------------------------------
*/
	#define ___DUESTUFF
	#define _FASTCPU
	#include "Arduino.h"
	#ifndef __PGMSPACE_H_
	  #define __PGMSPACE_H_ 1
	  #define PROGMEM
	  #define PGM_P  const char *
	  #define PSTR(str) (str)
	  #define pgm_read_byte_near(addr) pgm_read_byte(addr)
	  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
	#define __PRGMTAG_	
#elif defined (__arm__) && defined(__SAM3X8E__)
/* 
--------------------------------------------------------------
			ARDUINO DUE COMPATIBLE
	Fully supported (as DUE, tested)
--------------------------------------------------------------
*/
	#define ___DUESTUFF
	#define _FASTCPU
	#include "Arduino.h"
	#ifndef __PGMSPACE_H_
	  #define __PGMSPACE_H_ 1
	  #define PROGMEM
	  #define PGM_P  const char *
	  #define PSTR(str) (str)
	  #define pgm_read_byte_near(addr) pgm_read_byte(addr)
	  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
#elif defined(STM32F2XX) || defined(STM32F10X_MD) || defined(STM32_SERIES_F1) || defined(STM32_SERIES_F2)
/* 
--------------------------------------------------------------
			STM32 BOARDS
	Initial support and actually won't compile
--------------------------------------------------------------
*/
	#define ___STM32STUFF
	#include "Arduino.h"
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
	#endif
	#define __PRGMTAG_	
#elif defined(__arm__) && !defined(__XTENSA__) && !defined(___TEENSYES)
/* 
--------------------------------------------------------------
			ARM generic
	Mistery....
--------------------------------------------------------------
*/
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
	#endif
	#include "Arduino.h"
	#define __PRGMTAG_	
	#warning "Generic Arm detected, not sure if your board it's compatible!"
#elif defined(__XTENSA__)
/* 
--------------------------------------------------------------
			XTENSA (ESP)
	It compiles but never tested
--------------------------------------------------------------
*/
	#include "Arduino.h"
	#if defined(_FORCE_PROGMEM__)
		#undef _FORCE_PROGMEM__
		#define PROGMEM __attribute__((section(".progmem.data")))
	#endif
	#define __PRGMTAG_	
#else
	#error "your board it's not supported yet!"
#endif

#include "Print.h"

#endif