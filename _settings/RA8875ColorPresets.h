#ifndef _RA8875COLORPRESETS_H_
#define _RA8875COLORPRESETS_H_

// Colors preset (RGB565)
const uint16_t	RA8875_BLACK            = 0x0000;
const uint16_t 	RA8875_WHITE            = 0xFFFF;

const uint16_t	RA8875_RED              = 0xF800;
const uint16_t	RA8875_GREEN            = 0x07E0;
const uint16_t	RA8875_BLUE             = 0x001F;

const uint16_t 	RA8875_CYAN             = RA8875_GREEN | RA8875_BLUE;//0x07FF;
const uint16_t 	RA8875_MAGENTA          = 0xF81F;
const uint16_t 	RA8875_YELLOW           = RA8875_RED | RA8875_GREEN;//0xFFE0;  
const uint16_t 	RA8875_LIGHT_GREY 		= 0xB5B2; // the experimentalist
const uint16_t 	RA8875_LIGHT_ORANGE 	= 0xFC80; // the experimentalist
const uint16_t 	RA8875_DARK_ORANGE 		= 0xFB60; // the experimentalist
const uint16_t 	RA8875_PINK 			= 0xFCFF; // M.Sandercock
const uint16_t 	RA8875_PURPLE 			= 0x8017; // M.Sandercock
const uint16_t 	RA8875_GRAYSCALE 		= 2113;//grayscale30 = RA8875_GRAYSCALE*30

#endif