#ifndef _ST7735_ARIALNUMBERS_FONT_H
#define _ST7735_ARIALNUMBERS_FONT_H
#include <stdio.h>
	#if defined(_FORCE_PROGMEM__)
		#include "arial_numbers_prgm.c"
	#else
		#include "arial_numbers.c"
	#endif
#endif