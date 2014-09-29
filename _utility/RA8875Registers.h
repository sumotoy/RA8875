#ifndef _RA8875CONF_H_
#define _RA8875CONF_H_

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            System & Configuration Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Power and Display Control Register [0x01]
----- Bit 7 (LCD Display Off)
0:off, 1:on
----- Bit 6,5,4,3,2, (na)
----- Bit 1 (Sleep Mode)
0:normal, 1:sleep
----- Bit 0 (Software Reset)
0:no action, 1:reset */
#define RA8875_PWRR             		0x01//Power and Display Control Register
	#define RA8875_PWRR_DISPON      0x80
	#define RA8875_PWRR_DISPOFF     0x00
	#define RA8875_PWRR_SLEEP       0x02
	#define RA8875_PWRR_NORMAL      0x00
	#define RA8875_PWRR_SOFTRESET   0x01

#define RA8875_MRWC             		0x02//Memory Read/Write Command
	#define RA8875_CMDWRITE         	0x80
	#define RA8875_CMDREAD          	0xC0
	#define RA8875_DATAWRITE        	0x00
	#define RA8875_DATAREAD         	0x40
	#define RA8875_STATREG				0x40
/* Pixel Clock Setting Register 	[0x04]
----- Bit 7 (PCLK Inversion)
0:PDAT at PLCK rising , 1:PDAT at PLCK falling
----- Bit 6,5,4,3,2 (na)
----- Bit 1,0 (PCLK Period Setting)
00: PCLK period = System Clock period.
01: PCLK period = 2 times of System Clock period
10: PCLK period = 4 times of System Clock period
11: PCLK period = 8 times of System Clock period*/
#define RA8875_PCSR             	  0x04//Pixel Clock Setting Register
/* Serial Flash/ROM Configuration 	 [0x05]
----- Bit 7 (Serial Flash/ROM I/F # Select)
0:Serial Flash/ROM 0 , 1:Serial Flash/ROM 1
----- Bit 6 (Serial Flash/ROM Address Mode)
0: 24 bits address mode
----- Bit 5 (Serial Flash/ROM Waveform Mode)
----- Bit 4,3 (Serial Flash /ROM Read Cycle)
00: 4 bus -> no dummy cycle
01: 5 bus -> 1 byte dummy cycle
1x: 6 bus -> 2 byte dummy cycle
----- Bit 2 (Serial Flash /ROM Access Mode)
0:Font mode, 1:DMA mode
----- Bit 1,0 (Serial Flash /ROM I/F Data Latch Mode Select)
0x: Single Mode
10: Dual Mode 0
11: Dual Mode 1*/

	#define RA8875_SROC         		  0x05//Serial Flash/ROM Configuration
/* Serial Flash/ROM CLK			     [0x06]
----- Bit 7,6,5,4,3,2 (na) 
----- Bit 1,0 (Serial Flash/ROM Clock Frequency Setting) 
0x: SFCL frequency = System clock frequency(DMA on and 256 clr)
10: SFCL frequency = System clock frequency / 2
11: SFCL frequency = System clock frequency / 4 */
#define RA8875_SFCLR         		  0x06//Serial Flash/ROM CLK
	#define EXTROM_SFCLSPEED	0b00000011// /4 0b00000010 /2
/* System Configuration Register		 [0x10]
----- Bit 7,6,5,4 (na) 
----- Bit 3,2 (Color Depth Setting) 
00: 8-bpp generic TFT, i.e. 256 colors
1x: 16-bpp generic TFT, i.e. 65K colors
----- Bit 1,0 (MCUIF Selection) 
00: 8-bit MCU Interface
1x: 16-bit MCU Interface */
#define RA8875_SYSR             	  0x10//System Configuration Register
/* LCD Horizontal Display Width Register [0x14]
----- Bit 7 (na)
----- Bit 6,5,4,3,2,1,0 (Horizontal Display Width Setting Bit)
no more than 0x64( max with = 800)
note: Horizontal display width(pixels) = (HDWR + 1) * 8 */
#define RA8875_HDWR             	  0x14//LCD Horizontal Display Width Register
/* Horizontal Non-Display Period Fine Tuning Option Register [0x15]
----- Bit 7 (DE polarity)
0:High, 1:Low
----- Bit 6,5,4 (na)
----- Bit 3,2,1,0 (Horizontal Non-Display Period Fine Tuning(HNDFT)) */
#define RA8875_HNDFTR           	  0x15//Horizontal Non-Display Period Fine Tuning Option Register
/* LCD Horizontal Non-Display Period Register [0x16]
----- Bit 7,6,5 (na)
----- Bit 4,0 (HSYNC Start Position)
note: HSYNC Start Position(pixels) = (HSTR + 1) * 8 */
#define RA8875_HNDR             	  0x16//LCD Horizontal Non-Display Period Register
/* HSYNC Start Position Register	 [0x17]
----- Bit 7,6,5 (na)
----- Bit 4,0 (HSYNC Start Position)
note: HSYNC Start Position(pixels) = (HSTR + 1) * 8 */
#define RA8875_HSTR             	  0x17//HSYNC Start Position Register
/* HSYNC Pulse Width Register		 [0x18]
----- Bit 7 (HSYNC Polarity)
0:Low, 1:High
----- Bit 6,5 (na)
----- Bit 4,0 (HSYNC Pulse Width(HPW))
note: HSYNC Pulse Width(pixels) = (HPW + 1) * 8 */
#define RA8875_HPWR             	  0x18//HSYNC Pulse Width Register
#define RA8875_VDHR0            	  0x19//LCD Vertical Display Height Register 0
#define RA8875_VDHR1            	  0x1A//LCD Vertical Display Height Register 1
#define RA8875_VNDR0            	  0x1B//LCD Vertical Non-Display Period Register 0
#define RA8875_VNDR1            	  0x1C//LCD Vertical Non-Display Period Register 1
#define RA8875_VSTR0            	  0x1D//VSYNC Start Position Register 0
#define RA8875_VSTR1            	  0x1E//VSYNC Start Position Register 1
#define RA8875_VPWR             	  0x1F//VSYNC Pulse Width Register
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                           LCD Display Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Display Configuration Register	  [0x20]
----- Bit 7 (Layer Setting Control)
0:one Layer, 1:two Layers
----- Bit 6,5,4 (na)
----- Bit 3 (Horizontal Scan Direction, for n = SEG number)
0: SEG0 to SEG(n-1), 1: SEG(n-1) to SEG0
----- Bit 2 (Vertical Scan direction, for n = COM number)
0: COM0 to COM(n-1), 1: COM(n-1) to COM0
----- Bit 1,0 (na) */
#define RA8875_DPCR				  	  0x20//Display Configuration Register
/* Font Control Register 0			  [0x21]
----- Bit 7 (CGRAM/CGROM Font Selection Bit in Text Mode)
0:CGROM font, 1:CGRAM font
----- Bit 6 (na)
----- Bit 5 (External/Internal CGROM)
0:Internal CGROM (RA8875_SFRSET=0), 1:External CGROM(RA8875_FWTSET bit6,7 = 0)
----- Bit 4,3,2 (na)
----- Bit 1,0 (Font Selection for internal CGROM)
00: ISO/IEC 8859-1
01: ISO/IEC 8859-2
10: ISO/IEC 8859-3
11: ISO/IEC 8859-4 */
#define RA8875_FNCR0				  0x21//Font Control Register 0
/* Font Control Register 1			 [0x22]
----- Bit 7 (Full Alignment)
0:disabled, 1:enabled
----- Bit 6 (Font Transparency)
0:disabled, 1:enabled
----- Bit 5 (na)
----- Bit 4 (Font Rotation)
0:normal, 1:90 degrees
----- Bit 3,2 (Horizontal Font Enlargement)
00:normal, 01:x2, 10:x3, 11:x4
----- Bit 1,0 (Vertical Font Enlargement)
00:normal, 01:x2, 10:x3, 11:x4 */
#define RA8875_FNCR1				  0x22//Font Control Register 1
/* CGRAM Select Register			  [0x23]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_CGSR				      0x23//CGRAM Select Register
/* Horizontal Scroll Offset Register 0 [0x24]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_HOFS0				  0x24//Horizontal Scroll Offset Register 0
/* Horizontal Scroll Offset Register 1 [0x25]
----- Bit 7,6,5,4,3 (na) ------------- 
----- Bit 2,0 (Horizontal Display Scroll Offset) */
#define RA8875_HOFS1				  0x25//Horizontal Scroll Offset Register 1
/* Vertical Scroll Offset Register 0 [0x26]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_VOFS0				  0x26//Vertical Scroll Offset Register 0
/* Vertical Scroll Offset Register 1 [0x27]
----- Bit 7,6,5,4,3,2 (na) ------------- 
----- Bit 1,0 (Vertical Display Scroll Offset) ------------- */
#define RA8875_VOFS1				  0x27//Vertical Scroll Offset Register 1
/* Font Line Distance Setting Register[0x29]
----- Bit 7,6,5 (na) ------------- 
----- Bit 4,0 (Font Line Distance Setting) */
#define RA8875_FLDR				  	  0x29//Font Line Distance Setting Register

/* Font Write Cursor Horizontal Position Register 0 [0x2A]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_F_CURXL				  0x2A//Font Write Cursor Horizontal Position Register 0
/* Font Write Cursor Horizontal Position Register 1 [0x2B]
----- Bit 7,2 (na) ------------- 
----- Bit 1,0 (Font Write Cursor Horizontal Position) */
#define RA8875_F_CURXH				  0x2B//Font Write Cursor Horizontal Position Register 1
/* Font Write Cursor Vertical Position Register 0 [0x2C]
----- Bit 7,6,5,4,3,2,1,0 ------------- */
#define RA8875_F_CURYL				  0x2C//Font Write Cursor Vertical Position Register 0
/* Font Write Cursor Vertical Position Register 1 [0x2D]
----- Bit 7,1 (na) ------------- 
----- Bit 0 (Font Write Cursor Vertical Position Register 1) */
#define RA8875_F_CURYH				  0x2D//Font Write Cursor Vertical Position Register 1
/* Font Write Type Setting Register [0x2E]
----- Bit 7,6 -------------
00: 16x16(full) - 8x16(half) - nX16(var)
01: 24x24(full) - 12x24(half) - nX24(var)
1x: 32x32(full) - 16x32(half) - nX32(var)
----- Bit 5,0 -------------
Font to Font Width Setting
00 --> 3F (0 to 63 pixels) */
#define RA8875_FWTSET         		  0x2E//Font Write Type Setting Register
/* Serial Font ROM Setting 			 [0x2F]
----- Bit 7,6,5 -------------
000: GT21L16TW / GT21H16T1W
001: GT30L16U2W
010: GT30L24T3Y / GT30H24T3Y
011: GT30L24M1Z
100: GT30L32S4W / GT30H32S4W
----- Bit 4,3,2 -------------
000: GB2312
001: GB12345/GB18030
010: BIG5
011: UNICODE
100: ASCII
101: UNI-Japanese
110: JIS0208
111: Latin/Greek/ Cyrillic / Arabic
----- Bit 1,0 -------------
00: normal(ASCII) - normal(LGC) -    na(arabic)
01: Arial(ASCII) -  var width(LGC) - PresFormA(arabic)
10: Roman(ASCII) -  na(LGC) -        na(arabic) */
#define RA8875_SFRSET         		  0x2F//Serial Font ROM Setting
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Active Window & Scroll Window Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_HSAW0            	  0x30//Horizontal Start Point 0 of Active Window
#define RA8875_HSAW1            	  0x31//Horizontal Start Point 1 of Active Window
#define RA8875_VSAW0            	  0x32//Vertical   Start Point 0 of Active Window
#define RA8875_VSAW1            	  0x33//Vertical   Start Point 1 of Active Window
#define RA8875_HEAW0            	  0x34//Horizontal End   Point 0 of Active Window
#define RA8875_HEAW1            	  0x35//Horizontal End   Point 1 of Active Window
#define RA8875_VEAW0           		  0x36//Vertical   End   Point of Active Window 0
#define RA8875_VEAW1            	  0x37//Vertical   End   Point of Active Window 1

#define RA8875_HSSW0            	  0x38//Horizontal Start Point 0 of Scroll Window
#define RA8875_HSSW1            	  0x39//Horizontal Start Point 1 of Scroll Window
#define RA8875_VSSW0            	  0x3A//Vertical 	 Start Point 0 of Scroll Window
#define RA8875_VSSW1            	  0x3B//Vertical 	 Start Point 1 of Scroll Window
#define RA8875_HESW0            	  0x3C//Horizontal End   Point 0 of Scroll Window
#define RA8875_HESW1            	  0x3D//Horizontal End   Point 1 of Scroll Window
#define RA8875_VESW0            	  0x3E//Vertical 	 End   Point 0 of Scroll Window
#define RA8875_VESW1            	  0x3F//Vertical 	 End   Point 1 of Scroll Window
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Cursor Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Memory Write Control Register 0   [0x40]
----- Bit 7 (Select Mode)
0: Graphic Mode
1: Text Mode
----- Bit 6 (Font Write Cursor/ Memory Write Cursor Enable)
0: Font write cursor/ Memory Write Cursor is not visible
1: Font write cursor/ Memory Write Cursor is visible
----- Bit 5 (Font Write Cursor/ Memory Write Cursor Blink Enable)
0: Normal display
1: Blink display
----- Bit 4 (na)
----- Bit 3,2 (Memory Write Direction (Only for Graphic Mode)
00: Left -> Right then Top -> Down
01: Right -> Left then Top -> Down
10: Top -> Down then Left -> Right
11: Down -> Top then Left -> Right
----- Bit 1 (Memory Write Cursor Auto-Increase Disable)
0: Cursor auto-increases when memory write
1: Cursor doesn’t auto-increases when memory write
----- Bit 0(Memory Read Cursor Auto-Increase Disable) 
0: Cursor auto-increases when memory read
1: Cursor doesn’t auto-increases when memory read */
#define RA8875_MWCR0            	  0x40//Memory Write Control Register 0

/* Memory Write Control Register 1   [0x41]
----- Bit 7 (Graphic Cursor Enable)
0:disable, 1:enable
----- Bit 6,5,4 (Graphic Cursor Selection)
000: Graphic Cursor Set 1
...
111: Graphic Cursor Set 8
----- Bit 3,2 (Write Destination Selection)
00: Layer 1~2
01: CGRAM
10: Graphic Cursor
11: Pattern
Note : When CGRAM is selected , RA8875_FNCR0 bit 7 must be set as 0.
----- Bit 1 (na)
----- Bit 0 (Layer No. for Read/Write Selection)
When resolution =< 480x400 or color depth = 8bpp:
0: Layer 1
1: Layer 2
When resolution > 480x400 and color depth > 8bpp:
na */
#define RA8875_MWCR1            	  0x41//Memory Write Control Register 1
/*
from 0 to 255
*/
#define RA8875_BTCR            	  	  0x44//Blink Time Control Register
/* Memory Read Cursor Direction      [0x45]
----- Bit 7,6,5,4,3,2(na)
----- Bit 1,0(Memory Read Direction (Only for Graphic Mode))
00: Left -> Right then Top -> Down
01: Right -> Left then Top -> Down
10: Top -> Down then Left -> Right
11: Down -> Top then Left -> Right */
#define RA8875_MRCD            	  	  0x45//Memory Read Cursor Direction
#define RA8875_CURH0            	  0x46//Memory Write Cursor Horizontal Position Register 0
#define RA8875_CURH1            	  0x47//Memory Write Cursor Horizontal Position Register 1
#define RA8875_CURV0            	  0x48//Memory Write Cursor Vertical Position Register 0
#define RA8875_CURV1            	  0x49//Memory Write Cursor Vertical Position Register 1

#define RA8875_RCURH0           	  0x4A//Memory Read Cursor Horizontal Position Register 0
#define RA8875_RCURH1           	  0x4B//Memory Read Cursor Horizontal Position Register 1
#define RA8875_RCURV0           	  0x4C//Memory Read Cursor Vertical Position Register 0
#define RA8875_RCURV1           	  0x4D//Memory Read Cursor Vertical Position Register 1


/* Font Write Cursor and Memory Write Cursor Horizontal Size  [0x4E]
----- Bit 7,6,5 (na)
----- Bit 4,0(Font Write Cursor Horizontal Size in pixels) */
#define RA8875_CURHS            	  0x4E//Font Write Cursor and Memory Write Cursor Horizontal Size Register
/* Font Write Cursor Vertical Size Register   [0x4F]
----- Bit 7,6,5 (na)
----- Bit 4,0(Font Write Cursor Vertical Size in pixels) */
#define RA8875_CURVS            	  0x4F//Font Write Cursor Vertical Size Register
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//              Block Transfer Engine(BTE) Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_BECR0            	  0x50//BTE Function Control Register 0
#define RA8875_BECR1            	  0x51//BTE Function Control Register 1
/* Layer Transparency Register 0     [0x52]
----- Bit 7,6 (Layer1/2 Scroll Mode)
00: Layer 1/2 scroll simultaneously
01: Only Layer 1 scroll
10: Only Layer 2 scroll
11: Buffer scroll (using Layer 2 as scroll buffer)
----- Bit 5 (Floating Windows Transparency Display With BGTR)
0:disable, 1:enable
----- Bit 4,3 (na)
----- Bit 2,1,0(Layer1/2 Display Mode) 
000: Only Layer 1 is visible
001: Only Layer 2 is visible
010: Lighten-overlay mode
011: Transparent mode
100: Boolean OR
101: Boolean AND
110: Floating window mode
111: Reserve */
#define RA8875_LTPR0            	  0x52//Layer Transparency Register 0
/* Layer Transparency Register 1     [0x53]
----- Bit 7,6,5,4 (Layer Transparency Setting for Layer 2)
0000: Total display
0001: 7/8 display
0010: 3/4 display
0011: 5/8 display
0100: 1/2 display
0101: 3/8 display
0110: 1/4 display
0111: 1/8 display
1000: Display disable
----- Bit 3,2,1,0 (Layer Transparency Setting for Layer 1)
0000: Total display
0001: 7/8 display
0010: 3/4 display
0011: 5/8 display
0100: 1/2 display
0101: 3/8 display
0110: 1/4 display
0111: 1/8 display
1000: Display disable */
#define RA8875_LTPR1            	  0x53//Layer Transparency Register 1
#define RA8875_HSBE0				  0x54//Horizontal Source Point 0 of BTE
#define RA8875_HSBE1				  0x55//Horizontal Source Point 1 of BTE
#define RA8875_VSBE0				  0x56//Vertical Source Point 0 of BTE
#define RA8875_VSBE1				  0x57//Vertical Source Point 1 of BTE
#define RA8875_HDBE0				  0x58//Horizontal Destination Point 0 of BTE
#define RA8875_HDBE1				  0x59//Horizontal Destination Point 1 of BTE
#define RA8875_VDBE0				  0x5A//Vertical Destination Point 0 of BTE
#define RA8875_VDBE1				  0x5B//Vertical Destination Point 1 of BTE
#define RA8875_BEWR0				  0x5C//BTE Width Register 0
#define RA8875_BEWR1				  0x5D//BTE Width Register 1
#define RA8875_BEHR0				  0x5E//BTE Height Register 0
#define RA8875_BEHR1				  0x5F//BTE Height Register 1
/* Pattern Set No for BTE            [0x65]
----- Bit 7 (Pattern Format)
0: 8x8
1: 16x16
----- Bit 6,5,4 (na)
----- Bit 3,2,1,0 (Pattern Set No)
If pattern Format = 8x8 then Pattern Set [3:0]
If pattern Format = 16x16 then Pattern Set [1:0] is valid */
#define RA8875_PTNO				  	  0x65//Pattern Set No for BTE
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Color Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_BGCR0				  0x60//Background Color Register 0 (R)
#define RA8875_BGCR1				  0x61//Background Color Register 1 (G)
#define RA8875_BGCR2				  0x62//Background Color Register 2 (B)
#define RA8875_FGCR0				  0x63//Foreground Color Register 0 (R)
#define RA8875_FGCR1				  0x64//Foreground Color Register 1 (G)
#define RA8875_FGCR2				  0x65//Foreground Color Register 2 (B)
#define RA8875_BGTR0				  0x67//Background Color Register for Transparent 0 (R)
#define RA8875_BGTR1				  0x68//Background Color Register for Transparent 1 (G)
#define RA8875_BGTR2				  0x69//Background Color Register for Transparent 2 (B)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            TOUCH SCREEN REGISTERS
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if !defined(USE_EXTERNALTOUCH)
#define RA8875_TPCR0                  0x70//Touch Panel Control Register 0
	//#define RA8875_TPCR0_ENABLE           0x80
	//#define RA8875_TPCR0_DISABLE          0x00
	#define RA8875_TPCR0_WAIT_512CLK      0x00
	#define RA8875_TPCR0_WAIT_1024CLK     0x10
	#define RA8875_TPCR0_WAIT_2048CLK     0x20
	#define RA8875_TPCR0_WAIT_4096CLK     0x30
	#define RA8875_TPCR0_WAIT_8192CLK     0x40
	#define RA8875_TPCR0_WAIT_16384CLK    0x50
	#define RA8875_TPCR0_WAIT_32768CLK    0x60
	#define RA8875_TPCR0_WAIT_65536CLK    0x70
	#define RA8875_TPCR0_WAKEENABLE       0x08
	#define RA8875_TPCR0_WAKEDISABLE      0x00
	#define RA8875_TPCR0_ADCCLK_DIV1      0x00
	#define RA8875_TPCR0_ADCCLK_DIV2      0x01
	#define RA8875_TPCR0_ADCCLK_DIV4      0x02
	#define RA8875_TPCR0_ADCCLK_DIV8      0x03
	#define RA8875_TPCR0_ADCCLK_DIV16     0x04
	#define RA8875_TPCR0_ADCCLK_DIV32     0x05
	#define RA8875_TPCR0_ADCCLK_DIV64     0x06
	#define RA8875_TPCR0_ADCCLK_DIV128    0x07

#define RA8875_TPCR1            	  0x71//Touch Panel Control Register 1
	#define RA8875_TPCR1_AUTO       0x00
	#define RA8875_TPCR1_MANUAL     0x40
	#define RA8875_TPCR1_VREFINT    0x00
	#define RA8875_TPCR1_VREFEXT    0x20
	#define RA8875_TPCR1_DEBOUNCE   0x04
	#define RA8875_TPCR1_NODEBOUNCE 0x00
	#define RA8875_TPCR1_IDLE       0x00
	#define RA8875_TPCR1_WAIT       0x01
	#define RA8875_TPCR1_LATCHX     0x02
	#define RA8875_TPCR1_LATCHY     0x03

#define RA8875_TPXH             	  0x72//Touch Panel X High Byte Data Register
#define RA8875_TPYH             	  0x73//Touch Panel Y High Byte Data Register
#define RA8875_TPXYL            	  0x74//Touch Panel X/Y Low Byte Data Register
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Graphic Cursor Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_GCHP0            	  0x80//Graphic Cursor Horizontal Position Register 0
#define RA8875_GCHP1            	  0x81//Graphic Cursor Horizontal Position Register 1
#define RA8875_GCVP0            	  0x82//Graphic Cursor Vertical Position Register 0
#define RA8875_GCVP1            	  0x83//Graphic Cursor Vertical Position Register 0
#define RA8875_GCC0            	      0x84//Graphic Cursor Color 0
#define RA8875_GCC1            	      0x85//Graphic Cursor Color 1
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            PLL Setting Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_PLLC1            	  0x88//PLL Control Register 1
#define RA8875_PLLC2            	  0x89//PLL Control Register 2

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            PWM Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_P1CR             	  0x8A//PWM1 Control Register
#define RA8875_P1DCR            	  0x8B//PWM1 Duty Cycle Register

#define RA8875_P2CR             	  0x8C//PWM2 Control Register
#define RA8875_P2DCR            	  0x8D//PWM2 Control Register

	#define RA8875_PxCR_ENABLE      0x80
	#define RA8875_PxCR_DISABLE     0x00
	#define RA8875_PxCR_CLKOUT      0x10
	#define RA8875_PxCR_PWMOUT      0x00



 	#define RA8875_PWM_CLK_DIV1     0x00
	#define RA8875_PWM_CLK_DIV2     0x01
	#define RA8875_PWM_CLK_DIV4     0x02
	#define RA8875_PWM_CLK_DIV8     0x03
	#define RA8875_PWM_CLK_DIV16    0x04
	#define RA8875_PWM_CLK_DIV32    0x05
	#define RA8875_PWM_CLK_DIV64    0x06
	#define RA8875_PWM_CLK_DIV128   0x07
	#define RA8875_PWM_CLK_DIV256   0x08
	#define RA8875_PWM_CLK_DIV512   0x09
	#define RA8875_PWM_CLK_DIV1024  0x0A
	#define RA8875_PWM_CLK_DIV2048  0x0B
	#define RA8875_PWM_CLK_DIV4096  0x0C
	#define RA8875_PWM_CLK_DIV8192  0x0D
	#define RA8875_PWM_CLK_DIV16384 0x0E
	#define RA8875_PWM_CLK_DIV32768 0x0F 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Memory Clear
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* Memory Clear Control Register     [0x8E]
----- Bit 7 (Memory Clear Function)
0: End or Stop (if read this bit and it's 0, clear completed)
1: Start the memory clear function
----- Bit 6 (Memory Clear Area Setting)
0: Clear the full window (ref. RA8875_HDWR,RA8875_VDHR0,RA8875_VDHR0ì1)
1: Clear the active window
----- Bit 5,4,3,2,1,0 (na)  */
#define RA8875_MCLR             	  0x8E//Memory Clear Control Register

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            Drawing Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_DCR                    0x90//Draw Line/Circle/Square Control Register
	#define RA8875_DCR_LINESQUTRI_START   0x80
	#define RA8875_DCR_LINESQUTRI_STOP    0x00
	#define RA8875_DCR_LINESQUTRI_STATUS  0x80
	#define RA8875_DCR_CIRCLE_START       0x40
	#define RA8875_DCR_CIRCLE_STATUS      0x40
	#define RA8875_DCR_CIRCLE_STOP        0x00
	#define RA8875_DCR_FILL               0x20
	#define RA8875_DCR_NOFILL             0x00
	#define RA8875_DCR_DRAWLINE           0x00
	#define RA8875_DCR_DRAWTRIANGLE       0x01
	#define RA8875_DCR_DRAWSQUARE         0x10

#define RA8875_DLHSR0         		  0x91//Draw Line/Square Horizontal Start Address Register0
#define RA8875_DLHSR1         		  0x92//Draw Line/Square Horizontal Start Address Register1
#define RA8875_DLVSR0         		  0x93//Draw Line/Square Vertical Start Address Register0
#define RA8875_DLVSR1         		  0x94//Draw Line/Square Vertical Start Address Register1
#define RA8875_DLHER0         		  0x95//Draw Line/Square Horizontal End Address Register0
#define RA8875_DLHER1         		  0x96//Draw Line/Square Horizontal End Address Register1
#define RA8875_DLVER0         		  0x97//Draw Line/Square Vertical End Address Register0
#define RA8875_DLVER1         		  0x98//Draw Line/Square Vertical End Address Register0

#define RA8875_DCHR0         		  0x99//Draw Circle Center Horizontal Address Register0
#define RA8875_DCHR1         		  0x9A//Draw Circle Center Horizontal Address Register1
#define RA8875_DCVR0         		  0x9B//Draw Circle Center Vertical Address Register0
#define RA8875_DCVR1         		  0x9C//Draw Circle Center Vertical Address Register1
#define RA8875_DCRR         		  0x9D//Draw Circle Radius Register

#define RA8875_ELLIPSE                0xA0//Draw Ellipse/Ellipse Curve/Circle Square Control Register
	#define RA8875_ELLIPSE_STATUS         0x80

#define RA8875_ELL_A0         		  0xA1//Draw Ellipse/Circle Square Long axis Setting Register0
#define RA8875_ELL_A1         		  0xA2//Draw Ellipse/Circle Square Long axis Setting Register1
#define RA8875_ELL_B0         		  0xA3//Draw Ellipse/Circle Square Short axis Setting Register0
#define RA8875_ELL_B1         		  0xA4//Draw Ellipse/Circle Square Short axis Setting Register1

#define RA8875_DEHR0         		  0xA5//Draw Ellipse/Circle Square Center Horizontal Address Register0
#define RA8875_DEHR1         		  0xA6//Draw Ellipse/Circle Square Center Horizontal Address Register1
#define RA8875_DEVR0         		  0xA7//Draw Ellipse/Circle Square Center Vertical Address Register0
#define RA8875_DEVR1         		  0xA8//Draw Ellipse/Circle Square Center Vertical Address Register1

#define RA8875_DTPH0         		  0xA9//Draw Triangle Point 2 Horizontal Address Register0
#define RA8875_DTPH1         		  0xAA//Draw Triangle Point 2 Horizontal Address Register1
#define RA8875_DTPV0         		  0xAB//Draw Triangle Point 2 Vertical Address Register0
#define RA8875_DTPV1         		  0xAC//Draw Triangle Point 2 Vertical Address Register1

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            DMA REGISTERS
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define RA8875_SSAR0				  0xB0//Source Starting Address REG 0
#define RA8875_SSAR1				  0xB1//Source Starting Address REG 1
#define RA8875_SSAR2				  0xB2//Source Starting Address REG 2
//#define RA8875_????					0xB3//???????????

#define RA8875_DTNR0				  0xB4//Block Width REG 0(BWR0) / DMA Transfer Number REG 0
#define RA8875_BWR1					  0xB5//Block Width REG 1
#define RA8875_DTNR1				  0xB6//Block Height REG 0(BHR0) /DMA Transfer Number REG 1
#define RA8875_BHR1					  0xB7//Block Height REG 1
#define RA8875_DTNR2				  0xB8//Source Picture Width REG 0(SPWR0) / DMA Transfer Number REG 2
#define RA8875_SPWR1				  0xB9//Source Picture Width REG 1
#define RA8875_DMACR				  0xBF//DMA Configuration REG


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                            GPIO
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_GPIOX            	  0xC7

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                         Interrupt Control Registers
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define RA8875_INTC1            	  0xF0//Interrupt Control Register1
#define RA8875_INTC2            	  0xF1//Interrupt Control Register2
	#define RA8875_INTCx_KEY        	  0x10
	#define RA8875_INTCx_DMA        	  0x08
	#define RA8875_INTCx_TP         	  0x04
	#define RA8875_INTCx_BTE        	  0x02

#endif