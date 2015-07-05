#ifndef _ST7735_HISCORENUM_FONT_H
#define _ST7735_HISCORENUM_FONT_H
#include <stdio.h>
	#if defined(_FORCE_PROGMEM__)
		#include "hiscore_num_prgm.c"
	#else
		#include "hiscore_num.c"
	#endif
#endif