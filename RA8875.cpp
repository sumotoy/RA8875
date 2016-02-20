/*
Part of RA8875 library from https://github.com/sumotoy/RA8875
License:GNU General Public License v3.0

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

#include <SPI.h>
#include "RA8875.h"


#if defined (USE_FT5206_TOUCH)
	#include <Wire.h>
	#if defined(___DUESTUFF) && defined(USE_DUE_WIRE1_INTERFACE)
		#define Wire Wire1
	#endif
	const uint8_t _ctpAdrs = 0x38;
	const uint8_t coordRegStart[5] = {0x03,0x09,0x0F,0x15,0x1B};
	static volatile bool _FT5206_INT = false;
#endif

static volatile uint8_t _RA8875_INTS = 0b00000000;//container for INT states
/*------------------------------
Bit:	Called by:		In use:
--------------------------------
0: 		isr triggered	[*]
1: 		Resistive TS	[*]
2: 		KeyScan			[*]
3: 		DMA				 	
4: 		BTE				 
5: 		FT5206 TS		[*]
6: 		-na-			 
7: 		-na-			 
--------------------------------*/


/**************************************************************************/
/*!
	Contructors
	CSp: SPI SS pin
	RSTp: Reset pin
	INTp: INT pin
	//Teensy 3.0 , 3.1 , LC
	mosi_pin
	sclk_pin
	miso_pin
	Note:
	Teensy CS SPI1:[MOSI1(0) MISO1(1) CLK1(20) CS1(6)]
*/
/**************************************************************************/
//------------------------------TEENSY 3/3.1 ---------------------------------------
#if defined(__MK20DX128__) || defined(__MK20DX256__)	
	RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
	{
		_mosi = mosi_pin;
		_miso = miso_pin;
		_sclk = sclk_pin;
		_cs = CSp;
		_rst = RSTp;
//------------------------------Teensy LC-------------------------------------------
#elif defined(__MKL26Z64__)
	RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
	{
		_mosi = mosi_pin;
		_miso = miso_pin;
		_sclk = sclk_pin;
		_cs = CSp;
		_rst = RSTp;
		_altSPI = false;
//---------------------------------DUE--------------------------------------------
#elif defined(___DUESTUFF)//DUE
	RA8875::RA8875(const uint8_t CSp, const uint8_t RSTp) 
	{
		_cs = CSp;
		_rst = RSTp;
//------------------------------ENERGIA-------------------------------------------
#elif defined(NEEDS_SET_MODULE)
	RA8875::RA8875(const uint8_t module, const uint8_t RSTp) 
	{
		selectCS(module);
		_rst = RSTp;
		_cs = 255;
//----------------------------8 BIT ARDUINO's---------------------------------------
#else
	RA8875::RA8875(const uint8_t CSp, const uint8_t RSTp) 
	{
		_cs = CSp;
		_rst = RSTp;
#endif
}

/**************************************************************************/
/*!	
	Helper for Energia, it will set CS pin accordly module selected
	module: 0...3
	[private]
*/
/**************************************************************************/
#if defined(NEEDS_SET_MODULE)
void RA8875::selectCS(uint8_t module) 
{
	if (module > 3) module = 3;
	switch(module){
		case 0:
			_cs = PA_3;
		break;
		case 1:
			_cs = PF_3;
		break;
		case 2:
			_cs = PB_5;
		break;
		case 3:
			_cs = PD_1;
		break;
	}
	SPImodule = module;
}
#endif


/**************************************************************************/
/*!
	Initialize library and SPI
	Parameter:
	(display type)
    RA8875_480x272 (4.3" displays)
    RA8875_800x480 (5" and 7" displays)
	Adafruit_480x272 (4.3" Adafruit displays)
	Adafruit_800x480 (5" and 7" Adafruit displays)
	(colors) - The color depth (default 16) 8 or 16 (bit)
	-------------------------------------------------------------
	UPDATE! in Energia IDE some devices needs an extra parameter!
	module: sets the SPI interface (it depends from MCU). Default:0
*/
/**************************************************************************/
void RA8875::begin(const enum RA8875sizes s,uint8_t colors) 
{
	_errorCode = 0;
	_displaySize = s;
	_rotation = 0;
	_portrait = false;
	_inited = false;
	_sleep = false;
	_hasLayerLimits = false;
	_intPin = 255;
	_intNum = 0;
	_useISR = false;
	_enabledInterrups = 0b00000000;
	/* used to understand wat causes an INT
	bit
	0:
	1:
	2: Touch (resistive)
	3:
	4:
	5:
	6:
	7:
	8:
	*/
	#if defined(USE_FT5206_TOUCH)
		_intCTSPin = 255;
		_intCTSNum = 0;
	#endif
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTForeColor = _RA8875_DEFAULTTXTFRGRND;
		#if defined(_RA8875_DEFAULTTXTBKGRND)
			_TXTBackColor = _RA8875_DEFAULTTXTBKGRND;
		#else
			_TXTBackColor = 0x0000;
		#endif
		_TXTrecoverColor = false;
	#endif
	_maxLayers = 2;
	_currentLayer = 0;
	_useMultiLayers = false;//starts with one layer only
	_textMode = false;
	_brightness = 255;
	_cursorX = 0; _cursorY = 0; _scrollXL = 0; _scrollXR = 0; _scrollYT = 0; _scrollYB = 0;
	_scaleX = 1; _scaleY = 1;
	_scaling = false;
	_EXTFNTsize = X16;
	_FNTspacing = 0;
	//_FNTrender = false;
	/* set-->  _TXTparameters  <--
	0:_extFontRom = false;
	1:_autoAdvance = true;
	2:_textWrap = user defined
	3:_fontFullAlig = false;
	4:_fontRotation = false;//not used
	5:_alignXToCenter = false;
	6:_alignYToCenter = false;
	7: render         = false;
	*/
	_TXTparameters = 0b00000010;
	bitWrite(_TXTparameters,2,_DFT_RA8875_TEXTWRAP);//set _textWrap
	_relativeCenter = false;
	_absoluteCenter = false;
	_EXTFNTrom = _DFT_RA8875_EXTFONTROMTYPE;
	_EXTFNTcoding = _DFT_RA8875_EXTFONTROMCODING;
	//_FNTsource = INT;
	_FNTinterline = 0;
	_EXTFNTfamily = STANDARD;
	_FNTcursorType = NOCURSOR;
	_FNTgrandient = false;
	_arcAngle_max = ARC_ANGLE_MAX;
	_arcAngle_offset = ARC_ANGLE_OFFSET;
	_angle_offset = ANGLE_OFFSET;
	_color_bpp = 16;
	_colorIndex = 0;

	if (colors != 16) { _color_bpp = 8; _colorIndex = 3; }

	switch (_displaySize){
		case RA8875_480x272:
		case Adafruit_480x272:
			_width = 	480;
			_height = 	272;
			_initIndex = 0;
		break;
		case RA8875_800x480:
		case Adafruit_800x480:
		case RA8875_800x480ALT:
			_width = 	800;
			_height = 	480;
			_hasLayerLimits = true;
			_maxLayers = 1;
			if (_color_bpp < 16) _maxLayers = 2;
			_initIndex = 1;
			if (_displaySize == RA8875_800x480ALT) _initIndex = 2;
		break;
		default:
		_errorCode |= (1 << 0);//set
		return;
	}
	RA8875_WIDTH = _width;
	RA8875_HEIGHT = _height;
	_activeWindowXL = 0;
	_activeWindowXR = RA8875_WIDTH;
	_activeWindowYT = 0;
	_activeWindowYB = RA8875_HEIGHT;
	#if !defined(_AVOID_TOUCHSCREEN)//common to all touch
		_clearTInt = false;
		_touchEnabled = false;
		#if defined(USE_RA8875_TOUCH)//resistive touch
			_calibrated = _isCalibrated();//check calibration at startup
			if (!_calibrated) {
				_tsAdcMinX = 0; _tsAdcMinY = 0; _tsAdcMaxX = 1023; _tsAdcMaxY = 1023;
			} else {
				_tsAdcMinX = TOUCSRCAL_XLOW; _tsAdcMinY = TOUCSRCAL_YLOW; _tsAdcMaxX = TOUCSRCAL_XHIGH; _tsAdcMaxY = TOUCSRCAL_YHIGH;
			}
		#endif
	#endif

	#if defined(USE_RA8875_KEYMATRIX)
		_keyMatrixEnabled = false;
	#endif
/* Display Configuration Register	  [0x20]
	  7: (Layer Setting Control) 0:one Layer, 1:two Layers
	  6,5,4: (na)
	  3: (Horizontal Scan Direction) 0: SEG0 to SEG(n-1), 1: SEG(n-1) to SEG0
	  2: (Vertical Scan direction) 0: COM0 to COM(n-1), 1: COM(n-1) to COM0
	  1,0: (na) */
	_DPCR_Reg = 0b00000000;
/*	Memory Write Control Register 0 [0x40]
	7: 0(graphic mode), 1(textx mode)
	6: 0(font-memory cursor not visible), 1(visible)
	5: 0(normal), 1(blinking)
	4: na
	3-2: 00(LR,TB), 01(RL,TB), 10(TB,LR), 11(BT,LR)
	1: 0(Auto Increase in write), 1(no)
	0: 0(Auto Increase in read), 1(no) */
	_MWCR0_Reg = 0b00000000;
	
/*	Font Control Register 0 [0x21]
	7: 0(CGROM font is selected), 1(CGRAM font is selected)
	6: na
	5: 0(Internal CGROM [reg 0x2F to 00]), 1(External CGROM [0x2E reg, bit6,7 to 0)
	4-2: na
	1-0: 00(ISO/IEC 8859-1), 01(ISO/IEC 8859-2), 10(ISO/IEC 8859-3), 11(ISO/IEC 8859-4)*/
	
	_FNCR0_Reg = 0b00000000;
/*	Font Control Register 1 [0x22]
	7: 0(Full Alignment off), 1(Full Alignment on)
	6: 0(no-trasparent), 1(trasparent)
	5: na
	4: 0(normal), 1(90degrees)
	3-2: 00(x1), 01(x2), 10(x3), 11(x3) Horizontal font scale
	1-0: 00(x1), 01(x2), 10(x3), 11(x3) Vertical font scale */
	_FNCR1_Reg = 0b00000000;

	/*	Font Write Type Setting Register [0x2E]
	7-6: 00(16x16,8x16,nx16), 01(24x24,12x24,nx24), 1x(32x32,16x32, nx32)
	5-0: 00...3F (font width off to 63 pixels)*/
	_FWTSET_Reg = 0b00000000;
	
	/*	Serial Font ROM Setting [0x2F]
	GT Serial Font ROM Select
	7-5: 000(GT21L16TW/GT21H16T1W),001(GT30L16U2W),010(GT30L24T3Y/GT30H24T3Y),011(GT30L24M1Z),111(GT30L32S4W/GT30H32S4W)
	FONT ROM Coding Setting
	4-2: 000(GB2312),001(GB12345/GB18030),010(BIG5),011(UNICODE),100(ASCII),101(UNI-Japanese),110(JIS0208),111(Latin/Greek/Cyrillic/Arabic)
	1-0: 00...11 
		 bits	ASCII		Lat/Gr/Cyr		Arabic
		 00		normal		normal			na
		 01		Arial		var Wdth		Pres Forms A
		 10		Roman		na				Pres Forms B
		 11		Bold		na				na */
	_SFRSET_Reg = 0b00000000;
	
	/*	Interrupt Control Register1		  [0xF0]
	7,6,5: (na)
	4: KEYSCAN Interrupt Enable Bit
	3: DMA Interrupt Enable Bit
	2: TOUCH Panel Interrupt Enable Bit
	1: BTE Process Complete Interrupt Enable Bit
	0:
	When MCU-relative BTE operation is selected(*1) and BTE
	Function is Enabled(REG[50h] Bit7 = 1), this bit is used to
		Enable the BTE Interrupt for MCU R/W:
		0 : Disable BTE interrupt for MCU R/W.
		1 : Enable BTE interrupt for MCU R/W.
	When the BTE Function is Disabled, this bit is used to
		Enable the Interrupt of Font Write Function:
		0 : Disable font write interrupt.
		1 : Enable font write interrupt.
	*/
	_INTC1_Reg = 0b00000000;

	//------------------------------- Start SPI initialization ------------------------------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__)//Teensy 3,3.1
		//always uses SPI transaction
		if ((_mosi == 11 || _mosi == 7) && (_miso == 12 || _miso == 8) && (_sclk == 13 || _sclk == 14)) {//valid SPI pins?
			if (_mosi != 11) SPI.setMOSI(_mosi);
			if (_miso != 12) SPI.setMISO(_miso);
			if (_sclk != 13) SPI.setSCK(_sclk);
		} else {
			_errorCode |= (1 << 1);//set
			return;
		}
		if (!SPI.pinIsChipSelect(_cs)) {
			_errorCode |= (1 << 2);//set
			return;
		}
		pinMode(_cs, OUTPUT);
		SPI.begin();
		
	#elif defined(__MKL26Z64__)//TeensyLC
		//always uses SPI ransaction
		#if TEENSYDUINO > 121//not supported prior 1.22!
			if ((_mosi == 11 || _mosi == 7 || _mosi == 0 || _mosi == 21) && (_miso == 12 || _miso == 8 || _miso == 1 || _miso == 5) && (_sclk == 13 || _sclk == 14 || _sclk == 20)) {//valid SPI pins?
				if ((_mosi == 0 || _mosi == 21) && (_miso == 1 || _miso == 5) && (_sclk == 20)) {//identify alternate SPI channel 1 (24Mhz)
					_altSPI = true;
					if (_cs != 6){//on SPI1 cs should be only 6!
						_errorCode |= (1 << 2);//set
						return;
					}
					if (_mosi != 11) SPI1.setMOSI(_mosi);
					if (_miso != 12) SPI1.setMISO(_miso);
					if (_sclk != 13) SPI1.setSCK(_sclk);
					pinMode(_cs, OUTPUT);
					SPI1.begin();
				} else {//default SPI channel 0 (12Mhz)
					_altSPI = false;
					if (_mosi != 11) SPI.setMOSI(_mosi);
					if (_miso != 12) SPI.setMISO(_miso);
					if (_sclk != 13) SPI.setSCK(_sclk);
					if (!SPI.pinIsChipSelect(_cs)) {//ERROR
						_errorCode |= (1 << 2);//set
						return;
					}
					pinMode(_cs, OUTPUT);
					SPI.begin();
				}
			} else {
				_errorCode |= (1 << 1);//set
				return;
			}
		#else
			_altSPI = false;
			pinMode(_cs, OUTPUT);
			SPI.begin();
			_errorCode |= (1 << 3);//set
		#endif
	#endif//end Teensy LC
	
	#if !defined(ENERGIA)//everithing but ENERGIA
		#if defined(___TEENSYES)//all of them (32 bit only)
			//pinMode(_cs, OUTPUT);
			digitalWrite(_cs, HIGH);
		#elif defined(___DUESTUFF)// DUE
			#if defined(SPI_DUE_MODE_EXTENDED)
				//DUE SPI mode extended you can use only follow pins
				if (_cs == 4 || _cs == 10 || _cs == 52) {
					SPI.begin(_cs);
				} else {
					_errorCode |= (1 << 2);//error! wrong cs pin
					return;
				}
			#else
				//DUE in normal SPI mode
				pinMode(_cs, OUTPUT);
				SPI.begin();
				#if defined(_FASTSSPORT)
					csport = portOutputRegister(digitalPinToPort(_cs));
					cspinmask = digitalPinToBitMask(_cs);
					*csport |= cspinmask;//hi
				#else
					digitalWrite(_cs, HIGH);
				#endif
			#endif
		#elif defined(ESP8266)
			pinMode(_cs, OUTPUT);
			SPI.begin();
			#if defined(_FASTSSPORT)
				GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, _pinRegister(_cs));//H
			#else
				digitalWrite(_cs, HIGH);//for now
			#endif
		#else
			//UNO,MEGA,Yun,nano,duemilanove and other 8 bit arduino's
			pinMode(_cs, OUTPUT);
			SPI.begin();
			csport = portOutputRegister(digitalPinToPort(_cs));//pinMode(_cs, OUTPUT);
			cspinmask = digitalPinToBitMask(_cs);
			*csport |= cspinmask;//hi
		#endif
	#endif
	if (_rst < 255){//time for hardware reset RA8875
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(10);
		digitalWrite(_rst, LOW);
		delay(220);//120
		digitalWrite(_rst, HIGH);
		delay(300);//200
	}
	
	#if defined(ENERGIA) && defined(NEEDS_SET_MODULE)//energia specific
		SPI.setModule(SPImodule);
	#endif

	//set SPI SPEED, starting at low speed, after init will raise up!
	#if defined(SPI_HAS_TRANSACTION)
		_SPImaxSpeed = 4000000UL;//we start in low speed here!
	#else//do not use SPItransactons
		#if defined (__AVR__)//8 bit arduino's
			pinMode(_cs, OUTPUT);
			SPI.begin();
			SPI.setClockDivider(SPI_SPEED_SAFE);
			delay(1);
			SPI.setDataMode(SPI_MODE3);
		#else
			#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_SAFE);
				delay(1);
				SPI.setDataMode(_cs,SPI_MODE3);
			#else
				SPI.setClockDivider(SPI_SPEED_SAFE);
				delay(1);
				SPI.setDataMode(SPI_MODE3);
			#endif
		#endif
	#endif
	#if defined(ENERGIA)//dunno why but energia wants this here or not work!
		pinMode(_cs, OUTPUT);
		digitalWrite(_cs, HIGH);
	#endif
	//SPI initialization done
	if (_errorCode != 0) return;//ouch, error/s.Better stop here!
	_initialize();//----->Time to Initialize the RA8875!
	//------- time for capacitive touch stuff -----------------
	#if defined(USE_FT5206_TOUCH)
		Wire.begin();
		#if defined(___DUESTUFF)
			Wire.setClock(400000UL); // Set I2C frequency to 400kHz
			/*
			#if !defined(USE_DUE_WIRE1_INTERFACE)//sorry but I do not own a DUE, have to learn about Wire1
			// Force 400 KHz I2C, rawr! (Uses pins 20, 21 for SDA, SCL)
			TWI1->TWI_CWGR = 0;
			TWI1->TWI_CWGR = ((VARIANT_MCK / (2 * 400000)) - 4) * 0x101;
			#endif
			*/
		#else
			#if ARDUINO >= 157
				Wire.setClock(400000UL); // Set I2C frequency to 400kHz
			#else
				TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
			#endif
		#endif
		delay(10);
		_initializeFT5206();//initialize FT5206 controller
		_maxTouch = 5;
		_gesture = 0;
		_currentTouches = 0;
		_currentTouchState = 0;
		_needISRrearm = false;
		//TO DO
		//Modify RA8875 registers and disconnect internal Touch Screen totally
	#endif
}


/************************* Initialization *********************************/

/**************************************************************************/
/*!
      Hardware initialization of RA8875 and turn on
	  [private]
*/
/**************************************************************************/
void RA8875::_initialize() 
{
	_inited = false;
	if (_rst == 255) {// soft reset time ?
		writeCommand(RA8875_PWRR);
		_writeData(RA8875_PWRR_SOFTRESET);
		delay(20);
		_writeData(RA8875_PWRR_NORMAL);
		delay(200);
	}
	//set the sysClock
	_setSysClock(initStrings[_initIndex][0],initStrings[_initIndex][1],initStrings[_initIndex][2]);
	
	//color space setup
	if (_color_bpp < 16){//256
		_writeRegister(RA8875_SYSR,0x00);//256
		_colorIndex = 3;
	} else {
		_writeRegister(RA8875_SYSR,0x0C);//65K
		_colorIndex = 0;
	}
	
	_writeRegister(RA8875_HDWR,initStrings[_initIndex][3]);		//LCD Horizontal Display Width Register
	_writeRegister(RA8875_HNDFTR,initStrings[_initIndex][4]);	//Horizontal Non-Display Period Fine Tuning Option Register
	_writeRegister(RA8875_HNDR,initStrings[_initIndex][5]);		//LCD Horizontal Non-Display Period Register
	_writeRegister(RA8875_HSTR,initStrings[_initIndex][6]);		//HSYNC Start Position Register
	_writeRegister(RA8875_HPWR,initStrings[_initIndex][7]);		//HSYNC Pulse Width Register
	_writeRegister(RA8875_VDHR0,initStrings[_initIndex][8]);	//LCD Vertical Display Height Register0
	_writeRegister(RA8875_VDHR0+1,initStrings[_initIndex][9]);	//LCD Vertical Display Height Register1
	_writeRegister(RA8875_VNDR0,initStrings[_initIndex][10]);	//LCD Vertical Non-Display Period Register 0
	_writeRegister(RA8875_VNDR0+1,initStrings[_initIndex][11]);	//LCD Vertical Non-Display Period Register 1
	_writeRegister(RA8875_VSTR0,initStrings[_initIndex][12]);	//VSYNC Start Position Register 0
	_writeRegister(RA8875_VSTR0+1,initStrings[_initIndex][13]);	//VSYNC Start Position Register 1
	_writeRegister(RA8875_VPWR,initStrings[_initIndex][14]);	//VSYNC Pulse Width Register
	_updateActiveWindow(true);									//set the whole screen as active
	//clearActiveWindow();
	delay(10); //100
	setCursorBlinkRate(DEFAULTCURSORBLINKRATE);//set default blink rate
	setIntFontCoding(DEFAULTINTENCODING);//set default internal font encoding
	setFont(INT);	//set internal font use
	//postburner PLL!
	_setSysClock(sysClockPar[_initIndex][0],sysClockPar[_initIndex][1],initStrings[_initIndex][2]);
	_inited = true;//from here we will go at high speed!
	#if defined(_FASTCPU)
		_slowDownSPI(false);
	#else
		#if defined(SPI_HAS_TRANSACTION)
			#if defined(__MKL26Z64__)
				if (_altSPI){
					_SPImaxSpeed = MAXSPISPEED2;
				} else {
					_SPImaxSpeed = MAXSPISPEED;
				}
			#else			
				_SPImaxSpeed = MAXSPISPEED;
			#endif
		#else
			#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_WRITE);
			#else
				SPI.setClockDivider(SPI_SPEED_WRITE);
			#endif
		#endif
	#endif
	delay(1);
	clearMemory();//clearMemory(true);
	delay(1);
	displayOn(true);//turn On Display
	delay(1);
	fillWindow(_RA8875_DEFAULTBACKLIGHT);//set screen black
	backlight(true);
	setRotation(_RA8875_DEFAULTSCRROT);
	_setTextMode(false);
	setActiveWindow();
	#if defined(_RA8875_DEFAULTTXTBKGRND)
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			setForegroundColor(_TXTForeColor);
			setBackgroundColor(_TXTBackColor);
		#else
			setForegroundColor(_RA8875_DEFAULTTXTFRGRND);
			setBackgroundColor(_RA8875_DEFAULTTXTBKGRND);
		#endif
		_backTransparent = false;
		_FNCR1_Reg &= ~(1 << 6);//clear
	#else
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			setForegroundColor(_TXTForeColor);
		#else
			setForegroundColor(_RA8875_DEFAULTTXTFRGRND);
		#endif
		_backTransparent = true;
		_FNCR1_Reg |= (1 << 6);//set
	#endif
	_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	
	setCursor(0,0);
}

/**************************************************************************/
/*!
      This function set the sysClock accordly datasheet
	  Parameters:
	  pll1: PLL Control Register 1
	  pll2: PLL Control Register 2
	  pixclk: Pixel Clock Setting Register
	  [private]
*/
/**************************************************************************/
void RA8875::_setSysClock(uint8_t pll1,uint8_t pll2,uint8_t pixclk)
{
	_writeRegister(RA8875_PLLC1,pll1);////PLL Control Register 1
	delay(1);
	_writeRegister(RA8875_PLLC1+1,pll2);////PLL Control Register 2
	delay(1);
	_writeRegister(RA8875_PCSR,pixclk);//Pixel Clock Setting Register
	delay(1);
}

/**************************************************************************/
/*!
      This return a byte with the error code/s:
	  bit--------------------------------------------------------------------
	  0:	The display it's not supported!
	  1:	The MOSI or MISO or SCLK choosed for Teensy it's out permitted range!
	  2:	The CS pin you selected it's out permitted range!
	  3:	You have to upgrade to Teensyduino 1.22 or better to use this feature!
	  4:
	  5:
	  6:
	  7:
	 0b00000000 = no error
*/
/**************************************************************************/
uint8_t RA8875::errorCode(void) 
{
	return _errorCode;
}

/**************************************************************************/
/*!
	return true when register has done the job, otherwise false.
*/
/**************************************************************************/
boolean RA8875::_waitPoll(uint8_t regname, uint8_t waitflag) 
{
	uint8_t temp;
	unsigned long timeout = millis();
	
	while (1) {
		temp = _readRegister(regname);
		if (!(temp & waitflag)) return true;
		if ((millis() - timeout) > 20) return false;//emergency exit!
	}  
	return false;
}

/**************************************************************************/
/*!
	Just another specified wait routine until job it's done
	Parameters:
	res:
	0x80(for most operations),
	0x40(BTE wait), 
	0x01(DMA wait)
*/
/**************************************************************************/
void RA8875::_waitBusy(uint8_t res) 
{
	uint8_t temp; 	
	unsigned long start = millis();//M.Sandercock
	do {
		if (res == 0x01) writeCommand(RA8875_DMACR);//dma
		temp = readStatus();
		if ((millis() - start) > 10) return;
	} while ((temp & res) == res);
}

/**************************************************************************/
/*!		
		Clear memory (different from fillWindow!)
	    Parameters:
		full: true(clear all memory), false(clear active window only)
		When in multilayer it automatically clear L1 & L1 and switch back to current layer
*/
/**************************************************************************/
/*
void RA8875::clearMemory(boolean full)
{
	uint8_t temp = 0b10000000;
	if (!full && !_useMultiLayers) temp |= (1 << 6);//set 6
	_writeRegister(RA8875_MCLR,temp);
	_waitBusy(0x80);
	if (full && _useMultiLayers){
		temp = 0b10000000;
		uint8_t templ = _currentLayer;//remember current layer
		if (templ > 0){//we are in L2
			writeTo(L1);//switch to L1
		} else {
			writeTo(L2);//switch to L2
		}
		_writeRegister(RA8875_MCLR,temp);
		_waitBusy(0x80);
		if (templ > 0){//we was in L2
			writeTo(L2);//switch back to L2
		} else {
			writeTo(L1);//switch back to L1
		}
	}
}
*/

/**************************************************************************/
/*!		
		Clear memory (different from fillWindow!)
	    Parameters:
		stop: stop clear memory operation
*/
/**************************************************************************/
void RA8875::clearMemory(bool stop)
{
	uint8_t temp;
	temp = _readRegister(RA8875_MCLR);
	stop == true ? temp &= ~(1 << 7) : temp |= (1 << 7);
	_writeData(temp); 
	if (!stop) _waitBusy(0x80);
}

/**************************************************************************/
/*!		
		Clear the active window
	    Parameters:
		full: false(clear current window), true clear full window
*/
/**************************************************************************/
void RA8875::clearActiveWindow(bool full)
{
	uint8_t temp;
	temp = _readRegister(RA8875_MCLR);
	full == true ? temp &= ~(1 << 6) : temp |= (1 << 6);
	_writeData(temp);  
	//_waitBusy(0x80);
}

/**************************************************************************/
/*!		
		Clear width BG color
	    Parameters:
		bte: false(clear width BTE BG color), true(clear width font BG color)
*/
/**************************************************************************/
void RA8875::clearWidthColor(bool bte)
{
	uint8_t temp;
	temp = _readRegister(RA8875_MCLR);
	bte == true ? temp &= ~(1 << 0) : temp |= (1 << 0);
	_writeData(temp);  
	//_waitBusy(0x80);
}

/**************************************************************************/
/*!
      turn display on/off
*/
/**************************************************************************/
void RA8875::displayOn(boolean on) 
{
	on == true ? _writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON) : _writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

/**************************************************************************/
/*!		
		Set the Active Window
	    Parameters:
		XL: Horizontal Left
		XR: Horizontal Right
		YT: Vertical TOP
		YB: Vertical Bottom
*/
/**************************************************************************/
void RA8875::setActiveWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB)
{
	if (_portrait){ swapvals(XL,YT); swapvals(XR,YB);}
	
	if (XR >= RA8875_WIDTH) XR = RA8875_WIDTH;
	if (YB >= RA8875_HEIGHT) YB = RA8875_HEIGHT;
	
	_activeWindowXL = XL; _activeWindowXR = XR;
	_activeWindowYT = YT; _activeWindowYB = YB;
	_updateActiveWindow(false);
}

/**************************************************************************/
/*!		
		Set the Active Window as FULL SCREEN
*/
/**************************************************************************/
void RA8875::setActiveWindow(void)
{
	_activeWindowXL = 0; _activeWindowXR = RA8875_WIDTH;
	_activeWindowYT = 0; _activeWindowYB = RA8875_HEIGHT;
	if (_portrait){swapvals(_activeWindowXL,_activeWindowYT); swapvals(_activeWindowXR,_activeWindowYB);}
	_updateActiveWindow(true);
}

/**************************************************************************/
/*!		
		Set the Active Window
	    Parameters:
		XL: Horizontal Left
		XR: Horizontal Right
		YT: Vertical TOP
		YB: Vertical Bottom
*/
/**************************************************************************/
void RA8875::getActiveWindow(int16_t &XL,int16_t &XR ,int16_t &YT ,int16_t &YB)//0.69b24
{
	XL = _activeWindowXL; XR = _activeWindowXR;
	YT = _activeWindowYT; YB = _activeWindowYB;
}

/**************************************************************************/
/*!		
		Return the max tft width.
		Parameters: 
		absolute: if true will return the phisical width
*/
/**************************************************************************/
uint16_t RA8875::width(bool absolute) const 
{ 
	if (absolute) return RA8875_WIDTH;
	return _width; 
}

/**************************************************************************/
/*!		
		Return the max tft height.
		Parameters: 
		absolute: if true will return the phisical height
*/
/**************************************************************************/
uint16_t RA8875::height(bool absolute) const 
{ 
	if (absolute) return RA8875_HEIGHT;
	return _height; 
}


/**************************************************************************/
/*!  
		Change the mode between graphic and text
		Parameters:
		m: can be GRAPHIC or TEXT
		[private]
*/
/**************************************************************************/
void RA8875::_setTextMode(bool m) 
{
	if (m == _textMode) return;
	writeCommand(RA8875_MWCR0);
	//if (m != 0){//text
	if (m){//text
		_MWCR0_Reg |= (1 << 7);
		_textMode = true;
	} else {//graph
		_MWCR0_Reg &= ~(1 << 7);
		_textMode = false;
	}
	_writeData(_MWCR0_Reg);
}

/**************************************************************************/
/*!
      Change the beam scan direction on display
	  Parameters:
	  invertH: true(inverted),false(normal) horizontal
	  invertV: true(inverted),false(normal) vertical
*/
/**************************************************************************/
void RA8875::_scanDirection(boolean invertH,boolean invertV)
{
	invertH == true ? _DPCR_Reg |= (1 << 3) : _DPCR_Reg &= ~(1 << 3);
	invertV == true ? _DPCR_Reg |= (1 << 2) : _DPCR_Reg &= ~(1 << 2);
	_writeRegister(RA8875_DPCR,_DPCR_Reg);
}

/**************************************************************************/
/*!
      Change the rotation of the screen
	  Parameters:
	  rotation:
	    0 = default
		1 = 90
		2 = 180
		3 = 270
*/
/**************************************************************************/
void RA8875::setRotation(uint8_t rotation)//0.69b32 - less code
{
	_rotation = rotation % 4; //limit to the range 0-3
	switch (_rotation) {
	case 0:
		//default, connector to bottom
		_portrait = false;
		_scanDirection(0,0);
		#if defined(USE_RA8875_TOUCH)
			if (!_calibrated) {
				_tsAdcMinX = 0;  _tsAdcMinY = 0; _tsAdcMaxX = 1023;  _tsAdcMaxY = 1023;
			} else {
				_tsAdcMinX = TOUCSRCAL_XLOW; _tsAdcMinY = TOUCSRCAL_YLOW; _tsAdcMaxX = TOUCSRCAL_XHIGH; _tsAdcMaxY = TOUCSRCAL_YHIGH;
			}
		#endif
    break;
	case 1:
		//90
		_portrait = true;
		_scanDirection(1,0);
		#if defined(USE_RA8875_TOUCH)
			if (!_calibrated) {
				_tsAdcMinX = 1023; _tsAdcMinY = 0; _tsAdcMaxX = 0; _tsAdcMaxY = 1023;
			} else {
				_tsAdcMinX = TOUCSRCAL_XHIGH; _tsAdcMinY = TOUCSRCAL_YLOW; _tsAdcMaxX = TOUCSRCAL_XLOW; _tsAdcMaxY = TOUCSRCAL_YHIGH;
			}
		#endif
    break;
	case 2:
		//180
		_portrait = false;
		_scanDirection(1,1);
		#if defined(USE_RA8875_TOUCH)
			if (!_calibrated) {
				_tsAdcMinX = 1023; _tsAdcMinY = 1023; _tsAdcMaxX = 0; _tsAdcMaxY = 0;
			} else {
				_tsAdcMinX = TOUCSRCAL_XHIGH; _tsAdcMinY = TOUCSRCAL_YHIGH; _tsAdcMaxX = TOUCSRCAL_XLOW; _tsAdcMaxY = TOUCSRCAL_YLOW;
			}
		#endif
    break;
	case 3:
		//270
		_portrait = true;
		_scanDirection(0,1);
		#if defined(USE_RA8875_TOUCH)
			if (!_calibrated) {
				_tsAdcMinX = 0; _tsAdcMinY = 1023; _tsAdcMaxX = 1023; _tsAdcMaxY = 0;
			} else {
				_tsAdcMinX = TOUCSRCAL_XLOW; _tsAdcMinY = TOUCSRCAL_YHIGH; _tsAdcMaxX = TOUCSRCAL_XHIGH; _tsAdcMaxY = TOUCSRCAL_YLOW;
			}
		#endif
    break;
	}
	if (_portrait){
		_width = RA8875_HEIGHT;
		_height = RA8875_WIDTH;
		_FNCR1_Reg |= (1 << 4);
	} else {
		_width = RA8875_WIDTH;
		_height = RA8875_HEIGHT;
		_FNCR1_Reg &= ~(1 << 4);
	}
	_writeRegister(RA8875_FNCR1,_FNCR1_Reg);//0.69b21
	setActiveWindow();
}

/**************************************************************************/
/*!
      Get rotation setting
*/
/**************************************************************************/
uint8_t RA8875::getRotation()
{
	return _rotation;
}

/**************************************************************************/
/*!
      true if rotation 1 or 3
*/
/**************************************************************************/
boolean RA8875::isPortrait(void)
{
	return _portrait;
}

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								TEXT STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/**************************************************************************/
/*!		Upload user custom char or symbol to CGRAM, max 255
		Parameters:
		symbol[]: an 8bit x 16 char in an array. Must be exact 16 bytes
		address: 0...255 the address of the CGRAM where to store the char
*/
/**************************************************************************/
void RA8875::uploadUserChar(const uint8_t symbol[],uint8_t address) 
{
	uint8_t tempMWCR1 = _readRegister(RA8875_MWCR1);//thanks MorganSandercock
	uint8_t i;
	if (_textMode) _setTextMode(false);//we are in text mode?
	_writeRegister(RA8875_CGSR,address);
	writeTo(CGRAM);
	writeCommand(RA8875_MRWC);
	for (i=0;i<16;i++){
		_writeData(symbol[i]);
	}
	_writeRegister(RA8875_MWCR1, tempMWCR1);//restore register
}

/**************************************************************************/
/*!		Retrieve and print to screen the user custom char or symbol
		User have to store a custom char before use this function
		Parameters:
		address: 0...255 the address of the CGRAM where char it's stored
		wide:0 for single 8x16 char, if you have wider chars that use 
		more than a char slot they can be showed combined (see examples)
*/
/**************************************************************************/
void RA8875::showUserChar(uint8_t symbolAddrs,uint8_t wide) 
{
	if (!_textMode) _setTextMode(true);//we are in graph mode?
	uint8_t oldReg1State = _FNCR0_Reg;
	uint8_t oldReg2State = 0;
	uint8_t i;
	oldReg1State |= (1 << 7);//set to CGRAM
	oldReg1State |= (1 << 5);//TODO:check this (page 19)
	_writeRegister(RA8875_FNCR0,oldReg1State);
	if (_scaling){//reset scale (not compatible with this!)
		oldReg2State = _FNCR1_Reg;
		oldReg2State &= ~(0xF); // clear bits from 0 to 3
		_writeRegister(RA8875_FNCR1,oldReg2State);
	}
	//layers?

 	if (_useMultiLayers){
		if (_currentLayer == 0){
			writeTo(L1);
		} else {
			writeTo(L2);
		}
	} else {
		//writeTo(L1);
	}

	writeCommand(RA8875_MRWC);
	_writeData(symbolAddrs);
	if (wide > 0){
		for (i=1;i<=wide;i++){
			_writeData(symbolAddrs+i);
		}
	}
	if (oldReg2State != 0) _writeRegister(RA8875_FNCR1,_FNCR1_Reg);//put back scale as it was
	if (oldReg1State != _FNCR0_Reg) _writeRegister(RA8875_FNCR0,_FNCR0_Reg);//put back state
}

/**************************************************************************/
/*!   
		Set internal Font Encoding
		Parameters:
		f: ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4
		default: ISO_IEC_8859_1
*/
/**************************************************************************/
void RA8875::setIntFontCoding(enum RA8875fontCoding f) 
{
	uint8_t temp = _FNCR0_Reg;
	temp &= ~((1<<1) | (1<<0));// Clear bits 1 and 0
	switch (f){
		case ISO_IEC_8859_1:
			 //do nothing
		break;
		case ISO_IEC_8859_2:
			temp |= (1 << 0);
		break;
		case ISO_IEC_8859_3:
			temp |= (1 << 1);
		break;
		case ISO_IEC_8859_4:
			temp |= ((1<<1) | (1<<0));// Set bits 1 and 0
		break;
		default:
		return;
	}
	_FNCR0_Reg = temp;
	_writeRegister(RA8875_FNCR0,_FNCR0_Reg);
}

/**************************************************************************/
/*!  
		External Font Rom setup
		This will not phisically change the register but should be called before setFont(EXT)!
		You should use this values accordly Font ROM datasheet!
		Parameters:
		ert: ROM Type          (GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, GT23L32S4W, GT30H32S4W)
		erc: ROM Font Encoding (GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN)
		erf: ROM Font Family   (STANDARD, ARIAL, ROMAN, BOLD)
*/
/**************************************************************************/
void RA8875::setExternalFontRom(enum RA8875extRomType ert, enum RA8875extRomCoding erc, enum RA8875extRomFamily erf)
{
	if (!_textMode) _setTextMode(true);
	_SFRSET_Reg = _readRegister(RA8875_FNCR0);;//just to preserve the reg in case something wrong
	uint8_t temp = 0b00000000;
	switch(ert){ //type of rom
		case GT21L16T1W:
		case GT21H16T1W:
			temp &= 0x1F;
		break;
		case GT23L16U2W:
		case GT30L16U2W:
		case ER3301_1:
			temp &= 0x1F; temp |= 0x20;
		break;
		case GT23L24T3Y:
		case GT30H24T3Y:
		case ER3303_1://encoding GB12345
			temp &= 0x1F; temp |= 0x40;
		break;
		case GT23L24M1Z:
			temp &= 0x1F; temp |= 0x60;
		break;
		case GT23L32S4W:
		case GT30H32S4W:
		case GT30L32S4W:
		case ER3304_1://encoding GB2312
			temp &= 0x1F; temp |= 0x80;
		break;
		default:
			_TXTparameters &= ~(1 << 0);//wrong type, better avoid for future
			return;//cannot continue, exit
		}
		_EXTFNTrom = ert;
	switch(erc){	//check rom font coding
		case GB2312:
			temp &= 0xE3;
		break;
		case GB12345:
			temp &= 0xE3; temp |= 0x04;
		break;
		case BIG5:
			temp &= 0xE3; temp |= 0x08;
		break;
		case UNICODE:
			temp &= 0xE3; temp |= 0x0C;
		break;
		case ASCII:
			temp &= 0xE3; temp |= 0x10;
		break;
		case UNIJIS:
			temp &= 0xE3; temp |= 0x14;
		break;
		case JIS0208:
			temp &= 0xE3; temp |= 0x18;
		break;
		case LATIN:
			temp &= 0xE3; temp |= 0x1C;
		break;
		default:
			_TXTparameters &= ~(1 << 0);//wrong coding, better avoid for future
			return;//cannot continue, exit
		}
		_EXTFNTcoding = erc;
		_SFRSET_Reg = temp;
		setExtFontFamily(erf,false);	
		_TXTparameters |= (1 << 0); //bit set 0
		//_writeRegister(RA8875_SFRSET,_SFRSET_Reg);//0x2F
		//delay(4);
}

void RA8875::fontRomSpeed(uint8_t sp)
{
	_writeRegister(0x28,sp);
}

/**************************************************************************/
/*!  
		select the font family for the external Font Rom Chip
		Parameters:
		erf: STANDARD, ARIAL, ROMAN, BOLD
		setReg: 
		true(send phisically the register, useful when you change
		family after set setExternalFontRom)
		false:(change only the register container, useful during config)
		NOTE: works only when external font rom it's active
*/
/**************************************************************************/
void RA8875::setExtFontFamily(enum RA8875extRomFamily erf,boolean setReg) 
{
	if (_FNTsource == EXT) {//only on EXT ROM fonts!
		_EXTFNTfamily = erf;
		_SFRSET_Reg &= ~(0x03); // clear bits from 0 to 1
		switch(erf){	//check rom font family
			case STANDARD:
				_SFRSET_Reg &= 0xFC;
			break;
			case ARIAL:
				_SFRSET_Reg &= 0xFC; _SFRSET_Reg |= 0x01;
			break;
			case ROMAN:
				_SFRSET_Reg &= 0xFC; _SFRSET_Reg |= 0x02;
			break;
			case BOLD:
				_SFRSET_Reg |= ((1<<1) | (1<<0)); // set bits 1 and 0
			break;
			default:
				_EXTFNTfamily = STANDARD; _SFRSET_Reg &= 0xFC;
			return;
		}
		if (setReg) _writeRegister(RA8875_SFRSET,_SFRSET_Reg);
	}
}


/**************************************************************************/
/*!  
		choose from internal/external (if exist) Font Rom
		Parameters:
		s: Font source (INT,EXT)
*/
/**************************************************************************/
void RA8875::setFont(enum RA8875fontSource s) 
{
	if (!_textMode) _setTextMode(true);//we are in graph mode?
	_TXTparameters &= ~(1 << 7);//render OFF
	if (s == INT){
		_setFNTdimensions(0);
		//check the font coding
		if (bitRead(_TXTparameters,0) == 1) {//0.96b22 _extFontRom = true
			setFontSize(X16);
			_writeRegister(RA8875_SFRSET,0b00000000);//_SFRSET_Reg
		}
		_FNCR0_Reg &= ~((1<<7) | (1<<5));// Clear bits 7 and 5 (set internal CGROM)
		_writeRegister(RA8875_FNCR0,_FNCR0_Reg);
		_FNTsource = s;
		delay(1);
	} else if (s == EXT){
		if (bitRead(_TXTparameters,0) == 1) {//0.96b22 _extFontRom = true
			_FNTsource = s;
			//now switch
			_FNCR0_Reg &= ~(1 << 7);//clearBit 7
			_FNCR0_Reg |= (1 << 5);//setBit 5
			_writeRegister(RA8875_FNCR0,_FNCR0_Reg);//0x21
			delay(1);
			_writeRegister(RA8875_SFCLR,0x03);//0x02 Serial Flash/ROM CLK frequency/2
			setFontSize(X16);
			_writeRegister(RA8875_SFRSET,_SFRSET_Reg);//at this point should be already set
			delay(4);
			_writeRegister(RA8875_SROC,0x28);// 0x28 rom 0,24bit adrs,wave 3,1 byte dummy,font mode, single mode 00101000
			delay(4);
		} else {
			setFont(INT);
			_setFNTdimensions(0);
		}
	} else {
		return;
	}
	_spaceCharWidth = _FNTwidth;
	//setFontScale(0);
	_scaleX = 1; _scaleY = 1;//reset font scale
}


void RA8875::_setFNTdimensions(uint8_t index) 
{
	_FNTwidth 		= 	fontDimPar[index][0];
	_FNTheight 		= 	fontDimPar[index][1];
	_FNTbaselineLow  = 	fontDimPar[index][2];
	_FNTbaselineTop  = 	fontDimPar[index][3];
}

/**************************************************************************/
/*!  
		choose an external font that will be rendered
		Of course slower that internal fonts!
		Parameters:
		*font: &myfont
*/
/**************************************************************************/
void RA8875::setFont(const tFont *font) 
{
	_currentFont = font;
	_FNTheight = 		_currentFont->font_height;
	_FNTwidth = 		_currentFont->font_width;//if 0 it's variable width font
	_FNTcompression = 	_currentFont->rle;
	//get all needed infos
	if (_FNTwidth > 0){
		_spaceCharWidth = _FNTwidth;
	} else {
		//_FNTwidth will be 0 to inform other functions that this it's a variable w font
		// We just get the space width now...
		int temp = _getCharCode(0x20);
		if (temp > -1){
		#if defined(_FORCE_PROGMEM__)
			#if defined(ESP8266)
			_spaceCharWidth = FPSTR(&_currentFont->chars[temp].image->image_width);
			#else
			_spaceCharWidth = PROGMEM_read(&_currentFont->chars[temp].image->image_width);
			#endif
		#else
			_spaceCharWidth = (_currentFont->chars[temp].image->image_width);
		#endif
		} else {
			//font malformed, doesn't have needed space parameter
			//will return to system font
			setFont(INT);
			return;
		}
	}
	_scaleX = 1; _scaleY = 1;//reset font scale
	//setFontScale(0);
	_TXTparameters |= (1 << 7);//render ON
}

/**************************************************************************/
/*!  
		Enable/Disable the Font Full Alignemet feature (default off)
		Parameters:
		align: true,false
		Note: not active with rendered fonts
*/
/**************************************************************************/
void RA8875::setFontFullAlign(boolean align) 
{
	if (bitRead(_TXTparameters,7) == 0){
		align == true ? _FNCR1_Reg |= (1 << 7) : _FNCR1_Reg &= ~(1 << 7);
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
}


/**************************************************************************/
/*!  
		Set distance between text lines (default off)
		Parameters:
		pix: 0...63 pixels
		Note: active with rendered fonts
*/
/**************************************************************************/
void RA8875::setFontInterline(uint8_t pix)
{
	if (bitRead(_TXTparameters,7) == 1){
		_FNTinterline = pix;
	} else {
		if (pix > 0x3F) pix = 0x3F;
		_FNTinterline = pix;
		//_FWTSET_Reg &= 0xC0;
		//_FWTSET_Reg |= spc & 0x3F; 
		_writeRegister(RA8875_FLDR,_FNTinterline);
	}	
}

/**************************************************************************/
/*!   
		Set the Text position for write Text only.
		Parameters:
		x:horizontal in pixels or CENTER(of the screen)
		y:vertical in pixels or CENTER(of the screen)
		autocenter:center text to choosed x,y regardless text lenght
		false: |ABCD
		true:  AB|CD
		NOTE: works with any font
*/
/**************************************************************************/
void RA8875::setCursor(int16_t x, int16_t y,bool autocenter) 
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	
	_absoluteCenter = autocenter;
	
	if (_portrait) {//rotation 1,3
		swapvals(x,y);
		if (y == CENTER) {//swapped OK
			y = _width/2;
			if (!autocenter) {
				_relativeCenter = true;
				_TXTparameters |= (1 << 6);//set x flag
			}
		}
		if (x == CENTER) {//swapped
			x = _height/2;
			if (!autocenter) {
				_relativeCenter = true;
				_TXTparameters |= (1 << 5);//set y flag
			}
		}
	} else {//rotation 0,2
		if (x == CENTER) {
			x = _width/2;
			if (!autocenter) {
				_relativeCenter = true;
				_TXTparameters |= (1 << 5);
			}
		}
		if (y == CENTER) {
			y = _height/2;
			if (!autocenter) {
				_relativeCenter = true;
				_TXTparameters |= (1 << 6);
			}
		}
	}
	//TODO: This one? Useless?
	if (bitRead(_TXTparameters,2) == 0){//textWrap
		if (x >= _width) x = _width-1;
		if (y >= _height) y = _height-1;
	}
	
	_cursorX = x;
	_cursorY = y;

	//if _relativeCenter or _absoluteCenter do not apply to registers yet!
	// Have to go to _textWrite first to calculate the lenght of the entire string and recalculate the correct x,y
	if (_relativeCenter || _absoluteCenter) return;
	if (bitRead(_TXTparameters,7) == 0) _textPosition(x,y,false);
}

/**************************************************************************/
/*!   
		Set the x,y position for text only
		Parameters:
		x: horizontal pos in pixels
		y: vertical pos in pixels
		update: true track the actual text position internally
		note: not active with rendered fonts, just set x,y internal tracked param
		[private]
*/
/**************************************************************************/
void RA8875::_textPosition(int16_t x, int16_t y,bool update)
{
	#if defined(FORCE_RA8875_TXTREND_FOLLOW_CURS)
		_writeRegister(RA8875_F_CURXL,(x & 0xFF));
		_writeRegister(RA8875_F_CURXH,(x >> 8));
		_writeRegister(RA8875_F_CURYL,(y & 0xFF));
		_writeRegister(RA8875_F_CURYH,(y >> 8));
	#else
		if (bitRead(_TXTparameters,7) == 0){
			_writeRegister(RA8875_F_CURXL,(x & 0xFF));
			_writeRegister(RA8875_F_CURXH,(x >> 8));
			_writeRegister(RA8875_F_CURYL,(y & 0xFF));
			_writeRegister(RA8875_F_CURYH,(y >> 8));
		}
	#endif
	if (update){ _cursorX = x; _cursorY = y;}
}

/**************************************************************************/
/*!   
		Give you back the current text cursor position by reading inside RA8875
		Parameters:
		x: horizontal pos in pixels
		y: vertical pos in pixels
		note: works also with rendered fonts
		USE: xxx.getCursor(myX,myY);
*/
/**************************************************************************/
void RA8875::getCursor(int16_t &x, int16_t &y) 
{
	if (bitRead(_TXTparameters,7) == 1) {
		getCursorFast(x,y);
	} else {
		uint8_t t1,t2,t3,t4;
		t1 = _readRegister(RA8875_F_CURXL);
		t2 = _readRegister(RA8875_F_CURXH);
		t3 = _readRegister(RA8875_F_CURYL);
		t4 = _readRegister(RA8875_F_CURYH);
		x = (t2 << 8) | (t1 & 0xFF);
		y = (t4 << 8) | (t3 & 0xFF);
		if (_portrait) swapvals(x,y);
	}
}

/**************************************************************************/
/*!   
		Give you back the current text cursor position as tracked by library (fast)
		Parameters:
		x: horizontal pos in pixels
		y: vertical pos in pixels
		note: works also with rendered fonts
		USE: xxx.getCursor(myX,myY);
*/
/**************************************************************************/
void RA8875::getCursorFast(int16_t &x, int16_t &y)
{
	x = _cursorX;
	y = _cursorY;
	if (_portrait) swapvals(x,y);
}

int16_t RA8875::getCursorX(void)
{
	if (_portrait) return _cursorY;
	return _cursorX;
}

int16_t RA8875::getCursorY(void)
{
	if (_portrait) return _cursorX;
	return _cursorY;
}
/**************************************************************************/
/*!     Show/Hide text cursor
		Parameters:
		c: cursor type (NOCURSOR,IBEAM,UNDER,BLOCK)
		note: not active with rendered fonts
		blink: true=blink cursor
*/
/**************************************************************************/
void RA8875::showCursor(enum RA8875tcursor c,bool blink)
{
    //uint8_t MWCR1Reg = _readRegister(RA8875_MWCR1) & 0x01;(needed?)
    uint8_t cW = 0;
    uint8_t cH = 0;
	_FNTcursorType = c;
	c == NOCURSOR ? _MWCR0_Reg &= ~(1 << 6) : _MWCR0_Reg |= (1 << 6);
    if (blink) _MWCR0_Reg |= 0x20;//blink or not?
    _writeRegister(RA8875_MWCR0, _MWCR0_Reg);//set cursor
    //_writeRegister(RA8875_MWCR1, MWCR1Reg);//close graphic cursor(needed?)
    switch (c) {
        case IBEAM:
            cW = 0x01;
            cH = 0x1F;
            break;
        case UNDER:
            cW = 0x07;
            cH = 0x01;
            break;
        case BLOCK:
            cW = 0x07;
            cH = 0x1F;
            break;
        case NOCURSOR:
        default:
            break;
    }
	//set cursor size
    _writeRegister(RA8875_CURHS, cW);
    _writeRegister(RA8875_CURVS, cH);
}

/**************************************************************************/
/*!     Set cursor property blink and his rate
		Parameters:
		rate: blink speed (fast 0...255 slow)
		note: not active with rendered fonts
*/
/**************************************************************************/
void RA8875::setCursorBlinkRate(uint8_t rate)
{
	_writeRegister(RA8875_BTCR,rate);//set blink rate
}

/**************************************************************************/
/*!		
		set the text color and his background
		Parameters:
		fcolor: 16bit foreground color (text) RGB565
		bcolor: 16bit background color RGB565
		NOTE: will set background trasparent OFF
		It also works with rendered fonts.
*/
/**************************************************************************/
void RA8875::setTextColor(uint16_t fcolor, uint16_t bcolor)//0.69b30
{
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		if (fcolor != _TXTForeColor) {
			_TXTForeColor = fcolor;
			setForegroundColor(fcolor);
		}
		if (bcolor != _TXTBackColor) {
			_TXTBackColor = bcolor;
			setBackgroundColor(bcolor);
		}
		_backTransparent = false;
	#else
		_backTransparent = false;
		if (fcolor != _foreColor) setForegroundColor(fcolor);
		if (bcolor != _backColor) setBackgroundColor(bcolor);
	#endif
	if (bitRead(_TXTparameters,7) == 0) {
		_FNCR1_Reg &= ~(1 << 6);//clear
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
}

/**************************************************************************/
/*!		
		set the text color w transparent background
		Parameters:
		fColor: 16bit foreground color (text) RGB565
		NOTE: will set background trasparent ON
		It also works with rendered fonts.
*/
/**************************************************************************/

void RA8875::setTextColor(uint16_t fcolor)
{
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		if (fcolor != _TXTForeColor) {
			_TXTForeColor = fcolor;
			setForegroundColor(fcolor);
		}
		_backTransparent = true;
	#else
		_backTransparent = true;
		if (fcolor != _foreColor) setForegroundColor(fcolor);
	#endif
	if (bitRead(_TXTparameters,7) == 0) {
		_FNCR1_Reg |= (1 << 6);//set
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
}

void RA8875::setTextGrandient(uint16_t fcolor1,uint16_t fcolor2)
{
	_FNTgrandient = true;
	_FNTgrandientColor1 = fcolor1;
	_FNTgrandientColor2 = fcolor2;
}
/**************************************************************************/
/*!		
		Set the Text size by it's multiple. normal should=0, max is 3 (x4) for internal fonts
		With Rendered fonts the max scale it's not limited
		Parameters:
		scale: 0..3  -> 0:normal, 1:x2, 2:x3, 3:x4
*/
/**************************************************************************/
void RA8875::setFontScale(uint8_t scale)
{
	setFontScale(scale,scale);
}

/**************************************************************************/
/*!		
		Set the Text size by it's multiple. normal should=0, max is 3 (x4) for internal fonts
		With Rendered fonts the max scale it's not limited
		This time you can specify different values for vertical and horizontal
		Parameters:
		xscale: 0..3  -> 0:normal, 1:x2, 2:x3, 3:x4 for internal fonts - 0...xxx for Rendered Fonts
		yscale: 0..3  -> 0:normal, 1:x2, 2:x3, 3:x4 for internal fonts - 0...xxx for Rendered Fonts
*/
/**************************************************************************/
void RA8875::setFontScale(uint8_t xscale,uint8_t yscale)
{
	_scaling = false;
	if (bitRead(_TXTparameters,7) == 0){
		xscale = xscale % 4; //limit to the range 0-3
		yscale = yscale % 4; //limit to the range 0-3
		_FNCR1_Reg &= ~(0xF); // clear bits from 0 to 3
		_FNCR1_Reg |= xscale << 2;
		_FNCR1_Reg |= yscale;
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
	_scaleX = xscale + 1;
	_scaleY = yscale + 1;
	if (_scaleX > 1 || _scaleY > 1) _scaling = true;
}

/**************************************************************************/
/*!
		Normally at every char the cursor advance by one
		You can stop/enable this by using this function
		Parameters:
		on: true(auto advance - default), false:(stop auto advance)
		Note: Inactive with rendered fonts
*/
/**************************************************************************/
void RA8875::cursorIncrement(bool on)
{
	if (bitRead(_TXTparameters,7) == 0){
		on == true ? _MWCR0_Reg &= ~(1 << 1) : _MWCR0_Reg |= (1 << 1);
		bitWrite(_TXTparameters,1,on);
		_writeRegister(RA8875_MWCR0,_MWCR0_Reg);
	}
}

/**************************************************************************/
/*!		
		Choose between 16x16(8x16) - 24x24(12x24) - 32x32(16x32)
		for External Font ROM
		Parameters:
		ts: X16,X24,X32
		Note: Inactive with rendered fonts
		TODO: Modify font size variables accordly font size!
*/
/**************************************************************************/
void RA8875::setFontSize(enum RA8875tsize ts)
{
	if (_FNTsource == EXT && bitRead(_TXTparameters,7) == 0) {
		switch(ts){
			case X16:
				_FWTSET_Reg &= 0x3F;
				_setFNTdimensions(1);
			break;
			case X24:
				_FWTSET_Reg &= 0x3F; _FWTSET_Reg |= 0x40; 
				_setFNTdimensions(2);
			break;
			case X32:
				_FWTSET_Reg &= 0x3F; _FWTSET_Reg |= 0x80; 
				_setFNTdimensions(3);
			break;
			default:
			return;
		}
		_EXTFNTsize = ts;
		_writeRegister(RA8875_FWTSET,_FWTSET_Reg);
	}
}

/**************************************************************************/
/*!		
		return the current width of the font in pixel
		If font it's scaled, it will multiply.
		It's a fast business since the register it's internally tracked
		It can also return the usable rows based on the actual fontWidth
		Parameters: inColums (true:returns max colums)
		TODO: modded for Rendered Fonts
*/
/**************************************************************************/
uint8_t RA8875::getFontWidth(boolean inColums) 
{
	uint8_t temp;
	if (bitRead(_TXTparameters,7) == 1){
		temp = _FNTwidth;
		if (temp < 1) return 0; //variable with
	} else {
		temp = (((_FNCR0_Reg >> 2) & 0x3) + 1) * _FNTwidth;
	}
	if (inColums){
		if (_scaleX < 2) return (_width / temp);
		temp = temp * _scaleX;
		return (_width / temp);
	} else {
		if (_scaleX < 2) return temp;
		temp = temp * _scaleX;
		return temp;
	}
}

/**************************************************************************/
/*!		
		return the current heigh of the font in pixel
		If font it's scaled, it will multiply.
		It's a fast business since the register it's internally tracked
		It can also return the usable rows based on the actual fontHeight
		Parameters: inRows (true:returns max rows)
		TODO: modded for Rendered Fonts
*/		
/**************************************************************************/
uint8_t RA8875::getFontHeight(boolean inRows) 
{
	uint8_t temp;
	if (bitRead(_TXTparameters,7) == 1){
		temp = _FNTheight;
	} else {
		temp = (((_FNCR0_Reg >> 0) & 0x3) + 1) * _FNTheight;
	}
	if (inRows){
		if (_scaleY < 2) return (_height / temp);
		temp = temp * _scaleY;
		return (_height / temp);
	} else {
		if (_scaleY < 2) return temp;
		temp = temp * _scaleY;
		return temp;
	}
}

/**************************************************************************/
/*!		
		Choose space in pixels between chars
		Parameters:
		spc: 0...63pix (default 0=off)
		TODO: modded for Rendered Fonts
*/
/**************************************************************************/
void RA8875::setFontSpacing(uint8_t spc)
{
	if (spc > 0x3F) spc = 0x3F;
	_FNTspacing = spc;
	if (bitRead(_TXTparameters,7) == 0){
		_FWTSET_Reg &= 0xC0;
		_FWTSET_Reg |= spc & 0x3F; 
		_writeRegister(RA8875_FWTSET,_FWTSET_Reg);
	}

}
/**************************************************************************/
/*!	PRIVATE
		draw a string
		Works for all fonts, internal, ROM, external (render)
*/
/**************************************************************************/

void RA8875::_textWrite(const char* buffer, uint16_t len)
 {
	uint16_t i;
	if (len == 0) len = strlen(buffer);//try get the info from the buffer
	if (len == 0) return;//better stop here, the string it's really empty!
	bool renderOn = bitRead(_TXTparameters,7);//detect if render fonts active
	
	uint8_t loVOffset = 0;
	uint8_t hiVOffset = 0;
	uint8_t interlineOffset = 0;
	uint16_t fcolor = _foreColor;
	uint16_t bcolor = _backColor;
	uint16_t strngWidth = 0;
	uint16_t strngHeight = 0;
	if (!renderOn){
		loVOffset = _FNTbaselineLow * _scaleY;//calculate lower baseline
		hiVOffset = _FNTbaselineTop * _scaleY;//calculate topline
		//now check for offset if using an external fonts rom (RA8875 bug)
		if (bitRead(_TXTparameters,0) == 1) interlineOffset = 3 * _scaleY;
	}
	
	//_absoluteCenter or _relativeCenter cases...................
	//plus calculate the real width & height of the entire text in render mode (not trasparent)
	if (_absoluteCenter || _relativeCenter || (renderOn && !_backTransparent)){
		strngWidth = _STRlen_helper(buffer,len) * _scaleX;//this calculates the width of the entire text
		strngHeight = (_FNTheight * _scaleY) - (loVOffset + hiVOffset);//the REAL heigh
		if (_absoluteCenter && strngWidth > 0){//Avoid operations for strngWidth = 0
			_absoluteCenter = false;
			_cursorX = _cursorX - (strngWidth / 2);
			_cursorY = _cursorY - (strngHeight / 2) - hiVOffset;
			if (_portrait) swapvals(_cursorX,_cursorY);
		} else if (_relativeCenter && strngWidth > 0){//Avoid operations for strngWidth = 0
			_relativeCenter = false;
			if (bitRead(_TXTparameters,5)) {//X = center
				if (!_portrait){
					_cursorX = (_width / 2) - (strngWidth / 2);
				} else {
					_cursorX = (_height / 2) - (strngHeight / 2) - hiVOffset;
				}
				_TXTparameters &= ~(1 << 5);//reset
			}
			if (bitRead(_TXTparameters,6)) {//Y = center
				if (!_portrait){
					_cursorY = (_height / 2) - (strngHeight / 2) - hiVOffset;
				} else {
					_cursorY = (_width / 2) - (strngWidth / 2);
				}
				_TXTparameters &= ~(1 << 6);//reset
			}
		}
		//if ((_absoluteCenter || _relativeCenter) &&  strngWidth > 0){//Avoid operations for strngWidth = 0
		if (strngWidth > 0){//Avoid operations for strngWidth = 0
			#if defined(FORCE_RA8875_TXTREND_FOLLOW_CURS)
				_textPosition(_cursorX,_cursorY,false);
			#else
				if (!renderOn) _textPosition(_cursorX,_cursorY,false);
			#endif
		}
	}//_absoluteCenter,_relativeCenter,(renderOn && !_backTransparent)
//-----------------------------------------------------------------------------------------------
	if (!_textMode && !renderOn) _setTextMode(true);//   go to text
	if (_textMode && renderOn)   _setTextMode(false);//  go to graphic
	//colored text vars
	uint16_t grandientLen = 0;
	uint16_t grandientIndex = 0;
	uint16_t recoverColor = fcolor;
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		if (_textMode && _TXTrecoverColor){
			if (_foreColor != _TXTForeColor) {_TXTrecoverColor = false;setForegroundColor(_TXTForeColor);}
			if (_backColor != _TXTBackColor) {_TXTrecoverColor = false;setBackgroundColor(_TXTBackColor);}
		} else {
			fcolor = _TXTForeColor;
			bcolor = _TXTBackColor;
		}
	#endif
	if (_FNTgrandient){//coloring text
		recoverColor = _TXTForeColor;
		for (i=0;i<len;i++){//avoid non char in color index
			if (buffer[i] != 13 && buffer[i] != 10 && buffer[i] != 32) grandientLen++;//lenght of the interpolation
		}
	}
	#if defined(RA8875_TXTBENCH) && !defined(RA8875_VISPIXDEBUG)//for testing purposes
		unsigned long start = micros();
	#endif
	#if defined(_RA8875_TXTRNDOPTIMIZER)
	//instead write the background by using pixels (trough text rendering) better this trick
	if (renderOn && !_backTransparent && strngWidth > 0) fillRect(_cursorX,_cursorY,strngWidth,strngHeight,_backColor);//bColor
	#endif
	//Loop trough every char and write them one by one...
	for (i=0;i<len;i++){
		if (_FNTgrandient){
			if (buffer[i] != 13 && buffer[i] != 10 && buffer[i] != 32){
				if (!renderOn){
					setTextColor(colorInterpolation(_FNTgrandientColor1,_FNTgrandientColor2,grandientIndex++,grandientLen));
				} else {
					fcolor = colorInterpolation(_FNTgrandientColor1,_FNTgrandientColor2,grandientIndex++,grandientLen);
				}
			}
		}
		if (!renderOn){
			_charWrite(buffer[i],interlineOffset);					// internal,ROM fonts
		} else {
			_charWriteR(buffer[i],interlineOffset,fcolor,bcolor);   // user fonts
		}
	}//end loop
	if (_FNTgrandient){//recover text color after colored text
		_FNTgrandient = false;
		 //recover original text color
		if (!renderOn){
			#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
				setTextColor(recoverColor,_backColor);
			#else
				setTextColor(recoverColor,_backColor);
			#endif
		} else {
			fcolor = recoverColor;
		}
	}
	#if defined(RA8875_TXTBENCH) && !defined(RA8875_VISPIXDEBUG)
	  unsigned long result = micros() - start;
	  Serial.print("Text Rendered in:");
	  Serial.print(result);
	  Serial.print(" ms");
	  Serial.print("\n");
	#endif
}

/**************************************************************************/
/*!	PRIVATE
	Main routine that write a single char in render mode, this actually call another subroutine that do the paint job
	but this one take care of all the calculations...
	NOTE: It identify correctly println and /n & /r
*/
/**************************************************************************/
void RA8875::_charWriteR(const char c,uint8_t offset,uint16_t fcolor,uint16_t bcolor)
{
	if (c == 13){//------------------------------- CARRIAGE ----------------------------------
		//ignore
	} else if (c == 10){//------------------------- NEW LINE ---------------------------------
		if (!_portrait){
			_cursorX = 0;
			_cursorY += (_FNTheight * _scaleY) + _FNTinterline + offset;
		} else {
			_cursorX += (_FNTheight * _scaleY) + _FNTinterline + offset;
			_cursorY = 0;
		}
		#if defined(FORCE_RA8875_TXTREND_FOLLOW_CURS)
			_textPosition(_cursorX,_cursorY,false);
		#endif
	} else if (c == 32){//--------------------------- SPACE ---------------------------------
		if (!_portrait){
			fillRect(_cursorX,_cursorY,(_spaceCharWidth * _scaleX),(_FNTheight * _scaleY),bcolor);//bColor
			_cursorX += (_spaceCharWidth * _scaleX) + _FNTspacing;
		} else {
			fillRect(_cursorY,_cursorX,(_spaceCharWidth * _scaleX),(_FNTheight * _scaleY),bcolor);//bColor
			_cursorY += (_spaceCharWidth * _scaleX) + _FNTspacing;
		}
		// #if defined(FORCE_RA8875_TXTREND_FOLLOW_CURS)
			// _textPosition(_cursorX,_cursorY,false);
		// #endif
	} else {//-------------------------------------- CHAR ------------------------------------
		int charIndex = _getCharCode(c);//get char code
		if (charIndex > -1){//valid?
			int charW = 0;
			//get charW and glyph
			#if defined(_FORCE_PROGMEM__)
				#if defined(ESP8266)
						charW = FPSTR(&_currentFont->chars[charIndex].image->image_width);
					#if !defined(_RA8875_TXTRNDOPTIMIZER)
						const uint8_t * charGlyp = FPSTR(&_currentFont->chars[charIndex].image->data);
					#endif
				#else
						charW = PROGMEM_read(&_currentFont->chars[charIndex].image->image_width);
					#if !defined(_RA8875_TXTRNDOPTIMIZER)
						const uint8_t * charGlyp = PROGMEM_read(&_currentFont->chars[charIndex].image->data);
					#endif
				#endif
			#else
					charW = _currentFont->chars[charIndex].image->image_width;
				#if !defined(_RA8875_TXTRNDOPTIMIZER)
					const uint8_t * charGlyp = _currentFont->chars[charIndex].image->data;
				#endif
			#endif
			//check if goes out of screen and goes to a new line (if wrap) or just avoid
			if (bitRead(_TXTparameters,2)){//wrap?
				if (!_portrait && (_cursorX + charW * _scaleX) >= _width){
					_cursorX = 0;
					_cursorY += (_FNTheight * _scaleY) + _FNTinterline + offset;
				} else if (_portrait && (_cursorY + charW * _scaleY) >= _width){
					_cursorX += (_FNTheight * _scaleY) + _FNTinterline + offset;
					_cursorY = 0;
				}
				#if defined(FORCE_RA8875_TXTREND_FOLLOW_CURS)
					//_textPosition(_cursorX,_cursorY,false);
				#endif
			} else {
				if (_portrait){
					if (_cursorY + charW * _scaleY >= _width) return;
				} else {
					if (_cursorX + charW * _scaleX >= _width) return;
				}
			}
			//test purposes ----------------------------------------------------------------
			/*
			if (!_portrait){
				fillRect(_cursorX,_cursorY,(charW * _scaleX),(_FNTheight * _scaleY),RA8875_YELLOW);//bColor
			} else {
				fillRect(_cursorY,_cursorX,(charW * _scaleX),(_FNTheight * _scaleY),RA8875_YELLOW);//bColor
			}
			*/
			//-------------------------Actual single char drawing here -----------------------------------
			if (!_FNTcompression){
				#if defined(_RA8875_TXTRNDOPTIMIZER)
					if (!_portrait){
						_drawChar_unc(_cursorX,_cursorY,charW,charIndex,fcolor);
					} else {
						_drawChar_unc(_cursorY,_cursorX,charW,charIndex,fcolor);
					}
				#else
					if (!_portrait){
						_drawChar_unc(_cursorX,_cursorY,charW,charGlyp,fcolor,bcolor);
					} else {
						_drawChar_unc(_cursorY,_cursorX,charW,charGlyp,fcolor,bcolor);
					}
				#endif
			} else {
				//TODO
				//RLE compressed fonts
			}

			//add charW to total -----------------------------------------------------
			if (!_portrait){
				_cursorX += (charW * _scaleX) + _FNTspacing;
			} else {
				_cursorY += (charW * _scaleX) + _FNTspacing;
			}

			// #if defined(FORCE_RA8875_TXTREND_FOLLOW_CURS)
				// _textPosition(_cursorX,_cursorY,false);
			// #endif
		}//end valid
	}//end char
}

/**************************************************************************/
/*!	PRIVATE
		Write a single char, only INT and FONT ROM char (internal RA9975 render)
		NOTE: It identify correctly println and /n & /r
*/
/**************************************************************************/
void RA8875::_charWrite(const char c,uint8_t offset)
{
	bool dtacmd = false;
	if (c == 13){//'\r'
		//Ignore carriage-return
	} else if (c == 10){//'\n'
		if (!_portrait){
			_cursorX = 0;
			_cursorY += (_FNTheight + (_FNTheight * (_scaleY - 1))) + _FNTinterline + offset;
		} else {
			_cursorX += (_FNTheight + (_FNTheight * (_scaleY - 1))) + _FNTinterline + offset;
			_cursorY = 0;
		}
		_textPosition(_cursorX,_cursorY,false);
		dtacmd = false;
	} else {
		if (!dtacmd){
			dtacmd = true;
			
			if (!_textMode) _setTextMode(true);//we are in graph mode?
			writeCommand(RA8875_MRWC);
		}
		_writeData(c);
		_waitBusy(0x80);
		//update cursor
		if (!_portrait){
			_cursorX += _FNTwidth;
		} else {
			_cursorY += _FNTwidth;
		}
	}
}


/**************************************************************************/
/*!	PRIVATE
		Search for glyph char code in font array
		It return font index or -1 if not found.
*/
/**************************************************************************/
int RA8875::_getCharCode(uint8_t ch)
{
	int i;
	for (i=0;i<_currentFont->length;i++){//search for char code
		#if defined(_FORCE_PROGMEM__)
			uint8_t ccode = _currentFont->chars[i].char_code;
			if (ccode == ch) return i;
		#else
			if (_currentFont->chars[i].char_code == ch) return i;
		#endif
	}//i
	return -1;
}

/**************************************************************************/
/*!	PRIVATE
		This helper loop trough a text string and return how long is (in pixel)
		NOTE: It identify correctly println and /n & /r and forget non present chars
*/
/**************************************************************************/
int16_t RA8875::_STRlen_helper(const char* buffer,uint16_t len)
{
	if (bitRead(_TXTparameters,7) == 0){		//_renderFont not active
		return (len * _FNTwidth);
	} else {									//_renderFont active
		int charIndex = -1;
		uint16_t i;
		if (len == 0) len = strlen(buffer);		//try to get data from string
		if (len == 0) return 0;					//better stop here
		if (_FNTwidth > 0){						// fixed width font
			return ((len * _spaceCharWidth));
		} else {								// variable width, need to loop trough entire string!
			uint16_t totW = 0;
			for (i = 0;i < len;i++){			//loop trough buffer
				if (buffer[i] == 32){			//a space
					totW += _spaceCharWidth;
				} else if (buffer[i] != 13 && buffer[i] != 10 && buffer[i] != 32){//avoid special char
					charIndex = _getCharCode(buffer[i]);
					if (charIndex > -1) {		//found!
						#if defined(_FORCE_PROGMEM__)
							#if defined(ESP8266)
								totW += (FPSTR(&_currentFont->chars[charIndex].image->image_width));
							#else
								totW += (PROGMEM_read(&_currentFont->chars[charIndex].image->image_width));
							#endif
						#else
							totW += (_currentFont->chars[charIndex].image->image_width);
						#endif
					}
				}//inside permitted chars
			}//buffer loop
			return totW;						//return data
		}//end variable w font
	}
}



#if defined(_RA8875_TXTRNDOPTIMIZER)

/**************************************************************************/
/*!	PRIVATE
		Here's the char render engine for uncompressed fonts, it actually render a single char.
		It's actually 2 functions, this one take care of every glyph line
		and perform some optimization second one paint concurrent pixels in chunks.
		To show how optimizations works try uncomment RA8875_VISPIXDEBUG in settings.
		Please do not steal this part of code!
*/
/**************************************************************************/
void RA8875::_drawChar_unc(int16_t x,int16_t y,int charW,int index,uint16_t fcolor)
{
	//start by getting some glyph data...
	#if defined(_FORCE_PROGMEM__)
		#if defined(ESP8266)
			const uint8_t * charGlyp = FPSTR(&_currentFont->chars[index].image->data);//char data
			int			  totalBytes = FPSTR(&_currentFont->chars[index].image->image_datalen);
		#else
			const uint8_t * charGlyp = PROGMEM_read(&_currentFont->chars[index].image->data);//char data
			int			  totalBytes = PROGMEM_read(&_currentFont->chars[index].image->image_datalen);
		#endif
	#else
		const uint8_t * charGlyp = _currentFont->chars[index].image->data;
		int			  totalBytes = _currentFont->chars[index].image->image_datalen;
	#endif
	int i;
	uint8_t temp = 0;
	//some basic variable...
	uint8_t currentXposition = 0;//the current position of the writing cursor in the x axis, from 0 to charW
	uint8_t currentYposition = 1;//the current position of the writing cursor in the y axis, from 1 to _FNTheight
	int currentByte = 0;//the current byte in reading (from 0 to totalBytes)
	bool lineBuffer[charW];//the temporary line buffer (will be _FNTheight each char)
	int lineChecksum = 0;//part of the optimizer
	/*
	uint8_t bytesInLine = 0;
	//try to understand how many bytes in a line
	if (charW % 8 == 0) {	// divisible by 8
		bytesInLine = charW / 8;
	} else {						// when it's divisible by 8?
		bytesInLine = charW;
		while (bytesInLine % 8) { bytesInLine--;}
		bytesInLine = bytesInLine / 8;
	}
	*/
	//the main loop that will read all bytes of the glyph
	while (currentByte < totalBytes){
		//read n byte
		#if defined(_FORCE_PROGMEM__)
			#if defined(ESP8266)
				temp = FPSTR(&charGlyp[currentByte]);
			#else
				temp = PROGMEM_read(&charGlyp[currentByte]);
			#endif
		#else
			temp = charGlyp[currentByte];
		#endif
		for (i=7; i>=0; i--){
			//----------------------------------- exception
			if (currentXposition >= charW){
				//line buffer has been filled!
				currentXposition = 0;//reset the line x position
				if (lineChecksum < 1){//empty line
					#if defined(RA8875_VISPIXDEBUG)
						drawRect(x,y + (currentYposition * _scaleY),charW * _scaleX,_scaleY,RA8875_BLUE);
					#endif
				} else if (lineChecksum == charW){//full line
					#if !defined(RA8875_VISPIXDEBUG)
					fillRect(
					#else
					drawRect(
					#endif
						x,y + (currentYposition * _scaleY),charW * _scaleX,_scaleY,fcolor);
				} else { //line render
					_charLineRender(lineBuffer,charW,x,y,currentYposition,fcolor);
				}
				currentYposition++;//next line
				lineChecksum = 0;//reset checksum
			}//end exception
			//-------------------------------------------------------
			lineBuffer[currentXposition] = bitRead(temp,i);//continue fill line buffer
			lineChecksum += lineBuffer[currentXposition];
			currentXposition++;
		}
		currentByte++;
	}
}

/**************************************************************************/
/*!	PRIVATE
		Font Line render optimized routine
		This will render ONLY a single font line by grouping chunks of same pixels
		Version 3.0 (fixed a bug that cause xlinePos to jump of 1 pixel
*/
/**************************************************************************/
void RA8875::_charLineRender(bool lineBuffer[],int charW,int16_t x,int16_t y,int16_t currentYposition,uint16_t fcolor)
{
	int xlinePos = 0;
	int px;
	uint8_t endPix = 0;
	bool refPixel = false;
	while (xlinePos < charW){
		refPixel = lineBuffer[xlinePos];//xlinePos pix as reference value for next pixels
		//detect and render concurrent pixels
		for (px = xlinePos;px <= charW;px++){
			if (lineBuffer[px] == lineBuffer[xlinePos] && px < charW){//grouping pixels with same val
				endPix++;
			} else {
				if (refPixel){
					#if defined(RA8875_VISPIXDEBUG)
					drawRect(
					#else
					fillRect(
					#endif
						x,y + (currentYposition * _scaleY),endPix * _scaleX,_scaleY,fcolor);
				} else {
					#if defined(RA8875_VISPIXDEBUG)
					drawRect(x,y + (currentYposition * _scaleY),endPix * _scaleX,_scaleY,RA8875_BLUE);
					#endif
				}
				//reset and update some vals
				xlinePos += endPix;
				x += endPix * _scaleX;
				endPix = 0;
				break;//exit cycle for...
			}
		}
	}
}

#else
/**************************************************************************/
/*!
      This is the old rendering engine, pretty basic but slow, here for an alternative.
	  Note that this can be enabled only by commenting 
	  
	  #define _RA8875_TXTRNDOPTIMIZER 
	  
	  in RA8875UserSettings.h file!
	  Parameters:
	  x:
	  Y:
	  w: the width of the font
	  data: the data glyph
	  fcolor: foreground color
	  bcolor: background color
*/
/**************************************************************************/	
void RA8875::_drawChar_unc(int16_t x,int16_t y,int16_t w,const uint8_t *data,uint16_t fcolor,uint16_t bcolor)
{
	// if ((x >= _width)             			||  // Clip right
		// (y >= _height)           			||  // Clip bottom
		// ((x + w * _FNTscaleX - 1) < 0) 	||  // Clip left
		// ((y + _FNTheight * _FNTscaleY - 1) < 0))   	// Clip top
    // return;
	uint16_t color;
	uint16_t bitCount = 0;
	uint8_t line = 0;
	int j;
	uint16_t i;//,idx;
	for (i=0; i<_FNTheight; i++) {	//Y
		for (j = 0; j<w; j++) {			//X		
			if (bitCount++%8 == 0) {
				#if defined(_FORCE_PROGMEM__)
					#if defined(ESP8266)
						line = FPSTR(&*data++);
					#else
						line = PROGMEM_read(&*data++);
					#endif
				#else
					line = *data++;
				#endif
			}
			#if defined(RA8875_VISPIXDEBUG)
			color = RA8875_BLUE;
			#else
			color = bcolor;
			#endif
			if (line & 0x80) color = fcolor;
			if (_scaling) {// big
				#if defined(RA8875_VISPIXDEBUG)
				drawRect(
				#else
				fillRect(
				#endif
					x + (j * _scaleX), 
					y + (i * _scaleY), 
					_scaleX, 
					_scaleY, 
					color
				);
			} else {  // def size
				drawPixel(x + j, y + i, color);
			}
			line <<= 1;
		}
	}
}
#endif

/*
void RA8875::_drawChar_com(int16_t x,int16_t y,int16_t w,const uint8_t *data)
{
}
*/

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								COLOR STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/**************************************************************************/
/*!
      Set the display 'Color Space'
	  Parameters:
	  Bit per Pixel color (colors): 8 or 16 bit
	  NOTE:
	  For display over 272*480 give the ability to use
	  Layers since at 16 bit it's not possible.
*/
/**************************************************************************/
void RA8875::setColorBpp(uint8_t colors)
{
	if (colors != _color_bpp){//only if necessary
		if (colors < 16) {
			_color_bpp = 8;
			_colorIndex = 3;
			_writeRegister(RA8875_SYSR,0x00);
			if (_hasLayerLimits) _maxLayers = 2;
		} else if (colors > 8) {//65K
			_color_bpp = 16;
			_colorIndex = 0;
			_writeRegister(RA8875_SYSR,0x0C);
			if (_hasLayerLimits) _maxLayers = 1;
			_currentLayer = 0;
		}
	}
}

/**************************************************************************/
/*!
      Return current Color Space (8 or 16)
*/
/**************************************************************************/
uint8_t RA8875::getColorBpp(void)
{
	return _color_bpp;
}

/**************************************************************************/
/*!
      Sets set the foreground color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color: 16bit color RGB565
*/
/**************************************************************************/
void RA8875::setForegroundColor(uint16_t color)
{
	_foreColor = color;//keep track
	_writeRegister(RA8875_FGCR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));
	_writeRegister(RA8875_FGCR0+1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));
	_writeRegister(RA8875_FGCR0+2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));
}
/**************************************************************************/
/*!
      Sets set the foreground color using 8bit R,G,B
	  Parameters:
	  R: 8bit RED
	  G: 8bit GREEN
	  B: 8bit BLUE
*/
/**************************************************************************/
void RA8875::setForegroundColor(uint8_t R,uint8_t G,uint8_t B)
{
	_foreColor = Color565(R,G,B);//keep track
	_writeRegister(RA8875_FGCR0,R);
	_writeRegister(RA8875_FGCR0+1,G);
	_writeRegister(RA8875_FGCR0+2,B);
}
/**************************************************************************/
/*!
      Sets set the background color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color: 16bit color RGB565
	  Note: will set background Trasparency OFF
*/
/**************************************************************************/
void RA8875::setBackgroundColor(uint16_t color)
{
	_backColor = color;//keep track
	_writeRegister(RA8875_BGCR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));//11
	_writeRegister(RA8875_BGCR0+1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));//5
	_writeRegister(RA8875_BGCR0+2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));//0
}
/**************************************************************************/
/*!
      Sets set the background color using 8bit R,G,B
	  Parameters:
	  R: 8bit RED
	  G: 8bit GREEN
	  B: 8bit BLUE
	  Note: will set background Trasparency OFF
*/
/**************************************************************************/
void RA8875::setBackgroundColor(uint8_t R,uint8_t G,uint8_t B)
{
	_backColor = Color565(R,G,B);//keep track
	_writeRegister(RA8875_BGCR0,R);
	_writeRegister(RA8875_BGCR0+1,G);
	_writeRegister(RA8875_BGCR0+2,B);
}
/**************************************************************************/
/*!
      Sets set the trasparent background color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color: 16bit color RGB565
	  Note: will set background Trasparency ON
*/
/**************************************************************************/
void RA8875::setTransparentColor(uint16_t color)
{
	_backColor = color;
	_writeRegister(RA8875_BGTR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));
	_writeRegister(RA8875_BGTR0+1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));
	_writeRegister(RA8875_BGTR0+2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));
}
/**************************************************************************/
/*!
      Sets set the Trasparent background color using 8bit R,G,B
	  Parameters:
	  R: 8bit RED
	  G: 8bit GREEN
	  B: 8bit BLUE
	  Note: will set background Trasparency ON
*/
/**************************************************************************/
void RA8875::setTransparentColor(uint8_t R,uint8_t G,uint8_t B)
{
	_backColor = Color565(R,G,B);//keep track
	_writeRegister(RA8875_BGTR0,R);
	_writeRegister(RA8875_BGTR0+1,G);
	_writeRegister(RA8875_BGTR0+2,B);
}

/**************************************************************************/
/*!		
		set foreground,background color (plus transparent background)
		Parameters:
		fColor: 16bit foreground color (text) RGB565
		bColor: 16bit background color RGB565
		backTransp:if true the bColor will be transparent
*/
/**************************************************************************/
void RA8875::setColor(uint16_t fcolor,uint16_t bcolor,bool bcolorTraspFlag)//0.69b30
{
	if (fcolor != _foreColor) setForegroundColor(fcolor);
	if (bcolorTraspFlag){
		setTransparentColor(bcolor);
	} else {
		if (bcolor != _backColor) setBackgroundColor(bcolor);
	}
}


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								DRAW STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/**************************************************************************/
/*!		Set graphic cursor beween 8 different ones.
	    Graphic cursors has to be inserted before use!
		Parameters:
		cur: 0...7
*/
/**************************************************************************/
void RA8875::setGraphicCursor(uint8_t cur) 
{
	if (cur > 7) cur = 7;
	uint8_t temp = _readRegister(RA8875_MWCR1);
	temp &= ~(0x70);//clear bit 6,5,4
	temp |= cur << 4;
	temp |= cur;
	if (_useMultiLayers){
		_currentLayer == 1 ? temp |= (1 << 0) : temp &= ~(1 << 0);
	} else {
		temp &= ~(1 << 0);
	}
	_writeData(temp);
}

/**************************************************************************/
/*!		Show the graphic cursor
	    Graphic cursors has to be inserted before use!
		Parameters:
		cur: true,false
*/
/**************************************************************************/
void RA8875::showGraphicCursor(boolean cur) 
{
	uint8_t temp = _readRegister(RA8875_MWCR1);
	cur == true ? temp |= (1 << 7) : temp &= ~(1 << 7);
	if (_useMultiLayers){
		_currentLayer == 1 ? temp |= (1 << 0) : temp &= ~(1 << 0);
	} else {
		temp &= ~(1 << 0);
	}
	_writeData(temp);
}

/**************************************************************************/
/*!		
		Set the position for Graphic Write
		Parameters:
		x: horizontal position
		y: vertical position
*/
/**************************************************************************/

void RA8875::setXY(int16_t x, int16_t y) 
{
	setX(x);
	setY(y);
}

/**************************************************************************/
/*!		
		Set the x position for Graphic Write
		Parameters:
		x: horizontal position
*/
/**************************************************************************/
void RA8875::setX(int16_t x) 
{
	if (x < 0) x = 0;
	if (_portrait){//fix 0.69b21
		if (x >= RA8875_HEIGHT) x = RA8875_HEIGHT-1;
		_writeRegister(RA8875_CURV0, x & 0xFF);
		_writeRegister(RA8875_CURV0+1, x >> 8);
	} else {
		if (x >= RA8875_WIDTH) x = RA8875_WIDTH-1;
		_writeRegister(RA8875_CURH0, x & 0xFF);
		_writeRegister(RA8875_CURH0+1, (x >> 8)); 
	}
}

/**************************************************************************/
/*!		
		Set the y position for Graphic Write
		Parameters:
		y: vertical position
*/
/**************************************************************************/
void RA8875::setY(int16_t y) 
{
	if (y < 0) y = 0;
	if (_portrait){//fix 0.69b21
		if (y >= RA8875_WIDTH) y = RA8875_WIDTH-1;
		_writeRegister(RA8875_CURH0, y & 0xFF);
		_writeRegister(RA8875_CURH0+1, (y >> 8)); 
	} else {
		if (y >= RA8875_HEIGHT) y = RA8875_HEIGHT-1;
		_writeRegister(RA8875_CURV0, y & 0xFF);
		_writeRegister(RA8875_CURV0+1, y >> 8);
	}
}

 
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								SCROLL STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/**************************************************************************/
/*!     
        Sets the scroll mode. This is controlled by bits 6 and 7 of  
        REG[52h] Layer Transparency Register0 (LTPR0)
		Author: The Experimentalist
*/
/**************************************************************************/
void RA8875::setScrollMode(enum RA8875scrollMode mode)
{
    uint8_t temp = _readRegister(RA8875_LTPR0);
    temp &= 0x3F;            // Clear bits 6 and 7 to zero
    switch(mode){           // bit 7,6 of LTPR0
        case SIMULTANEOUS:  // 00b : Layer 1/2 scroll simultaneously.
            // Do nothing
        break;
        case LAYER1ONLY:        // 01b : Only Layer 1 scroll.
            temp |= 0x40;
        break;
        case LAYER2ONLY:        // 10b : Only Layer 2 scroll.
            temp |= 0x80;
        break;
        case BUFFERED:      	// 11b: Buffer scroll (using Layer 2 as scroll buffer)
            temp |= 0xC0;
        break;
        default:
            return;         	//do nothing
    }
    //TODO: Should this be conditional on multi layer?
    //if (_useMultiLayers) _writeRegister(RA8875_LTPR0,temp);
    //_writeRegister(RA8875_LTPR0,temp);
	_writeData(temp);
}

/**************************************************************************/
/*!		
		Define a window for perform scroll
		Parameters:
		XL: x window start left
		XR: x window end right
		YT: y window start top
		YB: y window end bottom

*/
/**************************************************************************/
void RA8875::setScrollWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB)
{
	if (_portrait){//0.69b22 (fixed)
		swapvals(XL,YT);
		swapvals(XR,YB);
	}
	
	_checkLimits_helper(XL,YT);
	_checkLimits_helper(XR,YB);
	
	_scrollXL = XL; _scrollXR = XR; _scrollYT = YT; _scrollYB = YB;
    _writeRegister(RA8875_HSSW0,(_scrollXL & 0xFF));
    _writeRegister(RA8875_HSSW0+1,(_scrollXL >> 8));
  
    _writeRegister(RA8875_HESW0,(_scrollXR & 0xFF));
    _writeRegister(RA8875_HESW0+1,(_scrollXR >> 8));   
    
    _writeRegister(RA8875_VSSW0,(_scrollYT & 0xFF));
    _writeRegister(RA8875_VSSW0+1,(_scrollYT >> 8));   
 
    _writeRegister(RA8875_VESW0,(_scrollYB & 0xFF));
    _writeRegister(RA8875_VESW0+1,(_scrollYB >> 8));
	delay(1);
}

/**************************************************************************/
/*!
		Perform the scroll

*/
/**************************************************************************/
void RA8875::scroll(int16_t x,int16_t y)
{ 
	if (_portrait) swapvals(x,y);
	//if (y > _scrollYB) y = _scrollYB;//??? mmmm... not sure
	if (_scrollXL == 0 && _scrollXR == 0 && _scrollYT == 0 && _scrollYB == 0){
		//do nothing, scroll window inactive
	} else {
		_writeRegister(RA8875_HOFS0,(x & 0xFF)); 
		_writeRegister(RA8875_HOFS1,(x >> 8));
 
		_writeRegister(RA8875_VOFS0,(y & 0xFF));
		_writeRegister(RA8875_VOFS1,(y >> 8));
	}
}	 

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								DMA STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/**************************************************************************/
/*!

*/
/**************************************************************************/
void RA8875::DMA_blockModeSize(int16_t BWR,int16_t BHR,int16_t SPWR)
{
  	_writeRegister(RA8875_DTNR0,BWR & 0xFF);
  	_writeRegister(RA8875_BWR1,BWR >> 8);

  	_writeRegister(RA8875_DTNR1,BHR & 0xFF);
  	_writeRegister(RA8875_BHR1,BHR >> 8);

  	_writeRegister(RA8875_DTNR2,SPWR & 0xFF);
  	_writeRegister(RA8875_SPWR1,SPWR >> 8); 
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void RA8875::DMA_startAddress(unsigned long adrs)
{ 
  	_writeRegister(RA8875_SSAR0,adrs & 0xFF);
  	_writeRegister(RA8875_SSAR0+1,adrs >> 8);
	_writeRegister(RA8875_SSAR0+2,adrs >> 16);
  	//_writeRegister(0xB3,adrs >> 24);// not more in datasheet!
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void RA8875::DMA_enable(void)
{ 
	uint8_t temp = _readRegister(RA8875_DMACR);
	temp |= 0x01;
	_writeData(temp);
	_waitBusy(0x01);
}
/**************************************************************************/
/*! (STILL IN DEVELOP, please do not complain)
		Display an image stored in Flash RAM
		Note: you should have the optional FLASH Chip connected to RA8875!
		Note: You should store some image in that chip!
		Note: Never tried!!!!!!!

*/
/**************************************************************************/
void RA8875::drawFlashImage(int16_t x,int16_t y,int16_t w,int16_t h,uint8_t picnum)
{  
	if (_portrait){swapvals(x,y); swapvals(w,h);}//0.69b21 -have to check this, not verified
	if (_textMode) _setTextMode(false);//we are in text mode?
	_writeRegister(RA8875_SFCLR,0x00);
	_writeRegister(RA8875_SROC,0x87);
	_writeRegister(RA8875_DMACR,0x02);
	//setActiveWindow(0,_width-1,0,_height-1); 
	_checkLimits_helper(x,y);
	_checkLimits_helper(w,h);
	_portrait == true ? setXY(y,x) : setXY(x,y);
	
	DMA_startAddress(261120 * (picnum-1));
	DMA_blockModeSize(w,h,w);   
	_writeRegister(RA8875_DMACR,0x03);
	_waitBusy(0x01);
} 


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								BTE STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/**************************************************************************/
/* 
		Block Transfer Move
		Can move a rectangular block from any area of memory (eg. layer 1) to any other (eg layer 2)
		Can move with transparency - note THE TRANSPARENT COLOUR IS THE TEXT FOREGROUND COLOUR
		ReverseDir is for moving overlapping areas - may need to use reverse to prevent it double-copying the overlapping area (this option not available with transparency or monochrome)
		ROP is Raster Operation. Usually use RA8875_ROP_SOURCE but a few others are defined
		Defaults to current layer if not given or layer is zero.
		Monochrome uses the colour-expansion mode: the input is a bit map which is then converted to the current foreground and background colours, transparent background is optional
		Monochrome data is assumed to be linear, originally written to the screen memory in 16-bit chunks with drawPixels().
		Monochrome mode uses the ROP to define the offset of the first image bit within the first byte. This also depends on the width of the block you are trying to display.
		Monochrome skips 16-bit words in the input pattern - see the example for more explanation and a trick to interleave 2 characters in the space of one.

		This function returns immediately but the actual transfer can take some time
		Caller should check the busy status before issuing any more RS8875 commands.

		Basic usage:
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY) = copy something visible on the current layer
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 2) = copy something from layer 2 to the current layer
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 2, 1, true) = copy from layer 2 to layer 1, with the transparency option
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 0, 0, true, RA8875_BTEROP_ADD) = copy on the current layer, using transparency and the ADD/brighter operation 
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 0, 0, false, RA8875_BTEROP_SOURCE, false, true) = copy on the current layer using the reverse direction option for overlapping areas
*/

void  RA8875::BTE_move(int16_t SourceX, int16_t SourceY, int16_t Width, int16_t Height, int16_t DestX, int16_t DestY, uint8_t SourceLayer, uint8_t DestLayer,bool Transparent, uint8_t ROP, bool Monochrome, bool ReverseDir)
{
	
	if (SourceLayer == 0) SourceLayer = _currentLayer;	
	if (DestLayer == 0) DestLayer = _currentLayer;
	if (SourceLayer == 2) SourceY |= 0x8000; //set the high bit of the vertical coordinate to indicate layer 2
	if (DestLayer == 2) DestY |= 0x8000; //set the high bit of the vertical coordinate to indicate layer 2
	ROP &= 0xF0; //Ensure the lower bits of ROP are zero
	if (Transparent) {
		if (Monochrome) {
			ROP |= 0x0A; //colour-expand transparent
		} else {
			ROP |= 0x05; //set the transparency option 
		}
	} else {
		if (Monochrome) {
			ROP |= 0x0B; //colour-expand normal
		} else {
			if (ReverseDir) {
				ROP |= 0x03; //set the reverse option
			} else {
				ROP |= 0x02; //standard block-move operation
			}
		}
	}

	_waitBusy(0x40); //Check that another BTE operation is not still in progress
	if (_textMode) _setTextMode(false);//we are in text mode?
	BTE_moveFrom(SourceX,SourceY);
	BTE_size(Width,Height);
	BTE_moveTo(DestX,DestY);
	BTE_ropcode(ROP);
	//Execute BTE! (This selects linear addressing mode for the monochrome source data)
	if (Monochrome) _writeRegister(RA8875_BECR0, 0xC0); else _writeRegister(RA8875_BECR0, 0x80);
	_waitBusy(0x40);
	//we are supposed to wait for the thing to become unbusy
	//caller can call _waitBusy(0x40) to check the BTE busy status (except it's private)
}

/**************************************************************************/
/*! TESTING

*/
/**************************************************************************/
void RA8875::BTE_size(int16_t w, int16_t h)
{
	//0.69b21 -have to check this, not verified
	if (_portrait) swapvals(w,h);
    _writeRegister(RA8875_BEWR0,w & 0xFF);//BET area width literacy  
    _writeRegister(RA8875_BEWR0+1,w >> 8);//BET area width literacy	   
    _writeRegister(RA8875_BEHR0,h & 0xFF);//BET area height literacy
    _writeRegister(RA8875_BEHR0+1,h >> 8);//BET area height literacy	   
}	

/**************************************************************************/
/*!

*/
/**************************************************************************/

void RA8875::BTE_moveFrom(int16_t SX,int16_t SY)
{
	if (_portrait) swapvals(SX,SY);
	_writeRegister(RA8875_HSBE0,SX & 0xFF);
	_writeRegister(RA8875_HSBE0+1,SX >> 8);
	_writeRegister(RA8875_VSBE0,SY & 0xFF);
	_writeRegister(RA8875_VSBE0+1,SY >> 8);
}	

/**************************************************************************/
/*!

*/
/**************************************************************************/

void RA8875::BTE_moveTo(int16_t DX,int16_t DY)
{
	if (_portrait) swapvals(DX,DY);
	_writeRegister(RA8875_HDBE0,DX & 0xFF);
	_writeRegister(RA8875_HDBE0+1,DX >> 8);
	_writeRegister(RA8875_VDBE0,DY & 0xFF);
	_writeRegister(RA8875_VDBE0+1,DY >> 8);
}	

/**************************************************************************/
/*! TESTING
	Use a ROP code EFX
*/
/**************************************************************************/
void RA8875::BTE_ropcode(unsigned char setx)
{
    _writeRegister(RA8875_BECR1,setx);//BECR1	   
}

/**************************************************************************/
/*! TESTING
	Enable BTE transfer
*/
/**************************************************************************/
void RA8875::BTE_enable(bool on) 
{	
	uint8_t temp = _readRegister(RA8875_BECR0);
	on == true ? temp &= ~(1 << 7) : temp |= (1 << 7);
	_writeData(temp);
	//_writeRegister(RA8875_BECR0,temp);  
	_waitBusy(0x40);
}


/**************************************************************************/
/*! TESTING
	Select BTE mode (CONT (continuous) or RECT)
*/
/**************************************************************************/
void RA8875::BTE_dataMode(enum RA8875btedatam m) 
{	
	uint8_t temp = _readRegister(RA8875_BECR0);
	m == CONT ? temp &= ~(1 << 6) : temp |= (1 << 6);
	_writeData(temp);
	//_writeRegister(RA8875_BECR0,temp);  
}

/**************************************************************************/
/*! TESTING
	Select the BTE SOURCE or DEST layer (1 or 2)
*/
/**************************************************************************/

void RA8875::BTE_layer(enum RA8875btelayer sd,uint8_t l)
{
	uint8_t temp;
	sd == SOURCE ? temp = _readRegister(RA8875_VSBE0+1) : temp = _readRegister(RA8875_VDBE0+1);
	l == 1 ? temp &= ~(1 << 7) : temp |= (1 << 7);
	_writeData(temp);
	//_writeRegister(RA8875_VSBE1,temp);  
}



/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								LAYER STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/**************************************************************************/
/*!
		Instruct the RA8875 chip to use 2 layers
		If resolution bring to restrictions it will switch to 8 bit
		so you can always use layers.
		Parameters:
		on: true (enable multiple layers), false (disable)
      
*/
/**************************************************************************/
void RA8875::useLayers(boolean on) 
{
	if (_useMultiLayers == on) return; //no reason to do change that it's already as desidered.
	//bool clearBuffer = false;
	if (_hasLayerLimits && _color_bpp > 8) { //try to set up 8bit color space
		setColorBpp(8);
		_waitBusy();
		_maxLayers = 2;
	}
	if (on){
		_useMultiLayers = true;
		_DPCR_Reg |= (1 << 7);
		//clearBuffer = true;
		clearActiveWindow(true);
	} else {
		_useMultiLayers = false;
		_DPCR_Reg &= ~(1 << 7);
		clearActiveWindow(false);
		
	}
	
	_writeRegister(RA8875_DPCR,_DPCR_Reg);
	if (!_useMultiLayers && _color_bpp < 16) setColorBpp(16);//bring color back to 16
	/*
	if (clearBuffer) { 
		clearWindow(true);
		//for some reason if you switch to multilayer the layer 2 has garbage better clear
		//writeTo(L2);//switch to layer 2
		//clearMemory(false);//clear memory of layer 2
		//clearWindow(false);
		//writeTo(L1);//switch to layer 1
	}
	*/
}


/**************************************************************************/
/*!

      
*/
/**************************************************************************/
void RA8875::layerEffect(enum RA8875boolean efx)
{
	uint8_t	reg = 0b00000000;
	//reg &= ~(0x07);//clear bit 2,1,0
	if (!_useMultiLayers) useLayers(true);//turn on multiple layers if it's off
	switch(efx){//                       bit 2,1,0 of LTPR0
		case LAYER1: //only layer 1 visible  [000]
			//do nothing
		break;
		case LAYER2: //only layer 2 visible  [001]
			reg |= (1 << 0);
		break;
		case TRANSPARENT: //transparent mode [011]
			reg |= (1 << 0); reg |= (1 << 1);
		break;
		case LIGHTEN: //lighten-overlay mode [010]
			reg |= (1 << 1);
		break;
		case OR: //boolean OR mode           [100]
			reg |= (1 << 2);
		break;
		case AND: //boolean AND mode         [101]
			reg |= (1 << 0); reg |= (1 << 2);
		break;
		case FLOATING: //floating windows    [110]
			reg |= (1 << 1); reg |= (1 << 2);
		break;
		default:
			//do nothing
		break;
	}
	_writeRegister(RA8875_LTPR0,reg);
}

/**************************************************************************/
/*!

      
*/
/**************************************************************************/
void RA8875::layerTransparency(uint8_t layer1,uint8_t layer2)
{
	if (layer1 > 8) layer1 = 8;
	if (layer2 > 8) layer2 = 8;
	if (!_useMultiLayers) useLayers(true);//turn on multiple layers if it's off
	//if (_useMultiLayers) _writeRegister(RA8875_LTPR1, ((layer2 & 0x0F) << 4) | (layer1 & 0x0F));
	//uint8_t res = 0b00000000;//RA8875_LTPR1
	//reg &= ~(0x07);//clear bit 2,1,0
	_writeRegister(RA8875_LTPR1, ((layer2 & 0xF) << 4) | (layer1 & 0xF));
}


/**************************************************************************/
/*! return the current drawing layer. If layers are OFF, return 255

*/
/**************************************************************************/
uint8_t RA8875::getCurrentLayer(void)
{
	if (!_useMultiLayers) return 255;
	return _currentLayer;
}

/**************************************************************************/
/*! select pattern

*/
/**************************************************************************/
void RA8875::setPattern(uint8_t num, enum RA8875pattern p)
{
	uint8_t maxLoc;
	uint8_t temp = 0b00000000;
	if (p != P16X16) {
		maxLoc = 16;//at 8x8 max 16 locations
	} else {
		maxLoc = 4;//at 16x16 max 4 locations
		temp |= (1 << 7);
	}
	if (num > (maxLoc - 1)) num = maxLoc - 1;
	temp = temp | num;
	writeTo(PATTERN);
	_writeRegister(RA8875_PTNO,temp);
}

/**************************************************************************/
/*! write pattern

*/
/**************************************************************************/
void RA8875::writePattern(int16_t x,int16_t y,const uint8_t *data,uint8_t size,bool setAW)
{
	int16_t i;
	int16_t a,b,c,d;
	if (size < 8 || size > 16) return;
	if (setAW) getActiveWindow(a,b,c,d);
	setActiveWindow(x,x+size-1,y,y+size-1);
	setXY(x,y);
	
	if (_textMode) _setTextMode(false);//we are in text mode?
	writeCommand(RA8875_MRWC);
	for (i=0;i<(size*size);i++) {
		_writeData(data[i*2]);
		_writeData(data[i*2+1]);
		_waitBusy(0x80);
	}
	if (setAW) setActiveWindow(a,b,c,d);//set as it was before
}

/**************************************************************************/
/*! This is the most important function to write on:
	LAYERS
	CGRAM
	PATTERN
	CURSOR
	Parameter:
	d (L1, L2, CGRAM, PATTERN, CURSOR)
	When writing on layers 0 or 1, if the layers are not enable it will enable automatically
	If the display doesn't support layers, it will automatically switch to 8bit color
	Remember that when layers are ON you need to disable manually, once that only Layer 1 will be visible

*/
/**************************************************************************/
void RA8875::writeTo(enum RA8875writes d)
{
	uint8_t temp = _readRegister(RA8875_MWCR1);
	//bool trigMultilayer = false;
	switch(d){
		case L1:
			temp &= ~((1<<3) | (1<<2));// Clear bits 3 and 2
			temp &= ~(1 << 0); //clear bit 0
			_currentLayer = 0;
			//trigMultilayer = true;
			_writeData(temp);  
			if (!_useMultiLayers) useLayers(true);
		break;
		case L2:
			temp &= ~((1<<3) | (1<<2));// Clear bits 3 and 2
			temp |= (1 << 0); //bit set 0
			_currentLayer = 1;
			//trigMultilayer = true;
			_writeData(temp);  
			if (!_useMultiLayers) useLayers(true);
		break;
		case CGRAM: 
			temp &= ~(1 << 3); //clear bit 3
			temp |= (1 << 2); //bit set 2
			if (bitRead(_FNCR0_Reg,7)){//REG[0x21] bit7 must be 0
				_FNCR0_Reg &= ~(1 << 7); //clear bit 7
				_writeRegister(RA8875_FNCR0,_FNCR0_Reg);  
				_writeRegister(RA8875_MWCR1,temp);
			} else {
				_writeData(temp);  
			}
		break;
		case PATTERN:
			temp |= (1 << 3); //bit set 3
			temp |= (1 << 2); //bit set 2
			_writeData(temp);  
		break;
		case CURSOR:
			temp |= (1 << 3); //bit set 3
			temp &= ~(1 << 2); //clear bit 2
			_writeData(temp);  
		break;
		default:
		//break;
		return;
	}
	//if (trigMultilayer && !_useMultiLayers) useLayers(true);//turn on multiple layers if it's off
	//_writeRegister(RA8875_MWCR1,temp);   
}


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+						GEOMETRIC PRIMITIVE  STUFF									 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/**************************************************************************/
/*!

*/
/**************************************************************************/
/* void RA8875::fillRect(void) {
	writeCommand(RA8875_DCR);
	_writeData(RA8875_DCR_LINESQUTRI_STOP | RA8875_DCR_DRAWSQUARE);
	_writeData(RA8875_DCR_LINESQUTRI_START | RA8875_DCR_FILL | RA8875_DCR_DRAWSQUARE);
}
 */
/**************************************************************************/
/*!
      Write a single pixel
	  Parameters:
	  x: horizontal pos
	  y: vertical pos
	  color: RGB565 color
	  NOTE:
	  In 8bit bpp RA8875 needs a 8bit color(332) and NOT a 16bit(565),
	  the routine deal with this...
*/
/**************************************************************************/
void RA8875::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	//setXY(x,y);
	if (_textMode) _setTextMode(false);//we are in text mode?
	setXY(x,y);
	writeCommand(RA8875_MRWC);
	if (_color_bpp > 8){
		writeData16(color); 
	} else {//TOTEST:layer bug workaround for 8bit color!
		_writeData(_color16To8bpp(color)); 
	}
}

/**************************************************************************/
/*!
      Draw a series of pixels
	  Parameters:
	  p: an array of 16bit colors (pixels)
	  count: how many pixels
	  x: horizontal pos
	  y: vertical pos
	  NOTE:
	  In 8bit bpp RA8875 needs a 8bit color(332) and NOT a 16bit(565),
	  the routine deal with this...
*/
/**************************************************************************/
void RA8875::drawPixels(uint16_t p[], uint16_t count, int16_t x, int16_t y)
{
    //setXY(x,y);
	uint16_t temp = 0;
	uint16_t i;
	if (_textMode) _setTextMode(false);//we are in text mode?
	setXY(x,y);
    writeCommand(RA8875_MRWC);
    _startSend();
	//set data
	#if defined(__AVR__) && defined(_FASTSSPORT)
		_spiwrite(RA8875_DATAWRITE);
	#else
		#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
			if (_altSPI){
				SPI1.transfer(RA8875_DATAWRITE);
			} else {
				SPI.transfer(RA8875_DATAWRITE);
			}
		#else
			SPI.transfer(RA8875_DATAWRITE);
		#endif
	#endif
	//the loop
	for (i=0;i<count;i++){
		if (_color_bpp < 16) {
			temp = _color16To8bpp(p[i]);//TOTEST:layer bug workaround for 8bit color!
		} else {
			temp = p[i];
		}
	#if !defined(ENERGIA) && !defined(___DUESTUFF) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
		#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
			if (_color_bpp > 8){
				if (_altSPI){
					SPI1.transfer16(temp);
				} else {
					SPI.transfer16(temp);
				}
			} else {//TOTEST:layer bug workaround for 8bit color!
				if (_altSPI){
					SPI1.transfer(temp);
				} else {
					SPI.transfer(temp & 0xFF);
				}
			}
		#else
			if (_color_bpp > 8){
				SPI.transfer16(temp);
			} else {//TOTEST:layer bug workaround for 8bit color!
				SPI.transfer(temp & 0xFF);
			}
		#endif
	#else
		#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
			if (_color_bpp > 8){
				SPI.transfer(_cs, temp >> 8, SPI_CONTINUE); 
				SPI.transfer(_cs, temp & 0xFF, SPI_LAST);
			} else {//TOTEST:layer bug workaround for 8bit color!
				SPI.transfer(_cs, temp & 0xFF, SPI_LAST);
			}
		#else
			#if defined(__AVR__) && defined(_FASTSSPORT)
				if (_color_bpp > 8){
					_spiwrite16(temp);
				} else {//TOTEST:layer bug workaround for 8bit color!
					_spiwrite(temp >> 8);
				}
			#else
				if (_color_bpp > 8){
					SPI.transfer(temp >> 8);
					SPI.transfer(temp & 0xFF);
				} else {//TOTEST:layer bug workaround for 8bit color!
					SPI.transfer(temp & 0xFF);
				}
			#endif
		#endif
	#endif
    }
    _endSend();
}


/**************************************************************************/
/*!
      Get a pixel color from screen
	  Parameters:
	  x: horizontal pos
	  y: vertical pos
*/
/**************************************************************************/
uint16_t RA8875::getPixel(int16_t x, int16_t y)
{
    uint16_t color;
    setXY(x,y);
	if (_textMode) _setTextMode(false);//we are in text mode?
    writeCommand(RA8875_MRWC);
	#if defined(_FASTCPU)
		_slowDownSPI(true);
	#endif
    _startSend();
	#if defined(__AVR__) && defined(_FASTSSPORT)
		_spiwrite(RA8875_DATAREAD);
		_spiwrite(0x00);
	#else
		#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
			if (_altSPI){
				SPI1.transfer(RA8875_DATAREAD);
				SPI1.transfer(0x00);//first byte it's dummy
			} else {
				SPI.transfer(RA8875_DATAREAD);
				SPI.transfer(0x00);//first byte it's dummy
			}
		#else
			SPI.transfer(RA8875_DATAREAD);
			SPI.transfer(0x00);//first byte it's dummy
		#endif
	#endif
	#if !defined(___DUESTUFF) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
		#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
			if (_altSPI){
				color  = SPI1.transfer16(0x0);
			} else {
				color  = SPI.transfer16(0x0);
			}
		#else
			color  = SPI.transfer16(0x0);
		#endif
	#else
		#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
			color  = SPI.transfer(_cs, 0x0, SPI_CONTINUE); 
			color |= (SPI.transfer(_cs, 0x0, SPI_LAST) << 8);
		#else
			#if defined(__AVR__) && defined(_FASTSSPORT)
				color  = _spiread();
				color |= (_spiread() << 8);
			#else
				color  = SPI.transfer(0x0);
				color |= (SPI.transfer(0x0) << 8);
			#endif
		#endif
	#endif
	#if defined(_FASTCPU)
		_slowDownSPI(false);
	#endif
    _endSend();
    return color;
}


/*
void RA8875::getPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y)
{
    uint16_t color; 
    if (_textMode) _setTextMode(false);//we are in text mode?
    setXY(x,y);
    writeCommand(RA8875_MRWC);
	#if defined(_FASTCPU)
		_slowDownSPI(true);
	#endif
    _startSend();
	SPI.transfer(RA8875_DATAREAD);
	#if !defined(ENERGIA) && !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
		SPI.transfer16(0x0);//dummy
	#else
		SPI.transfer(0x0);//dummy
		SPI.transfer(0x0);//dummy
	#endif
    while (count--) {
		#if !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
			color  = SPI.transfer16(0x0);
		#else
			color  = SPI.transfer(0x0);
			color |= (SPI.transfer(0x0) << 8);
		#endif
        *p++ = color;
    }
	#if defined(_FASTCPU)
		_slowDownSPI(false);
	#endif
    _endSend();
}
*/
/**************************************************************************/
/*!
      Basic line draw
	  Parameters:
	  x0: horizontal start pos
	  y0: vertical start
	  x1: horizontal end pos
	  y1: vertical end pos
	  color: RGB565 color
	  NOTE:
	  Remember that this write from->to so: drawLine(0,0,2,0,RA8875_RED);
	  result a 3 pixel long! (0..1..2)
*/
/**************************************************************************/
void RA8875::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	if (x0 == x1 && y0 == y1) {
		drawPixel(x0,y0,color);
		return;
	}
	//if ((x1 - x0 == 1) && (y1 - y0 == 1)) drawPixel(x0,y0,color);
	if (_portrait) { swapvals(x0,y0); swapvals(x1,y1);}
	if (_textMode) _setTextMode(false);//we are in text mode?
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTrecoverColor = true;
	#endif
	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid 3 useless SPI calls
	
	_line_addressing(x0,y0,x1,y1);
	
	_writeRegister(RA8875_DCR,0x80);
	_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Basic line by using Angle as parameter
	  Parameters:
	  x: horizontal start pos
	  y: vertical start
	  angle: the angle of the line
	  length: lenght of the line
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawLineAngle(int16_t x, int16_t y, int16_t angle, uint16_t length, uint16_t color,int offset)
{
	if (length < 2) {//NEW
		drawPixel(x,y,color);
	} else {
		drawLine(
		x,
		y,
		x + round((length-1) * _cosDeg_helper(angle + offset)),//_angle_offset
		y + round((length-1) * _sinDeg_helper(angle + offset)), 
		color);
	}
}

/**************************************************************************/
/*!
      Basic line by using Angle as parameter
	  Parameters:
	  x: horizontal start pos
	  y: vertical start
	  angle: the angle of the line
	  start: where line start
	  length: lenght of the line
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawLineAngle(int16_t x, int16_t y, int16_t angle, uint16_t start, uint16_t length, uint16_t color,int offset)
{
	if (start - length < 2) {//NEW
		drawPixel(x,y,color);
	} else {
		drawLine(
		x + round(start * _cosDeg_helper(angle + offset)),//_angle_offset
		y + round(start * _sinDeg_helper(angle + offset)),
		x + round((start + length-1) * _cosDeg_helper(angle + offset)),
		y + round((start + length-1) * _sinDeg_helper(angle + offset)), 
		color);
	}
}

void RA8875::roundGaugeTicker(uint16_t x, uint16_t y, uint16_t r, int from, int to, float dev,uint16_t color) 
{
  float dsec;
  int i;
  for (i = from; i <= to; i += 30) {
    dsec = i * (PI / 180);
    drawLine(
		x + (cos(dsec) * (r / dev)) + 1,
		y + (sin(dsec) * (r / dev)) + 1,
		x + (cos(dsec) * r) + 1,
		y + (sin(dsec) * r) + 1, 
		color);
  }
}

/**************************************************************************/
/*!
      for compatibility with popular Adafruit_GFX
	  draws a single vertical line
	  Parameters:
	  x: horizontal start
	  y: vertical start
	  h: height
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	if (h < 1) h = 1;
	h < 2 ? drawPixel(x,y,color) : drawLine(x, y, x, (y+h)-1, color);
}

/**************************************************************************/
/*!
      for compatibility with popular Adafruit_GFX
	  draws a single orizontal line
	  Parameters:
	  x: horizontal start
	  y: vertical start
	  w: width
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	if (w < 1) w = 1;
	w < 2 ? drawPixel(x,y,color) : drawLine(x, y, (w+x)-1, y, color);
}

/**************************************************************************/
/*!
	  draws a rectangle
	  Parameters:
	  x: horizontal start
	  y: vertical start
	  w: width
	  h: height
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	//RA8875 it's not out-of-range tolerant so this is a workaround
	if (w < 1 || h < 1) return;//it cannot be!
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
		_rect_helper(x,y,(w+x)-1,(h+y)-1,color,false);//thanks the experimentalist
	}
}

/**************************************************************************/
/*!
	  draws a FILLED rectangle
	  Parameters:
	  x: horizontal start
	  y: vertical start
	  w: width
	  h: height
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	//RA8875 it's not out-of-range tolerant so this is a workaround
	if (w < 1 || h < 1) return;//it cannot be!
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
		_rect_helper(x,y,(x+w)-1,(y+h)-1,color,true);//thanks the experimentalist
	}
}
/**************************************************************************/
/*!
	  calculate a grandient color
	  return a spectrum starting at blue to red (0...127)
*/
/**************************************************************************/
uint16_t RA8875::grandient(uint8_t val)
{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t q = val / 32;
	switch(q){
		case 0:
			r = 0; g = 2 * (val % 32); b = 31;
		break;
		case 1:
			r = 0; g = 63; b = 31 - (val % 32);
		break;
		case 2:
			r = val % 32; g = 63; b = 0;
		break;
		case 3:
			r = 31; g = 63 - 2 * (val % 32); b = 0;
		break;
	}
	return (r << 11) + (g << 5) + b;
}

/**************************************************************************/
/*!
	  interpolate 2 16bit colors
	  return a 16bit mixed color between the two
	  Parameters:
	  color1:
	  color2:
	  pos:0...div (mix percentage) (0:color1, div:color2)
	  div:divisions between color1 and color 2
*/
/**************************************************************************/
uint16_t RA8875::colorInterpolation(uint16_t color1,uint16_t color2,uint16_t pos,uint16_t div)
{
    if (pos == 0) return color1;
    if (pos >= div) return color2;
	uint8_t r1,g1,b1;
	Color565ToRGB(color1,r1,g1,b1);//split in r,g,b
	uint8_t r2,g2,b2;
	Color565ToRGB(color2,r2,g2,b2);//split in r,g,b
	return colorInterpolation(r1,g1,b1,r2,g2,b2,pos,div);
}

/**************************************************************************/
/*!
	  interpolate 2 r,g,b colors
	  return a 16bit mixed color between the two
	  Parameters:
	  r1.
	  g1:
	  b1:
	  r2:
	  g2:
	  b2:
	  pos:0...div (mix percentage) (0:color1, div:color2)
	  div:divisions between color1 and color 2
*/
/**************************************************************************/
uint16_t RA8875::colorInterpolation(uint8_t r1,uint8_t g1,uint8_t b1,uint8_t r2,uint8_t g2,uint8_t b2,uint16_t pos,uint16_t div)
{
    if (pos == 0) return Color565(r1,g1,b1);
    if (pos >= div) return Color565(r2,g2,b2);
	float pos2 = (float)pos/div;
	return Color565(
				(uint8_t)(((1.0 - pos2) * r1) + (pos2 * r2)),
				(uint8_t)((1.0 - pos2) * g1 + (pos2 * g2)),
				(uint8_t)(((1.0 - pos2) * b1) + (pos2 * b2))
	);
}
/**************************************************************************/
/*!
	  draws a dots filled area
	  Parameters:
	  x: horizontal origin
	  y: vertical origin
	  w: width
	  h: height
	  spacing: space between dots in pixels (min 2pix)
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawMesh(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t spacing, uint16_t color)
{
	if (spacing < 2) spacing = 2;
	if (((x + w) - 1) >= _width)  w = _width  - x;
	if (((y + h) - 1) >= _height) h = _height - y;
	
	int16_t n, m;

	if (w < x) {n = w; w = x; x = n;}
	if (h < y) {n = h; h = y; y = n;}
	for (m = y; m <= h; m += spacing) {
		for (n = x; n <= w; n += spacing) {
			drawPixel(n, m, color);
		}
	}
}
/**************************************************************************/
/*!
      Fill the ActiveWindow by using a specified RGB565 color
	  Parameters:
	  color: RGB565 color (default=BLACK)
*/
/**************************************************************************/
void RA8875::fillWindow(uint16_t color)
{  
	_line_addressing(0,0,RA8875_WIDTH-1, RA8875_HEIGHT-1);
	setForegroundColor(color);
	writeCommand(RA8875_DCR);
	_writeData(0xB0);
	_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTrecoverColor = true;
	#endif
}

/**************************************************************************/
/*!
      clearScreen it's different from fillWindow because it doesn't depends
	  from the active window settings so it will clear all the screen.
	  It should be used only when needed since it's slower than fillWindow.
	  parameter:
	  color: 16bit color (default=BLACK)
*/
/**************************************************************************/
void RA8875::clearScreen(uint16_t color)//0.69b24
{  
	setActiveWindow();
	fillWindow(color);
}

/**************************************************************************/
/*!
      Draw circle
	  Parameters:
      x0: The 0-based x location of the center of the circle
      y0: The 0-based y location of the center of the circle
      r: radius
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	_center_helper(x0,y0);
	if (r < 1) return;
	if (r < 2) {
		drawPixel(x0,y0,color);
		return;
	}
	_circle_helper(x0, y0, r, color, false);
}

/**************************************************************************/
/*!
      Draw filled circle
	  Parameters:
      x0: The 0-based x location of the center of the circle
      y0: The 0-based y location of the center of the circle
      r: radius
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	_center_helper(x0,y0);
	if (r <= 0) return;
	_circle_helper(x0, y0, r, color, true);
}

/**************************************************************************/
/*!
      Draw a quadrilater by connecting 4 points
	  Parameters:
	  x0:
	  y0:
	  x1:
	  y1:
	  x2:
	  y2:
	  x3:
	  y3:
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2,int16_t x3, int16_t y3, uint16_t color) 
{
	drawLine(x0, y0, x1, y1, color);//low 1
	drawLine(x1, y1, x2, y2, color);//high 1
	drawLine(x2, y2, x3, y3, color);//high 2
	drawLine(x3, y3, x0, y0, color);//low 2
}


/**************************************************************************/
/*!
      Draw a filled quadrilater by connecting 4 points
	  Parameters:
	  x0:
	  y0:
	  x1:
	  y1:
	  x2:
	  y2:
	  x3:
	  y3:
      color: RGB565 color
	  triangled: if true a full quad will be generated, false generate a low res quad (faster)
	  *NOTE: a bug in _triangle_helper create some problem, still fixing....
*/
/**************************************************************************/
void RA8875::fillQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color, bool triangled) 
{
	  _triangle_helper(x0, y0, x1, y1, x2, y2, color,true);
	  if (triangled) _triangle_helper(x2, y2, x3, y3, x0, y0, color,true);
      _triangle_helper(x1, y1, x2, y2, x3, y3, color,true);
}

/**************************************************************************/
/*!
      Draw a polygon from a center
	  Parameters:
	  cx: x center of the polygon
	  cy: y center of the polygon
	  sides: how many sides (min 3)
	  diameter: diameter of the polygon
	  rot: angle rotation of the polygon
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color)
{ 
	_center_helper(cx,cy);
	sides = (sides > 2? sides : 3);
	float dtr = (PI / 180.0) + PI;
	float rads = 360.0 / sides;//points spacd equally
	uint8_t i;
	for (i = 0; i < sides; i++) { 
		drawLine(
			cx + (sin((i*rads + rot) * dtr) * diameter),
			cy + (cos((i*rads + rot) * dtr) * diameter),
			cx + (sin(((i+1)*rads + rot) * dtr) * diameter),
			cy + (cos(((i+1)*rads + rot) * dtr) * diameter),
			color);
	}
}


/**************************************************************************/
/*!
      ringMeter 
	  (adapted from Alan Senior (thanks man!))
	  it create a ring meter with a lot of personalizations,
	  it return the width of the gauge so you can use this value
	  for positioning other gauges near the one just created easily
	  Parameters:
	  val:  your value
	  minV: the minimum value possible
	  maxV: the max value possible
	  x:    the position on x axis
	  y:    the position on y axis
	  r:    the radius of the gauge (minimum 50)
	  units: a text that shows the units, if "none" all text will be avoided
	  scheme:0...7 or 16 bit color (not BLACK or WHITE)
	  0:red
	  1:green
	  2:blue
	  3:blue->red
	  4:green->red
	  5:red->green
	  6:red->green->blue
	  7:cyan->green->red
	  8:black->white linear interpolation
	  9:violet->yellow linear interpolation
	  or
      RGB565 color (not BLACK or WHITE)
	  backSegColor: the color of the segments not active (default BLACK)
	  angle:		90 -> 180 (the shape of the meter, 90:halfway, 180:full round, 150:default)
	  inc: 			5...20 (5:solid, 20:sparse divisions, default:10)
*/
/**************************************************************************/
void RA8875::ringMeter(int val, int minV, int maxV, int16_t x, int16_t y, uint16_t r, const char* units, uint16_t colorScheme,uint16_t backSegColor,int16_t angle,uint8_t inc)
{
	if (inc < 5) inc = 5;
	if (inc > 20) inc = 20;
	if (r < 50) r = 50;
	if (angle < 90) angle = 90;
	if (angle > 180) angle = 180;
	int curAngle = map(val, minV, maxV, -angle, angle);
	uint16_t colour;
	x += r;
	y += r;   // Calculate coords of centre of ring
	uint16_t w = r / 4;    // Width of outer ring is 1/4 of radius
	const uint8_t seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
	// Draw colour blocks every inc degrees
	for (int16_t i = -angle; i < angle; i += inc) {
		colour = RA8875_BLACK;
		switch (colorScheme) {
			case 0:
				colour = RA8875_RED;
				break; // Fixed colour
			case 1:
				colour = RA8875_GREEN;
				break; // Fixed colour
			case 2:
				colour = RA8875_BLUE;
				break; // Fixed colour
			case 3:
				colour = grandient(map(i, -angle, angle, 0, 127));
				break; // Full spectrum blue to red
			case 4:
				colour = grandient(map(i, -angle, angle, 63, 127));
				break; // Green to red (high temperature etc)
			case 5:
				colour = grandient(map(i, -angle, angle, 127, 63));
				break; // Red to green (low battery etc)
			case 6:
				colour = grandient(map(i, -angle, angle, 127, 0));
				break; // Red to blue (air cond reverse)
			case 7:
				colour = grandient(map(i, -angle, angle, 35, 127));
				break; // cyan to red 
			case 8:
				colour = colorInterpolation(0,0,0,255,255,255,map(i,-angle,angle,0,w),w);
				break; // black to white
			case 9:
				colour = colorInterpolation(0x80,0,0xC0,0xFF,0xFF,0,map(i,-angle,angle,0,w),w);
				break; // violet to yellow
			default:
				if (colorScheme > 9){
					colour = colorScheme;
				} else {
					colour = RA8875_BLUE;
				}
				break; // Fixed colour
		}
		// Calculate pair of coordinates for segment start
		float xStart = cos((i - 90) * 0.0174532925);
		float yStart = sin((i - 90) * 0.0174532925);
		uint16_t x0 = xStart * (r - w) + x;
		uint16_t y0 = yStart * (r - w) + y;
		uint16_t x1 = xStart * r + x;
		uint16_t y1 = yStart * r + y;

		// Calculate pair of coordinates for segment end
		float xEnd = cos((i + seg - 90) * 0.0174532925);
		float yEnd = sin((i + seg - 90) * 0.0174532925);
		int16_t x2 = xEnd * (r - w) + x;
		int16_t y2 = yEnd * (r - w) + y;
		int16_t x3 = xEnd * r + x;
		int16_t y3 = yEnd * r + y;

		if (i < curAngle) { // Fill in coloured segments with 2 triangles
			fillQuad(x0, y0, x1, y1, x2, y2, x3, y3, colour, false);
		} else {// Fill in blank segments
			fillQuad(x0, y0, x1, y1, x2, y2, x3, y3, backSegColor, false);
		}
	}

	// text
	if (strcmp(units, "none") != 0){
		//erase internal background
		if (angle > 90) {
			fillCircle(x, y, r - w, _backColor); 
		} else {
			fillCurve(x, y + getFontHeight() / 2, r - w, r - w, 1, _backColor);
			fillCurve(x, y + getFontHeight() / 2, r - w, r - w, 2, _backColor);
		}
		//prepare for write text
		if (r > 84) {
			setFontScale(1);
		} else {
			setFontScale(0);
		}
		if (_portrait){
			setCursor(y, x - 15, true);
		} else {
			setCursor(x - 15, y, true);
		}
		print(val);
		print(" ");
		print(units);
	}

	// Calculate and return right hand side x coordinate
	//return x + r;
	
}

/**************************************************************************/
/*!
      Draw Triangle
	  Parameters:
      x0: The 0-based x location of the point 0 of the triangle bottom LEFT
      y0: The 0-based y location of the point 0 of the triangle bottom LEFT
      x1: The 0-based x location of the point 1 of the triangle middle TOP
      y1: The 0-based y location of the point 1 of the triangle middle TOP
      x2: The 0-based x location of the point 2 of the triangle bottom RIGHT
      y2: The 0-based y location of the point 2 of the triangle bottom RIGHT
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	_triangle_helper(x0, y0, x1, y1, x2, y2, color, false);
}

/**************************************************************************/
/*!
      Draw filled Triangle
	  Parameters:
      x0: The 0-based x location of the point 0 of the triangle
      y0: The 0-based y location of the point 0 of the triangle
      x1: The 0-based x location of the point 1 of the triangle
      y1: The 0-based y location of the point 1 of the triangle
      x2: The 0-based x location of the point 2 of the triangle
      y2: The 0-based y location of the point 2 of the triangle
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	_triangle_helper(x0, y0, x1, y1, x2, y2, color, true);
}

/**************************************************************************/
/*!
      Draw an ellipse
	  Parameters:
      xCenter:   x location of the center of the ellipse
      yCenter:   y location of the center of the ellipse
      longAxis:  Size in pixels of the long axis
      shortAxis: Size in pixels of the short axis
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
	_ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, 255, color, false);
}

/**************************************************************************/
/*!
      Draw a filled ellipse
	  Parameters:
      xCenter:   x location of the center of the ellipse
      yCenter:   y location of the center of the ellipse
      longAxis:  Size in pixels of the long axis
      shortAxis: Size in pixels of the short axis
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
	_ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, 255, color, true);
}

/**************************************************************************/
/*!
      Draw a curve
      Parameters:
      xCenter:]   x location of the ellipse center
      yCenter:   y location of the ellipse center
      longAxis:  Size in pixels of the long axis
      shortAxis: Size in pixels of the short axis
      curvePart: Curve to draw in clock-wise dir: 0[180-270°],1[270-0°],2[0-90°],3[90-180°]
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
	curvePart = curvePart % 4; //limit to the range 0-3
	if (_portrait) {//fix a problem with rotation
		if (curvePart == 0) {
			curvePart = 2;
		} else if (curvePart == 2) {
			curvePart = 0;
		}
	}
	_ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
}

/**************************************************************************/
/*!
      Draw a filled curve
      Parameters:
      xCenter:]   x location of the ellipse center
      yCenter:   y location of the ellipse center
      longAxis:  Size in pixels of the long axis
      shortAxis: Size in pixels of the short axis
      curvePart: Curve to draw in clock-wise dir: 0[180-270°],1[270-0°],2[0-90°],3[90-180°]
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
	curvePart = curvePart % 4; //limit to the range 0-3
	if (_portrait) {//fix a problem with rotation
		if (curvePart == 0) {
			curvePart = 2;
		} else if (curvePart == 2) {
			curvePart = 0;
		}
	}
	_ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
}

/**************************************************************************/
/*!
      Draw a rounded rectangle
	  Parameters:
      x:   x location of the rectangle
      y:   y location of the rectangle
      w:  the width in pix
      h:  the height in pix
	  r:  the radius of the rounded corner
      color: RGB565 color
	  _roundRect_helper it's not tolerant to improper values
	  so there's some value check here
*/
/**************************************************************************/
void RA8875::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	if (r == 0) drawRect(x,y,w,h,color);
	if (w < 1 || h < 1) return;//it cannot be!
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
		if (w < h && (r * 2) >= w) r = (w / 2) - 1;
		if (w > h && (r * 2) >= h) r = (h / 2) - 1;
		if (r == w || r == h) drawRect(x,y,w,h,color);
		_roundRect_helper(x, y, (x + w) - 1, (y + h) - 1, r, color, false);
	}
}


/**************************************************************************/
/*!
      Draw a filled rounded rectangle
	  Parameters:
      x:   x location of the rectangle
      y:   y location of the rectangle
      w:  the width in pix
      h:  the height in pix
	  r:  the radius of the rounded corner
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	if (r == 0) fillRect(x,y,w,h,color);
	if (w < 1 || h < 1) return;//it cannot be!
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
		if (w < h && (r * 2) >= w) r = (w / 2) - 1;
		if (w > h && (r  *2) >= h) r = (h / 2) - 1;
		if (r == w || r == h) drawRect(x,y,w,h,color);
		_roundRect_helper(x, y, (x + w) - 1, (y + h) - 1, r, color, true);
	}
}



/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+					GEOMETRIC PRIMITIVE HELPERS STUFF								 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/**************************************************************************/
/*!
      helper function for circles
	  [private]
*/
/**************************************************************************/
void RA8875::_circle_helper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled)//0.69b32 fixed an undocumented hardware limit
{
	if (_portrait) swapvals(x0,y0);//0.69b21

	if (r < 1) r = 1;
	if (r < 2) {//NEW
		drawPixel(x0,y0,color);
		return;
	}
	if (r > RA8875_HEIGHT / 2) r = (RA8875_HEIGHT / 2) - 1;//this is the (undocumented) hardware limit of RA8875
	
	if (_textMode) _setTextMode(false);//we are in text mode?
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTrecoverColor = true;
	#endif
	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid several SPI calls
	
	_writeRegister(RA8875_DCHR0,    x0 & 0xFF);
	_writeRegister(RA8875_DCHR0 + 1,x0 >> 8);

	_writeRegister(RA8875_DCVR0,    y0 & 0xFF);
	_writeRegister(RA8875_DCVR0 + 1,y0 >> 8);	   
	_writeRegister(RA8875_DCRR,r); 

	writeCommand(RA8875_DCR);
	#if defined(_FASTCPU)
		_slowDownSPI(true);
	#endif
	filled == true ? _writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL) : _writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
	_waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);//ZzZzz
	#if defined(_FASTCPU)
		_slowDownSPI(false);
	#endif
}


/**************************************************************************/
/*!
		helper function for rects (filled or not)
		[private]
*/
/**************************************************************************/
void RA8875::_rect_helper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled)
{
	if (w < 1 || h < 1) return;//why draw invisible rects?
	if (w >= _width) return;
	if (h >= _height) return;
	
	if (_portrait) {swapvals(x,y); swapvals(w,h);}

	_checkLimits_helper(x,y);

	if (_textMode) _setTextMode(false);//we are in text mode?
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTrecoverColor = true;
	#endif
	if (color != _foreColor) setForegroundColor(color);
	
	_line_addressing(x,y,w,h);

	writeCommand(RA8875_DCR);
	filled == true ? _writeData(0xB0) : _writeData(0x90);
	_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}


/**************************************************************************/
/*!
      helper function for triangles
	  [private]
*/
/**************************************************************************/
void RA8875::_triangle_helper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled)
{
	if (x0 >= _width || x1 >= _width || x2 >= _width) return;
	if (y0 >= _height || y1 >= _height || y2 >= _height) return;
	
	if (_portrait) {swapvals(x0,y0); swapvals(x1,y1); swapvals(x2,y2);}
	
	if (x0 == x1 && y0 == y1){
		drawLine(x0, y0, x2, y2,color);
		return;
	} else if (x0 == x2 && y0 == y2){
		drawLine(x0, y0, x1, y1,color);
		return;
	} else if (x0 == x1 && y0 == y1 && x0 == x2 && y0 == y2) {//new
        drawPixel(x0, y0, color);
		return;
	}
	
	if (y0 > y1) {swapvals(y0, y1); swapvals(x0, x1);}
	
    if (y1 > y2) {swapvals(y2, y1); swapvals(x2, x1);}
	
    if (y0 > y1) {swapvals(y0, y1); swapvals(x0, x1);}
	
	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		int16_t a, b;
        a = b = x0;
		if (x1 < a) {     
			a = x1;
		} else if (x1 > b) {
			b = x1;
		}
        if (x2 < a) { 
			a = x2;
		} else if (x2 > b) {
			b = x2;
		}
        drawFastHLine(a, y0, b-a+1, color);
        return;
    }
	
	if (_textMode) _setTextMode(false);//we are in text mode?
	
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTrecoverColor = true;
	#endif
	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid several SPI calls
	
	//_checkLimits_helper(x0,y0);
	//_checkLimits_helper(x1,y1);
	
	_line_addressing(x0,y0,x1,y1);
	//p2

	_writeRegister(RA8875_DTPH0,    x2 & 0xFF);
	_writeRegister(RA8875_DTPH0 + 1,x2 >> 8);
	_writeRegister(RA8875_DTPV0,    y2 & 0xFF);
	_writeRegister(RA8875_DTPV0 + 1,y2 >> 8);
	
	writeCommand(RA8875_DCR);
	filled == true ? _writeData(0xA1) : _writeData(0x81);
	
	_waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      helper function for ellipse and curve
	  [private]
*/
/**************************************************************************/
void RA8875::_ellipseCurve_helper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart,uint16_t color, bool filled)
{
	_center_helper(xCenter,yCenter);//use CENTER?
	
	if (_portrait) {
		swapvals(xCenter,yCenter);
		swapvals(longAxis,shortAxis);
		if (longAxis > _height/2) longAxis = (_height / 2) - 1;
		if (shortAxis > _width/2) shortAxis = (_width / 2) - 1;
	} else {
		if (longAxis > _width/2) longAxis = (_width / 2) - 1;
		if (shortAxis > _height/2) shortAxis = (_height / 2) - 1;
	}
	if (longAxis == 1 && shortAxis == 1) {
		drawPixel(xCenter,yCenter,color);
		return;
	}
	_checkLimits_helper(xCenter,yCenter);
	
	if (_textMode) _setTextMode(false);//we are in text mode?
	
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTrecoverColor = true;
	#endif
	if (color != _foreColor) setForegroundColor(color);
	
	_curve_addressing(xCenter,yCenter,longAxis,shortAxis);
	writeCommand(RA8875_ELLIPSE);
	
	if (curvePart != 255){
		curvePart = curvePart % 4; //limit to the range 0-3
		filled == true ? _writeData(0xD0 | (curvePart & 0x03)) : _writeData(0x90 | (curvePart & 0x03));
	} else {
		filled == true ? _writeData(0xC0) : _writeData(0x80);
	}
	_waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}



/**************************************************************************/
/*!
	  helper function for rounded Rects
	  PARAMETERS
	  x:
	  y:
	  w:
	  h:
	  r:
	  color:
	  filled:
	  [private]
*/
/**************************************************************************/
void RA8875::_roundRect_helper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled)
{
	if (_portrait) {swapvals(x,y); swapvals(w,h);}
	
	if (_textMode) _setTextMode(false);//we are in text mode?
	
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_TXTrecoverColor = true;
	#endif
	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid several SPI calls

	
	_line_addressing(x,y,w,h);

	_writeRegister(RA8875_ELL_A0,    r & 0xFF);
	_writeRegister(RA8875_ELL_A0 + 1,r >> 8);
	_writeRegister(RA8875_ELL_B0,    r & 0xFF);
	_writeRegister(RA8875_ELL_B0 + 1,r >> 8);

	writeCommand(RA8875_ELLIPSE);
	filled == true ? _writeData(0xE0) : _writeData(0xA0);
	_waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
	  helper function for draw arcs in degrees
	  DrawArc function thanks to Jnmattern and his Arc_2.0 (https://github.com/Jnmattern)
	  Adapted for DUE by Marek Buriak https://github.com/marekburiak/ILI9341_Due
	  Re-Adapted for this library by sumotoy
	  PARAMETERS
	  cx: center x
	  cy: center y
	  radius: the radius of the arc
	  thickness:
	  start: where arc start in degrees
	  end:	 where arc end in degrees
	  color:
	  [private]
*/
/**************************************************************************/
void RA8875::_drawArc_helper(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
	
	//_center_helper(cx,cy);//use CENTER?
	int16_t xmin = 65535, xmax = -32767, ymin = 32767, ymax = -32767;
	float cosStart, sinStart, cosEnd, sinEnd;
	float r, t;
	float startAngle, endAngle;

	startAngle = (start / _arcAngle_max) * 360;	// 252
	endAngle = (end / _arcAngle_max) * 360;		// 807

	while (startAngle < 0)   startAngle += 360;
	while (endAngle < 0)     endAngle += 360;
	while (startAngle > 360) startAngle -= 360;
	while (endAngle > 360)   endAngle -= 360;


	if (startAngle > endAngle) {
		_drawArc_helper(cx, cy, radius, thickness, ((startAngle) / (float)360) * _arcAngle_max, _arcAngle_max, color);
		_drawArc_helper(cx, cy, radius, thickness, 0, ((endAngle) / (float)360) * _arcAngle_max, color);
	} else {
		//if (_textMode) _setTextMode(false);//we are in text mode?
		// Calculate bounding box for the arc to be drawn
		cosStart = _cosDeg_helper(startAngle);
		sinStart = _sinDeg_helper(startAngle);
		cosEnd = _cosDeg_helper(endAngle);
		sinEnd = _sinDeg_helper(endAngle);

		r = radius;
		// Point 1: radius & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 2: radius & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		r = radius - thickness;
		// Point 3: radius-thickness & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 4: radius-thickness & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;
		// Corrections if arc crosses X or Y axis
		if ((startAngle < 90) && (endAngle > 90)) ymax = radius;
		if ((startAngle < 180) && (endAngle > 180)) xmin = -radius;
		if ((startAngle < 270) && (endAngle > 270)) ymin = -radius;

		// Slopes for the two sides of the arc
		float sslope = (float)cosStart / (float)sinStart;
		float eslope = (float)cosEnd / (float)sinEnd;
		if (endAngle == 360) eslope = -1000000;
		int ir2 = (radius - thickness) * (radius - thickness);
		int or2 = radius * radius;
		for (int x = xmin; x <= xmax; x++) {
			bool y1StartFound = false, y2StartFound = false;
			bool y1EndFound = false, y2EndSearching = false;
			int y1s = 0, y1e = 0, y2s = 0;//, y2e = 0;
			for (int y = ymin; y <= ymax; y++) {
				int x2 = x * x;
				int y2 = y * y;

				if (
					(x2 + y2 < or2 && x2 + y2 >= ir2) && (
					(y > 0 && startAngle < 180 && x <= y * sslope) ||
					(y < 0 && startAngle > 180 && x >= y * sslope) ||
					(y < 0 && startAngle <= 180) ||
					(y == 0 && startAngle <= 180 && x < 0) ||
					(y == 0 && startAngle == 0 && x > 0)
					) && (
					(y > 0 && endAngle < 180 && x >= y * eslope) ||
					(y < 0 && endAngle > 180 && x <= y * eslope) ||
					(y > 0 && endAngle >= 180) ||
					(y == 0 && endAngle >= 180 && x < 0) ||
					(y == 0 && startAngle == 0 && x > 0)))
				{
					if (!y1StartFound) {	//start of the higher line found
						y1StartFound = true;
						y1s = y;
					} else if (y1EndFound && !y2StartFound) {//start of the lower line found
						y2StartFound = true;
						y2s = y;
						y += y1e - y1s - 1;	// calculate the most probable end of the lower line (in most cases the length of lower line is equal to length of upper line), in the next loop we will validate if the end of line is really there
						if (y > ymax - 1) {// the most probable end of line 2 is beyond ymax so line 2 must be shorter, thus continue with pixel by pixel search
							y = y2s;	// reset y and continue with pixel by pixel search
							y2EndSearching = true;
						}
					} else if (y2StartFound && !y2EndSearching) {
						// we validated that the probable end of the lower line has a pixel, continue with pixel by pixel search, in most cases next loop with confirm the end of lower line as it will not find a valid pixel
						y2EndSearching = true;
					}
				} else {
					if (y1StartFound && !y1EndFound) {//higher line end found
						y1EndFound = true;
						y1e = y - 1;
						drawFastVLine(cx + x, cy + y1s, y - y1s, color);
						if (y < 0) {
							y = abs(y); // skip the empty middle
						}
						else
							break;
					} else if (y2StartFound) {
						if (y2EndSearching) {
							// we found the end of the lower line after pixel by pixel search
							drawFastVLine(cx + x, cy + y2s, y - y2s, color);
							y2EndSearching = false;
							break;
						} else {
							// the expected end of the lower line is not there so the lower line must be shorter
							y = y2s;	// put the y back to the lower line start and go pixel by pixel to find the end
							y2EndSearching = true;
						}
					}
				}
			}
			if (y1StartFound && !y1EndFound){
				y1e = ymax;
				drawFastVLine(cx + x, cy + y1s, y1e - y1s + 1, color);
			} else if (y2StartFound && y2EndSearching)	{// we found start of lower line but we are still searching for the end
														// which we haven't found in the loop so the last pixel in a column must be the end
				drawFastVLine(cx + x, cy + y2s, ymax - y2s + 1, color);
			}
		}
	}
}


/**************************************************************************/
/*!
		this update the RA8875 Active Window registers
		[private]
*/
/**************************************************************************/
void RA8875::_updateActiveWindow(bool full)
{
	if (full){
		// X
		_writeRegister(RA8875_HSAW0,    0x00);
		_writeRegister(RA8875_HSAW0 + 1,0x00);   
		_writeRegister(RA8875_HEAW0,    (RA8875_WIDTH) & 0xFF);
		_writeRegister(RA8875_HEAW0 + 1,(RA8875_WIDTH) >> 8);
		// Y 
		_writeRegister(RA8875_VSAW0,    0x00);
		_writeRegister(RA8875_VSAW0 + 1,0x00); 
		_writeRegister(RA8875_VEAW0,    (RA8875_HEIGHT) & 0xFF); 
		_writeRegister(RA8875_VEAW0 + 1,(RA8875_HEIGHT) >> 8);
	} else {
		// X
		_writeRegister(RA8875_HSAW0,    _activeWindowXL & 0xFF);
		_writeRegister(RA8875_HSAW0 + 1,_activeWindowXL >> 8);   
		_writeRegister(RA8875_HEAW0,    _activeWindowXR & 0xFF);
		_writeRegister(RA8875_HEAW0 + 1,_activeWindowXR >> 8);
		// Y 
		_writeRegister(RA8875_VSAW0,     _activeWindowYT & 0xFF);
		_writeRegister(RA8875_VSAW0 + 1,_activeWindowYT >> 8); 
		_writeRegister(RA8875_VEAW0,    _activeWindowYB & 0xFF); 
		_writeRegister(RA8875_VEAW0 + 1,_activeWindowYB >> 8);
	}
}

/**************************************************************************/
/*!
		Graphic line addressing helper
		[private]
*/
/**************************************************************************/
void RA8875::_line_addressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	//X0
	_writeRegister(RA8875_DLHSR0,    x0 & 0xFF);
	_writeRegister(RA8875_DLHSR0 + 1,x0 >> 8);
	//Y0
	_writeRegister(RA8875_DLVSR0,    y0 & 0xFF);
	_writeRegister(RA8875_DLVSR0 + 1,y0 >> 8);
	//X1
	_writeRegister(RA8875_DLHER0,    x1 & 0xFF);
	_writeRegister(RA8875_DLHER0 + 1,x1 >> 8);
	//Y1
	_writeRegister(RA8875_DLVER0,    y1 & 0xFF);
	_writeRegister(RA8875_DLVER0 + 1,y1 >> 8);
}

/**************************************************************************/
/*!	
		curve addressing helper
		[private]
*/
/**************************************************************************/
void RA8875::_curve_addressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	//center
	_writeRegister(RA8875_DEHR0,    x0 & 0xFF);
	_writeRegister(RA8875_DEHR0 + 1,x0 >> 8);
	_writeRegister(RA8875_DEVR0,    y0 & 0xFF);
	_writeRegister(RA8875_DEVR0 + 1,y0 >> 8);
	//long,short ax
	_writeRegister(RA8875_ELL_A0,    x1 & 0xFF);
	_writeRegister(RA8875_ELL_A0 + 1,x1 >> 8);
	_writeRegister(RA8875_ELL_B0,    y1 & 0xFF);
	_writeRegister(RA8875_ELL_B0 + 1,y1 >> 8);
}


/**************************************************************************/
/*!	
		sin e cos helpers
		[private]
*/
/**************************************************************************/
float RA8875::_cosDeg_helper(float angle)
{
	float radians = angle / (float)360 * 2 * PI;
	return cos(radians);
}

float RA8875::_sinDeg_helper(float angle)
{
	float radians = angle / (float)360 * 2 * PI;
	return sin(radians);
}

/**************************************************************************/
/*!	
		change the arc default parameters
*/
/**************************************************************************/
void RA8875::setArcParams(float arcAngleMax, int arcAngleOffset)
{
	_arcAngle_max = arcAngleMax;
	_arcAngle_offset = arcAngleOffset;
}

/**************************************************************************/
/*!	
		change the angle offset parameter from default one
*/
/**************************************************************************/
void RA8875::setAngleOffset(int16_t angleOffset)
{
	_angle_offset = ANGLE_OFFSET + angleOffset;
}


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								PWM STUFF											 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/**************************************************************************/
/*!
		on/off GPIO (basic for Adafruit module
*/
/**************************************************************************/
void RA8875::GPIOX(boolean on) 
{
    _writeRegister(RA8875_GPIOX, on);
}

/**************************************************************************/
/*!
		PWM out
		Parameters:
		pw: pwm selection (1,2)
		p: 0...255 rate
		
*/
/**************************************************************************/
void RA8875::PWMout(uint8_t pw,uint8_t p)
{
	uint8_t reg;
	pw > 1 ? reg = RA8875_P2DCR : reg = RA8875_P1DCR;
	_writeRegister(reg, p);
}

/**************************************************************************/
/*!
		Set the brightness of the backlight (if connected to pwm)
		(basic controls pwm 1)
		Parameters:
		val: 0...255
*/
/**************************************************************************/
void RA8875::brightness(uint8_t val) 
{
	_brightness = val;
	PWMout(1,_brightness);
}

/**************************************************************************/
/*!
		controls the backligh by using PWM engine.
		It handles adafruit board separately
		Parameters:
		on: true(backlight on), false(backlight off)
*/
/**************************************************************************/
void RA8875::backlight(boolean on) //0.69b31 (fixed an issue with adafruit backlight)
{
	if (_displaySize == Adafruit_480x272 || _displaySize == Adafruit_800x480) GPIOX(on);
	if (on == true){
		PWMsetup(1,true, RA8875_PWM_CLK_DIV1024);//setup PWM ch 1 for backlight
		PWMout(1,_brightness);//turn on PWM1
	} else {
		PWMsetup(1,false, RA8875_PWM_CLK_DIV1024);//setup PWM ch 1 for backlight
	}
}

/**************************************************************************/
/*!
		Setup PWM engine
		Parameters:
		pw: pwm selection (1,2)
		on: turn on/off
		clock: the clock setting
		[private]
*/
/**************************************************************************/
void RA8875::PWMsetup(uint8_t pw,boolean on, uint8_t clock) 
{
	uint8_t reg;
	uint8_t set;
	if (pw > 1){
		reg = RA8875_P2CR;
		on == true ? set = RA8875_PxCR_ENABLE : set = RA8875_PxCR_DISABLE;
	} else {
		reg = RA8875_P1CR;
		on == true ? set = RA8875_PxCR_ENABLE : set = RA8875_PxCR_DISABLE;
	}
	_writeRegister(reg,(set | (clock & 0xF)));
}

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								         ISR										 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

void RA8875::setInternalInt(enum RA8875intlist b)
{
	_enabledInterrups |= (1 << b);//set
}

void RA8875::clearInternalInt(enum RA8875intlist b)
{
	_enabledInterrups &= ~(1 << b);//clear
}

/**************************************************************************/
/*!
	RA8875 can use the INT pin for many purposes.
	This function just tell the CPU to use a pin for listen to RA8875 pin,
	no matter if we will use ISR or DigitalRead
------------------------------
Bit:	Called by:		In use:
--------------------------------
0: 		isr triggered	[*]
1: 		Resistive TS	[*]
2: 		KeyScan			[*]
3: 		DMA				 	
4: 		BTE				 
5: 		-na-		
6: 		-na-			 
7: 		-na-			 
--------------------------------
	Parameters:
	INTpin: it's the pin where we listen to ISR
	INTnum: it's the INT related to pin. On some processor it's not needed
	This last parameter it's used only when decide to use an ISR.
*/
/**************************************************************************/

void RA8875::useINT(const uint8_t INTpin,const uint8_t INTnum) 
{
	_intPin = INTpin;
	_intNum = INTnum;
	#if defined(___TEENSYES)//all of them (32 bit only)
		pinMode(_intPin ,INPUT_PULLUP);
	#else
		pinMode(_intPin ,INPUT);
	#endif
}

/**************************************************************************/
/*!
		the generic ISR routine, will set to 1 bit 0 of _RA8875_INTS
		[private]
*/
/**************************************************************************/
void RA8875::_isr(void)
{
	_RA8875_INTS |= (1 << 0);//set
}

/**************************************************************************/
/*!
		Enable the ISR, after this any falling edge on
		_intPin pin will trigger ISR
		Parameters:
		force: if true will force attach interrupt
		NOTE:
		if parameter _needISRrearm = true will rearm interrupt
*/
/**************************************************************************/
void RA8875::enableISR(bool force) 
{
	if (force || _needISRrearm){
		_needISRrearm = false;
		#ifdef digitalPinToInterrupt
			attachInterrupt(digitalPinToInterrupt(_intPin),_isr,FALLING);
		#else
			attachInterrupt(_intNum,_isr,FALLING);
		#endif
		_RA8875_INTS = 0b00000000;//reset all INT bits flags
		_useISR = true;
	}
	#if defined(USE_RA8875_TOUCH)
		if (_touchEnabled) _checkInterrupt(2);//clear internal RA int to engage
	#endif
}

/**************************************************************************/
/*!
		Disable ISR
		Works only if previously enabled or do nothing.
*/
/**************************************************************************/
void RA8875::_disableISR(void) 
{
	if (_useISR){
		#if defined(USE_RA8875_TOUCH)
			if (_touchEnabled) _checkInterrupt(2);//clear internal RA int to engage
		#endif
		#ifdef digitalPinToInterrupt
			detachInterrupt(digitalPinToInterrupt(_intPin));
		#else
			detachInterrupt(_intNum);
		#endif
		_RA8875_INTS = 0b00000000;//reset all bits
		_useISR = false;
	}
}

/**************************************************************************/
/*!
		Check the [interrupt register] for an interrupt,
		if found it will reset it.
		bit
		0: complicated....
		1: BTE INT
		2: TOUCH INT
		3: DMA INT
		4: Keyscan INT
*/
/**************************************************************************/
bool RA8875::_checkInterrupt(uint8_t _bit,bool _clear) 
{
	if (_bit > 4) _bit = 4;
	uint8_t temp = _readRegister(RA8875_INTC2);
	if (bitRead(temp,_bit) == 1){
		if (_clear){
			temp |= (1 << _bit);//bitSet(temp,_bit);
			//if (bitRead(temp,0)) bitSet(temp,0);//Mmmmm...
			_writeRegister(RA8875_INTC2, temp);//clear int
		}
		return true;
	}
	return false;
}

#if defined(USE_FT5206_TOUCH)
/**************************************************************************/
/*!
	The FT5206 Capacitive Touch driver uses a different INT pin than RA8875
	and it's not controlled by RA chip of course, so we need a separate code
	for that purpose, no matter we decide to use an ISR or digitalRead.
	no matter if we will use ISR or DigitalRead
	Parameters:
	INTpin: it's the pin where we listen to ISR
	INTnum: it's the INT related to pin. On some processor it's not needed
	This last parameter it's used only when decide to use an ISR.
*/
/**************************************************************************/
void RA8875::useCapINT(const uint8_t INTpin,const uint8_t INTnum) 
{
	_intCTSPin = INTpin;
	_intCTSNum = INTnum;
	#if defined(___TEENSYES)//all of them (32 bit only)
		pinMode(_intCTSPin ,INPUT_PULLUP);
	#else
		pinMode(_intCTSPin ,INPUT);
	#endif
}

/**************************************************************************/
/*!
		Since FT5206 uses a different INT pin, we need a separate isr routine. 
		[private]
*/
/**************************************************************************/
void RA8875::cts_isr(void)
{
	_FT5206_INT = true;
}

/**************************************************************************/
/*!
		Enable the ISR, after this any falling edge on
		_intCTSPin pin will trigger ISR
		Parameters:
		force: if true will force attach interrup
		NOTE:
		if parameter _needCTS_ISRrearm = true will rearm interrupt
*/
/**************************************************************************/
void RA8875::enableCapISR(bool force) 
{
	if (force || _needCTS_ISRrearm){
		_needCTS_ISRrearm = false;
		#ifdef digitalPinToInterrupt
			attachInterrupt(digitalPinToInterrupt(_intCTSPin),cts_isr,FALLING);
		#else
			attachInterrupt(_intCTSNum,cts_isr,FALLING);
		#endif
		_FT5206_INT = false;
		_useISR = true;
	}
}

/**************************************************************************/
/*!
		Disable ISR [FT5206 version]
		Works only if previously enabled or do nothing.
*/
/**************************************************************************/
void RA8875::_disableCapISR(void) 
{
	if (_useISR){
		#ifdef digitalPinToInterrupt
			detachInterrupt(digitalPinToInterrupt(_intCTSPin));
		#else
			detachInterrupt(_intCTSNum);
		#endif
		_FT5206_INT = false;
		_useISR = false;
	}
}
#endif

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+							     TOUCH SCREEN COMMONS						         +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#if !defined(_AVOID_TOUCHSCREEN)//Common touch screend methods (for capacitive and resistive)

/**************************************************************************/
/*!
		Checks an interrupt has occurred. return true if yes.
		Designed to run in loop.
		It works with ISR or DigitalRead methods
		Parameters:
		safe: true  (detach interrupt routine, has to be re-engaged manually!)
			  false (
*/
/**************************************************************************/
bool RA8875::touched(bool safe)
{
	if (_useISR){//using interrupts
		#if defined(USE_FT5206_TOUCH)
			_needCTS_ISRrearm = safe;
			if (_FT5206_INT)			{
		#elif defined(USE_RA8875_TOUCH)
			_needISRrearm = safe;
			if (bitRead(_RA8875_INTS,0)){
		#endif
			//there was an interrupt
				#if defined(USE_FT5206_TOUCH)
					if (_needCTS_ISRrearm){//safe was true, detach int
						_disableCapISR();
					#else
					if (_needISRrearm){//safe was true, detach int
						_disableISR();
					#endif
					} else {//safe was false, do not detatch int and clear INT flag
						#if defined(USE_FT5206_TOUCH)
							_FT5206_INT = false;
						#elif defined(USE_RA8875_TOUCH)
							_RA8875_INTS &= ~(1 << 0);//clear
							_checkInterrupt(2);//clear internal RA int to re-engage
						#endif
					}
					return true;
			}
			return false;
	} else {//not use ISR, digitalRead method
			#if defined(USE_FT5206_TOUCH)
			//TODO
			
			#elif defined(USE_RA8875_TOUCH)
				if (_touchEnabled){
					#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
						if (!digitalReadFast(_intPin)) {
					#else
						if (!digitalRead(_intPin)) {
					#endif
							_clearTInt = true;
							if (_checkInterrupt(2)){
								return true;
							} else {
								return false;
							}
						}//digitalRead
					
						if (_clearTInt){
							_clearTInt = false;
							_checkInterrupt(2);//clear internal RA int
							delay(1);
						}
					
						return false;
					
				} else {//_touchEnabled
					return false;
				}
			#endif
			return false;
	}
}

void RA8875::setTouchLimit(uint8_t limit)
{
	#if defined(USE_FT5206_TOUCH)
	if (limit > 5) limit = 5;//max 5 allowed
	#else
		limit = 1;//always 1
	#endif
	_maxTouch = limit;
}

uint8_t RA8875::getTouchLimit(void)
{
	return _maxTouch;
}
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+				CAPACITIVE TOUCH SCREEN CONTROLLER	FT5206						     +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#if defined(USE_FT5206_TOUCH)


/**************************************************************************/
/*!
		Initialization sequence of the FT5206
*/
/**************************************************************************/
void RA8875::_initializeFT5206(void)
{
	uint8_t i;
	for (i=0x80;i<=0x89;i++){
		_sendRegFT5206(i,_FT5206REgisters[i-0x80]);
	}
	_sendRegFT5206(0x00,0x00);//Device Mode
}

/**************************************************************************/
/*!
		Communicate with FT5206
*/
/**************************************************************************/
void RA8875::_sendRegFT5206(uint8_t reg,const uint8_t val)
{
    // save I2C bitrate
	#if !defined(___DUESTUFF)
		uint8_t twbrbackup = TWBR;
		TWBR = 12; // upgrade to 400KHz!
	#endif
	Wire.beginTransmission(_ctpAdrs);
	Wire.write(reg);
	Wire.write(val);
	Wire.endTransmission(_ctpAdrs);
	#if !defined(___DUESTUFF)
		TWBR = twbrbackup;
	#endif
}


/**************************************************************************/
/*!
		This is the function that update the current state of the Touch Screen
		It's developed for use in loop
*/
/**************************************************************************/
void RA8875::updateTS(void)
{
    Wire.requestFrom((uint8_t)_ctpAdrs,(uint8_t)28); //get 28 registers
    uint8_t index = 0;
    while(Wire.available()) {
      _cptRegisters[index++] = Wire.read();//fill registers
    }
	_currentTouches = _cptRegisters[0x02] & 0xF;
	if (_currentTouches > _maxTouch) _currentTouches = _maxTouch;
	_gesture = _cptRegisters[0x01];
	if (_maxTouch < 2) _gesture = 0;
	uint8_t temp = _cptRegisters[0x03];
	_currentTouchState = 0;
	if (!bitRead(temp,7) && bitRead(temp,6)) _currentTouchState = 1;//finger up
	if (bitRead(temp,7) && !bitRead(temp,6)) _currentTouchState = 2;//finger down
}

/**************************************************************************/
/*!
		It gets coordinates out from data collected by updateTS function
		Actually it will not communicate with FT5206, just reorder collected data
		so it MUST be used after updateTS!
*/
/**************************************************************************/
uint8_t RA8875::getTScoordinates(uint16_t (*touch_coordinates)[2])
{
	uint8_t i;
	if (_currentTouches < 1) return 0;
 	for (i=1;i<=_currentTouches;i++){
		switch(_rotation){
			case 0://ok
				//touch_coordinates[i-1][0] = _width - (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]) / (4096/_width);
				//touch_coordinates[i-1][1] = (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]) / (4096/_height);
				touch_coordinates[i-1][0] = ((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1];
				touch_coordinates[i-1][1] = ((_cptRegisters[coordRegStart[i-1] + 2] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 3];
			break;
			case 1://ok
				touch_coordinates[i-1][0] = (((_cptRegisters[coordRegStart[i-1] + 2] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 3]);
				touch_coordinates[i-1][1] = (RA8875_WIDTH - 1) - (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]);
			break;
			case 2://ok
				touch_coordinates[i-1][0] = (RA8875_WIDTH - 1) - (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]);
				touch_coordinates[i-1][1] = (RA8875_HEIGHT - 1) -(((_cptRegisters[coordRegStart[i-1] + 2] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 3]);
			break;
			case 3://ok
				touch_coordinates[i-1][0] = (RA8875_HEIGHT - 1) - (((_cptRegisters[coordRegStart[i-1] + 2] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 3]);
				touch_coordinates[i-1][1] = (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]);
			break;
		}
		if (i == _maxTouch) return i;
	} 
    return _currentTouches;
}

/**************************************************************************/
/*!
		Gets the current Touch State, must be used AFTER updateTS!
*/
/**************************************************************************/
uint8_t RA8875::getTouchState(void)
{
	return _currentTouchState;
}

/**************************************************************************/
/*!
		Gets the number of touches, must be used AFTER updateTS!
		Return 0..5
		0: no touch
*/
/**************************************************************************/
uint8_t RA8875::getTouches(void)
{
	return _currentTouches;
}

/**************************************************************************/
/*!
		Gets the gesture, if identified, must be used AFTER updateTS!
*/
/**************************************************************************/
uint8_t RA8875::getGesture(void)
{
	return _gesture;
}

#elif defined(USE_RA8875_TOUCH)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+						RESISTIVE TOUCH STUFF										 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


/**************************************************************************/
/*!   
	  Initialize support for on-chip resistive Touch Screen controller
	  It also enable the Touch Screen
	  NOTE:
	  You need to use useINT(pin) [before] to define an INT pin!
	  You need to use enableISR() [after] to enable ISR on MCU and RA8875!
*/
/**************************************************************************/
void RA8875::touchBegin(void) 
{
	if (_intPin < 255){
/*		Touch Panel Control Register 0  TPCR0  [0x70]
		7: 0(disable, 1:(enable)
		6,5,4:TP Sample Time Adjusting (000...111)
		3:Touch Panel Wakeup Enable 0(disable),1(enable)
		2,1,0:ADC Clock Setting (000...111) set fixed to 010: (System CLK) / 4, 10Mhz Max! */
		#if defined(___TEENSYES) ||  defined(___DUESTUFF)//fast 32 bit processors
			_writeRegister(RA8875_TPCR0, TP_ENABLE | TP_ADC_SAMPLE_16384_CLKS | TP_ADC_CLKDIV_32);
		#else
			_writeRegister(RA8875_TPCR0, TP_ENABLE | TP_ADC_SAMPLE_4096_CLKS | TP_ADC_CLKDIV_16);
		#endif
		_writeRegister(RA8875_TPCR1, TP_MODE_AUTO | TP_DEBOUNCE_ON);
		setInternalInt(TOUCH);
		_INTC1_Reg |= (1 << 2);//set
		_writeRegister(RA8875_INTC1, _INTC1_Reg);
		_touchEnabled = true;
	} else {
		_touchEnabled = false;
	}
}

/**************************************************************************/
/*! 
      Enables or disables the on-chip touch screen controller
	  You must use touchBegin at list once to instruct the RA8875
	  Parameters:
	  enabled: true(enable),false(disable)
*/
/**************************************************************************/
void RA8875::touchEnable(boolean enabled) {
	if (_intPin < 255){
		/* another grrrr bug of the RA8875!
		if we are in text mode the RA chip cannot get back the
		INT mode!
		*/
		if (_textMode) _setTextMode(false);
		if (!_touchEnabled && enabled) {//Enable
			//enableISR(true);
			// bitSet(_INTC1_Reg,2);
			// _writeRegister(RA8875_INTC1, _INTC1_Reg);
			_touchEnabled = true;
			_checkInterrupt(2);
		} else if (_touchEnabled && !enabled) {//disable
			//_disableISR();
			// bitClear(_INTC1_Reg,2);
			// _writeRegister(RA8875_INTC1, _INTC1_Reg);
			_checkInterrupt(2);
			_touchEnabled = false;
		}
	} else {
		_touchEnabled = false;
	}
}



/**************************************************************************/
/*!   
	  Read 10bit internal ADC of RA8875 registers and perform corrections
	  It will return always RAW data
	  Parameters:
	  x: out 0...1023
	  Y: out 0...1023

*/
/**************************************************************************/
void RA8875::readTouchADC(uint16_t *x, uint16_t *y) 
{
	#if defined(_FASTCPU)
		_slowDownSPI(true);
	#endif
	uint16_t tx =  _readRegister(RA8875_TPXH);
	uint16_t ty =  _readRegister(RA8875_TPYH);
	uint8_t remain = _readRegister(RA8875_TPXYL);
	#if defined(_FASTCPU)
		_slowDownSPI(false);
	#endif
	tx <<= 2;
	ty <<= 2;
	tx |= remain & 0x03;        // get the bottom x bits
	ty |= (remain >> 2) & 0x03; // get the bottom y bits
	  if (_portrait){
		*x = ty;
		*y = tx;
	  } else {
		tx = 1024 - tx;
		ty = 1024 - ty;
		*x = tx;
		*y = ty;
	  }
}

/**************************************************************************/
/*!   
	  Returns 10bit x,y data with TRUE scale (0...1023)
	  Parameters:
	  x: out 0...1023
	  Y: out 0...1023
*/
/**************************************************************************/
void RA8875::touchReadAdc(uint16_t *x, uint16_t *y) 
{
	uint16_t tx,ty;
	readTouchADC(&tx,&ty);
	#if (defined(TOUCSRCAL_XLOW) && (TOUCSRCAL_XLOW != 0)) || (defined(TOUCSRCAL_XHIGH) && (TOUCSRCAL_XHIGH != 0))
		*x = map(tx,_tsAdcMinX,_tsAdcMaxX,0,1024);
	#else
		*x = tx;
	#endif
	#if (defined(TOUCSRCAL_YLOW) && (TOUCSRCAL_YLOW != 0)) || (defined(TOUCSRCAL_YHIGH) && (TOUCSRCAL_YHIGH != 0))
		*y = map(ty,_tsAdcMinY,_tsAdcMaxY,0,1024);
	#else
		*y = ty;
	#endif
	_checkInterrupt(2);
}

/**************************************************************************/
/*!   
	  Returns pixel x,y data with SCREEN scale (screen width, screen Height)
	  Parameters:
	  x: out 0...screen width  (pixels)
	  Y: out 0...screen Height (pixels)
	  Check for out-of-bounds here as touches near the edge of the screen
	  can be safely mapped to the nearest point of the screen.
	  If the screen is rotated, then the min and max will be modified elsewhere
	  so that this always corresponds to screen pixel coordinates.
	  /M.SANDERSCROCK added constrain
*/
/**************************************************************************/
void RA8875::touchReadPixel(uint16_t *x, uint16_t *y) 
{
	uint16_t tx,ty;
	readTouchADC(&tx,&ty);
	//*x = map(tx,_tsAdcMinX,_tsAdcMaxX,0,_width-1);
	*x = constrain(map(tx,_tsAdcMinX,_tsAdcMaxX,0,_width-1),0,_width-1);
	//*y = map(ty,_tsAdcMinY,_tsAdcMaxY,0,_height-1);
	*y = constrain(map(ty,_tsAdcMinY,_tsAdcMaxY,0,_height-1),0,_height-1);
	_checkInterrupt(2);
}

boolean RA8875::touchCalibrated(void) 
{
	return _calibrated;
}

/**************************************************************************/
/*!   A service utility that detects if system has been calibrated in the past
	  Return true if an old calibration exists
	  [private]
*/
/**************************************************************************/
boolean RA8875::_isCalibrated(void) 
{
	uint8_t uncaltetection = 4;
	#if defined(TOUCSRCAL_XLOW) && (TOUCSRCAL_XLOW != 0)
		uncaltetection--;
	#endif
	#if defined(TOUCSRCAL_YLOW) && (TOUCSRCAL_YLOW != 0)
		uncaltetection--;
	#endif
	#if defined(TOUCSRCAL_XHIGH) && (TOUCSRCAL_XHIGH != 0)
		uncaltetection--;
	#endif
	#if defined(TOUCSRCAL_YHIGH) && (TOUCSRCAL_YHIGH != 0)
		uncaltetection--;
	#endif
	if (uncaltetection < 4) return true;
	return false;
}

#endif
#endif

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								  SLEEP STUFF										 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/**************************************************************************/
/*!
    Sleep mode on/off (complete sequence)
	The sleep on/off sequence it's quite tricky on RA8875 when in SPI mode!
*/
/**************************************************************************/
void RA8875::sleep(boolean sleep) 
{
	if (_sleep != sleep){//only if it's needed
		_sleep = sleep;
		if (_sleep == true){
			//1)turn off backlight
			if (_displaySize == Adafruit_480x272 || _displaySize == Adafruit_800x480/* || _displaySize == Adafruit_640x480*/) GPIOX(false);
			//2)decelerate SPI clock
			#if defined(_FASTCPU)
				_slowDownSPI(true);
			#else
				#if defined(SPI_HAS_TRANSACTION)
					_SPImaxSpeed = 4000000UL;
				#else
					#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
						SPI.setClockDivider(_cs,SPI_SPEED_READ);
					#else
						SPI.setClockDivider(SPI_SPEED_READ);
					#endif
				#endif
			#endif
			//3)set PLL to default
			_setSysClock(0x07,0x03,0x02);
			//4)display off & sleep
			_writeRegister(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
			delay(100);
		} else {
			//1)wake up with display off(100ms)
			_writeRegister(RA8875_PWRR, RA8875_PWRR_DISPOFF);
			delay(100);
			//2)bring back the pll
			_setSysClock(initStrings[_initIndex][0],initStrings[_initIndex][1],initStrings[_initIndex][2]);
			//_writeRegister(RA8875_PCSR,initStrings[_initIndex][2]);//Pixel Clock Setting Register
			delay(20);
			_writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);//disp on
			delay(20);
			//4)resume SPI speed
			#if defined(_FASTCPU)
				_slowDownSPI(false);
			#else
				#if defined(SPI_HAS_TRANSACTION)
					#if defined(__MKL26Z64__)
						if (_altSPI){
							_SPImaxSpeed = MAXSPISPEED2;
						} else {
							_SPImaxSpeed = MAXSPISPEED;
						}
					#else			
						_SPImaxSpeed = MAXSPISPEED;
					#endif
				#else
					#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
						SPI.setClockDivider(_cs,SPI_SPEED_WRITE);
					#else
						SPI.setClockDivider(SPI_SPEED_WRITE);
					#endif
				#endif
			#endif
			//5)PLL afterburn!
			_setSysClock(sysClockPar[_initIndex][0],sysClockPar[_initIndex][1],initStrings[_initIndex][2]);
			//5)turn on backlight
			if (_displaySize == Adafruit_480x272 || _displaySize == Adafruit_800x480/* || _displaySize == Adafruit_640x480*/) GPIOX(true);
			//_writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL);
		}
	}
}



/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+							SPI & LOW LEVEL STUFF									 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/**************************************************************************/
/*! PRIVATE
		Write in a register
		Parameters:
		reg: the register
		val: the data
*/
/**************************************************************************/
void RA8875::_writeRegister(const uint8_t reg, uint8_t val) 
{
	writeCommand(reg);
	_writeData(val);
}

/**************************************************************************/
/*! PRIVATE
		Returns the value inside register
		Parameters:
		reg: the register
*/
/**************************************************************************/
uint8_t RA8875::_readRegister(const uint8_t reg) 
{
	writeCommand(reg);
	return _readData(false);
}

/**************************************************************************/
/*!
		Write data
		Parameters:
		d: the data
*/
/**************************************************************************/
void RA8875::_writeData(uint8_t data) 
{
	_startSend();
	#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
		SPI.transfer(_cs, RA8875_DATAWRITE, SPI_CONTINUE); 
		SPI.transfer(_cs, data, SPI_LAST);
	#else
		#if defined(__AVR__) && defined(_FASTSSPORT)
			_spiwrite(RA8875_DATAWRITE);
			_spiwrite(data);
		#else
			#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
				if (_altSPI){
					SPI1.transfer(RA8875_DATAWRITE);
					SPI1.transfer(data);
				} else {
					SPI.transfer(RA8875_DATAWRITE);
					SPI.transfer(data);
				}
			#else
				SPI.transfer(RA8875_DATAWRITE);
				SPI.transfer(data);
			#endif
		#endif
	#endif
	_endSend();
}

/**************************************************************************/
/*! 
		Write 16 bit data
		Parameters:
		d: the data (16 bit)
*/
/**************************************************************************/
void  RA8875::writeData16(uint16_t data) 
{
	_startSend();
	#if defined(__AVR__) && defined(_FASTSSPORT)
		_spiwrite(RA8875_DATAWRITE);
	#else
		#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
			if (_altSPI){
				SPI1.transfer(RA8875_DATAWRITE);
			} else {
				SPI.transfer(RA8875_DATAWRITE);
			}
		#else
			SPI.transfer(RA8875_DATAWRITE);
		#endif
	#endif
	#if !defined(ENERGIA) && !defined(___DUESTUFF) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
		#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
			if (_altSPI){
				SPI1.transfer16(data);
			} else {
				SPI.transfer16(data);
			}
		#else
			SPI.transfer16(data);
		#endif
	#else
		#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
			SPI.transfer(_cs, highByte(data), SPI_CONTINUE); 
			SPI.transfer(_cs, lowByte(data), SPI_LAST);
		#else
			#if defined(__AVR__) && defined(_FASTSSPORT)
				_spiwrite16(data);
			#else
				SPI.transfer(data >> 8);
				SPI.transfer(data & 0xFF);
			#endif
		#endif
	#endif
	_endSend();
}

/**************************************************************************/
/*!	PRIVATE

*/
/**************************************************************************/
uint8_t RA8875::_readData(bool stat) 
{
	#if defined(SPI_HAS_TRANSACTION)
		if (_inited) _SPImaxSpeed = _SPImaxSpeed/2;
	#else
		#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
			if (_inited) SPI.setClockDivider(_cs,SPI_SPEED_READ);
		#else
			if (_inited) SPI.setClockDivider(SPI_SPEED_READ);
		#endif
	#endif
	_startSend();
	#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
		stat == true ? SPI.transfer(_cs, RA8875_CMDREAD, SPI_CONTINUE) : SPI.transfer(_cs, RA8875_DATAREAD, SPI_CONTINUE);
		uint8_t x = SPI.transfer(_cs, 0x0, SPI_LAST);
	#else
		#if defined(__AVR__) && defined(_FASTSSPORT)
			stat == true ? _spiwrite(RA8875_CMDREAD) : _spiwrite(RA8875_DATAREAD);
			uint8_t x = _spiread();
		#else
			#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
				uint8_t x;
				if (_altSPI){
					stat == true ? SPI1.transfer(RA8875_CMDREAD) : SPI1.transfer(RA8875_DATAREAD);
					x = SPI1.transfer(0x0);
				} else {
					stat == true ? SPI.transfer(RA8875_CMDREAD) : SPI.transfer(RA8875_DATAREAD);
					x = SPI.transfer(0x0);
				}
			#else
				stat == true ? SPI.transfer(RA8875_CMDREAD) : SPI.transfer(RA8875_DATAREAD);
				uint8_t x = SPI.transfer(0x0);
			#endif
		#endif
	#endif
	_endSend();
	#if defined(SPI_HAS_TRANSACTION)
		if (_inited) _SPImaxSpeed = _SPImaxSpeed*2;
	#else
		#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
			if (_inited) SPI.setClockDivider(_cs,SPI_SPEED_WRITE);
		#else
			if (_inited) SPI.setClockDivider(SPI_SPEED_WRITE);
		#endif
	#endif
	return x;

}

/**************************************************************************/
/*!

*/
/**************************************************************************/
uint8_t	RA8875::readStatus(void) 
{
	return _readData(true);
}

/**************************************************************************/
/*! PRIVATE
		Write a command
		Parameters:
		d: the command
*/
/**************************************************************************/
void RA8875::writeCommand(const uint8_t d) 
{
	_startSend();
	#if defined(___DUESTUFF) && defined(SPI_DUE_MODE_EXTENDED)
		SPI.transfer(_cs, RA8875_CMDWRITE, SPI_CONTINUE); 
		SPI.transfer(_cs, d, SPI_LAST);
	#else
		#if defined(__AVR__) && defined(_FASTSSPORT)
			_spiwrite(RA8875_CMDWRITE);
			_spiwrite(d);
		#else
			#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
				if (_altSPI){
					SPI1.transfer(RA8875_CMDWRITE);
					SPI1.transfer(d);
				} else {
					SPI.transfer(RA8875_CMDWRITE);
					SPI.transfer(d);
				}
			#else
				SPI.transfer(RA8875_CMDWRITE);
				SPI.transfer(d);
			#endif
		#endif
	#endif
	_endSend();
}


/*
 void RA8875::debugData(uint16_t data,uint8_t len)
{
	int i;
	for (i=len-1; i>=0; i--){
		if (bitRead(data,i)==1){
			Serial.print("1");
		} else {
			Serial.print("0");
		}
	}
	Serial.print(" -> 0x");
	Serial.print(data,HEX);
	Serial.print("\n");
} 
*/

/*
 void RA8875::showLineBuffer(uint8_t data[],int len)
{
	int i;
	for (i=0; i<len; i++){
		if (data[i] == 1){
			Serial.print("1");
		} else {
			Serial.print("0");
		}
	}
	Serial.print("\n");
} 
*/
