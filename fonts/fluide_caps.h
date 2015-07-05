#ifndef _ST7735_FLUIDECAPS_FONT_H
#define _ST7735_FLUIDECAPS_FONT_H
#include <stdio.h>
	#if defined(_FORCE_PROGMEM__)
		#include "fluide_caps_prgm.c"
	#else
		#include "fluide_caps.c"
	#endif
#endif