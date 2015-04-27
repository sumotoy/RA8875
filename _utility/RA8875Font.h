
#ifndef __RA8875EXTFONT__
#define __RA8875EXTFONT__
struct FONT_DEF {
   const uint8_t glyph_width;                 
   const uint8_t glyph_height;
   const unsigned char *glyph_table;
   const unsigned char *mapping_table;
   const unsigned char *width_table;
   const unsigned int *offset_table;
	};
#endif