#include <SPI.h>
#include "RA8875.h"



#ifdef SPI_HAS_TRANSACTION
static SPISettings settings;
#endif

/**************************************************************************/
/*!
	Contructor
	CS: SPI SS pin
	RST: Reset pin
	If Teensy CS used you can choose the SPI0 or SPI1
	spiInterface: 0 or 1 - default 0
*/
/**************************************************************************/
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
/*
	#if defined(__MKL26Z64__)//this one has 2 SPI
		RA8875::RA8875(const uint8_t CS,const uint8_t RST,uint8_t spiInterface){
			_SPIint = spiInterface;
			if (_SPIint > 1) _SPIint = 1;
	#else
		RA8875::RA8875(const uint8_t CS,const uint8_t RST){
	#endif
*/
	RA8875::RA8875(const uint8_t CS,const uint8_t RST,uint8_t mosi_pin,uint8_t sclk_pin,uint8_t miso_pin){
			_cs = CS;
			_rst = 255;
			_mosi = mosi_pin;
			_miso = miso_pin;
			_sclk = sclk_pin;
			if (RST != 255) _rst = RST;
		}
#else
	#if defined(NEEDS_SET_MODULE)
		RA8875::RA8875(const uint8_t module, const uint8_t RST) {
			selectCS(module);
	#else
/**************************************************************************/
/*!
	Constructor
	CS: SPI SS pin
	RST: Reset pin (255 disable it)
*/
/**************************************************************************/

		RA8875::RA8875(const uint8_t CS, const uint8_t RST) {
			_cs = CS;
	#endif
			_rst = 255;
			if (RST != 255) _rst = RST;
		}
#endif

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
	_size = s;
	_portrait = false;
	_unsupported = false;
	_inited = false;
	_sleep = false;
	_hasLayerLimits = false;
	_maxLayers = 2;
	_currentLayer = 0;
	_useMultiLayers = false;//starts with one layer only
	_currentMode = GRAPHIC;
	_brightness = 255;
	_cursorX = 0; _cursorY = 0; _scrollXL = 0; _scrollXR = 0; _scrollYT = 0; _scrollYB = 0;
	_textSize = X16;
	_fontSpacing = 0;
	/* set-->  _commonTextPar  <--
	_extFontRom = false;
	_autoAdvance = true;
	_textWrap = user defined
	_fontFullAlig = false;
	_fontRotation = false;
	*/
	_commonTextPar = 0b00000010;
	bitWrite(_commonTextPar,2,_DFT_RA8875_TEXTWRAP);//set _textWrap
	_fontRomType = _DFT_RA8875_EXTFONTROMTYPE;
	_fontRomCoding = _DFT_RA8875_EXTFONTROMCODING;
	_fontSource = INT;
	_txtForeColor = RA8875_WHITE;
	_txtBackColor = RA8875_BLACK;
	_rotation = 0;
	_fontInterline = 0;
	_fontFamily = STANDARD;
	_textCursorStyle = NOCURSOR;
	_color_bpp = 16;
	if (colors != 16) _color_bpp = 8;
	switch (_size){
		case RA8875_320x240:
			_width = 320;
			_height = 240;
			_initIndex = 0;
		break;
		case RA8875_480x272:
		case Adafruit_480x272:
			_width = 480;
			_height = 272;
			_initIndex = 1;
		break;
		case RA8875_640x480:
		case Adafruit_640x480:
			_width = 640;
			_height = 480;
			if (_color_bpp < 16){
				_maxLayers = 2;
			} else {
				_maxLayers = 1;
			}
			_hasLayerLimits = true;
			_initIndex = 2;
		break;
		case RA8875_800x480:
		case Adafruit_800x480:
			_width = 800;
			_height = 480;
			if (_color_bpp < 16){
				_maxLayers = 2;
			} else {
				_maxLayers = 1;
			}
			_hasLayerLimits = true;
			_initIndex = 3;
		break;
		default:
		_unsupported = true;//error, not supported
		return;
	}
	WIDTH = _width;
	HEIGHT = _height;
	#if !defined(USE_EXTERNALTOUCH)
		_touchPin = 255;
		_clearTInt = false;
		_touchEnabled = false;
		if (!touchCalibrated()) {//added by MorganSandercock
			_tsAdcMinX = 0; _tsAdcMinY = 0; _tsAdcMaxX = 1024; _tsAdcMaxY = 1024;
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
/*	Touch Panel Control Register 0     [0x70]
	7: 0(disable, 1:(enable)
	6,5,4:TP Sample Time Adjusting (000...111)
	3:Touch Panel Wakeup Enable 0(disable),1(enable)
	2,1,0:ADC Clock Setting (000...111) set fixed to 010: (System CLK) / 4, 10Mhz Max! */
		_TPCR0Reg = RA8875_TPCR0_WAIT_4096CLK | RA8875_TPCR0_WAKEDISABLE | RA8875_TPCR0_ADCCLK_DIV4;
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
	/*
	#if defined(__MKL26Z64__) && defined(SPI_HAS_TRANSACTION)//ready for the multi SPI
		#if !defined(SPI1_BR)
			#error you need to update SPI library!
		#endif
		//this is really theory since new Teensy SPI library it's still in development
		if (_SPIint > 0){
			SPI1.begin();
		} else {
			SPI.begin();
		}
	#else//rest of the world
		SPI.begin();
	#endif 
	*/
	//------------------------------- Start SPI initialization ------------------------------------------
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
		if ((_mosi == 11 || _mosi == 7) && (_miso == 12 || _miso == 8) && (_sclk == 13 || _sclk == 14)) {//valid SPI pins?
			SPI.setMOSI(_mosi);
			SPI.setMISO(_miso);
			SPI.setSCK(_sclk);
		} else {
			return;
		}
		if (!SPI.pinIsChipSelect(_cs)) return;
	#endif
	SPI.begin();
	#if !defined(ENERGIA)//energia needs this here
		pinMode(_cs, OUTPUT);
		digitalWrite(_cs, HIGH);
	#endif
	if (_rst < 255){//time for hardware reset RA8875
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(5);
		digitalWrite(_rst, LOW);
		delay(20);
		digitalWrite(_rst, HIGH);
		delay(150);
	}
#if defined(NEEDS_SET_MODULE)//energia specific
	SPI.setModule(SPImodule);
#endif

	
#if defined(SPI_HAS_TRANSACTION)
	settings = SPISettings(4000000, MSBFIRST, SPI_MODE3);//we start in low speed here!
#else//do not use SPItransactons
	#if defined(ENERGIA)
		SPI.setClockDivider(SPI_SPEED_WRITE);//4Mhz (6.6Mhz Max)
		delay(50);
	#else
		SPI.setClockDivider(SPI_CLOCK_DIV4);//4Mhz (6.6Mhz Max)
		delay(50);
	#endif
	SPI.setDataMode(SPI_MODE3);
#endif
	#if defined(ENERGIA)//dunno why but energia wants this here or not work!
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	#endif
	initialize();
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
	if (_unsupported) return;//better stop here!
	_inited = false;
	if (_rst > 254) {//No Hard Reset? time for soft reset
		writeCommand(RA8875_PWRR);
		writeData(RA8875_PWRR_SOFTRESET);
		delay(10);
		writeData(RA8875_PWRR_NORMAL);
		delay(200);
	}
	//set the sysClock
	setSysClock(initStrings[_initIndex][0],initStrings[_initIndex][1],initStrings[_initIndex][2]);
	//color space setup
	if (_color_bpp < 16){//256
		writeReg(RA8875_SYSR,0x00);//256
	} else {
		writeReg(RA8875_SYSR,0x0C);//65K
	}
	writeReg(RA8875_HDWR,initStrings[_initIndex][3]);//LCD Horizontal Display Width Register
	writeReg(RA8875_HNDFTR,initStrings[_initIndex][4]);//Horizontal Non-Display Period Fine Tuning Option Register
	writeReg(RA8875_HNDR,initStrings[_initIndex][5]);////LCD Horizontal Non-Display Period Register
	writeReg(RA8875_HSTR,initStrings[_initIndex][6]);////HSYNC Start Position Register
	writeReg(RA8875_HPWR,initStrings[_initIndex][7]);////HSYNC Pulse Width Register
	writeReg(RA8875_VDHR0,initStrings[_initIndex][8]);////LCD Vertical Display Height Register0
	writeReg(RA8875_VDHR1,initStrings[_initIndex][9]);////LCD Vertical Display Height Register1
	writeReg(RA8875_VNDR0,initStrings[_initIndex][10]);////LCD Vertical Non-Display Period Register 0
	writeReg(RA8875_VNDR1,initStrings[_initIndex][11]);////LCD Vertical Non-Display Period Register 1
	writeReg(RA8875_VSTR0,initStrings[_initIndex][12]);////VSYNC Start Position Register 0
	writeReg(RA8875_VSTR1,initStrings[_initIndex][13]);////VSYNC Start Position Register 1
	writeReg(RA8875_VPWR,initStrings[_initIndex][14]);////VSYNC Pulse Width Register
	setActiveWindow(0,(_width-1),0,(_height-1));//set the active window
	clearMemory(true);//clear memory (we are not in multilayer o the visible memory will be cleared)
	//end of hardware initialization
	delay(10); //100
    //now starts the first time setting up
	displayOn(true);//turn On Display
	delay(10);
	backlight(true);
	setCursorBlinkRate(DEFAULTCURSORBLINKRATE);//set default blink rate
	setIntFontCoding(DEFAULTINTENCODING);//set default internal font encoding
	setFont(INT);	//set internal font use
	setTextColor(RA8875_WHITE);//since the blackground it's black...
	//set cursor at 0,0
	writeReg(RA8875_F_CURXL,(0 & 0xFF));
	writeReg(RA8875_F_CURXH,(0 >> 8));
	writeReg(RA8875_F_CURYL,(0 & 0xFF));
	writeReg(RA8875_F_CURYH,(0 >> 8));
	delay(1);
	//postburner PLL!
	setSysClock(sysClockPar[_initIndex][0],sysClockPar[_initIndex][1],initStrings[_initIndex][2]);
	_inited = true;//from here we will go at high speed!
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
	return true when register has done the job, otherwise false.
*/
/**************************************************************************/
boolean RA8875::waitPoll(uint8_t regname, uint8_t waitflag) 
{
	unsigned long timeout = millis();
	while (1) {
		uint8_t temp = readReg(regname);
		if (!(temp & waitflag)) return true;
		if ((millis() - timeout) > 100) return false;//emergency exit!
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
	//Found this wasn't working properly on the Adafruit 480x272 4.3" display
	//It would just never come up as ready during the initialise.
	//clearMemory(true) calls waitBusy(0x80), looking for the memory read/write busy bit.
	//The status returned from the display was initially 0xC0 which the 
	//datasheet says corresponds to memory busy and BTE (block transfer) busy.
	//After about 5ms, this would change to 0x80 and stay there. It
	//never reported un-busy so this function would hang.
	//Putting in the timeout allowed the example programs to run and it works!
	//M.Sandercock
	uint8_t w; 	
	unsigned long start = millis();
	do {
	if (res == 0x01) writeCommand(RA8875_DMACR);//dma
	w = readStatus();
	if ((millis() - start) > 10) return;//expect initialization to take 5ms, but give it some leeway.
	} while ((w & res) == res);
}

/**************************************************************************/
/*!		
		Clear memory (different from fillScreen!)
	    Parameters:
		full: true(clear all memory), false(clear active window only)
		When in multilayer it automatically clear L1 & L1 and switch back to current layer
*/
/**************************************************************************/
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
void RA8875::setActiveWindow(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB)
{
	if (XR >= WIDTH) XR = WIDTH-1;
	if (YB >= HEIGHT) YB = HEIGHT-1;
    // X 
	writeReg(RA8875_HSAW0,XL);
	writeReg(RA8875_HSAW1,XL >> 8);   
	writeReg(RA8875_HEAW0,XR);
	writeReg(RA8875_HEAW1,XR >> 8);
    // Y 
	writeReg(RA8875_VSAW0,YT);
	writeReg(RA8875_VSAW1,YT >> 8); 
	writeReg(RA8875_VEAW0,YB); 
	writeReg(RA8875_VEAW1,YB >> 8);
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
void RA8875::changeMode(enum RA8875modes m) 
{
	//if (m == _currentMode) return;
	writeCommand(RA8875_MWCR0);
	if (m == GRAPHIC){
		 if (_currentMode == TEXT){//avoid consecutive calls
			 _MWCR0Reg &= ~(1 << 7);
			 _currentMode = GRAPHIC;
			writeData(_MWCR0Reg);
		}
	} else {
		if (_currentMode == GRAPHIC){//avoid consecutive calls
			_MWCR0Reg |= (1 << 7);
			_currentMode = TEXT;
			writeData(_MWCR0Reg);
		}
	}
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
	    Graphics and text will be rotated. Anything already on the screen
		    will be moved to the new orientation but mirrored if changing
			from portrait to landscape. 
	    Use same rotation numbers as other Adafruit displays.
		Coorinates are flipped, but not rotated:
			X is always the long axis of the screen
			Y is always the short axis of the screen
			Therefore _width and _height don't change
			(0,0) is top left for your orientation
	    Note this also flips the touchReadPixel() calibration to match.
	  Parameters:
	  rotation:
	    0 = default, connector to bottom
		1 = connector to right
		2 = connector to top
		3 = connector to left
*/
/**************************************************************************/
void RA8875::setRotation(uint8_t rotation)
{
	_rotation = rotation % 4; //limit to the range 0-3
	switch (_rotation) {
	case 0:
		//default, connector to bottom
		_portrait = false;
		scanDirection(0,0);
		_FNCR1Reg &= ~(1 << 4);
		_width = WIDTH;
		_height = HEIGHT;
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 0; 
				_tsAdcMinY = 0; 
				_tsAdcMaxX = 1024; 
				_tsAdcMaxY = 1024;
			} else {
				_tsAdcMinX = TOUCSRCAL_XLOW;
				_tsAdcMinY = TOUCSRCAL_YLOW;
				_tsAdcMaxX = TOUCSRCAL_XHIGH;
				_tsAdcMaxY = TOUCSRCAL_YHIGH;
			}
		#endif
    break;
	case 1:
		//connector to right
		_portrait = true;
		scanDirection(1,0);
		_FNCR1Reg |= (1 << 4);
		_width = HEIGHT;
		_height = WIDTH;
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 1024; 
				_tsAdcMinY = 0; 
				_tsAdcMaxX = 0; 
				_tsAdcMaxY = 1024;
			} else {
				_tsAdcMinX = TOUCSRCAL_XHIGH;
				_tsAdcMinY = TOUCSRCAL_YLOW;
				_tsAdcMaxX = TOUCSRCAL_XLOW;
				_tsAdcMaxY = TOUCSRCAL_YHIGH;
			}
		#endif
    break;
	case 2:
		//connector to top
		_portrait = false;
		scanDirection(1,1);
		_FNCR1Reg &= ~(1 << 4);
		_width = WIDTH;
		_height = HEIGHT;
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 1024; 
				_tsAdcMinY = 1024; 
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
		//connector to left
		_portrait = true;
		scanDirection(0,1);
		_FNCR1Reg |= (1 << 4);
		_width = HEIGHT;
		_height = WIDTH;
		#if !defined(USE_EXTERNALTOUCH)
			if (!touchCalibrated()) {
				_tsAdcMinX = 0; 
				_tsAdcMinY = 1024; 
				_tsAdcMaxX = 1024; 
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
	writeReg(RA8875_FNCR1,_FNCR1Reg);//0.69b21
	setTextColor(_txtForeColor,_txtBackColor);//0.69b21 after rotation text loose it's previus color parameters!
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
	bool modeChanged = false;
	uint8_t i;
	if (_currentMode != GRAPHIC) {//was in text!
		changeMode(GRAPHIC);
		modeChanged = true;
	}
	writeReg(RA8875_CGSR,address);
	writeTo(CGRAM);
	writeCommand(RA8875_MRWC);
	for (i=0;i<16;i++){
		writeData(symbol[i]);
	}
	writeReg(RA8875_MWCR1, tempMWCR1);//restore register (MorganSandercock)
	if (modeChanged) changeMode(TEXT);//back to text
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
	if (_currentMode != TEXT) changeMode(TEXT);
	uint8_t oldRegState = _FNCR0Reg;
	uint8_t i;
	bitSet(oldRegState,7);//set to CGRAM
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
		case ER3304_1://thanks the Experimentalist
			temp &= 0x1F; temp |= 0x80;
		break;
		default:
			//_extFontRom = false;//wrong type, better avoid for future
			bitClear(_commonTextPar,0);//wrong type, better avoid for future
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
			//_extFontRom = false;//wrong coding, better avoid for future
			bitClear(_commonTextPar,0);//wrong coding, better avoid for future
			return;//cannot continue, exit
		}
		_fontRomCoding = erc;
		_SFRSETReg = temp;
		setExtFontFamily(erf,false);	
		//_extFontRom = true;
		bitSet(_commonTextPar,0);
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
	//enum RA8875fontCoding c
	if (s == INT){
		//check the font coding
		if (bitRead(_commonTextPar,0) == 1) {//0.96b22 _extFontRom = true
			setFontSize(X16,false);
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
			setFontSize(X24,false);////X24 size
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
		Set global text wrapping on/off
		Parameters:
		wrap: true(wrap on), false(wrap off [default])
		0.69b23
*/
/**************************************************************************/
/*
void RA8875::setTextWrap(bool wrap)
{
	bitWrite(_commonTextPar,2,wrap);
		
}
*/
/**************************************************************************/
/*!   
		Set the Text position for write Text only.
		Parameters:
		x:horizontal in pixels
		y:vertical in pixels
*/
/**************************************************************************/
void RA8875::setCursor(uint16_t x, uint16_t y) 
{
	if (_portrait) swapvals(x,y);
	if (bitRead(_commonTextPar,2) == 0){//0.69b22 (_textWrap)
		if (x >= _width) x = _width-1;
		if (y >= _height) y = _height-1;
	}
	_cursorX = x;
	_cursorY = y;
	writeReg(RA8875_F_CURXL,(x & 0xFF));
	writeReg(RA8875_F_CURXH,(x >> 8));
	writeReg(RA8875_F_CURYL,(y & 0xFF));
	writeReg(RA8875_F_CURYH,(y >> 8));
}

/**************************************************************************/
/*!   
		Update the library tracked  _cursorX,_cursorX and give back
		Parameters:
		x*:horizontal pos in pixels
		y*:vertical pos in pixels
		USE: xxx.getCursor(&myX,&myY);
*/
/**************************************************************************/
void RA8875::getCursor(uint16_t *x, uint16_t *y) 
{
	//fixed in 0.69b22
	uint8_t t1,t2,t3,t4;
	t1 = readReg(RA8875_F_CURXL);
	t2 = readReg(RA8875_F_CURXH);
	t3 = readReg(RA8875_F_CURYL);
	t4 = readReg(RA8875_F_CURYH);
	if (_portrait){
		swapvals(t1,t3);
		swapvals(t2,t4);
	}
	_cursorX = (t2 << 8) | (t1 & 0xFF);
	_cursorY = (t4 << 8) | (t3 & 0xFF);
	*x = _cursorX;
	*y = _cursorY;
	/*
	_cursorX = (t2 << 8) | (t1 & 0xFF);
	t1 = readReg(RA8875_F_CURYL);
	t2 = readReg(RA8875_F_CURYH);
	_cursorY = (t2 << 8) | (t1 & 0xFF);
	*x = _cursorX;
	*y = _cursorY;
	*/
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
    uint8_t MWCR1Reg = readReg(RA8875_MWCR1) & 0x01;
    uint8_t cW = 0;
    uint8_t cH = 0;
	_textCursorStyle = c;
    if (c != NOCURSOR) {
		bitSet(_MWCR0Reg,6);
	} else {
		bitClear(_MWCR0Reg,6);
	}
    if (blink) _MWCR0Reg |= 0x20;//blink or not?
    writeReg(RA8875_MWCR0, _MWCR0Reg);//set cursor
    writeReg(RA8875_MWCR1, MWCR1Reg);//close graphic cursor
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
		fColor:16bit foreground color (text) RGB565
		bColor:16bit background color RGB565
*/
/**************************************************************************/
void RA8875::setTextColor(uint16_t fColor, uint16_t bColor)
{
	setForegroundColor(fColor);
	setBackgroundColor(bColor);
	_txtForeColor = fColor;
	_txtBackColor = bColor;
	_FNCR1Reg &= ~(1 << 6);
	writeReg(RA8875_FNCR1,_FNCR1Reg);
}

/**************************************************************************/
/*!		
		set the text color w transparent background
		Parameters:
		fColor:16bit foreground color (text) RGB565
*/
/**************************************************************************/

void RA8875::setTextColor(uint16_t fColor)
{
	setForegroundColor(fColor);
	_txtForeColor = fColor;
	_FNCR1Reg |= (1 << 6);
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
	if (scale > 3) scale = 3;
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
	if (vscale > 3) vscale = 3;
	if (hscale > 3) hscale = 3;
 	_FNCR1Reg &= ~(0xF); // clear bits from 0 to 3
	_FNCR1Reg |= hscale << 2;
	_FNCR1Reg |= vscale;
	writeReg(RA8875_FNCR1,_FNCR1Reg);
	_textHScale = hscale;
	_textVScale = vscale;
}

/**************************************************************************/
/*!		
		Normally at every char the cursor advance by one
		You can stop/enable this by using this function
		Parameters:
		on:true(auto advance - default), false:(stop auto advance)
*/
/**************************************************************************/
void RA8875::setFontAdvance(bool on)
{
	if (on){
		bitClear(_MWCR0Reg,1);
	} else {
		bitSet(_MWCR0Reg,1);
	}
	bitWrite(_commonTextPar,1,on);//0.69b22
	writeReg(RA8875_MWCR0,_MWCR0Reg);
}

/**************************************************************************/
/*!		
		Choose between 16x16(8x16) - 24x24(12x24) - 32x32(16x32)
		for External Font ROM
		Parameters:
		ts:X16,X24,X32
		halfSize:true/false (16x16 -> 8x16 and so on...)
*/
/**************************************************************************/
void RA8875::setFontSize(enum RA8875tsize ts,boolean halfSize)
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
	//if (_portrait) return getFontHeight(inColums);
    uint8_t temp = (((_FNCR0Reg >> 2) & 0x3) + 1) * 8;
	if (inColums){
		if (_textHScale < 1) return width() / temp;
		temp = temp * (_textHScale+1);
		return width() / temp;
	} else {
		if (_textHScale < 1) return temp;
		temp = temp * (_textHScale+1);
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
	//if (_portrait) return getFontWidth(inRows);
    uint8_t temp = (((_FNCR0Reg >> 0) & 0x3) + 1) * 16;
	if (inRows){
		if (_textVScale < 1) return height() / temp;
		temp = temp * (_textVScale+1);
		return height() / temp;
	} else {
		if (_textVScale < 1) return temp;
		temp = temp * (_textVScale+1);
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
		NOTE: It identify correctly (when I got it) println and nl & rt
		included fixes by Morgan Sandercrok (thanks man)
*/
/**************************************************************************/
void RA8875::textWrite(const char* buffer, uint16_t len)
 {
	uint16_t i,v;
	uint8_t t1,t2;
	if (_currentMode == GRAPHIC) changeMode(TEXT);
	if (len == 0) len = strlen(buffer);
	writeCommand(RA8875_MRWC);
	for (i=0;i<len;i++){
		switch(buffer[i]) {
			case '\r':
				//Ignore carriage-return, only detect \n newline 
			break;
			case '\n':
				//Move cursor down - X or Y depends on the rotation
				if (!_portrait) {
					//y coordinate is vertical
					t1 = readReg(RA8875_F_CURYL);
					t2 = readReg(RA8875_F_CURYH);
					v = (t2 << 8) | (t1 & 0xFF);
					v += (16 + (16 * _textVScale)) + _fontInterline;
					//setCursor(0,v);
				} else {
					//x coordinate is vertical
					t1 = readReg(RA8875_F_CURXL);
					t2 = readReg(RA8875_F_CURXH);
					v = (t2 << 8) | (t1 & 0xFF);
					v += (16 + (16 * _textVScale)) + _fontInterline;
					//setCursor(v,0);
				} 
				setCursor(0,v);//0.69b22
			break;
			default:
				//write a normal char
				writeData(buffer[i]);
				waitBusy(0x80);
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
		if (colors < 16) {//255
			_color_bpp = 8;
			writeReg(RA8875_SYSR,0x00);
			if (_hasLayerLimits) _maxLayers = 2;
		} else if (colors > 8) {//65K
			_color_bpp = 16;
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
	uint8_t idx = 0;
	if (_color_bpp < 16) idx = 3;//8bit
	writeReg(RA8875_FGCR0,((color & 0xF800) >> _RA8875colorMask[idx]));
	writeReg(RA8875_FGCR1,((color & 0x07E0) >> _RA8875colorMask[idx+1]));
	writeReg(RA8875_FGCR2,((color & 0x001F) >> _RA8875colorMask[idx+2]));
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
*/
/**************************************************************************/
void RA8875::setBackgroundColor(uint16_t color)
{
	uint8_t idx = 0;
	if (_color_bpp < 16) idx = 3;//8bit
	writeReg(RA8875_BGCR0,((color & 0xF800) >> _RA8875colorMask[idx]));//11
	writeReg(RA8875_BGCR1,((color & 0x07E0) >> _RA8875colorMask[idx+1]));//5
	writeReg(RA8875_BGCR2,((color & 0x001F) >> _RA8875colorMask[idx+2]));//0
}
/**************************************************************************/
/*!
      Sets set the background color using 8bit R,G,B
	  Parameters:
	  R:8bit RED
	  G:8bit GREEN
	  B:8bit BLUE
*/
/**************************************************************************/
void RA8875::setBackgroundColor(uint8_t R,uint8_t G,uint8_t B)
{
	//_color_bpp > 8
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
*/
/**************************************************************************/
void RA8875::setTrasparentColor(uint16_t color)
{
	uint8_t idx = 0;
	if (_color_bpp < 16) idx = 3;//8bit
	writeReg(RA8875_BGTR0,((color & 0xF800) >> _RA8875colorMask[idx]));
	writeReg(RA8875_BGTR1,((color & 0x07E0) >> _RA8875colorMask[idx+1]));
	writeReg(RA8875_BGTR2,((color & 0x001F) >> _RA8875colorMask[idx+2]));
}
/**************************************************************************/
/*!
      Sets set the Trasparent background color using 8bit R,G,B
	  Parameters:
	  R:8bit RED
	  G:8bit GREEN
	  B:8bit BLUE
*/
/**************************************************************************/
void RA8875::setTrasparentColor(uint8_t R,uint8_t G,uint8_t B)
{
	//_color_bpp > 8
	writeReg(RA8875_BGTR0,R);
	writeReg(RA8875_BGTR1,G);
	writeReg(RA8875_BGTR2,B);
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
		temp &= ~(1 << 0);//
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
		temp &= ~(1 << 0);//
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
/*
		//drawPixel(x,y,color);
		//0.69b21 fix!!!
		setXY(x,y);
		writeCommand(RA8875_MRWC);
		writeData16(color);
	*/
void RA8875::setXY(int16_t x, int16_t y) 
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	setX(x);
	setY(y);
}

void RA8875::setX(uint16_t x) 
{
	if (_portrait){//fix 0.69b21
		if (x >= HEIGHT) x = HEIGHT-1;
		writeReg(RA8875_CURV0, x);
		writeReg(RA8875_CURV1, x >> 8);
	} else {
		if (x >= WIDTH) x = WIDTH-1;
		writeReg(RA8875_CURH0, x);
		writeReg(RA8875_CURH1, (x >> 8)); 
	}
}

void RA8875::setY(uint16_t y) 
{
	if (_portrait){//fix 0.69b21
		if (y >= WIDTH) y = WIDTH-1;
		writeReg(RA8875_CURH0, y);
		writeReg(RA8875_CURH1, (y >> 8)); 
	} else {
		if (y >= HEIGHT) y = HEIGHT-1;
		writeReg(RA8875_CURV0, y);
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
    uint8_t reg = readReg(RA8875_LTPR0);
    reg &= 0x3F;            // Clear bits 6 and 7 to zero
    switch(mode){           // bit 7,6 of LTPR0
        case SIMULTANEOUS:  // 00b : Layer 1/2 scroll simultaneously.
            // Do nothing
        break;
        case LAYER1ONLY:        // 01b : Only Layer 1 scroll.
            reg |= 0x40;
        break;
        case LAYER2ONLY:        // 10b : Only Layer 2 scroll.
            reg |= 0x80;
        break;
        case BUFFERED:      	// 11b: Buffer scroll (using Layer 2 as scroll buffer)
            reg |= 0xC0;
        break;
        default:
            return;         	//do nothing
    }
    //TODO: Should this be conditional on multi layer?
    //if (_useMultiLayers) writeReg(RA8875_LTPR0,reg);
    writeReg(RA8875_LTPR0,reg);
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
    writeReg(RA8875_HSSW0,_scrollXL);
    writeReg(RA8875_HSSW1,(_scrollXL >> 8));
  
    writeReg(RA8875_HESW0,_scrollXR);
    writeReg(RA8875_HESW1,(_scrollXR >> 8));   
    
    writeReg(RA8875_VSSW0,_scrollYT);
    writeReg(RA8875_VSSW1,(_scrollYT >> 8));   
 
    writeReg(RA8875_VESW0,_scrollYB);
    writeReg(RA8875_VESW1,(_scrollYB >> 8));
}

/**************************************************************************/
/*!
		Perform the scroll

*/
/**************************************************************************/
void RA8875::scroll(uint16_t x,uint16_t y)
{ 
	if (_portrait){//0.69b22 ok
		swapvals(x,y);
	}
	if (y > _scrollYB) y = _scrollYB;//??? mmmm... not sure
	if (_scrollXL == 0 && _scrollXR == 0 && _scrollYT == 0 && _scrollYB == 0){
		//do nothing, scroll window inactive
	} else {
		writeReg(RA8875_HOFS0,x); 
		writeReg(RA8875_HOFS1,x >> 8);
 
		writeReg(RA8875_VOFS0,y);
		writeReg(RA8875_VOFS1,y >> 8);
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
  	writeReg(RA8875_DTNR0,BWR);
  	writeReg(RA8875_BWR1,BWR >> 8);

  	writeReg(RA8875_DTNR1,BHR);
  	writeReg(RA8875_BHR1,BHR >> 8);

  	writeReg(RA8875_DTNR2,SPWR);
  	writeReg(RA8875_SPWR1,SPWR >> 8); 
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void RA8875::DMA_startAddress(unsigned long adrs)
{ 
  	writeReg(RA8875_SSAR0,adrs);
  	writeReg(RA8875_SSAR1,adrs >> 8);
	writeReg(RA8875_SSAR2,adrs >> 16);
  	//writeReg(0xB3,adrs >> 24);// not more in datasheet!
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
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
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
	if (_portrait){
		setXY(y,x);
	} else {
		setXY(x,y);
	}
	
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
/*!

*/
/**************************************************************************/
void RA8875::BTE_size(uint16_t w, uint16_t h)
{
	if (_portrait){//0.69b21 -have to check this, not verified
		swapvals(w,h);
	}
	if (w > _width) w = _width;
	if (h > _height) h = _height;
    writeReg(RA8875_BEWR0,w);//BET area width literacy  
    writeReg(RA8875_BEWR1,w >> 8);//BET area width literacy	   
    writeReg(RA8875_BEHR0,h);//BET area height literacy
    writeReg(RA8875_BEHR1,h >> 8);//BET area height literacy	   
}	

/**************************************************************************/
/*!

*/
/**************************************************************************/
void RA8875::BTE_source(uint16_t SX,uint16_t DX ,uint16_t SY ,uint16_t DY)
{
	uint8_t temp0,temp1;
	if (_portrait){//0.69b21 -have to check this, not verified
		swapvals(SX,SY);
		swapvals(DX,DY);
	}
    writeReg(RA8875_HSBE0,SX);//BTE horizontal position of read/write data
    writeReg(RA8875_HSBE1,SX >> 8);//BTE horizontal position of read/write data   

    writeReg(RA8875_HDBE0,DX);//BET written to the target horizontal position
    writeReg(RA8875_HDBE1,DX >> 8);//BET written to the target horizontal position	   
    
    writeReg(RA8875_VSBE0,SY);//BTE vertical position of read/write data
	
	temp0 = SY >> 8;   
	temp1 = readReg(RA8875_VSBE1);
	temp1 &= 0x80;
    temp0 = temp0 | temp1; 
	writeReg(RA8875_VSBE1,temp0);//BTE vertical position of read/write data  
  
    writeReg(RA8875_VDBE0,DY);//BET written to the target  vertical  position
	
	temp0 = DY >> 8;   
	temp1 = readReg(RA8875_VDBE1);
	temp1 &= 0x80;
	temp0 = temp0 | temp1;	
	writeReg(RA8875_VDBE1,temp0);//BET written to the target  vertical  position 
}		

/**************************************************************************/
/*! TESTING

*/
/**************************************************************************/
void RA8875::BTE_ROP_code(unsigned char setx)
{
    writeReg(RA8875_BECR1,setx);//BECR1	   
}

/**************************************************************************/
/*! TESTING

*/
/**************************************************************************/
void RA8875::BTE_enable(bool on) 
{	
	uint8_t temp = readReg(RA8875_BECR0);
	if (on){
		//temp |= (1 << 7); //bit set 7
		bitSet(temp,7);
	} else {
		bitClear(temp,7);
	}
	writeReg(RA8875_BECR0,temp);  
}

void RA8875::BTE_dataMode(enum RA8875btedatam m) 
{	
	uint8_t temp = readReg(RA8875_BECR0);
	if (m != CONT){//rect
		bitClear(temp,6);
	} else {
		bitSet(temp,6);
	}
	writeReg(RA8875_BECR0,temp);  
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
	bool clearBuffer = false;
	if (_hasLayerLimits && _color_bpp > 8) { //try to set up 8bit color space
		setColorBpp(8);
		_maxLayers = 2;
	}
	if (on){
		_useMultiLayers = true;
		_DPCRReg |= (1 << 7);
		clearBuffer = true;
	} else {
		_useMultiLayers = false;
		_DPCRReg &= ~(1 << 7);
	}
	writeReg(RA8875_DPCR,_DPCRReg);
	if (clearBuffer) { 
		//for some reason if you switch to multilayer the layer 2 has garbage better clear
		writeTo(L2);//switch to layer 2
		clearMemory(false);//clear memory of layer 2
		writeTo(L1);//switch to layer 1
	}
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
		bitSet(temp,7);
	}
	if (num > (maxLoc - 1)) num = maxLoc - 1;
	temp = temp | num;
	writeTo(PATTERN);
	writeReg(RA8875_PTNO,temp);
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
	bool trigMultilayer = false;
	switch(d){
		case L1:
			temp &= ~((1<<3) | (1<<2));// Clear bits 3 and 2
			temp &= ~(1 << 0); //clear bit 0
			_currentLayer = 0;
			trigMultilayer = true;
		break;
		case L2:
			temp &= ~((1<<3) | (1<<2));// Clear bits 3 and 2
			temp |= (1 << 0); //bit set 0
			_currentLayer = 1;
			trigMultilayer = true;
		break;
		case CGRAM: 
			temp &= ~(1 << 3); //clear bit 3
			temp |= (1 << 2); //bit set 2
			if (bitRead(_FNCR0Reg,7)){//REG[0x21] bit7 must be 0
				_FNCR0Reg &= ~(1 << 7); //clear bit 7
				writeReg(RA8875_FNCR0,_FNCR0Reg);  
			}
		break;
		case PATTERN:
			temp |= (1 << 3); //bit set 3
			temp |= (1 << 2); //bit set 2
		break;
		case CURSOR:
			temp |= (1 << 3); //bit set 3
			temp &= ~(1 << 2); //clear bit 2
		break;
		default:
		break;
	}
	if (trigMultilayer && !_useMultiLayers) useLayers(true);//turn on multiple layers if it's off
	writeReg(RA8875_MWCR1,temp);  
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
      Basic pixel write
	  Parameters:
	  x:horizontal pos
	  y:vertical pos
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
	setXY(x,y);
	writeCommand(RA8875_MRWC);
	writeData16(color); 
}


void RA8875::drawPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y)
{
    if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
    setXY(x, y);
    writeCommand(RA8875_MRWC);
    startSend();
	SPI.transfer(RA8875_DATAWRITE);
	while (count--) {
	#if (ARDUINO >= 160) || TEENSYDUINO > 121
		SPI.transfer16(*p);//should be fixed already
	#else
		SPI.transfer(*p >> 8);
		SPI.transfer(*p & 0xFF);
	#endif
        p++;
    }
    endSend();
}


uint16_t RA8875::getPixel(int16_t x, int16_t y)
{
    uint16_t color;
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
    setXY(x,y);
    writeCommand(RA8875_MRWC);
	#if defined(SPI_HAS_TRANSACTION)
		if (_inited) settings = SPISettings(MAXSPISPEED/2, MSBFIRST, SPI_MODE3);
	#else
		#if defined(ENERGIA)
			SPI.setClockDivider(SPI_SPEED_READ);//should be checked
		#else
			SPI.setClockDivider(SPI_CLOCK_DIV8);//should be checked
		//TODO - depends of the CPU used!
		#endif
	#endif
    startSend();
    SPI.transfer(RA8875_DATAREAD);
    SPI.transfer(0x00);//first byte it's dummy
	#if (ARDUINO >= 160) || TEENSYDUINO > 121
		color  = SPI.transfer16(0x0);
	#else
		color  = SPI.transfer(0x0);
		color |= (SPI.transfer(0x0) << 8);
	#endif
	#if defined(SPI_HAS_TRANSACTION)
	if (_inited) settings = SPISettings(MAXSPISPEED, MSBFIRST, SPI_MODE3);
	#else
		#if defined(ENERGIA)
			SPI.setClockDivider(SPI_SPEED_WRITE);//should be checked
		#else
			SPI.setClockDivider(SPI_CLOCK_DIV4);//should be checked
		//TODO - depends of the CPU used!
		#endif
	#endif
    endSend();
    return color;
}



void RA8875::getPixels(uint16_t * p, uint32_t count, int16_t x, int16_t y)
{
    uint16_t color; 
    if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
    setXY(x,y);
    writeCommand(RA8875_MRWC);
	#if defined(SPI_HAS_TRANSACTION)
		if (_inited) settings = SPISettings(MAXSPISPEED/2, MSBFIRST, SPI_MODE3);
	#else
		#if defined(ENERGIA)
			SPI.setClockDivider(SPI_SPEED_READ);//should be checked
		#else
			SPI.setClockDivider(SPI_CLOCK_DIV8);//should be checked
		//TODO - depends of the CPU used!
		#endif
	#endif
    startSend();
	SPI.transfer(RA8875_DATAREAD);
	#if (ARDUINO >= 160) || TEENSYDUINO > 121
		SPI.transfer16(0x0);//dummy
	#else
		SPI.transfer(0x0);//dummy
		SPI.transfer(0x0);//dummy
	#endif
    while (count--) {
		#if (ARDUINO >= 160) || TEENSYDUINO > 121
			color  = SPI.transfer16(0x0);
		#else
			color  = SPI.transfer(0x0);
			color |= (SPI.transfer(0x0) << 8);
		#endif
        *p++ = color;
    }
    endSend();
}
/**************************************************************************/
/*!
      Basic line draw
	  Parameters:
	  x0:horizontal start pos
	  y0:vertical start
	  x1:horizontal end pos
	  y1:vertical end pos
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
	if (_portrait) {//0.69b21
		swapvals(x0,y0);
		swapvals(x1,y1);
	}
	checkLimitsHelper(x0,y0);
	//if (x1 >= _width) x1 = _width-1;
	//if (y1 >= _height) y1 = _height-1;
	
	lineAddressing(x0,y0,x1,y1);
	
	setForegroundColor(color);
	
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
//Fixed! Now h=10 it's 10 pixels (before 11)
void RA8875::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	if (h < 1) h = 1;
	//drawLine(x, y, x, y+h, color);
	uint16_t nH = (y+h)-1;
	if (nH < 2){ //draw a pixel!
		drawPixel(x,y,color);
	} else {
		drawLine(x, y, x, nH, color);
	}
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
//Fixed! Now w=10 it's 10 pixels (before 11)
void RA8875::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	if (w < 1) w = 1;
	//drawLine(x, y, x+w, y, color);
	uint16_t nW = (x+w)-1;
	if (nW < 2) {//draw a pixel!
		drawPixel(x,y,color);
	} else {
		drawLine(x, y, nW, y, color);
	}
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
//Fixed! now a 10x10 rect it's really 10x10 pixel!
void RA8875::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	if (w < 1 || h < 1) return;//it cannot be!
	//RA8875 it's not out-of-range tolerant so this is a workaround
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
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
		rectHelper(x,y,(x+w)-1,(y+h)-1,color,true);//thanks the experimentalist
	}
}



/**************************************************************************/
/*!
      Fill the screen by using a specified RGB565 color
	  Parameters:
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillScreen(uint16_t color)
{  
	lineAddressing(0,0,WIDTH-1, HEIGHT-1);
	setForegroundColor(color);
	writeCommand(RA8875_DCR);
	writeData(0xB0);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

//legacy
void RA8875::clearScreen(uint16_t color)
{  
	fillScreen(color);
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
	if (r <= 0) return;
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
	triangleHelper(x0, y0, x1, y1, x2, y2, color, false);
	//triangleHelper(x0, y0-1, x1-1, y1, x2-1, y2-1, color, false);
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
	triangleHelper(x0, y0, x1, y1, x2, y2, color, true);
	//triangleHelper(x0, y0-1, x1-1, y1, x2-1, y2-1, color, true);
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
	ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, false);
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
	ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, true);
	//TODO: correct dimensions?
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
	curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
	//TODO: correct dimensions?
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
	curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
	//TODO: correct dimensions?
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
*/
/**************************************************************************/
void RA8875::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	if (w < 1 || h < 1) return;//it cannot be!
	//RA8875 it's not out-of-range tolerant so this is a workaround
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
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
	//roundRectHelper(x, y, x+w, y+h, r, color, true);
	//RA8875 it's not out-of-range tolerant so this is a workaround
	if (w < 2 && h < 2){ //render as pixel
		drawPixel(x,y,color);
	} else {			 //render as rect
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
*/
/**************************************************************************/
void RA8875::circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
	if (_portrait) swapvals(x0,y0);//0.69b21
	checkLimitsHelper(x0,y0);
	if (r < 1) r = 1;
	writeReg(RA8875_DCHR0,x0);
	writeReg(RA8875_DCHR1,x0 >> 8);

	writeReg(RA8875_DCVR0,y0);
	writeReg(RA8875_DCVR1,y0 >> 8);	   

	writeReg(RA8875_DCRR,r); 
	
	setForegroundColor(color);

	writeCommand(RA8875_DCR);
	filled == true ? writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL) : writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
	waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);//ZzZzz
}

/**************************************************************************/
/*!
		helper function for rects (filled or not)
*/
/**************************************************************************/
void RA8875::rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
	if (_portrait) {//0.69b21
		swapvals(x,y);
		swapvals(w,h);
	}
	if (w > WIDTH) return;
	if (h > HEIGHT) return;
	if (w < 1) w = 1;
	if (h < 1) h = 1;
	checkLimitsHelper(x,y);

	lineAddressing(x,y,w,h);
	
	setForegroundColor(color);

	writeCommand(RA8875_DCR);
	filled == true ? writeData(0xB0) : writeData(0x90);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}


/**************************************************************************/
/*!
      helper function for triangles
*/
/**************************************************************************/
void RA8875::triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
	if (_portrait) {//0.69b21
		swapvals(x0,y0);
		swapvals(x1,y1);
		swapvals(x2,y2);
	}
	checkLimitsHelper(x0,y0);
	checkLimitsHelper(x1,y1);
	checkLimitsHelper(x2,y2);
	
	lineAddressing(x0,y0,x1,y1);
	//p2

	writeReg(RA8875_DTPH0,x2);
	writeReg(RA8875_DTPH1,x2 >> 8);
	writeReg(RA8875_DTPV0,y2);
	writeReg(RA8875_DTPV1,y2 >> 8);

	setForegroundColor(color);
	
	writeCommand(RA8875_DCR);
	filled == true ? writeData(0xA1) : writeData(0x81);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      helper function for ellipse
*/
/**************************************************************************/
void RA8875::ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?

	//TODO:limits!
	curveAddressing(xCenter,yCenter,longAxis,shortAxis);
	
	setForegroundColor(color);

	writeCommand(RA8875_ELLIPSE);
	filled == true ? writeData(0xC0) : writeData(0x80);
	waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/**************************************************************************/
/*!
      helper function for curve
*/
/**************************************************************************/
void RA8875::curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
	if (_portrait) {//0.69b21
		swapvals(xCenter,yCenter);
		swapvals(longAxis,shortAxis);
	}
	//TODO:limits!
	curveAddressing(xCenter,yCenter,longAxis,shortAxis);
	
	setForegroundColor(color);

	writeCommand(RA8875_ELLIPSE);
	filled == true ? writeData(0xD0 | (curvePart & 0x03)) : writeData(0x90 | (curvePart & 0x03));
	waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/**************************************************************************/
/*!
	  helper function for rounded Rects
*/
/**************************************************************************/
void RA8875::roundRectHelper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled)
{
	if (_currentMode == TEXT) changeMode(GRAPHIC);//we are in text mode?
	if (_portrait) {//0.69b21
		swapvals(x,y);
		swapvals(w,h);
	}
	checkLimitsHelper(x,y);
	checkLimitsHelper(w,h);
	if (r < 1 || (w <= (2*r)) || (h <= (2*r))) {
		if (w < 2 && h < 2) {//1 pixel, cannot draw a rect, draw Pixel instead
			if (_portrait) {
				drawPixel(y,x,color);//since in portait x,y will be swapped, have to swap again
			} else {
				drawPixel(x,y,color);
			}
		} else {
			if (_portrait) {//since in portait x,y,w,h will be swapped, have to swap again
				rectHelper(y,x,h,w,color,filled);
			} else {
				rectHelper(x,y,w,h,color,filled);
			}
		}
	} else {
		lineAddressing(x,y,w,h);

		writeReg(RA8875_ELL_A0,r);
		writeReg(RA8875_ELL_A1,r >> 8);
		writeReg(RA8875_ELL_B0,r);
		writeReg(RA8875_ELL_B1,r >> 8);

		setForegroundColor(color);

		writeCommand(RA8875_ELLIPSE);
		filled == true ? writeData(0xE0) : writeData(0xA0);
		waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
	}
}



/**************************************************************************/
/*!
		common helper for check value limiter
*/
/**************************************************************************/
void RA8875::checkLimitsHelper(int16_t &x,int16_t &y)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= WIDTH) x = WIDTH - 1;
	if (y >= HEIGHT) y = HEIGHT -1;
	x = x;
	y = y;
}

/**************************************************************************/
/*!
		Graphic line addressing helper
*/
/**************************************************************************/
void RA8875::lineAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	//X0
	writeReg(RA8875_DLHSR0,x0);
	writeReg(RA8875_DLHSR1,x0 >> 8);
	//Y0
	writeReg(RA8875_DLVSR0,y0);
	writeReg(RA8875_DLVSR1,y0 >> 8);
	//X1
	writeReg(RA8875_DLHER0,x1);
	writeReg(RA8875_DLHER1,(x1) >> 8);
	//Y1
	writeReg(RA8875_DLVER0,y1);
	writeReg(RA8875_DLVER1,(y1) >> 8);
}

/**************************************************************************/
/*!	
		curve addressing
*/
/**************************************************************************/
void RA8875::curveAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	if (_portrait) {//0.69b21
		swapvals(x0,y0);
		swapvals(x1,y1);
	}
	//center
	writeReg(RA8875_DEHR0,x0);
	writeReg(RA8875_DEHR1,x0 >> 8);
	writeReg(RA8875_DEVR0,y0);
	writeReg(RA8875_DEVR1,y0 >> 8);
	//long,short ax
	writeReg(RA8875_ELL_A0,x1);
	writeReg(RA8875_ELL_A1,x1 >> 8);
	writeReg(RA8875_ELL_B0,y1);
	writeReg(RA8875_ELL_B1,y1 >> 8);
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
		NOTE:on non adafruit board PWM1 it's used by backlight!
*/
/**************************************************************************/
void RA8875::PWMout(uint8_t pw,uint8_t p)
{
	uint8_t reg;
	if (pw > 1){
		reg = RA8875_P2DCR;
	} else {
		reg = RA8875_P1DCR;
	}
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
void RA8875::backlight(boolean on) 
{
	if (_size == Adafruit_480x272 || _size == Adafruit_800x480 || _size == Adafruit_640x480) {
		GPIOX(on);
	} else {
		if (on == true){
			PWMsetup(1,true, RA8875_PWM_CLK_DIV1024);//setup PWM ch 1 for backlight
			PWMout(1,_brightness);//turn on PWM1
		} else {
			PWMsetup(1,false, RA8875_PWM_CLK_DIV1024);//setup PWM ch 1 for backlight
		}
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
		if (on){
			set = RA8875_PxCR_ENABLE;
		} else {
			set = RA8875_PxCR_DISABLE;
		}
	} else {
		reg = RA8875_P1CR;
		if (on){
			set = RA8875_PxCR_ENABLE;
		} else {
			set = RA8875_PxCR_DISABLE;
		}
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
		if (!digitalRead(_touchPin)) {
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
	(oiriginally from adafruit)
      Checks if a touch event has occured
      
      @returns  True is a touch event has occured (reading it via
                touchRead() will clear the interrupt in memory)
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
	  x:out 0...1024
	  Y:out 0...1024
	  Modified 14/4/15 M Sandercock: remove range checking and inversions

*/
/**************************************************************************/
void RA8875::readTouchADC(uint16_t *x, uint16_t *y) 
{
	uint16_t tx =  readReg(RA8875_TPXH);
	uint16_t ty =  readReg(RA8875_TPYH);
	uint8_t temp = readReg(RA8875_TPXYL);
	tx <<= 2;
	ty <<= 2;
	tx |= temp & 0x03;        // get the bottom x bits
	ty |= (temp >> 2) & 0x03; // get the bottom y bits
	/*
	#if defined (INVERTETOUCH_X)
		tx = 1024 - tx;
	#endif
	
	#if defined (INVERTETOUCH_Y)
		ty = 1024 - ty;
	#endif
	//calibrate???
	  #if defined(TOUCSRCAL_XLOW) && (TOUCSRCAL_XLOW != 0)
		_tsAdcMinX = TOUCSRCAL_XLOW;
		if (tx < TOUCSRCAL_XLOW) tx = TOUCSRCAL_XLOW;
	  #endif
	  
	  #if defined(TOUCSRCAL_YLOW) && (TOUCSRCAL_YLOW != 0)
		_tsAdcMinY = TOUCSRCAL_YLOW;
		if (ty < TOUCSRCAL_YLOW) ty = TOUCSRCAL_YLOW;
	  #endif
	  
	  #if defined(TOUCSRCAL_XHIGH) && (TOUCSRCAL_XHIGH != 0)
		_tsAdcMaxX = TOUCSRCAL_XHIGH;
		if (tx > TOUCSRCAL_XHIGH) tx = TOUCSRCAL_XHIGH;
	  #endif
	  
	  #if defined(TOUCSRCAL_YHIGH) && (TOUCSRCAL_YHIGH != 0)
		_tsAdcMaxY = TOUCSRCAL_YHIGH;
		if (ty > TOUCSRCAL_YHIGH) ty = TOUCSRCAL_YHIGH;
	  #endif
	  */
	 *x = tx;
	 *y = ty;
}

/**************************************************************************/
/*!   
	  Returns 10bit x,y data with TRUE scale (0...1024)
	  Parameters:
	  x:out 0...1024
	  Y:out 0...1024
	  
	Odd that this is called 'Raw' when it applies the calibrations.
*/
/**************************************************************************/
void RA8875::touchReadRaw(uint16_t *x, uint16_t *y) 
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
			if (_size == Adafruit_480x272 || _size == Adafruit_800x480 || _size == Adafruit_640x480) GPIOX(false);
			//2)decelerate SPI clock
			#if defined(SPI_HAS_TRANSACTION)
				settings = SPISettings(1000000, MSBFIRST, SPI_MODE3);
			#else
				SPI.setClockDivider(SPI_CLOCK_DIV16);
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
			#if defined(SPI_HAS_TRANSACTION)
				settings = SPISettings(MAXSPISPEED, MSBFIRST, SPI_MODE3);
			#else
				SPI.setClockDivider(SPI_CLOCK_DIV4);
			#endif
			//5)PLL afterburn!
			setSysClock(sysClockPar[_initIndex][0],sysClockPar[_initIndex][1],initStrings[_initIndex][2]);
			//5)turn on backlight
			if (_size == Adafruit_480x272 || _size == Adafruit_800x480 || _size == Adafruit_640x480) GPIOX(true);
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
	SPI.transfer(RA8875_DATAWRITE);
	SPI.transfer(data);
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
	SPI.transfer(RA8875_DATAWRITE);
	#if (ARDUINO >= 160) || TEENSYDUINO > 121
		SPI.transfer16(data);//should be fixed already
	#else
		SPI.transfer(data >> 8);
		SPI.transfer(data);
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
		if (_inited) settings = SPISettings(MAXSPISPEED/2, MSBFIRST, SPI_MODE3);
	#else
		#if defined(ENERGIA)
			SPI.setClockDivider(SPI_SPEED_READ);//2Mhz (3.3Mhz max)
		#else
			SPI.setClockDivider(SPI_CLOCK_DIV8);//2Mhz (3.3Mhz max)
		//TODO - depends of the CPU used!
		#endif
	#endif
	startSend();
	if (stat){
		SPI.transfer(RA8875_CMDREAD);
	} else {
		SPI.transfer(RA8875_DATAREAD);
	}
	uint8_t x = SPI.transfer(0x0);
	endSend();
	#if defined(SPI_HAS_TRANSACTION)
	if (_inited) settings = SPISettings(MAXSPISPEED, MSBFIRST, SPI_MODE3);
	#else
		#if defined(ENERGIA)
			SPI.setClockDivider(SPI_SPEED_WRITE);//4Mhz (6.6Mhz Max)
		#else
			SPI.setClockDivider(SPI_CLOCK_DIV4);//4Mhz (6.6Mhz Max)
		//TODO - depends of the CPU used!
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
	SPI.transfer(RA8875_CMDWRITE);
	SPI.transfer(d);
	endSend();
}


/**************************************************************************/
/*! PRIVATE
		starts SPI communication
*/
/**************************************************************************/
void RA8875::startSend()
{
#if defined(SPI_HAS_TRANSACTION)
	SPI.beginTransaction(settings);
#elif !defined(ENERGIA)
	cli();//protect from interrupts
#endif
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
	digitalWriteFast(_cs, LOW);
#else
	digitalWrite(_cs, LOW);
#endif
}

/**************************************************************************/
/*! PRIVATE
		ends SPI communication
*/
/**************************************************************************/
void RA8875::endSend()
{
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
	digitalWriteFast(_cs, HIGH);
#else
	digitalWrite(_cs, HIGH);
#endif
#if defined(SPI_HAS_TRANSACTION)
	SPI.endTransaction();
#elif !defined(ENERGIA)
	sei();//enable interrupts
#endif
} 

void RA8875::debugData(uint16_t data,uint8_t len)
{
  for (int i=len-1; i>=0; i--){
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
