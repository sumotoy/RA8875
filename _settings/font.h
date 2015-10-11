#ifndef _ST7735_FONTS_H
	#define _ST7735_FONTS_H
	
	#include <stdio.h>

	#if defined(_FORCE_PROGMEM__)
/*
		#if defined(__AVR__)
			#include <avr/pgmspace.h>
		#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
			#include <avr/pgmspace.h>//Teensy3 and AVR arduinos can use pgmspace.h
			#ifdef PROGMEM
				#undef PROGMEM
				#define PROGMEM __attribute__((section(".progmem.data")))
			#endif
		#elif defined(__SAM3X8E__)
			#include <include/pio.h>
			#define PROGMEM
			#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
			#define pgm_read_word(addr) (*(const unsigned short *)(addr))
			typedef unsigned char prog_uchar;
		#elif defined(ESP8266)
		//none
		#endif
*/
 
		typedef struct PROGMEM {
				const uint8_t 	*data;
				uint8_t 		image_width;
		} tImage;

		typedef struct {
				uint8_t 		char_code;
				const tImage 	*image;
		} tChar;

		typedef struct {
				uint8_t 		length;
				const tChar 	*chars;
				uint8_t			font_width;
				uint8_t			font_height;
				bool 			rle;
		} tFont;
	#else
		typedef struct {
				const uint8_t 	*data;
				uint8_t 		image_width;
		} tImage;

		typedef struct {
				uint8_t 		char_code;
				const tImage 	*image;
		} tChar;

		typedef struct {
				uint8_t 		length;
				const tChar 	*chars;
				uint8_t			font_width;
				uint8_t			font_height;
				bool 			rle;
		} tFont;
	#endif
#endif
