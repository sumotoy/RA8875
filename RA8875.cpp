#include <SPI.h>
#include "RA8875.h"

#if defined (USE_FT5206_TOUCH)
	#include "Wire.h"
	static volatile boolean _touched = false;
	const uint8_t _ctpAdrs = 0x38;
	const uint8_t coordRegStart[5] = {0x03,0x09,0x0F,0x15,0x1B};
#endif


/**************************************************************************/
/*!
	Contructor
	CS: SPI SS pin
	RST: Reset pin
	If Teensy CS used you can choose the SPI0 or SPI1 
	spiInterface: 0 or 1 - default 0
	SPI1:[MOSI1(0) MISO1(1) CLK1(20) CS1(6)]
*/
/**************************************************************************/
//------------------------------TEENSY 3/3.1 ---------------------------------------
#if defined(__MK20DX128__) || defined(__MK20DX256__)
	#if defined (USE_FT5206_TOUCH)
		RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,const uint8_t INTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
		{
			_ctpInt = INTp;
	#else		
		RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
		{
			//uint8_t INTp = 255;
	#endif
			_mosi = mosi_pin;
			_miso = miso_pin;
			_sclk = sclk_pin;
			_cs = CSp;
			_rst = RSTp;
//------------------------------Teensy LC-------------------------------------------
#elif defined(__MKL26Z64__)
	#if defined (USE_FT5206_TOUCH)
		//RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,const uint8_t INTp,bool altSPI)
		//{
		RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,const uint8_t INTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
		{
			_ctpInt = INTp;
	#else
		//RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,bool altSPI)
		//{
		RA8875::RA8875(const uint8_t CSp,const uint8_t RSTp,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin)
		{
			//uint8_t INTp = 255;
	#endif
			_mosi = mosi_pin;
			_miso = miso_pin;
			_sclk = sclk_pin;
			_cs = CSp;
			_rst = RSTp;
			_altSPI = false;
//---------------------------------DUE--------------------------------------------
#elif defined(__SAM3X8E__)//DUE
	#if defined (USE_FT5206_TOUCH)
		RA8875::RA8875(const uint8_t CSp, const uint8_t RSTp,const uint8_t INTp) 
		{
			_ctpInt = INTp;
	#else
		RA8875::RA8875(const uint8_t CSp, const uint8_t RSTp) 
		{
			_cs = CSp;
			_rst = RSTp;
	#endif
//------------------------------ENERGIA-------------------------------------------
#elif defined(NEEDS_SET_MODULE)
	#if defined (USE_FT5206_TOUCH)
		RA8875::RA8875(const uint8_t module, const uint8_t RSTp,const uint8_t INTp) 
		{
			_ctpInt = INTp;
	#else
		RA8875::RA8875(const uint8_t module, const uint8_t RSTp) 
		{
			//uint8_t INTp = 255;
	#endif
			selectCS(module);
			_rst = RSTp;
			_cs = 255;
//----------------------------8 BIT ARDUINO's---------------------------------------
#else
	#if defined (USE_FT5206_TOUCH)
		RA8875::RA8875(const uint8_t CSp, const uint8_t RSTp,const uint8_t INTp) 
		{
			_ctpInt = INTp;
	#else
		RA8875::RA8875(const uint8_t CSp, const uint8_t RSTp) 
		{
			//uint8_t INTp = 255;
	#endif
			_cs = CSp;
			_rst = RSTp;
#endif
}

/**************************************************************************/
/*!	
	Helper, it will set CS pin accordly module selected
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
	RA8875_320x240 (exists?)
    RA8875_480x272 (4.3" displays)
    RA8875_800x480 (5" and 7" displays)
	Adafruit_480x272 (4.3" Adafruit displays)
	Adafruit_680x480 (4.3" exists?)
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
	_size = s;
	_rotation = 0;
	_portrait = false;
	_inited = false;
	_sleep = false;
	_hasLayerLimits = false;
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_textForeColor = _RA8875_DEFAULTTXTFRGRND;
		#if defined(_RA8875_DEFAULTTXTBKGRND)
			_textBackColor = _RA8875_DEFAULTTXTBKGRND;
		#else
			_textBackColor = 0x0000;
		#endif
		_recoverTextColor = false;
	#endif
	_maxLayers = 2;
	_currentLayer = 0;
	_useMultiLayers = false;//starts with one layer only
	_currentMode = false;
	_brightness = 255;
	_cursorX = 0; _cursorY = 0; _scrollXL = 0; _scrollXR = 0; _scrollYT = 0; _scrollYB = 0;
	_textSize = X16;
	_fontSpacing = 0;
	/* set-->  _commonTextPar  <--
	0:_extFontRom = false;
	1:_autoAdvance = true;
	2:_textWrap = user defined
	3:_fontFullAlig = false;
	4:_fontRotation = false;
	5:_alignXToCenter = false;
	6:_alignYToCenter = false;
	7: - reserved -
	*/
	_commonTextPar = 0b00000010;
	bitWrite(_commonTextPar,2,_DFT_RA8875_TEXTWRAP);//set _textWrap
	_centerFlag = false;
	_autocenter = false;
	_fontRomType = _DFT_RA8875_EXTFONTROMTYPE;
	_fontRomCoding = _DFT_RA8875_EXTFONTROMCODING;
	_fontSource = INT;
	_fontInterline = 0;
	_fontFamily = STANDARD;
	_textCursorStyle = NOCURSOR;
	_arcAngleMax = ARC_ANGLE_MAX;
	_arcAngleOffset = ARC_ANGLE_OFFSET;
	_color_bpp = 16;
	_colorIndex = 0;

	if (colors != 16) {
		_color_bpp = 8;
		_colorIndex = 3;
	}

	switch (_size){
		case RA8875_480x272:
		case Adafruit_480x272:
			_width = 480;
			_height = 272;
			_initIndex = 0;
		break;
		case RA8875_800x480:
		case Adafruit_800x480:
		case RA8875_800x480ALT:
			_width = 800;
			_height = 480;
			_hasLayerLimits = true;
			_maxLayers = 1;
			if (_color_bpp < 16) _maxLayers = 2;
			_initIndex = 1;
			if (_size == RA8875_800x480ALT) _initIndex = 2;
		break;
		default:
		_errorCode |= (1 << 0);//set
		return;
	}
	WIDTH = _width;
	HEIGHT = _height;
	_activeWindowXL = 0;
	_activeWindowXR = WIDTH;
	_activeWindowYT = 0;
	_activeWindowYB = HEIGHT;
	#if !defined(USE_EXTERNALTOUCH)
		_touchPin = 255;
		_clearTInt = false;
		_touchEnabled = false;
		if (!touchCalibrated()) {//added by MorganSandercock
			_tsAdcMinX = 0; _tsAdcMinY = 0; _tsAdcMaxX = 1023; _tsAdcMaxY = 1023;
		} else {
			//We have a valid calibration in _utilities\RA8875Calibration.h
			//Note that low may be a smaller value than high
			//These values may be updated by any rotation/orientation setter.
			_tsAdcMinX = TOUCSRCAL_XLOW;
			_tsAdcMinY = TOUCSRCAL_YLOW;
			_tsAdcMaxX = TOUCSRCAL_XHIGH;
			_tsAdcMaxY = TOUCSRCAL_YHIGH;
		}
	#else
/*		Touch Panel Control Register 0     [0x70]
		7: 0(disable, 1:(enable)
		6,5,4:TP Sample Time Adjusting (000...111)
		3:Touch Panel Wakeup Enable 0(disable),1(enable)
		2,1,0:ADC Clock Setting (000...111) set fixed to 010: (System CLK) / 4, 10Mhz Max! */
		_TPCR0Reg = RA8875_TPCR0_WAIT_4096CLK | RA8875_TPCR0_WAKEDISABLE | RA8875_TPCR0_ADCCLK_DIV4;
		//_TPCR0Reg = RA8875_TPCR0_WAIT_32768CLK | RA8875_TPCR0_WAKEDISABLE | RA8875_TPCR0_ADCCLK_DIV128;
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
	_DPCRReg = 0b00000000;
/*	Memory Write Control Register 0 [0x40]
	7: 0(graphic mode), 1(textx mode)
	6: 0(font-memory cursor not visible), 1(visible)
	5: 0(normal), 1(blinking)
	4: na
	3-2: 00(LR,TB), 01(RL,TB), 10(TB,LR), 11(BT,LR)
	1: 0(Auto Increase in write), 1(no)
	0: 0(Auto Increase in read), 1(no) */
	_MWCR0Reg = 0b00000000;
	
/*	Font Control Register 0 [0x21]
	7: 0(CGROM font is selected), 1(CGRAM font is selected)
	6: na
	5: 0(Internal CGROM [reg 0x2F to 00]), 1(External CGROM [0x2E reg, bit6,7 to 0)
	4-2: na
	1-0: 00(ISO/IEC 8859-1), 01(ISO/IEC 8859-2), 10(ISO/IEC 8859-3), 11(ISO/IEC 8859-4)*/
	
	_FNCR0Reg = 0b00000000;
/*	Font Control Register 1 [0x22]
	7: 0(Full Alignment off), 1(Full Alignment on)
	6: 0(no-trasparent), 1(trasparent)
	5: na
	4: 0(normal), 1(90degrees)
	3-2: 00(x1), 01(x2), 10(x3), 11(x3) Horizontal font scale
	1-0: 00(x1), 01(x2), 10(x3), 11(x3) Vertical font scale */
	_FNCR1Reg = 0b00000000;

	/*	Font Write Type Setting Register [0x2E]
	7-6: 00(16x16,8x16,nx16), 01(24x24,12x24,nx24), 1x(32x32,16x32, nx32)
	5-0: 00...3F (font width off to 63 pixels)*/
	_FWTSETReg = 0b00000000;
	
	/*	Serial Font ROM Setting [0x2F]
	GT Serial Font ROM Select
	7-5: 000(GT21L16TW/GT21H16T1W),001(GT30L16U2W),010(GT30L24T3Y/GT30H24T3Y),011(GT30L24M1Z),111(GT30L32S4W/GT30H32S4W)
	FONT ROM Coding Setting
	4-2: 000(GB2312),001(GB12345/GB18030),010(BIG5),011(UNICODE),100(ASCII),101(UNI-Japanese),110(JIS0208),111(Latin/Greek/Cyrillic/Arabic)
	1-0: 00...11 
		 bits	ASCII		Lat/Gr/Cyr		Arabit
		 00		normal		normal			na
		 01		Arial		var Wdth		Pres Forms A
		 10		Roman		na				Pres Forms B
		 11		Bold		na				na */
	_SFRSETReg = 0b00000000;
	
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
	_INTC1Reg = 0b00000000;

	//------------------------------- Start SPI initialization ------------------------------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__)//Teensy 3,3.1
		//always uses SPI ransaction
		if ((_mosi == 11 || _mosi == 7) && (_miso == 12 || _miso == 8) && (_sclk == 13 || _sclk == 14)) {//valid SPI pins?
			SPI.setMOSI(_mosi);
			SPI.setMISO(_miso);
			SPI.setSCK(_sclk);
		} else {
			_errorCode |= (1 << 1);//set
			return;
		}
		if (!SPI.pinIsChipSelect(_cs)) {
			_errorCode |= (1 << 2);//set
			return;
		}
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
					SPI1.setMOSI(_mosi);
					SPI1.setMISO(_miso);
					SPI1.setSCK(_sclk);
					SPI1.begin();
				} else {//default SPI channel 0 (12Mhz)
					_altSPI = false;
					SPI.setMOSI(_mosi);
					SPI.setMISO(_miso);
					SPI.setSCK(_sclk);
					if (!SPI.pinIsChipSelect(_cs)) {//ERROR
						_errorCode |= (1 << 2);//set
						return;
					}
					SPI.begin();
				}
			} else {
				_errorCode |= (1 << 1);//set
				return;
			}
		#else
			_altSPI = false;
			SPI.begin();
			_errorCode |= (1 << 3);//set
		#endif
	#endif
	#if !defined(ENERGIA)//everithing but ENERGIA
		#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)//all Tensy's 32bit
			pinMode(_cs, OUTPUT);
			digitalWrite(_cs, HIGH);
		#elif defined(__SAM3X8E__)// DUE
			#if defined(SPI_DUE_MODE_EXTENDED)//DUE SPI mode extended you can use only follow pins
				if (_cs == 4 || _cs == 10 || _cs == 52) {
					SPI.begin(_cs);
				} else {
					_errorCode |= (1 << 2);//error! wrong cs pin
					return;
				}
			#else//DUE in normal SPI mode
				SPI.begin();
				pinMode(_cs, OUTPUT);
				#if defined(_FASTSSPORT)
					csport = portOutputRegister(digitalPinToPort(_cs));
					cspinmask = digitalPinToBitMask(_cs);
					*csport |= cspinmask;//hi
				#else
					digitalWrite(_cs, HIGH);
				#endif
			#endif
		#else//UNO,MEGA,Yun,nano,duemilanove and other 8 bit arduino's
			SPI.begin();
			pinMode(_cs, OUTPUT);
			csport = portOutputRegister(digitalPinToPort(_cs));//pinMode(_cs, OUTPUT);
			cspinmask = digitalPinToBitMask(_cs);
			*csport |= cspinmask;//hi
		#endif
	#endif
	if (_rst != 255){//time for hardware reset RA8875
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
		_maxspeed = 4000000;//we start in low speed here!
	#else//do not use SPItransactons
		#if defined (__AVR__)//8 bit arduino's
			SPI.begin();
			SPI.setClockDivider(SPI_SPEED_SAFE);
			delay(1);
			SPI.setDataMode(SPI_MODE3);
		#else
			#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
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
	initialize();//----->Time to Initialize the RA8875!
	//------- time for capacitive touch stuff -----------------
	#if defined (USE_FT5206_TOUCH)
		Wire.begin();
		#if defined(__SAM3X8E__)
			// Force 400 KHz I2C, rawr! (Uses pins 20, 21 for SDA, SCL)
			TWI1->TWI_CWGR = 0;
			TWI1->TWI_CWGR = ((VARIANT_MCK / (2 * 400000)) - 4) * 0x101;
		#else
			#if ARDUINO >= 157
				Wire.setClock(400000UL); // Set I2C frequency to 400kHz
			#else
				TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
			#endif
		#endif
		delay(10);
		initFT5206();//initialize FT5206 controller
		pinMode(_ctpInt ,INPUT);//enable _ctpInt as input for listen interrupts
		_maxTouch = 5;
		_gesture = 0;
		_currentTouches = 0;
		_currentTouchState = 0;
		_needISRrearm = false;
	#endif
}


/************************* Initialization *********************************/

/**************************************************************************/
/*!
      Hardware initialization of RA8875 and turn on
	  [private]
*/
/**************************************************************************/
void RA8875::initialize() 
{
	_inited = false;
	if (_rst == 255) {// soft reset time ?
		writeCommand(RA8875_PWRR);
		writeData(RA8875_PWRR_SOFTRESET);
		delay(20);
		writeData(RA8875_PWRR_NORMAL);
		delay(200);
	}
	//set the sysClock
	setSysClock(initStrings[_initIndex][0],initStrings[_initIndex][1],initStrings[_initIndex][2]);
	//color space setup

	if (_color_bpp < 16){//256
		writeReg(RA8875_SYSR,0x00);//256
		_colorIndex = 3;
	} else {
		writeReg(RA8875_SYSR,0x0C);//65K
		_colorIndex = 0;
	}
	
	writeReg(RA8875_HDWR,initStrings[_initIndex][3]);	//LCD Horizontal Display Width Register
	writeReg(RA8875_HNDFTR,initStrings[_initIndex][4]);	//Horizontal Non-Display Period Fine Tuning Option Register
	writeReg(RA8875_HNDR,initStrings[_initIndex][5]);	//LCD Horizontal Non-Display Period Register
	writeReg(RA8875_HSTR,initStrings[_initIndex][6]);	//HSYNC Start Position Register
	writeReg(RA8875_HPWR,initStrings[_initIndex][7]);	//HSYNC Pulse Width Register
	writeReg(RA8875_VDHR0,initStrings[_initIndex][8]);	//LCD Vertical Display Height Register0
	writeReg(RA8875_VDHR1,initStrings[_initIndex][9]);	//LCD Vertical Display Height Register1
	writeReg(RA8875_VNDR0,initStrings[_initIndex][10]);	//LCD Vertical Non-Display Period Register 0
	writeReg(RA8875_VNDR1,initStrings[_initIndex][11]);	//LCD Vertical Non-Display Period Register 1
	writeReg(RA8875_VSTR0,initStrings[_initIndex][12]);	//VSYNC Start Position Register 0
	writeReg(RA8875_VSTR1,initStrings[_initIndex][13]);	//VSYNC Start Position Register 1
	writeReg(RA8875_VPWR,initStrings[_initIndex][14]);	//VSYNC Pulse Width Register
	updateActiveWindow(true);//set the whole screen as active
	//clearActiveWindow();
	delay(10); //100
	setCursorBlinkRate(DEFAULTCURSORBLINKRATE);//set default blink rate
	setIntFontCoding(DEFAULTINTENCODING);//set default internal font encoding
	setFont(INT);	//set internal font use
	//postburner PLL!
	setSysClock(sysClockPar[_initIndex][0],sysClockPar[_initIndex][1],initStrings[_initIndex][2]);
	_inited = true;//from here we will go at high speed!
	#if defined(_FASTCPU)
		slowDownSPI(false);
	#else
		#if defined(SPI_HAS_TRANSACTION)
			#if defined(__MKL26Z64__)
				if (_altSPI){
					_maxspeed = 22000000;
				} else {
					_maxspeed = MAXSPISPEED;
				}
			#else			
				_maxspeed = MAXSPISPEED;
			#endif
		#else
			#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
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

	#if defined(_RA8875_DEFAULTTXTBKGRND)
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			setForegroundColor(_textForeColor);
			setBackgroundColor(_textBackColor);
		#else
			setForegroundColor(_RA8875_DEFAULTTXTFRGRND);
			setBackgroundColor(_RA8875_DEFAULTTXTBKGRND);
		#endif
		_backTransparent = false;
		_FNCR1Reg &= ~(1 << 6);//clear
	#else
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			setForegroundColor(_textForeColor);
		#else
			setForegroundColor(_RA8875_DEFAULTTXTFRGRND);
		#endif
		_backTransparent = true;
		_FNCR1Reg |= (1 << 6);//set
	#endif
	writeReg(RA8875_FNCR1,_FNCR1Reg);
	
	setCursor(0,0);
}

/**************************************************************************/
/*!
      This function set the sysClock accordly datasheet
	  Parameters:
	  pll1:PLL Control Register 1
	  pll2:PLL Control Register 2
	  pixclk:Pixel Clock Setting Register
	  [private]
*/
/**************************************************************************/
void RA8875::setSysClock(uint8_t pll1,uint8_t pll2,uint8_t pixclk)
{
	writeReg(RA8875_PLLC1,pll1);////PLL Control Register 1
	delay(1);
	writeReg(RA8875_PLLC2,pll2);////PLL Control Register 2
	delay(1);
	writeReg(RA8875_PCSR,pixclk);//Pixel Clock Setting Register
	delay(1);
}

/**************************************************************************/
/*!
      This return a byte with the error code/s:
	  bit--------------------------------------------------------------------
	  0:The display it's not supported!
	  1:The MOSI or MISO or SCLK choosed for Teensy it's out permitted range!
	  2:The CS pin you selected it's out permitted range!
	  3:You have to upgrade to Teensyduino 1.22 or better to use this feature!
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
boolean RA8875::waitPoll(uint8_t regname, uint8_t waitflag) 
{
	uint8_t temp;
	unsigned long timeout = millis();
	
	while (1) {
		temp = readReg(regname);
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
void RA8875::waitBusy(uint8_t res) 
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
	writeReg(RA8875_MCLR,temp);
	waitBusy(0x80);
	if (full && _useMultiLayers){
		temp = 0b10000000;
		uint8_t templ = _currentLayer;//remember current layer
		if (templ > 0){//we are in L2
			writeTo(L1);//switch to L1
		} else {
			writeTo(L2);//switch to L2
		}
		writeReg(RA8875_MCLR,temp);
		waitBusy(0x80);
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
	temp = readReg(RA8875_MCLR);
	//stop == true ? bitClear(temp,7) : bitSet(temp,7);
	stop == true ? temp &= ~(1 << 7) : temp |= (1 << 7);
	writeData(temp); 
	if (!stop) waitBusy(0x80);
}

/**************************************************************************/
/*!		
		Clear the active window
	    Parameters:
		full: false(clear current window)
*/
/**************************************************************************/
void RA8875::clearActiveWindow(bool full)
{
	uint8_t temp;
	temp = readReg(RA8875_MCLR);
	//full == true ? bitClear(temp,6) : bitSet(temp,6);
	full == true ? temp &= ~(1 << 6) : temp |= (1 << 6);
	writeData(temp);  
	//waitBusy(0x80);
}

/**************************************************************************/
/*!
      turn display on/off
*/
/**************************************************************************/
void RA8875::displayOn(boolean on) 
{
	on == true ? writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON) : writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
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
	if (_portrait){//0.69b24
		swapvals(XL,YT);
		swapvals(XR,YB);
	}
	
	if (XR >= WIDTH) XR = WIDTH;
	if (YB >= HEIGHT) YB = HEIGHT;
	
	_activeWindowXL = XL;
	_activeWindowXR = XR;
	_activeWindowYT = YT;
	_activeWindowYB = YB;
	updateActiveWindow(false);
}

/**************************************************************************/
/*!		
		Set the Active Window as FULL SCREEN
*/
/**************************************************************************/
void RA8875::setActiveWindow(void)
{
	_activeWindowXL = 0;
	_activeWindowXR = WIDTH;
	_activeWindowYT = 0;
	_activeWindowYB = HEIGHT;
	if (_portrait){
		swapvals(_activeWindowXL,_activeWindowYT);
		swapvals(_activeWindowXR,_activeWindowYB);
	}
	updateActiveWindow(true);
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
	XL = _activeWindowXL;
	XR = _activeWindowXR;
	YT = _activeWindowYT;
	YB = _activeWindowYB;
}

/**************************************************************************/
/*!		
		Return the max tft width.
		Note that real size will start from 0
		so you need to subtract 1!
*/
/**************************************************************************/
uint16_t RA8875::width(void) const 
{ 
	return _width; 
}

/**************************************************************************/
/*!		
		Return the max tft height.
		Note that real size will start from 0
		so you need to subtract 1!
*/
/**************************************************************************/
uint16_t RA8875::height(void) const { 
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
void RA8875::changeMode(bool m) 
{
	if (m == _currentMode) return;
	writeCommand(RA8875_MWCR0);
	//if (m != 0){//text
	if (m){//text
		_MWCR0Reg |= (1 << 7);
		_currentMode = true;
	} else {//graph
		_MWCR0Reg &= ~(1 << 7);
		_currentMode = false;
	}
	writeData(_MWCR0Reg);
}

/**************************************************************************/
/*!
      Change the beam scan direction on display
	  Parameters:
	  invertH:true(inverted),false(normal) horizontal
	  invertV:true(inverted),false(normal) vertical
*/
/**************************************************************************/
void RA8875::scanDirection(boolean invertH,boolean invertV)
{
	invertH == true ? _DPCRReg |= (1 << 3) : _DPCRReg &= ~(1 << 3);
	invertV == true ? _DPCRReg |= (1 << 2) : _DPCRReg &= ~(1 << 2);
	writeReg(RA8875_DPCR,_DPCRReg);
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
		scanDirection(0,0);
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 0; 
				_tsAdcMinY = 0; 
				_tsAdcMaxX = 1023; 
				_tsAdcMaxY = 1023;
			} else {
				_tsAdcMinX = TOUCSRCAL_XLOW;
				_tsAdcMinY = TOUCSRCAL_YLOW;
				_tsAdcMaxX = TOUCSRCAL_XHIGH;
				_tsAdcMaxY = TOUCSRCAL_YHIGH;
			}
		#endif
    break;
	case 1:
		//90
		_portrait = true;
		scanDirection(1,0);
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 1023; 
				_tsAdcMinY = 0; 
				_tsAdcMaxX = 0; 
				_tsAdcMaxY = 1023;
			} else {
				_tsAdcMinX = TOUCSRCAL_XHIGH;
				_tsAdcMinY = TOUCSRCAL_YLOW;
				_tsAdcMaxX = TOUCSRCAL_XLOW;
				_tsAdcMaxY = TOUCSRCAL_YHIGH;
			}
		#endif
    break;
	case 2:
		//180
		_portrait = false;
		scanDirection(1,1);
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 1023; 
				_tsAdcMinY = 1023; 
				_tsAdcMaxX = 0; 
				_tsAdcMaxY = 0;
			} else {
				_tsAdcMinX = TOUCSRCAL_XHIGH;
				_tsAdcMinY = TOUCSRCAL_YHIGH;
				_tsAdcMaxX = TOUCSRCAL_XLOW;
				_tsAdcMaxY = TOUCSRCAL_YLOW;
			}
		#endif
    break;
	case 3:
		//270
		_portrait = true;
		scanDirection(0,1);
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 0; 
				_tsAdcMinY = 1023; 
				_tsAdcMaxX = 1023; 
				_tsAdcMaxY = 0;
			} else {
				_tsAdcMinX = TOUCSRCAL_XLOW;
				_tsAdcMinY = TOUCSRCAL_YHIGH;
				_tsAdcMaxX = TOUCSRCAL_XHIGH;
				_tsAdcMaxY = TOUCSRCAL_YLOW;
			}
		#endif
    break;
	}
	if (_portrait){
		_width = HEIGHT;
		_height = WIDTH;
		_FNCR1Reg |= (1 << 4);
	} else {
		_width = WIDTH;
		_height = HEIGHT;
		_FNCR1Reg &= ~(1 << 4);
	}
	writeReg(RA8875_FNCR1,_FNCR1Reg);//0.69b21
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
	uint8_t tempMWCR1 = readReg(RA8875_MWCR1);//thanks MorganSandercock
	uint8_t i;
	if (_currentMode) changeMode(false);//we are in text mode?
	writeReg(RA8875_CGSR,address);
	writeTo(CGRAM);
	writeCommand(RA8875_MRWC);
	for (i=0;i<16;i++){
		writeData(symbol[i]);
	}
	writeReg(RA8875_MWCR1, tempMWCR1);//restore register
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
	if (!_currentMode) changeMode(true);//we are in graph mode?
	uint8_t oldRegState = _FNCR0Reg;
	uint8_t i;
	oldRegState |= (1 << 7);//set to CGRAM
	writeReg(RA8875_FNCR0,oldRegState);
	//layers?
 	if (_useMultiLayers){
		if (_currentLayer == 0){
			writeTo(L1);
		} else {
			writeTo(L2);
		}
	} else {
		writeTo(L1);
	}
	writeCommand(RA8875_MRWC);
	writeData(symbolAddrs);
	if (wide > 0){
		for (i=1;i<=wide;i++){
			writeData(symbolAddrs+i);
		}
	}
	if (oldRegState != _FNCR0Reg) writeReg(RA8875_FNCR0,_FNCR0Reg);
}

/**************************************************************************/
/*!   
		Set internal Font Encoding
		Parameters:
		f:ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4
		default:ISO_IEC_8859_1
*/
/**************************************************************************/
void RA8875::setIntFontCoding(enum RA8875fontCoding f) 
{
	uint8_t temp = _FNCR0Reg;
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
	_FNCR0Reg = temp;
	writeReg(RA8875_FNCR0,_FNCR0Reg);
}

/**************************************************************************/
/*!  
		External Font Rom setup
		This will not phisically change the register but should be called before setFont(EXT)!
		You should use this values accordly Font ROM datasheet!
		Parameters:
		ert:ROM Type          (GT21L16T1W, GT21H16T1W, GT23L16U2W, GT30H24T3Y, GT23L24T3Y, GT23L24M1Z, GT23L32S4W, GT30H32S4W)
		erc:ROM Font Encoding (GB2312, GB12345, BIG5, UNICODE, ASCII, UNIJIS, JIS0208, LATIN)
		erf:ROM Font Family   (STANDARD, ARIAL, ROMAN, BOLD)
*/
/**************************************************************************/
void RA8875::setExternalFontRom(enum RA8875extRomType ert, enum RA8875extRomCoding erc, enum RA8875extRomFamily erf)
{
	uint8_t temp = _SFRSETReg;//just to preserve the reg in case something wrong
	switch(ert){ //type of rom
		case GT21L16T1W:
		case GT21H16T1W:
			temp &= 0x1F;
		break;
		case GT23L16U2W:
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
		case ER3304_1://the Experimentalist
			temp &= 0x1F; temp |= 0x80;
		break;
		default:
			_commonTextPar &= ~(1 << 0);//wrong type, better avoid for future
			return;//cannot continue, exit
		}
		_fontRomType = ert;
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
			_commonTextPar &= ~(1 << 0);//wrong coding, better avoid for future
			return;//cannot continue, exit
		}
		_fontRomCoding = erc;
		_SFRSETReg = temp;
		setExtFontFamily(erf,false);	
		//bitSet(_commonTextPar,0);
		_commonTextPar |= (1 << 0); //bit set 0
		//writeReg(RA8875_SFRSET,_SFRSETReg);//0x2F
		//delay(4);
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
*/
/**************************************************************************/
void RA8875::setExtFontFamily(enum RA8875extRomFamily erf,boolean setReg) 
{
	_fontFamily = erf;
	switch(erf){	//check rom font family
		case STANDARD:
			_SFRSETReg &= 0xFC;
		break;
		case ARIAL:
			_SFRSETReg &= 0xFC; _SFRSETReg |= 0x01;
		break;
		case ROMAN:
			_SFRSETReg &= 0xFC; _SFRSETReg |= 0x02;
		break;
		case BOLD:
			_SFRSETReg |= ((1<<1) | (1<<0)); // set bits 1 and 0
		break;
		default:
			_fontFamily = STANDARD; _SFRSETReg &= 0xFC;
			return;
	}
	if (setReg) writeReg(RA8875_SFRSET,_SFRSETReg);
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
	if (s == INT){
		//check the font coding
		if (bitRead(_commonTextPar,0) == 1) {//0.96b22 _extFontRom = true
			setFontSize(X16);
			writeReg(RA8875_SFRSET,0b00000000);//_SFRSETReg
		}
		_FNCR0Reg &= ~((1<<7) | (1<<5));// Clear bits 7 and 5
		writeReg(RA8875_FNCR0,_FNCR0Reg);
		_fontSource = s;
		delay(1);
	} else {
		if (bitRead(_commonTextPar,0) == 1) {//0.96b22 _extFontRom = true
			_fontSource = s;
			//now switch
			_FNCR0Reg |= (1 << 5);
			writeReg(RA8875_FNCR0,_FNCR0Reg);//0x21
			delay(1);
			writeReg(RA8875_SFCLR,0x02);//Serial Flash/ROM CLK frequency/2
			setFontSize(X24);////X24 size
			writeReg(RA8875_SFRSET,_SFRSETReg);//at this point should be already set
			delay(4);
			writeReg(RA8875_SROC,0x28);// 0x28 rom 0,24bit adrs,wave 3,1 byte dummy,font mode, single mode
			delay(4);
		} else {
			setFont(INT);
		}
	}
}

/**************************************************************************/
/*!  
		Enable/Disable the Font Full Alignemet feature (default off)
		Parameters:
		align: true,false
*/
/**************************************************************************/
void RA8875::setFontFullAlign(boolean align) 
{
	align == true ? _FNCR1Reg |= (1 << 7) : _FNCR1Reg &= ~(1 << 7);
	writeReg(RA8875_FNCR1,_FNCR1Reg);
}

/**************************************************************************/
/*!  
		Enable/Disable 90" Font Rotation (default off)
		Parameters:
		rot: true,false
		note:disabled because not compatible with setRotation!
*/
/**************************************************************************/
/*
void RA8875::setFontRotate(boolean rot) 
{
	if (_portrait) rot = !rot;
	rot == true ? _FNCR1Reg |= (1 << 4) : _FNCR1Reg &= ~(1 << 4);
	writeReg(RA8875_FNCR1,_FNCR1Reg);
}
*/

/**************************************************************************/
/*!  
		Set distance between text lines (default off)
		Parameters:
		pix: 0...63 pixels
*/
/**************************************************************************/
void RA8875::setFontInterline(uint8_t pix)
{
	if (pix > 0x3F) pix = 0x3F;
	_fontInterline = pix;
	//_FWTSETReg &= 0xC0;
	//_FWTSETReg |= spc & 0x3F; 
	writeReg(RA8875_FLDR,_fontInterline);
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
*/
/**************************************************************************/
void RA8875::setCursor(int16_t x, int16_t y,bool autocenter) 
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	
	_autocenter = autocenter;
	
	if (_portrait) {//rotation 1,3
		swapvals(x,y);
		if (y == CENTER) {//swapped OK
			y = _width/2;
			if (!autocenter) {
				_centerFlag = true;
				_commonTextPar |= (1 << 6);
			}
		}
		if (x == CENTER) {//swapped
			x = _height/2;
			if (!autocenter) {
				_centerFlag = true;
				_commonTextPar |= (1 << 5);
			}
		}
	} else {//rotation 0,2
		if (x == CENTER) {
			x = _width/2;
			if (!autocenter) {
				_centerFlag = true;
				_commonTextPar |= (1 << 5);
			}
		}
		if (y == CENTER) {
			y = _height/2;
			if (!autocenter) {
				_centerFlag = true;
				_commonTextPar |= (1 << 6);
			}
		}
	}
	
	if (bitRead(_commonTextPar,2) == 0){//textWrap
		if (x >= _width) x = _width-1;
		if (y >= _height) y = _height-1;
	}
	
	_cursorX = x;
	_cursorY = y;
	//if _centerFlag or _autocenter do not apply to registers yet!
	// Have to go to textWrite first to calculate the lenght of the entire string and recalculate the correct x,y
	if (_centerFlag || _autocenter) return;
	setTextPosition(x,y,false);
}

/**************************************************************************/
/*!   
		Set the x,y position for text only
		Parameters:
		x:horizontal pos in pixels
		y:vertical pos in pixels
		update:true track the actual text position internally
		[private]
*/
/**************************************************************************/
void RA8875::setTextPosition(int16_t x, int16_t y,bool update)
{
	writeReg(RA8875_F_CURXL,(x & 0xFF));
	writeReg(RA8875_F_CURXH,(x >> 8));
	writeReg(RA8875_F_CURYL,(y & 0xFF));
	writeReg(RA8875_F_CURYH,(y >> 8));
	if (update){
		_cursorX = x;
		_cursorY = y;
	}
}

/**************************************************************************/
/*!   
		Give you back the current text cursor position by reading inside RA8875
		Parameters:
		x:horizontal pos in pixels
		y:vertical pos in pixels
		USE: xxx.getCursor(myX,myY);
*/
/**************************************************************************/
void RA8875::getCursor(int16_t &x, int16_t &y) //fixed in 0.69b32
{
	uint8_t t1,t2,t3,t4;
	t1 = readReg(RA8875_F_CURXL);
	t2 = readReg(RA8875_F_CURXH);
	t3 = readReg(RA8875_F_CURYL);
	t4 = readReg(RA8875_F_CURYH);
	x = (t2 << 8) | (t1 & 0xFF);
	y = (t4 << 8) | (t3 & 0xFF);
	if (_portrait) swapvals(x,y);
}

/**************************************************************************/
/*!   
		Give you back the current text cursor position as tracked by library (fast)
		Parameters:
		x:horizontal pos in pixels
		y:vertical pos in pixels
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
		blink: true=blink cursor
*/
/**************************************************************************/
void RA8875::showCursor(enum RA8875tcursor c,bool blink)
{
    //uint8_t MWCR1Reg = readReg(RA8875_MWCR1) & 0x01;(needed?)
    uint8_t cW = 0;
    uint8_t cH = 0;
	_textCursorStyle = c;
	c == NOCURSOR ? _MWCR0Reg &= ~(1 << 6) : _MWCR0Reg |= (1 << 6);
    if (blink) _MWCR0Reg |= 0x20;//blink or not?
    writeReg(RA8875_MWCR0, _MWCR0Reg);//set cursor
    //writeReg(RA8875_MWCR1, MWCR1Reg);//close graphic cursor(needed?)
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
    writeReg(RA8875_CURHS, cW);
    writeReg(RA8875_CURVS, cH);
}

/**************************************************************************/
/*!     Set cursor property blink and his rate
		Parameters:
		rate:blink speed (fast 0...255 slow)
*/
/**************************************************************************/
void RA8875::setCursorBlinkRate(uint8_t rate)
{
	writeReg(RA8875_BTCR,rate);//set blink rate
}

/**************************************************************************/
/*!		
		set the text color and his background
		Parameters:
		fcolor:16bit foreground color (text) RGB565
		bcolor:16bit background color RGB565
		NOTE:will set background trasparent OFF
*/
/**************************************************************************/
void RA8875::setTextColor(uint16_t fcolor, uint16_t bcolor)//0.69b30
{
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		if (fcolor != _textForeColor) {
			_textForeColor = fcolor;
			setForegroundColor(fcolor);
		}
		if (bcolor != _textBackColor) {
			_textBackColor = bcolor;
			setBackgroundColor(bcolor);
		}
		_backTransparent = false;
	#else
		_backTransparent = false;
		if (fcolor != _foreColor) setForegroundColor(fcolor);
		if (bcolor != _backColor) setBackgroundColor(bcolor);
	#endif
	_FNCR1Reg &= ~(1 << 6);//clear
	writeReg(RA8875_FNCR1,_FNCR1Reg);
}

/**************************************************************************/
/*!		
		set the text color w transparent background
		Parameters:
		fColor:16bit foreground color (text) RGB565
		NOTE:will set background trasparent ON
*/
/**************************************************************************/

void RA8875::setTextColor(uint16_t fcolor)
{
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		if (fcolor != _textForeColor) {
			_textForeColor = fcolor;
			setForegroundColor(fcolor);
		}
		_backTransparent = true;
	#else
		_backTransparent = true;
		if (fcolor != _foreColor) setForegroundColor(fcolor);
	#endif
		_FNCR1Reg |= (1 << 6);//set
		writeReg(RA8875_FNCR1,_FNCR1Reg);
}

/**************************************************************************/
/*!		
		Set the Text size by it's multiple. normal should=0, max is 3 (x4)
		Parameters:
		scale:0..3  -> 0:normal, 1:x2, 2:x3, 3:x4
*/
/**************************************************************************/
void RA8875::setFontScale(uint8_t scale)
{
	//if (scale > 3) scale = 3;
	scale = scale % 4; //limit to the range 0-3
 	_FNCR1Reg &= ~(0xF); // clear bits from 0 to 3
	_FNCR1Reg |= scale << 2;
	_FNCR1Reg |= scale;
	writeReg(RA8875_FNCR1,_FNCR1Reg);
	_textHScale = scale;
	_textVScale = scale;
}

/**************************************************************************/
/*!		
		Set the Text size by it's multiple. normal should=0, max is 3 (x4)
		This time you can specify different values for vertical and horizontal
		Parameters:
		vscale:0..3  -> 0:normal, 1:x2, 2:x3, 3:x4
		hscale:0..3  -> 0:normal, 1:x2, 2:x3, 3:x4
*/
/**************************************************************************/
void RA8875::setFontScale(uint8_t vscale,uint8_t hscale)
{
	//if (vscale > 3) vscale = 3;
	//if (hscale > 3) hscale = 3;
	vscale = vscale % 4; //limit to the range 0-3
	hscale = hscale % 4; //limit to the range 0-3
 	_FNCR1Reg &= ~(0xF); // clear bits from 0 to 3
	_FNCR1Reg |= hscale << 2;
	_FNCR1Reg |= vscale;
	writeReg(RA8875_FNCR1,_FNCR1Reg);
	_textHScale = hscale;
	_textVScale = vscale;
}

/**************************************************************************/
/*!		was:setFontAdvance
		Normally at every char the cursor advance by one
		You can stop/enable this by using this function
		Parameters:
		on:true(auto advance - default), false:(stop auto advance)
*/
/**************************************************************************/
void RA8875::cursorIncrement(bool on)
{
	on == true ? _MWCR0Reg &= ~(1 << 1) : _MWCR0Reg |= (1 << 1);
	bitWrite(_commonTextPar,1,on);
	writeReg(RA8875_MWCR0,_MWCR0Reg);
}

/**************************************************************************/
/*!		
		Choose between 16x16(8x16) - 24x24(12x24) - 32x32(16x32)
		for External Font ROM
		Parameters:
		ts:X16,X24,X32
*/
/**************************************************************************/
void RA8875::setFontSize(enum RA8875tsize ts)
{
	switch(ts){
		case X16:
			_FWTSETReg &= 0x3F;
		break;
		case X24:
			_FWTSETReg &= 0x3F; _FWTSETReg |= 0x40; 
		break;
		case X32:
			_FWTSETReg &= 0x3F; _FWTSETReg |= 0x80; 
		break;
		default:
		return;
	}
	_textSize = ts;
	writeReg(RA8875_FWTSET,_FWTSETReg);
}

/**************************************************************************/
/*!		
		return the current width of the font in pixel
		If font it's scaled, it will multiply.
		It's a fast business since the register it's internally tracked
		It can also return the usable rows based on the actual fontWidth
		Parameters: inColums (true:returns max colums)
*/
/**************************************************************************/
uint8_t RA8875::getFontWidth(boolean inColums) 
{
    uint8_t temp = (((_FNCR0Reg >> 2) & 0x3) + 1) * 8;
	if (inColums){
		if (_textHScale < 1) return (_width / temp);
		temp = temp * (_textHScale + 1);
		return (_width / temp);
	} else {
		if (_textHScale < 1) return temp;
		temp = temp * (_textHScale + 1);
		return temp;
	}
}

/**************************************************************************/
/*!		
		return the current heigh of the font in pixel
		If font it's scaled, it will multiply.
		It's a fast business since the register it's internally tracked
		It can also return the usable rows based on the actual fontHeight
		Parameters: inRowss (true:returns max rows)
*/
/**************************************************************************/
uint8_t RA8875::getFontHeight(boolean inRows) 
{
    uint8_t temp = (((_FNCR0Reg >> 0) & 0x3) + 1) * 16;
	if (inRows){
		if (_textVScale < 1) return (_height / temp);
		temp = temp * (_textVScale + 1);
		return (_height / temp);
	} else {
		if (_textVScale < 1) return temp;
		temp = temp * (_textVScale + 1);
		return temp;
	}
}

/**************************************************************************/
/*!		
		Choose space in pixels between chars
		Parameters:
		spc:0...63pix (default 0=off)
*/
/**************************************************************************/
void RA8875::setFontSpacing(uint8_t spc)
{
	if (spc > 0x3F) spc = 0x3F;
	_fontSpacing = spc;
	_FWTSETReg &= 0xC0;
	_FWTSETReg |= spc & 0x3F; 
	writeReg(RA8875_FWTSET,_FWTSETReg);

}
/**************************************************************************/
/*!	PRIVATE
		This is the function that write text. Still in development
		NOTE: It identify correctly println and /n & /r
*/
/**************************************************************************/
void RA8875::textWrite(const char* buffer, uint16_t len)
 {
	uint16_t i;
	const uint8_t currentFontW = 8;
	const uint8_t currentFontH = 16;
	if (!_currentMode) changeMode(true);//we are in graph mode?
	if (len == 0) len = strlen(buffer);
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		if (_recoverTextColor){
			if (_foreColor != _textForeColor) {
				_recoverTextColor = false;
				setForegroundColor(_textForeColor);
			}
			if (_backColor != _textBackColor) {
				_recoverTextColor = false;
				setBackgroundColor(_textBackColor);
			}
		}
	#endif
	if (_autocenter){
		_autocenter = false;
		if (!_portrait){
			_cursorX = _cursorX - (((len * currentFontW) * (_textHScale+1))/2)+1;
			_cursorY = _cursorY - (currentFontH * (_textVScale + 1))/2 - 1 - _textVScale;
		} else {
			_cursorX = _cursorX - (currentFontH * (_textVScale + 1))/2 - 1 - _textVScale;
			_cursorY = _cursorY - (((len * currentFontW) * (_textHScale+1))/2)+1;
		}
		setTextPosition(_cursorX,_cursorY,false);
	} else if (_centerFlag){
		_centerFlag = false;
		if (!_portrait){
			if (bitRead(_commonTextPar,5)) {
				_cursorX = (_width/2) - (((len * currentFontW) * (_textHScale+1))/2)+1;
				_commonTextPar &= ~(1 << 5);//reset _alignXToCenter flag
			}
			if (bitRead(_commonTextPar,6)) {
				_cursorY = (_height/2) - (currentFontH * (_textVScale + 1))/2 - 1 - _textVScale;
				_commonTextPar &= ~(1 << 6);//reset _alignYToCenter flag
			}
		} else {
			if (bitRead(_commonTextPar,5)) {//Y = center
				_cursorX = (_height/2) - (currentFontH * (_textVScale + 1))/2 - 1 - _textVScale;
				_commonTextPar &= ~(1 << 5);//reset _alignXToCenter flag
			}
			if (bitRead(_commonTextPar,6)) {//X = center
				_cursorY = (_width/2) - (((len * currentFontW) * (_textHScale+1))/2)+1;
				_commonTextPar &= ~(1 << 6);//reset _alignYToCenter flag
			}
		}
		setTextPosition(_cursorX,_cursorY,false);
	}//end _autocenter/_centerFlag
	
	writeCommand(RA8875_MRWC);
	
	for (i=0;i<len;i++){
		switch(buffer[i]) {
			case '\r':
				//Ignore carriage-return, only detect \n newline 
			break;
			case '\n':
				if (_portrait){
					_cursorY = 0;
					_cursorX += (currentFontH + (currentFontH * _textVScale)) + _fontInterline;
				} else {
					_cursorX = 0;
					_cursorY += (currentFontH + (currentFontH * _textVScale)) + _fontInterline;
				}
				setTextPosition(_cursorX,_cursorY,false);
				
			break;
			default:
				//write a normal char
				writeData(buffer[i]);
				waitBusy(0x80);
				if (!_portrait){
					_cursorX += currentFontW;
				} else {
					_cursorY += currentFontW;
				}
				
		}
	}
}

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
			writeReg(RA8875_SYSR,0x00);
			if (_hasLayerLimits) _maxLayers = 2;
		} else if (colors > 8) {//65K
			_color_bpp = 16;
			_colorIndex = 0;
			writeReg(RA8875_SYSR,0x0C);
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
	  color:16bit color RGB565
*/
/**************************************************************************/
void RA8875::setForegroundColor(uint16_t color)
{
	_foreColor = color;//keep track
	writeReg(RA8875_FGCR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));
	writeReg(RA8875_FGCR1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));
	writeReg(RA8875_FGCR2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));
}
/**************************************************************************/
/*!
      Sets set the foreground color using 8bit R,G,B
	  Parameters:
	  R:8bit RED
	  G:8bit GREEN
	  B:8bit BLUE
*/
/**************************************************************************/
void RA8875::setForegroundColor(uint8_t R,uint8_t G,uint8_t B)
{
	_foreColor = Color565(R,G,B);//keep track
	writeReg(RA8875_FGCR0,R);
	writeReg(RA8875_FGCR1,G);
	writeReg(RA8875_FGCR2,B);
}
/**************************************************************************/
/*!
      Sets set the background color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color:16bit color RGB565
	  Note: will set background Trasparency OFF
*/
/**************************************************************************/
void RA8875::setBackgroundColor(uint16_t color)
{
	_backColor = color;//keep track
	writeReg(RA8875_BGCR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));//11
	writeReg(RA8875_BGCR1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));//5
	writeReg(RA8875_BGCR2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));//0
}
/**************************************************************************/
/*!
      Sets set the background color using 8bit R,G,B
	  Parameters:
	  R:8bit RED
	  G:8bit GREEN
	  B:8bit BLUE
	  Note: will set background Trasparency OFF
*/
/**************************************************************************/
void RA8875::setBackgroundColor(uint8_t R,uint8_t G,uint8_t B)
{
	_backColor = Color565(R,G,B);//keep track
	writeReg(RA8875_BGCR0,R);
	writeReg(RA8875_BGCR1,G);
	writeReg(RA8875_BGCR2,B);
}
/**************************************************************************/
/*!
      Sets set the trasparent background color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color:16bit color RGB565
	  Note: will set background Trasparency ON
*/
/**************************************************************************/
void RA8875::setTransparentColor(uint16_t color)
{
	_backColor = color;
	writeReg(RA8875_BGTR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));
	writeReg(RA8875_BGTR1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));
	writeReg(RA8875_BGTR2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));
}
/**************************************************************************/
/*!
      Sets set the Trasparent background color using 8bit R,G,B
	  Parameters:
	  R:8bit RED
	  G:8bit GREEN
	  B:8bit BLUE
	  Note: will set background Trasparency ON
*/
/**************************************************************************/
void RA8875::setTransparentColor(uint8_t R,uint8_t G,uint8_t B)
{
	_backColor = Color565(R,G,B);//keep track
	writeReg(RA8875_BGTR0,R);
	writeReg(RA8875_BGTR1,G);
	writeReg(RA8875_BGTR2,B);
}

/**************************************************************************/
/*!		
		set foreground,background color (plus transparent background)
		Parameters:
		fColor:16bit foreground color (text) RGB565
		bColor:16bit background color RGB565
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
	uint8_t temp = readReg(RA8875_MWCR1);
	temp &= ~(0x70);//clear bit 6,5,4
	temp |= cur << 4;
	temp |= cur;
	if (_useMultiLayers){
		_currentLayer == 1 ? temp |= (1 << 0) : temp &= ~(1 << 0);
	} else {
		temp &= ~(1 << 0);
	}
	writeData(temp);
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
	uint8_t temp = readReg(RA8875_MWCR1);
	cur == true ? temp |= (1 << 7) : temp &= ~(1 << 7);
	if (_useMultiLayers){
		_currentLayer == 1 ? temp |= (1 << 0) : temp &= ~(1 << 0);
	} else {
		temp &= ~(1 << 0);
	}
	writeData(temp);
}

/**************************************************************************/
/*!		
		Set the position for Graphic Write
		Parameters:
		x:horizontal position
		y:vertical position
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
		x:horizontal position
*/
/**************************************************************************/
void RA8875::setX(int16_t x) 
{
	if (x < 0) x = 0;
	if (_portrait){//fix 0.69b21
		if (x >= HEIGHT) x = HEIGHT-1;
		writeReg(RA8875_CURV0, x & 0xFF);
		writeReg(RA8875_CURV1, x >> 8);
	} else {
		if (x >= WIDTH) x = WIDTH-1;
		writeReg(RA8875_CURH0, x & 0xFF);
		writeReg(RA8875_CURH1, (x >> 8)); 
	}
}

/**************************************************************************/
/*!		
		Set the y position for Graphic Write
		Parameters:
		y:vertical position
*/
/**************************************************************************/
void RA8875::setY(int16_t y) 
{
	if (y < 0) y = 0;
	if (_portrait){//fix 0.69b21
		if (y >= WIDTH) y = WIDTH-1;
		writeReg(RA8875_CURH0, y & 0xFF);
		writeReg(RA8875_CURH1, (y >> 8)); 
	} else {
		if (y >= HEIGHT) y = HEIGHT-1;
		writeReg(RA8875_CURV0, y & 0xFF);
		writeReg(RA8875_CURV1, y >> 8);
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
    uint8_t temp = readReg(RA8875_LTPR0);
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
    //if (_useMultiLayers) writeReg(RA8875_LTPR0,temp);
    //writeReg(RA8875_LTPR0,temp);
	writeData(temp);
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
	
	checkLimitsHelper(XL,YT);
	checkLimitsHelper(XR,YB);
	
	_scrollXL = XL; _scrollXR = XR; _scrollYT = YT; _scrollYB = YB;
    writeReg(RA8875_HSSW0,(_scrollXL & 0xFF));
    writeReg(RA8875_HSSW1,(_scrollXL >> 8));
  
    writeReg(RA8875_HESW0,(_scrollXR & 0xFF));
    writeReg(RA8875_HESW1,(_scrollXR >> 8));   
    
    writeReg(RA8875_VSSW0,(_scrollYT & 0xFF));
    writeReg(RA8875_VSSW1,(_scrollYT >> 8));   
 
    writeReg(RA8875_VESW0,(_scrollYB & 0xFF));
    writeReg(RA8875_VESW1,(_scrollYB >> 8));
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
		writeReg(RA8875_HOFS0,(x & 0xFF)); 
		writeReg(RA8875_HOFS1,(x >> 8));
 
		writeReg(RA8875_VOFS0,(y & 0xFF));
		writeReg(RA8875_VOFS1,(y >> 8));
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
  	writeReg(RA8875_DTNR0,BWR & 0xFF);
  	writeReg(RA8875_BWR1,BWR >> 8);

  	writeReg(RA8875_DTNR1,BHR & 0xFF);
  	writeReg(RA8875_BHR1,BHR >> 8);

  	writeReg(RA8875_DTNR2,SPWR & 0xFF);
  	writeReg(RA8875_SPWR1,SPWR >> 8); 
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void RA8875::DMA_startAddress(unsigned long adrs)
{ 
  	writeReg(RA8875_SSAR0,adrs & 0xFF);
  	writeReg(RA8875_SSAR1,adrs >> 8);
	writeReg(RA8875_SSAR2,adrs >> 16);
  	//writeReg(0xB3,adrs >> 24);// not more in datasheet!
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void RA8875::DMA_enable(void)
{ 
	uint8_t temp = readReg(RA8875_DMACR);
	temp |= 0x01;
	writeData(temp);
	waitBusy(0x01);
}
/**************************************************************************/
/*! (STILL DEVELOPING)
		Display an image stored in Flash RAM
		Note: you should have the optional FLASH Chip connected to RA8875!
		Note: You should store some image in that chip!

*/
/**************************************************************************/
void RA8875::drawFlashImage(int16_t x,int16_t y,int16_t w,int16_t h,uint8_t picnum)
{  
	if (_currentMode) changeMode(false);//we are in text mode?
	if (_portrait){//0.69b21 -have to check this, not verified
		swapvals(x,y);
		swapvals(w,h);
	}
	writeReg(RA8875_SFCLR,0x00);
	writeReg(RA8875_SROC,0x87);
	writeReg(RA8875_DMACR,0x02);
	//setActiveWindow(0,_width-1,0,_height-1); 
	checkLimitsHelper(x,y);
	checkLimitsHelper(w,h);
	_portrait == true ? setXY(y,x) : setXY(x,y);
	
	DMA_startAddress(261120 * (picnum-1));
	DMA_blockModeSize(w,h,w);   
	writeReg(RA8875_DMACR,0x03);
	waitBusy(0x01);
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
	if (_currentMode) changeMode(false);//we are in text mode?
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

	waitBusy(0x40); //Check that another BTE operation is not still in progress
	BTE_moveFrom(SourceX,SourceY);
	BTE_size(Width,Height);
	BTE_moveTo(DestX,DestY);
	BTE_ropcode(ROP);
	//Execute BTE! (This selects linear addressing mode for the monochrome source data)
	if (Monochrome) writeReg(RA8875_BECR0, 0xC0); else writeReg(RA8875_BECR0, 0x80);
	waitBusy(0x40);
	//we are supposed to wait for the thing to become unbusy
	//caller can call waitBusy(0x40) to check the BTE busy status (except it's private)
}

/**************************************************************************/
/*! TESTING

*/
/**************************************************************************/
void RA8875::BTE_size(int16_t w, int16_t h)
{
	//0.69b21 -have to check this, not verified
	if (_portrait) swapvals(w,h);
    writeReg(RA8875_BEWR0,w & 0xFF);//BET area width literacy  
    writeReg(RA8875_BEWR1,w >> 8);//BET area width literacy	   
    writeReg(RA8875_BEHR0,h & 0xFF);//BET area height literacy
    writeReg(RA8875_BEHR1,h >> 8);//BET area height literacy	   
}	

/**************************************************************************/
/*!

*/
/**************************************************************************/

void RA8875::BTE_moveFrom(int16_t SX,int16_t SY)
{
	if (_portrait) swapvals(SX,SY);
	writeReg(RA8875_HSBE0,SX & 0xFF);
	writeReg(RA8875_HSBE1,SX >> 8);
	writeReg(RA8875_VSBE0,SY & 0xFF);
	writeReg(RA8875_VSBE1,SY >> 8);
}	

/**************************************************************************/
/*!

*/
/**************************************************************************/

void RA8875::BTE_moveTo(int16_t DX,int16_t DY)
{
	if (_portrait) swapvals(DX,DY);
	writeReg(RA8875_HDBE0,DX & 0xFF);
	writeReg(RA8875_HDBE1,DX >> 8);
	writeReg(RA8875_VDBE0,DY & 0xFF);
	writeReg(RA8875_VDBE1,DY >> 8);
}	

/**************************************************************************/
/*! TESTING
	Use a ROP code EFX
*/
/**************************************************************************/
void RA8875::BTE_ropcode(unsigned char setx)
{
    writeReg(RA8875_BECR1,setx);//BECR1	   
}

/**************************************************************************/
/*! TESTING
	Enable BTE transfer
*/
/**************************************************************************/
void RA8875::BTE_enable(bool on) 
{	
	uint8_t temp = readReg(RA8875_BECR0);
	on == true ? temp &= ~(1 << 7) : temp |= (1 << 7);
	writeData(temp);
	//writeReg(RA8875_BECR0,temp);  
	waitBusy(0x40);
}


/**************************************************************************/
/*! TESTING
	Select BTE mode (CONT (continuous) or RECT)
*/
/**************************************************************************/
void RA8875::BTE_dataMode(enum RA8875btedatam m) 
{	
	uint8_t temp = readReg(RA8875_BECR0);
	m == CONT ? temp &= ~(1 << 6) : temp |= (1 << 6);
	writeData(temp);
	//writeReg(RA8875_BECR0,temp);  
}

/**************************************************************************/
/*! TESTING
	Select the BTE SOURCE or DEST layer (1 or 2)
*/
/**************************************************************************/

void RA8875::BTE_layer(enum RA8875btelayer sd,uint8_t l)
{
	uint8_t temp;
	/*
	if (sd != SOURCE){//destination
		temp = readReg(RA8875_VDBE1);
	} else {//source
		temp = readReg(RA8875_VSBE1);
	}
	*/
	sd == SOURCE ? temp = readReg(RA8875_VSBE1) : temp = readReg(RA8875_VDBE1);
	//l == 1 ? bitClear(temp,7) : bitSet(temp,7);
	l == 1 ? temp &= ~(1 << 7) : temp |= (1 << 7);
	writeData(temp);
	//writeReg(RA8875_VSBE1,temp);  
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
		on:true (enable multiple layers), false (disable)
      
*/
/**************************************************************************/
void RA8875::useLayers(boolean on) 
{
	if (_useMultiLayers == on) return; //no reason to do change that it's already as desidered.
	//bool clearBuffer = false;
	if (_hasLayerLimits && _color_bpp > 8) { //try to set up 8bit color space
		setColorBpp(8);
		waitBusy();
		_maxLayers = 2;
	}
	if (on){
		_useMultiLayers = true;
		_DPCRReg |= (1 << 7);
		//clearBuffer = true;
		clearActiveWindow(true);
	} else {
		_useMultiLayers = false;
		_DPCRReg &= ~(1 << 7);
		clearActiveWindow(false);
	}
	
	writeReg(RA8875_DPCR,_DPCRReg);
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
	writeReg(RA8875_LTPR0,reg);
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
	//if (_useMultiLayers) writeReg(RA8875_LTPR1, ((layer2 & 0x0F) << 4) | (layer1 & 0x0F));
	//uint8_t res = 0b00000000;//RA8875_LTPR1
	//reg &= ~(0x07);//clear bit 2,1,0
	writeReg(RA8875_LTPR1, ((layer2 & 0xF) << 4) | (layer1 & 0xF));
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
	writeReg(RA8875_PTNO,temp);
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
	
	if (_currentMode) changeMode(false);//we are in text mode?
	writeCommand(RA8875_MRWC);
	for (i=0;i<(size*size);i++) {
		writeData(data[i*2]);
		writeData(data[i*2+1]);
		waitBusy(0x80);
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
	uint8_t temp = readReg(RA8875_MWCR1);
	//bool trigMultilayer = false;
	switch(d){
		case L1:
			temp &= ~((1<<3) | (1<<2));// Clear bits 3 and 2
			temp &= ~(1 << 0); //clear bit 0
			_currentLayer = 0;
			//trigMultilayer = true;
			writeData(temp);  
			if (!_useMultiLayers) useLayers(true);
		break;
		case L2:
			temp &= ~((1<<3) | (1<<2));// Clear bits 3 and 2
			temp |= (1 << 0); //bit set 0
			_currentLayer = 1;
			//trigMultilayer = true;
			writeData(temp);  
			if (!_useMultiLayers) useLayers(true);
		break;
		case CGRAM: 
			temp &= ~(1 << 3); //clear bit 3
			temp |= (1 << 2); //bit set 2
			if (bitRead(_FNCR0Reg,7)){//REG[0x21] bit7 must be 0
				_FNCR0Reg &= ~(1 << 7); //clear bit 7
				writeReg(RA8875_FNCR0,_FNCR0Reg);  
				writeReg(RA8875_MWCR1,temp);
			} else {
				writeData(temp);  
			}
		break;
		case PATTERN:
			temp |= (1 << 3); //bit set 3
			temp |= (1 << 2); //bit set 2
			writeData(temp);  
		break;
		case CURSOR:
			temp |= (1 << 3); //bit set 3
			temp &= ~(1 << 2); //clear bit 2
			writeData(temp);  
		break;
		default:
		//break;
		return;
	}
	//if (trigMultilayer && !_useMultiLayers) useLayers(true);//turn on multiple layers if it's off
	//writeReg(RA8875_MWCR1,temp);   
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
	writeData(RA8875_DCR_LINESQUTRI_STOP | RA8875_DCR_DRAWSQUARE);
	writeData(RA8875_DCR_LINESQUTRI_START | RA8875_DCR_FILL | RA8875_DCR_DRAWSQUARE);
}
 */
/**************************************************************************/
/*!
      Write a single pixel
	  Parameters:
	  x:horizontal pos
	  y:vertical pos
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if (_currentMode) changeMode(false);//we are in text mode?
	setXY(x,y);
	writeCommand(RA8875_MRWC);
	writeData16(color); 
}

/**************************************************************************/
/*!
      Draw a series of pixels
	  Parameters:
	  p:an array of 16bit colors (pixels)
	  count:how many pixels
	  x:horizontal pos
	  y:vertical pos
*/
/**************************************************************************/
void RA8875::drawPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y)
{
	if (_currentMode) changeMode(false);//we are in text mode?
    setXY(x,y);
    writeCommand(RA8875_MRWC);
    startSend();
	#if defined(__AVR__) && defined(_FASTSSPORT)
		spiwrite(RA8875_DATAWRITE);
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
	while (count--) {
	#if !defined(ENERGIA) && !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
		#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
			if (_altSPI){
				SPI1.transfer16(*p);
			} else {
				SPI.transfer16(*p);
			}
		#else
			SPI.transfer16(*p);
		#endif
	#else
		#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
			SPI.transfer(_cs, *p >> 8, SPI_CONTINUE); 
			SPI.transfer(_cs, *p & 0xFF, SPI_LAST);
		#else
			#if defined(__AVR__) && defined(_FASTSSPORT)
				spiwrite16(*p);
			#else
				SPI.transfer(*p >> 8);
				SPI.transfer(*p & 0xFF);
			#endif
		#endif
	#endif
        p++;
    }
    endSend();
}


/**************************************************************************/
/*!
      Get a pixel color from screen
	  Parameters:
	  x:horizontal pos
	  y:vertical pos
*/
/**************************************************************************/
uint16_t RA8875::getPixel(int16_t x, int16_t y)
{
    uint16_t color;
	if (_currentMode) changeMode(false);//we are in text mode?
    setXY(x,y);
    writeCommand(RA8875_MRWC);
	#if defined(_FASTCPU)
		slowDownSPI(true);
	#endif
    startSend();
	#if defined(__AVR__) && defined(_FASTSSPORT)
		spiwrite(RA8875_DATAREAD);
		spiwrite(0x00);
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
	#if !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
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
		#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
			color  = SPI.transfer(_cs, 0x0, SPI_CONTINUE); 
			color |= (SPI.transfer(_cs, 0x0, SPI_LAST) << 8);
		#else
			#if defined(__AVR__) && defined(_FASTSSPORT)
				color  = spiread();
				color |= (spiread() << 8);
			#else
				color  = SPI.transfer(0x0);
				color |= (SPI.transfer(0x0) << 8);
			#endif
		#endif
	#endif
	#if defined(_FASTCPU)
		slowDownSPI(false);
	#endif
    endSend();
    return color;
}


/*
void RA8875::getPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y)
{
    uint16_t color; 
    if (_currentMode != 0) changeMode(0);//we are in text mode?
    setXY(x,y);
    writeCommand(RA8875_MRWC);
	#if defined(_FASTCPU)
		slowDownSPI(true);
	#endif
    startSend();
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
		slowDownSPI(false);
	#endif
    endSend();
}
*/
/**************************************************************************/
/*!
      Basic line draw
	  Parameters:
	  x0:horizontal start pos
	  y0:vertical start
	  x1:horizontal end pos
	  y1:vertical end pos
	  color: RGB565 color
	  NOTE:
	  Remember that this write from->to so: drawLine(0,0,2,0,RA8875_RED);
	  result a 3 pixel long! (0..1..2)
*/
/**************************************************************************/
void RA8875::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	if (_currentMode) changeMode(false);//we are in text mode?
	
	if (_portrait) {//0.69b21
		swapvals(x0,y0);
		swapvals(x1,y1);
	}

	//checkLimitsHelper(x0,y0);
	//if (x1 >= _width) x1 = _width-1;
	//if (y1 >= _height) y1 = _height-1;

	lineAddressing(x0,y0,x1,y1);
	
	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid 3 useless SPI calls
	writeReg(RA8875_DCR,0x80);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      for compatibility with popular Adafruit_GFX
	  draws a single vertical line
	  Parameters:
	  x:horizontal start
	  y:vertical start
	  h:height
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
	  x:horizontal start
	  y:vertical start
	  w:width
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
	  x:horizontal start
	  y:vertical start
	  w: width
	  h:height
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	if (w < 1 || h < 1) return;//it cannot be!
	//RA8875 it's not out-of-range tolerant so this is a workaround
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			_recoverTextColor = true;
		#endif
		rectHelper(x,y,(w+x)-1,(h+y)-1,color,false);//thanks the experimentalist
	}
}

/**************************************************************************/
/*!
	  draws a FILLED rectangle
	  Parameters:
	  x:horizontal start
	  y:vertical start
	  w: width
	  h:height
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	//RA8875 it's not out-of-range tolerant so this is a workaround
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			_recoverTextColor = true;
		#endif
		rectHelper(x,y,(x+w)-1,(y+h)-1,color,true);//thanks the experimentalist
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
	lineAddressing(0,0,WIDTH-1, HEIGHT-1);
	setForegroundColor(color);
	writeCommand(RA8875_DCR);
	writeData(0xB0);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
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
	/*
	updateActiveWindow(true);//temporarily set all window
	fillWindow(color);
	updateActiveWindow(false);//go back to the old win settings
	*/
	setActiveWindow();
	fillWindow(color);
}

/**************************************************************************/
/*!
      Draw circle
	  Parameters:
      x0:The 0-based x location of the center of the circle
      y0:The 0-based y location of the center of the circle
      r:radius
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	if (r < 1) return;
	if (r < 2) {
		drawPixel(x0,y0,color);
		return;
	}
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	circleHelper(x0, y0, r, color, false);
}

/**************************************************************************/
/*!
      Draw filled circle
	  Parameters:
      x0:The 0-based x location of the center of the circle
      y0:The 0-based y location of the center of the circle
      r:radius
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	if (r <= 0) return;
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	circleHelper(x0, y0, r, color, true);
}

/**************************************************************************/
/*!
      Draw Triangle
	  Parameters:
      x0:The 0-based x location of the point 0 of the triangle bottom LEFT
      y0:The 0-based y location of the point 0 of the triangle bottom LEFT
      x1:The 0-based x location of the point 1 of the triangle middle TOP
      y1:The 0-based y location of the point 1 of the triangle middle TOP
      x2:The 0-based x location of the point 2 of the triangle bottom RIGHT
      y2:The 0-based y location of the point 2 of the triangle bottom RIGHT
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	triangleHelper(x0, y0, x1, y1, x2, y2, color, false);
}

/**************************************************************************/
/*!
      Draw filled Triangle
	  Parameters:
      x0:The 0-based x location of the point 0 of the triangle
      y0:The 0-based y location of the point 0 of the triangle
      x1:The 0-based x location of the point 1 of the triangle
      y1:The 0-based y location of the point 1 of the triangle
      x2:The 0-based x location of the point 2 of the triangle
      y2:The 0-based y location of the point 2 of the triangle
      color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	triangleHelper(x0, y0, x1, y1, x2, y2, color, true);
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
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	ellipseCurveHelper(xCenter, yCenter, longAxis, shortAxis, 255, color, false);
	//TODO: correct dimensions?
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
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	ellipseCurveHelper(xCenter, yCenter, longAxis, shortAxis, 255, color, true);
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
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	ellipseCurveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
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
	#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
		_recoverTextColor = true;
	#endif
	ellipseCurveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
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
	  roundRectHelper it's not tolerant to improper values
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
		if (w < h && (r*2) >= w) r = w/2-1;
		if (w > h && (r*2) >= h) r = h/2-1;
		if (r == w || r == h) drawRect(x,y,w,h,color);
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			_recoverTextColor = true;
		#endif
		roundRectHelper(x, y, (x+w)-1, (y+h)-1, r, color, false);
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
		if (w < h && (r*2) >= w) r = w/2-1;
		if (w > h && (r*2) >= h) r = h/2-1;
		if (r == w || r == h) drawRect(x,y,w,h,color);
		#if defined(USE_RA8875_SEPARATE_TEXT_COLOR)
			_recoverTextColor = true;
		#endif
		roundRectHelper(x, y, (x+w)-1, (y+h)-1, r, color, true);
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
void RA8875::circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled)//0.69b32 fixed an undocumented hardware limit
{
	if (_portrait) swapvals(x0,y0);//0.69b21

	if (r < 1) r = 1;
	if (r > HEIGHT/2) r = (HEIGHT/2) - 1;//this is the (undocumented) hardware limit of RA8875
	
	if (_currentMode) changeMode(false);//we are in text mode?
	
	writeReg(RA8875_DCHR0,x0 & 0xFF);
	writeReg(RA8875_DCHR1,x0 >> 8);

	writeReg(RA8875_DCVR0,y0 & 0xFF);
	writeReg(RA8875_DCVR1,y0 >> 8);	   
	writeReg(RA8875_DCRR,r); 
	
	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid several SPI calls

	writeCommand(RA8875_DCR);
	#if defined(_FASTCPU)
		slowDownSPI(true);
	#endif
	filled == true ? writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL) : writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
	waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);//ZzZzz
	#if defined(_FASTCPU)
		slowDownSPI(false);
	#endif
}

/**************************************************************************/
/*!
      fix an hardware limit of RA8875
	  [private]
*/
/**************************************************************************/

/*
#if defined(_FASTCPU)
void RA8875::slowDownSPI(bool slow)
{
	#if defined(SPI_HAS_TRANSACTION)
		if (slow){
			_maxspeed = 10000000;
		} else {
			#if defined(__MKL26Z64__)	
				if (_altSPI){
					_maxspeed = 22000000;//TeensyLC max SPI speed on alternate SPI
				} else {
					_maxspeed = MAXSPISPEED;
				}
			#else
				_maxspeed = MAXSPISPEED;
			#endif
		}
	#else
		if (slow){
			#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_SAFE);
			#else
				SPI.setClockDivider(SPI_SPEED_SAFE);
			#endif
		} else {
			#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
				SPI.setClockDivider(_cs,SPI_SPEED_WRITE);
			#else
				SPI.setClockDivider(SPI_SPEED_WRITE);
			#endif
		}
	#endif
}
#endif
*/

/**************************************************************************/
/*!
		helper function for rects (filled or not)
		[private]
*/
/**************************************************************************/
void RA8875::rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled)
{

	if (_portrait) {//0.69b21
		swapvals(x,y);
		swapvals(w,h);
	}

	if (w < 1) return;//why draw invisible rects?
	if (h < 1) return;//why draw invisible rects?
	//if (w > _width) return;
	//if (h > _height) return;
	checkLimitsHelper(x,y);
	
	if (_currentMode) changeMode(false);//we are in text mode?

	lineAddressing(x,y,w,h);
	
	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid several SPI calls

	writeCommand(RA8875_DCR);
	filled == true ? writeData(0xB0) : writeData(0x90);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}


/**************************************************************************/
/*!
      helper function for triangles
	  [private]
*/
/**************************************************************************/
void RA8875::triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled)
{
	
	if (_portrait) {//0.69b21
		swapvals(x0,y0);
		swapvals(x1,y1);
		swapvals(x2,y2);
	}
	
	//checkLimitsHelper(x0,y0);
	//checkLimitsHelper(x1,y1);
	//checkLimitsHelper(x2,y2);
	if (_currentMode) changeMode(false);//we are in text mode?
	lineAddressing(x0,y0,x1,y1);
	//p2

	writeReg(RA8875_DTPH0,x2 & 0xFF);
	writeReg(RA8875_DTPH1,x2 >> 8);
	writeReg(RA8875_DTPV0,y2 & 0xFF);
	writeReg(RA8875_DTPV1,y2 >> 8);

	if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid several SPI calls
	
	writeCommand(RA8875_DCR);
	filled == true ? writeData(0xA1) : writeData(0x81);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      helper function for ellipse and curve
	  [private]
*/
/**************************************************************************/
void RA8875::ellipseCurveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart,uint16_t color, bool filled)
{
	if (_portrait) {
		swapvals(xCenter,yCenter);
		swapvals(longAxis,shortAxis);
		checkLimitsHelper(xCenter,yCenter);
		if (longAxis > _height/2) longAxis = (_height/2) - 1;
		if (shortAxis > _width/2) shortAxis = (_width/2) - 1;
	} else {
		checkLimitsHelper(xCenter,yCenter);
		if (longAxis > _width/2) longAxis = (_width/2) - 1;
		if (shortAxis > _height/2) shortAxis = (_height/2) - 1;
	}
	if (_currentMode) changeMode(false);//we are in text mode?
	curveAddressing(xCenter,yCenter,longAxis,shortAxis);
	
	if (color != _foreColor) setForegroundColor(color);

	writeCommand(RA8875_ELLIPSE);
	
	if (curvePart != 255){
		curvePart = curvePart % 4; //limit to the range 0-3
		filled == true ? writeData(0xD0 | (curvePart & 0x03)) : writeData(0x90 | (curvePart & 0x03));
	} else {
		filled == true ? writeData(0xC0) : writeData(0x80);
	}
	waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
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
void RA8875::roundRectHelper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled)
{

	if (_portrait) {//0.69b21
		swapvals(x,y);
		swapvals(w,h);
	}
		if (_currentMode) changeMode(false);//we are in text mode?

		lineAddressing(x,y,w,h);

		writeReg(RA8875_ELL_A0,r & 0xFF);
		writeReg(RA8875_ELL_A1,r >> 8);
		writeReg(RA8875_ELL_B0,r & 0xFF);
		writeReg(RA8875_ELL_B1,r >> 8);

		if (color != _foreColor) setForegroundColor(color);//0.69b30 avoid several SPI calls

		writeCommand(RA8875_ELLIPSE);
		filled == true ? writeData(0xE0) : writeData(0xA0);
		waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
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
void RA8875::drawArcHelper(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
	if (_currentMode) changeMode(false);//we are in text mode?
	int16_t xmin = 65535, xmax = -32767, ymin = 32767, ymax = -32767;
	float cosStart, sinStart, cosEnd, sinEnd;
	float r, t;
	float startAngle, endAngle;

	startAngle = (start / _arcAngleMax) * 360;	// 252
	endAngle = (end / _arcAngleMax) * 360;		// 807

	while (startAngle < 0) startAngle += 360;
	while (endAngle < 0) endAngle += 360;
	while (startAngle > 360) startAngle -= 360;
	while (endAngle > 360) endAngle -= 360;


	if (startAngle > endAngle) {
		drawArcHelper(cx, cy, radius, thickness, ((startAngle) / (float)360) * _arcAngleMax, _arcAngleMax, color);
		drawArcHelper(cx, cy, radius, thickness, 0, ((endAngle) / (float)360) * _arcAngleMax, color);
	} else {
		// Calculate bounding box for the arc to be drawn
		cosStart = cosDegrees(startAngle);
		sinStart = sinDegrees(startAngle);
		cosEnd = cosDegrees(endAngle);
		sinEnd = sinDegrees(endAngle);

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
void RA8875::updateActiveWindow(bool full)
{
	if (full){
		// X
		writeReg(RA8875_HSAW0,0);
		writeReg(RA8875_HSAW1,0);   
		writeReg(RA8875_HEAW0,(WIDTH) & 0xFF);
		writeReg(RA8875_HEAW1,(WIDTH) >> 8);
		// Y 
		writeReg(RA8875_VSAW0,0);
		writeReg(RA8875_VSAW1,0); 
		writeReg(RA8875_VEAW0,(HEIGHT) & 0xFF); 
		writeReg(RA8875_VEAW1,(HEIGHT) >> 8);
	} else {
		// X
		writeReg(RA8875_HSAW0,_activeWindowXL & 0xFF);
		writeReg(RA8875_HSAW1,_activeWindowXL >> 8);   
		writeReg(RA8875_HEAW0,_activeWindowXR & 0xFF);
		writeReg(RA8875_HEAW1,_activeWindowXR >> 8);
		// Y 
		writeReg(RA8875_VSAW0,_activeWindowYT & 0xFF);
		writeReg(RA8875_VSAW1,_activeWindowYT >> 8); 
		writeReg(RA8875_VEAW0,_activeWindowYB & 0xFF); 
		writeReg(RA8875_VEAW1,_activeWindowYB >> 8);
	}
}

/**************************************************************************/
/*!
		Graphic line addressing helper
		[private]
*/
/**************************************************************************/
void RA8875::lineAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	/*
	if (x0 > WIDTH-1) x0 = WIDTH - 1;
	if (y0 > HEIGHT-1) y0 = HEIGHT -1;
	if (x1 > WIDTH-1) x1 = WIDTH - 1;
	if (y1 > HEIGHT-1) y1 = HEIGHT -1;
	
	checkLimitsHelper(x0,y0);
	checkLimitsHelper(x1,y1);
	*/
	//X0
	writeReg(RA8875_DLHSR0,x0 & 0xFF);
	writeReg(RA8875_DLHSR1,x0 >> 8);
	//Y0
	writeReg(RA8875_DLVSR0,y0 & 0xFF);
	writeReg(RA8875_DLVSR1,y0 >> 8);
	//X1
	writeReg(RA8875_DLHER0,x1 & 0xFF);
	writeReg(RA8875_DLHER1,(x1) >> 8);
	//Y1
	writeReg(RA8875_DLVER0,y1 & 0xFF);
	writeReg(RA8875_DLVER1,(y1) >> 8);
}

/**************************************************************************/
/*!	
		curve addressing helper
		[private]
*/
/**************************************************************************/
void RA8875::curveAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	/*
	if (_portrait) {//0.69b21
		swapvals(x0,y0);
		swapvals(x1,y1);
	}
	*/
	//center
	writeReg(RA8875_DEHR0,x0 & 0xFF);
	writeReg(RA8875_DEHR1,x0 >> 8);
	writeReg(RA8875_DEVR0,y0 & 0xFF);
	writeReg(RA8875_DEVR1,y0 >> 8);
	//long,short ax
	writeReg(RA8875_ELL_A0,x1 & 0xFF);
	writeReg(RA8875_ELL_A1,x1 >> 8);
	writeReg(RA8875_ELL_B0,y1 & 0xFF);
	writeReg(RA8875_ELL_B1,y1 >> 8);
}



float RA8875::cosDegrees(float angle)
{
	float radians = angle / (float)360 * 2 * PI;
	return cos(radians);
}

float RA8875::sinDegrees(float angle)
{
	float radians = angle / (float)360 * 2 * PI;
	return sin(radians);
}

void RA8875::setArcParams(float arcAngleMax, int arcAngleOffset)
{
	_arcAngleMax = arcAngleMax;
	_arcAngleOffset = arcAngleOffset;
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
    writeReg(RA8875_GPIOX, on);
}

/**************************************************************************/
/*!
		PWM out
		Parameters:
		pw:pwm selection (1,2)
		p:0...255 rate
		
*/
/**************************************************************************/
void RA8875::PWMout(uint8_t pw,uint8_t p)
{
	uint8_t reg;
	pw > 1 ? reg = RA8875_P2DCR : reg = RA8875_P1DCR;
	writeReg(reg, p);
}

/**************************************************************************/
/*!
		Set the brightness of the backlight (if connected to pwm)
		(basic controls pwm 1)
		Parameters:
		val:0...255
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
		on:true(backlight on), false(backlight off)
*/
/**************************************************************************/
void RA8875::backlight(boolean on) //0.69b31 (fixed an issue with adafruit backlight)
{
	if (_size == Adafruit_480x272 || _size == Adafruit_800x480/* || _size == Adafruit_640x480*/) GPIOX(on);
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
		pw:pwm selection (1,2)
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
	writeReg(reg,(set | (clock & 0xF)));
}

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+						RESISTIVE TOUCH STUFF										 +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#if !defined(USE_EXTERNALTOUCH)//useless if you are using a capacitive TS

/**************************************************************************/
/*!   
	  Initialize support for on-chip resistive Touch Screen controller
	  It also enable the Touch Screen
	  Parameters:
	  intPin:pin connected to RA8875 INT
*/
/**************************************************************************/
void RA8875::touchBegin(uint8_t intPin) 
{
	_touchPin = intPin;
	pinMode(_touchPin, INPUT);
	digitalWrite(_touchPin, HIGH);
	//auto mode + debounce on
	writeReg(RA8875_TPCR1, RA8875_TPCR1_AUTO | RA8875_TPCR1_DEBOUNCE);
	touchEnable(true);
}

/**************************************************************************/
/*! 
      Enables or disables the on-chip touch screen controller
	  Parameters:
	  enabled:true(enable),false(disable)
*/
/**************************************************************************/
void RA8875::touchEnable(boolean enabled) 
{
	if (_touchPin < 255){
		if (!_touchEnabled && enabled) {
			_INTC1Reg |= (1 << 2); //bit set 2
			writeReg(RA8875_INTC1, _INTC1Reg);
			_TPCR0Reg |= (1 << 7); //bit set 7
			writeReg(RA8875_TPCR0, _TPCR0Reg);
			_touchEnabled = true;
		} else if (_touchEnabled && !enabled) {
			_INTC1Reg &= ~(1 << 2); //clear bit 2
			writeReg(RA8875_INTC1, _INTC1Reg);
			_TPCR0Reg &= ~(1 << 7); //clear bit 7
			writeReg(RA8875_TPCR0, _TPCR0Reg);
			_touchEnabled = false;
		}
	}
}

/**************************************************************************/
/*!   
	  Detect a touch and return true, otherwise false.
	  It also correctly resets INT register to avoid false detections.
	  Will not work at all (return false) if touchBegin/touchEnable not set.
	  Using autoclear=true: 
	  This is useful to detect any press without get coordinates!
	  Note that if you are not using autoclear you will need a readTouchADC or readTouchPixel
	  just after touchDetect or the INT register will not clear and you will get a loopback!
	  In contrast, using autoclear and readTouchADC/readTouchPixel will result in wrong readings.
	  Parameters:
	  Autoclear:(true/false) set true when you want to use this function standalone and
	  not followed by a coordinate reading with readTouchADC/readTouchPixel
*/
/**************************************************************************/
boolean RA8875::touchDetect(boolean autoclear) 
{
	if (_touchEnabled){
		#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
		if (!digitalReadFast(_touchPin)) {
		#else
		if (!digitalRead(_touchPin)) {
		#endif
			_clearTInt = true;
			if (touched()){
				if (autoclear) clearTouchInt();
				return true;
			} else {
				return false;
			}
		}
		if (_clearTInt){
			_clearTInt = false;
			clearTouchInt();
			delay(1);
		}
		return false;
	} else {
		return false;
	}
}


/**************************************************************************/
/*! 
	Return if the screen has touched or not and reset the RA8875 int flasg
*/
/**************************************************************************/
boolean RA8875::touched(void) 
{
	if (readReg(RA8875_INTC2) & RA8875_INTCx_TP) return true;
	return false;
}

/**************************************************************************/
/*!   
	  Read 10bit internal ADC of RA8875 registers and perform corrections
	  It will return always RAW data
	  Parameters:
	  x:out 0...1023
	  Y:out 0...1023

*/
/**************************************************************************/
void RA8875::readTouchADC(uint16_t *x, uint16_t *y) 
{
	#if defined(_FASTCPU)
		slowDownSPI(true);
	#endif
	uint16_t tx =  readReg(RA8875_TPXH);
	uint16_t ty =  readReg(RA8875_TPYH);
	uint8_t remain = readReg(RA8875_TPXYL);
	#if defined(_FASTCPU)
		slowDownSPI(false);
	#endif
	tx <<= 2;
	ty <<= 2;
	tx |= remain & 0x03;        // get the bottom x bits
	ty |= (remain >> 2) & 0x03; // get the bottom y bits
	  if (_portrait){
		*x = ty;
		*y = tx;
	  } else {
		tx = 1023 - tx;
		ty = 1023 - ty;
		*x = tx;
		*y = ty;
	  }
}

/**************************************************************************/
/*!   
	  Returns 10bit x,y data with TRUE scale (0...1023)
	  Parameters:
	  x:out 0...1023
	  Y:out 0...1023
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
	clearTouchInt();
}

/**************************************************************************/
/*!   
	  Returns pixel x,y data with SCREEN scale (screen width, screen Height)
	  Parameters:
	  x:out 0...screen width  (pixels)
	  Y:out 0...screen Height (pixels)
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
	clearTouchInt();
}

/**************************************************************************/
/*!   A service utility that detects if system has been calibrated in the past
	  Return true if an old calibration exists
*/
/**************************************************************************/
boolean RA8875::touchCalibrated(void) 
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
	if (uncaltetection < 1) return true;
	return false;
}

/**************************************************************************/
/*!   PRIVATE
		helper
*/
/**************************************************************************/
void RA8875::clearTouchInt(void) 
{
	writeReg(RA8875_INTC2, RA8875_INTCx_TP);
}
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
			if (_size == Adafruit_480x272 || _size == Adafruit_800x480/* || _size == Adafruit_640x480*/) GPIOX(false);
			//2)decelerate SPI clock
			#if defined(_FASTCPU)
				slowDownSPI(true);
			#else
				#if defined(SPI_HAS_TRANSACTION)
					_maxspeed = 4000000;
				#else
					#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
						SPI.setClockDivider(_cs,SPI_SPEED_READ);
					#else
						SPI.setClockDivider(SPI_SPEED_READ);
					#endif
				#endif
			#endif
			//3)set PLL to default
			setSysClock(0x07,0x03,0x02);
			//4)display off & sleep
			writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
			delay(100);
		} else {
			//1)wake up with display off(100ms)
			writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF);
			delay(100);
			//2)bring back the pll
			setSysClock(initStrings[_initIndex][0],initStrings[_initIndex][1],initStrings[_initIndex][2]);
			//writeReg(RA8875_PCSR,initStrings[_initIndex][2]);//Pixel Clock Setting Register
			delay(20);
			writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);//disp on
			delay(20);
			//4)resume SPI speed
			#if defined(_FASTCPU)
				slowDownSPI(false);
			#else
				#if defined(SPI_HAS_TRANSACTION)
					#if defined(__MKL26Z64__)
						if (_altSPI){
							_maxspeed = 22000000;
						} else {
							_maxspeed = MAXSPISPEED;
						}
					#else			
						_maxspeed = MAXSPISPEED;
					#endif
				#else
					#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
						SPI.setClockDivider(_cs,SPI_SPEED_WRITE);
					#else
						SPI.setClockDivider(SPI_SPEED_WRITE);
					#endif
				#endif
			#endif
			//5)PLL afterburn!
			setSysClock(sysClockPar[_initIndex][0],sysClockPar[_initIndex][1],initStrings[_initIndex][2]);
			//5)turn on backlight
			if (_size == Adafruit_480x272 || _size == Adafruit_800x480/* || _size == Adafruit_640x480*/) GPIOX(true);
			//writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL);
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
void RA8875::writeReg(uint8_t reg, uint8_t val) 
{
	writeCommand(reg);
	writeData(val);
}

/**************************************************************************/
/*! PRIVATE
		Returns the value inside register
		Parameters:
		reg: the register
*/
/**************************************************************************/
uint8_t RA8875::readReg(uint8_t reg) 
{
	writeCommand(reg);
	return readData(false);
}

/**************************************************************************/
/*!
		Write data
		Parameters:
		d: the data
*/
/**************************************************************************/
void RA8875::writeData(uint8_t data) 
{
	startSend();
	#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
		SPI.transfer(_cs, RA8875_DATAWRITE, SPI_CONTINUE); 
		SPI.transfer(_cs, data, SPI_LAST);
	#else
		#if defined(__AVR__) && defined(_FASTSSPORT)
			spiwrite(RA8875_DATAWRITE);
			spiwrite(data);
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
	endSend();
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
	startSend();
	#if defined(__AVR__) && defined(_FASTSSPORT)
		spiwrite(RA8875_DATAWRITE);
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
	#if !defined(ENERGIA) && !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
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
		#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
			SPI.transfer(_cs, highByte(data), SPI_CONTINUE); 
			SPI.transfer(_cs, lowByte(data), SPI_LAST);
		#else
			#if defined(__AVR__) && defined(_FASTSSPORT)
				spiwrite16(data);
			#else
				SPI.transfer(data >> 8);
				SPI.transfer(data);
			#endif
		#endif
	#endif
	endSend();
}

/**************************************************************************/
/*!	PRIVATE

*/
/**************************************************************************/
uint8_t RA8875::readData(bool stat) 
{
	#if defined(SPI_HAS_TRANSACTION)
	//uint32_t oldSpeed = _maxspeed;
		if (_inited) _maxspeed = _maxspeed/2;
	#else
		#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
			if (_inited) SPI.setClockDivider(_cs,SPI_SPEED_READ);
		#else
			if (_inited) SPI.setClockDivider(SPI_SPEED_READ);
		#endif
	#endif
	startSend();
	#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
		stat == true ? SPI.transfer(_cs, RA8875_CMDREAD, SPI_CONTINUE) : SPI.transfer(_cs, RA8875_DATAREAD, SPI_CONTINUE);
		uint8_t x = SPI.transfer(_cs, 0x0, SPI_LAST);
	#else
		#if defined(__AVR__) && defined(_FASTSSPORT)
			stat == true ? spiwrite(RA8875_CMDREAD) : spiwrite(RA8875_DATAREAD);
			uint8_t x = spiread();
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
	endSend();
	#if defined(SPI_HAS_TRANSACTION)
		//if (_inited) _maxspeed = oldSpeed;
		if (_inited) _maxspeed = _maxspeed*2;
	#else
		#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
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
	return readData(true);
}

/**************************************************************************/
/*! PRIVATE
		Write a command
		Parameters:
		d: the command
*/
/**************************************************************************/
void RA8875::writeCommand(uint8_t d) 
{
	startSend();
	#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
		SPI.transfer(_cs, RA8875_CMDWRITE, SPI_CONTINUE); 
		SPI.transfer(_cs, d, SPI_LAST);
	#else
		#if defined(__AVR__) && defined(_FASTSSPORT)
			spiwrite(RA8875_CMDWRITE);
			spiwrite(d);
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
	endSend();
}


/**************************************************************************/
/*! PRIVATE
		starts SPI communication
*/
/**************************************************************************/
/*
void RA8875::startSend()
{
#if defined(SPI_HAS_TRANSACTION)
	#if defined(__MKL26Z64__)	
		if (_altSPI){
			SPI1.beginTransaction(SPISettings(_maxspeed, MSBFIRST, SPI_MODE3));
		} else {
			SPI.beginTransaction(SPISettings(_maxspeed, MSBFIRST, SPI_MODE3));
		}
	#else
		SPI.beginTransaction(SPISettings(_maxspeed, MSBFIRST, SPI_MODE3));
	#endif
#elif !defined(ENERGIA)
	cli();//protect from interrupts
#endif
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
	digitalWriteFast(_cs, LOW);
#else
	#if !defined(ENERGIA)//UNO,DUE,ETC.
		#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)//DUE extended SPI
			//nothing
		#else//DUE (normal),UNO,ETC.
			#if defined(_FASTSSPORT)
				*csport &= ~cspinmask;
			#else
				digitalWrite(_cs, LOW);
			#endif
		#endif
	#else//ENERGIA
		digitalWrite(_cs, LOW);
	#endif
#endif
}
*/

/**************************************************************************/
/*! PRIVATE
		ends SPI communication
*/
/**************************************************************************/
/*
void RA8875::endSend()
{
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
	digitalWriteFast(_cs, HIGH);
#else
	#if !defined(ENERGIA)
		#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)//DUE extended SPI
			//nothing
		#else//DUE (normal),UNO,ETC.
			#if defined(_FASTSSPORT)
				*csport |= cspinmask;
			#else
				digitalWrite(_cs, HIGH);
			#endif
		#endif
	#else//ENERGIA
		digitalWrite(_cs, HIGH);
	#endif
#endif
#if defined(SPI_HAS_TRANSACTION)
	#if defined(__MKL26Z64__)	
		if (_altSPI){
			SPI1.endTransaction();
		} else {
			SPI.endTransaction();
		}
	#else
		SPI.endTransaction();
	#endif
#elif !defined(ENERGIA)
	sei();//enable interrupts
#endif
} 
*/
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+				CAPACITIVE TOUCH SCREEN CONTROLLER	FT5206						     +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#if defined (USE_FT5206_TOUCH)

void RA8875::isr(void)
{
	_touched = true;
}

void RA8875::armTouchISR(bool force) 
{
	if (force || _needISRrearm){
		_needISRrearm = false;
#ifdef digitalPinToInterrupt
		attachInterrupt(digitalPinToInterrupt(_ctpInt),isr,FALLING);
#else
		attachInterrupt(0,isr,FALLING);
#endif
		_touched = false;
	}
}

void RA8875::initFT5206(void)
{
	regFT5206(0x80,0x16);//ID_G_THGROUP
	regFT5206(0x81,0x3C);//ID_G_THPEAK
	regFT5206(0x82,0xE9);//ID_G_THCAL
	regFT5206(0x83,0x01);//D_G_THWATER
	regFT5206(0x84,0x01);//ID_G_THTEMP
	regFT5206(0x85,0xA0);//ID_G_THDIFF
	regFT5206(0x87,0x0A);//ID_G_TIME_ENTER_MONITOR
	regFT5206(0x88,0x06);//ID_G_PERIODACTIVE
	regFT5206(0x89,0x28);//ID_G_PERIODMONITOR
	regFT5206(0x00,0x00);//Device Mode
}

void RA8875::regFT5206(uint8_t reg,uint8_t val)
{
    // save I2C bitrate
	#if !defined(__SAM3X8E__)
		uint8_t twbrbackup = TWBR;
		TWBR = 12; // upgrade to 400KHz!
	#endif
	Wire.beginTransmission(_ctpAdrs);
	Wire.write(reg);
	Wire.write(val);
	Wire.endTransmission(_ctpAdrs);
	#if !defined(__SAM3X8E__)
		TWBR = twbrbackup;
	#endif
}

bool RA8875::touched(bool safe)
{
	_needISRrearm = safe;
    if (_touched){
		if (_needISRrearm){
#ifdef digitalPinToInterrupt
			detachInterrupt(digitalPinToInterrupt(_ctpInt));
#else
			detachInterrupt(0);
#endif
		} else {
			_touched = false;
		}
		return true;
    }
	return false;
}

void RA8875::setTouchLimit(uint8_t limit)
{
	if (limit > 5) limit = 5;
	_maxTouch = limit;
}

uint8_t RA8875::getTouchLimit(void)
{
	return _maxTouch;
}

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
				touch_coordinates[i-1][1] = (WIDTH - 1) - (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]);
			break;
			case 2://ok
				touch_coordinates[i-1][0] = (WIDTH - 1) - (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]);
				touch_coordinates[i-1][1] = (HEIGHT - 1) -(((_cptRegisters[coordRegStart[i-1] + 2] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 3]);
			break;
			case 3://ok
				touch_coordinates[i-1][0] = (HEIGHT - 1) - (((_cptRegisters[coordRegStart[i-1] + 2] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 3]);
				touch_coordinates[i-1][1] = (((_cptRegisters[coordRegStart[i-1]] & 0x0f) << 8) | _cptRegisters[coordRegStart[i-1] + 1]);
			break;
		}
		if (i == _maxTouch) return i;
	} 
    return _currentTouches;
}

uint8_t RA8875::getTouchState(void)
{
	return _currentTouchState;
}

uint8_t RA8875::getTouches(void)
{
	return _currentTouches;
}

uint8_t RA8875::getGesture(void)
{
	return _gesture;
}

#endif

/*
void RA8875::debugData(uint16_t data,uint8_t len)
{
	int i;
  for (i=len-1; i>=0; i--){
    if (bitRead(data,i)==1){
      Serial.print("1");
    } 
    else {
      Serial.print("0");
    }
  }
  Serial.print(" -> 0x");
  Serial.print(data,HEX);
  Serial.print("\n");

}
*/

/*

 * top to bottom
 * forward
 * bands
var bandX = 0;
do {
    for (var y = 0; y < image.height; y++) {
        for (var x = 0; x < image.bandSize; x++) {
            drawPixel(bandX + x, y);
        }
    }
    bandX += image.bandSize;
} while (bandX < image.width);

*/




/**************************************************************************/
/*! 
		really experimental!!!!!
*/
/**************************************************************************/


void RA8875::gPrint(uint16_t x,uint16_t y,int num,uint16_t color,uint8_t scale,const struct FONT_DEF *strcut1)
{
	char in[8];
	itoa(num,in,10);
	gPrint(x,y,in,color,scale,strcut1);
}

/**************************************************************************/
/*! 
		Will change - only for test!!!!!
		Almost fixed, but still not able to work in rotate
*/
/**************************************************************************/
void RA8875::gPrint(uint16_t x,uint16_t y,const char *in,uint16_t color,uint8_t scale,const struct FONT_DEF *strcut1)
{
	if (_currentMode) changeMode(false);//we are in text mode?
	if (scale < 1) scale = 1;
	unsigned int offset;
	unsigned char by = 0, mask = 0;
	uint16_t i,j,h,w,NrBytes,idx,idy,s;
	unsigned char cmap;
	uint16_t allwidth = 0;
	while ((cmap = *in++)) {
		cmap = pgm_read_byte(&strcut1->mapping_table[cmap]);
		w = strcut1->glyph_width;//12
		if (w == 0) w = pgm_read_byte(&strcut1->width_table[cmap]);
		uint16_t buffer[w*scale];//temp buffer
		offset = pgm_read_word(&strcut1->offset_table[cmap]);
		h = strcut1->glyph_height;
        NrBytes = ((w - 1) / 8) + 1;
		idy = 0;
		//setActiveWindow(x+allwidth,(x+allwidth)+w*scale,h*scale,idy+y+(j/NrBytes));
		for (j = 0;j < (h * NrBytes); j+=NrBytes){// height
			idx = 0;
			for (i = 0;i < w; i++){//  width
				
			    if (i%8 == 0) {//read glyph
					by = pgm_read_byte(&strcut1->glyph_table[offset + j + (i/8)]);
					mask = 0x80;
			    }
				
				for (s=0;s<scale;s++){//scaling
					if (by & mask) {
						buffer[idx] = color;
						//Serial.print("*");
					} else {
						//background (to do)
						buffer[idx] = _RA8875_DEFAULTTXTBKGRND;
						//Serial.print(" ");
					}
					idx++;
				}//scale
				mask >>= 1;
			}//End i

			if (scale < 2) idy = 0;

			for (s=0;s<scale;s++){//scaling
				int16_t nX = (x+allwidth)+(w*scale);
				if (nX > _width) return;
				if (h*scale > _height) return;
				setXY(x+allwidth,idy+y+(j/NrBytes)-j);
				writeCommand(RA8875_MRWC);
				startSend();
				#if defined(__AVR__) && defined(_FASTSSPORT)
					spiwrite(RA8875_DATAWRITE);
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
				for (i=0;i<w*scale;i++){
					#if !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
						#if defined(SPI_HAS_TRANSACTION) && defined(__MKL26Z64__)	
							if (_altSPI){
								SPI1.transfer16(buffer[i]);//should be fixed already
							} else {
								SPI.transfer16(buffer[i]);//should be fixed already
							}
						#else
							SPI.transfer16(buffer[i]);//should be fixed already
						#endif
					#else
						#if defined(__SAM3X8E__) && defined(SPI_DUE_MODE_EXTENDED)
							SPI.transfer(_cs, buffer[i] >> 8, SPI_CONTINUE); 
							if (i != (w*scale)-1){
								SPI.transfer(_cs, buffer[i] & 0xFF, SPI_CONTINUE);
							} else {
								SPI.transfer(_cs, buffer[i] & 0xFF, SPI_LAST);
							}
						#else
							#if defined(__AVR__) && defined(_FASTSSPORT)
								spiwrite16(buffer[i]);
							#else
								SPI.transfer(buffer[i] >> 8);
								SPI.transfer(buffer[i] & 0xFF);
							#endif
						#endif
					#endif
				}
				endSend();
				idy++;
				
			}//scale
		}// End j
		allwidth+=w*scale;
	}// End K
} 


/**************************************************************************/
/*! 
		this will disappear - only for test!!!!!
*/
/**************************************************************************/

void RA8875::gPrintEfx(uint16_t x,uint16_t y,const char *in,uint16_t color,uint8_t pixellation,const struct FONT_DEF *strcut1)
{
	unsigned int offset;
	unsigned char by = 0, mask = 0;
	uint16_t i,j,h,w,NrBytes,cX,cY;
	unsigned char cmap;
	uint16_t allwidth = 0;
	if (pixellation < 1) pixellation = 1;
	while ((cmap = *in++)) {
		cmap = pgm_read_byte(&strcut1->mapping_table[cmap]);
		w = strcut1->glyph_width;
		if (w == 0) w = pgm_read_byte(&strcut1->width_table[cmap]);
		offset = pgm_read_word(&strcut1->offset_table[cmap]);
		h = strcut1->glyph_height;
        NrBytes = ((w - 1) / 8) + 1;
		cX = x;
		cY = y;
		for (j = 0;j < (h * NrBytes); j+=NrBytes){// height
			//fillRect(cX,y,w*pixellation,(h*pixellation),RA8875_WHITE);
			for (i = 0;i < w; i++){//  width
			    if (i%8 == 0) {
					by = pgm_read_byte(&strcut1->glyph_table[offset + j + (i/8)]);
					mask = 0x80;
			    }
				cX = x + (i + allwidth) * pixellation;
				cY = y + (j / NrBytes) * pixellation;
				if (by & mask) {
					drawPixel(cX,cY, color);
					drawCircle(cX,cY,2,RA8875_GRAYSCALE*j);
	 			}// else {
					//background (to do)
				//}
	 			mask >>= 1;
			}//End i
		}// End j
		allwidth+=w;
	}// End K
} 

/*---------------------------------------------------------------------------------------
						KEYPAD - still working on
						UNDER CONTRUCTION - PLEASE DO NOT ACTIVATE (NOT WORKING)
****************************************************************************************/

/*
#if defined(USE_RA8875_KEYMATRIX)
static const uint8_t DefaultKeyMap[(4*5)+2] = {
    0,
    1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    255
};

void RA8875::keypadInit(bool scanEnable, bool longDetect, uint8_t sampleTime, uint8_t scanFrequency,
                             uint8_t longTimeAdjustment, bool interruptEnable, bool wakeupEnable)
{
    uint8_t reg = 0;
	if (sampleTime > 3) sampleTime = 3;
	if (scanFrequency > 7) scanFrequency = 7;
	if (longTimeAdjustment > 3) scanFrequency = 3;
    reg |= (scanEnable) ? 0x80 : 0x00;
    reg |= (longDetect) ? 0x40 : 0x00;
    reg |= (sampleTime & 0x03) << 4;
    reg |= (scanFrequency & 0x07);
    writeReg(RA8875_KSCR1, reg);   // KSCR1 - Enable Key Scan

    reg = 0;
    reg |= (wakeupEnable) ? 0x80 : 0x00;
    reg |= (longTimeAdjustment & 0x03) << 2;
    writeReg(RA8875_KSCR2, reg);  // KSCR2

    reg = readReg(RA8875_INTC1);  // read INT
    reg &= ~0x10;
    reg |= (interruptEnable) ? 0x10 : 0x00;
    writeReg(RA8875_INTC1, reg);  // write INT
}


boolean RA8875::keypadTouched(void)
{
    return (readReg(RA8875_INTC2) & 0x10); 
}

//#define GETC_DEV
uint8_t RA8875::getKey(void)
{
	uint8_t pKeyMap[(4*5)+2];
	memcpy (pKeyMap,DefaultKeyMap,(4*5)+2);

#ifdef GETC_DEV
    uint8_t keyCode1, keyCode2;
#endif
    uint8_t keyCode3;
    static uint8_t count = 0;
    uint8_t col, row;
    uint8_t key;
	/*	
    while (!keypadTouched()) {
		delayMicroseconds(10);//10
    }
	*/
	/*
    // read the key press number
    uint8_t keyNumReg = readReg(RA8875_KSCR2) & 0x03;
    count++;
    switch (keyNumReg) {
        case 0x01:      // one key
            keyCode3 = readReg(RA8875_KSDR0);
#ifdef GETC_DEV
            keyCode2 = 0;
            keyCode1 = 0;
#endif
            break;
        case 0x02:      // two keys
            keyCode3 = readReg(RA8875_KSDR1);
#ifdef GETC_DEV
            keyCode2 = readReg(RA8875_KSDR0);
            keyCode1 = 0;
#endif
            break;
        case 0x03:      // three keys
            keyCode3 = readReg(RA8875_KSDR2);
#ifdef GETC_DEV
            keyCode2 = readReg(RA8875_KSDR1);
            keyCode1 = readReg(RA8875_KSDR0);
#endif
            break;
        default:         // no keys (key released)
            keyCode3 = 0xFF;
#ifdef GETC_DEV
            keyCode2 = 0;
            keyCode1 = 0;
#endif
            break;
    }
    if (keyCode3 == 0xFF) {
        key = pKeyMap[0];                    // Key value 0
    } else {
        row = (keyCode3 >> 4) & 0x03;
        col = (keyCode3 &  7);
        key = row * 5 + col + 1;    // Keys value 1 - 20
        if (key > 21) key = 21;
        key = pKeyMap[key];
        key |= (keyCode3 & 0x80);   // combine the key held flag
    }
#ifdef GETC_DEV // for Development only
    setCursor(0,20);
	setTextColor(0xFFFF,0x0000);
	print("              ");
	setCursor(0,20);
	print("   Reg: ");
	println(keyNumReg);
	print("   key1: ");
	println(keyCode1);
	print("   key2: ");
	println(keyCode2);
	print("   key3: ");
	println(keyCode3);
	print("  count: ");
	println(count);
	print("    key: ");
	println(key);
#endif
    writeReg(RA8875_INTC2, 0x10);       // Clear KS status
    return key;
}


void RA8875::enableKeyScan(bool on)
{
    if (on) {
        writeReg(RA8875_KSCR1, (1 << 7) | (0 << 4 ) | 1 );       // enable key scan
    } else {
        writeReg(RA8875_KSCR1, (0 << 7));
    }
}
 
uint8_t RA8875::getKeyValue(void)
{
    uint8_t data = 0xFF;
    data = readReg(RA8875_KSDR0);

    delay(1);
	#ifdef GETC_DEV
	if (data != 255){
		setTextColor(0xFFFF,0x0000);
		setCursor(0,20);
		print("                ");
		setCursor(0,20);
		print("Reg: ");
		println(data);
	}
	#endif
    // Clear key interrupt status
	uint8_t temp = readReg(RA8875_INTC2);
    writeReg(RA8875_INTC2,temp | 0x10);
    return data;
}
 
boolean RA8875::isKeyPress(void)
{
    uint8_t temp = readReg(RA8875_INTC2);
    if (temp & 0x10) return true;
    return false;
}

#endif
*/
