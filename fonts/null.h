#ifndef _ST7735_NULL_FONT_H
#define _ST7735_NULL_FONT_H
#include <stdio.h>
	#if defined(_FORCE_PROGMEM__)
		#include "null_prgm.c"
	#else
		#include "null.c"
	#endif
#endif