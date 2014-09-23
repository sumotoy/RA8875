#include <SPI.h>
#include "RA8875.h"

/**************************************************************************/
/*!
	Contructor
	CS: SPI SS pin
	RS: Reset pin
*/
/**************************************************************************/
RA8875::RA8875(uint8_t CS, uint8_t RST) {
	_cs = CS;
	_rst = RST;
}

/**************************************************************************/
/*!
	Contructor
	CS: SPI SS pin
*/
/**************************************************************************/
RA8875::RA8875(uint8_t CS) {
	_cs = CS;
	_rst = 255;
}

/**************************************************************************/
/*!
	Initialize library and SPI
	Parameter:
    RA8875_480x272 (4.3" displays)
    RA8875_800x480 (5" and 7" displays)
	Adafruit_480x272 (4.3" Adafruit displays)
	Adafruit_800x480 (5" and 7" Adafruit displays)
*/
/**************************************************************************/
boolean RA8875::begin(enum RA8875sizes s) {
	_size = s;
		_width = 480;
		_height = 272;
	if (_size == RA8875_480x272) {
		_width = 480;
		_height = 272;
	} 
	if (_size == RA8875_800x480) {
		_width = 800;
		_height = 480;
	}
	_currentMode = GRAPHIC;
	_spiSpeed = MAXSPISPEED;
	_cursorX = 0;
	_cursorY = 0;
	_textWrap = true;
	_textSize = X16;
	_fontSpacing = 0;
	_extFontRom = false;
	_fontRomType = GT21L16T1W;
	_fontRomCoding = GB2312;
	_fontSource = INT;
	_fontFullAlig = false;
	_fontRotation = false;
	_fontInterline = 0;
	_fontFamily = STANDARD;
	_textCursorStyle = BLINK;
/*	Memory Write Control Register 0
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
	
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	
	if (_rst < 255){
		pinMode(_rst, OUTPUT); 
		digitalWrite(_rst, LOW);
		delay(100);
		digitalWrite(_rst, HIGH);
		delay(100);
	}
	
	SPI.begin();
#if defined(SPI_HAS_TRANSACTION) && defined(USESPITRANSACTIONS)
	_spiSpeed = 125000;//roll back speed for init
	if (readReg(0) != 0x75) return false;
	_spiSpeed = MAXSPISPEED;//go back to full speed
#else//do not use SPItransactons
	SPI.setClockDivider(SPI_CLOCK_DIV4);//4Mhz
	SPI.setDataMode(SPI_MODE0);
	if (readReg(0) != 0x75) return false;
#endif
	
	initialize();
	return true;
}

/************************* Initialization *********************************/

/**************************************************************************/
/*!
      Software Reset
*/
/**************************************************************************/
void RA8875::softReset(void) {
	writeCommand(RA8875_PWRR);
	writeData(RA8875_PWRR_SOFTRESET);
	writeData(RA8875_PWRR_NORMAL);
	delay(1);
}

/**************************************************************************/
/*!
	PRIVATE
      Hardware initialization of RA8875 and turn on
*/
/**************************************************************************/
void RA8875::initialize(void) {
//set system basics
	uint8_t idx = 0;
	if (_size == RA8875_480x272 || _size == Adafruit_480x272) {
		idx = 1;
	} else if (_size == RA8875_800x480 || _size == Adafruit_800x480) {
		idx = 3;
	}
	
	const uint8_t initStrings[4][12] = {
	{0x0A,0x02,0x03,0x00,0x05,0x04,0x03,0x05,0x00,0x0E,0x00,0x02},
	{0x0A,0x02,0x82,0x00,0x01,0x00,0x05,0x02,0x00,0x07,0x00,0x09},
	{0x0B,0x02,0x01,0x05,0x0F,0x01,0x00,0x0A,0x00,0x0E,0x00,0x01},
	{0x0B,0x02,0x81,0x03,0x03,0x02,0x00,0x14,0x00,0x06,0x00,0x01}
	};
	
	writeReg(RA8875_SYSR,RA8875_SYSR_16BPP | RA8875_SYSR_MCU8); //0x0c SYSR bit[4:3]=00 256 color bit[2:1]= 00 8bit MPU interface,8bit MCU interface and 65k color display
	writeReg(RA8875_PLLC1,initStrings[idx][0]);//pll1
	delay(2);
	writeReg(RA8875_PLLC2,initStrings[idx][1]);//pll2
	delay(2);
	writeReg(RA8875_PCSR,initStrings[idx][2]);
	delay(2);
	writeReg(RA8875_HDWR,(_width / 8) - 1);
	writeReg(RA8875_HNDFTR,initStrings[idx][3]);
	writeReg(RA8875_HNDR,initStrings[idx][4]);
	writeReg(RA8875_HSTR,initStrings[idx][5]);
	writeReg(RA8875_HPWR,initStrings[idx][6]);
	writeReg(RA8875_VDHR0,(uint16_t)(_height - 1) & 0xFF);
	writeReg(RA8875_VDHR1,(uint16_t)(_height - 1) >> 8);
	writeReg(RA8875_VNDR0,initStrings[idx][7]);
	writeReg(RA8875_VNDR1,initStrings[idx][8]);
	writeReg(RA8875_VSTR0,initStrings[idx][9]);
	writeReg(RA8875_VSTR1,initStrings[idx][10]);
	writeReg(RA8875_VPWR,initStrings[idx][11]);
	setActiveWindow(0,_width-1,0,_height-1);//set the active winsow
	writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);//clear all
	delay(200); 
	
    //now starts the first time setting up
	displayOn(true);//turn On Display
	if (_size == Adafruit_480x272 || _size == Adafruit_800x480) GPIOX(true);//only for adafruit stuff 
	PWMsetup(1,true, RA8875_PWM_CLK_DIV1024);//setup PWM ch 1 for backlight
	PWMout(1,255);//turn on PWM1
	setCursorBlinkRate(DEFAULTCURSORBLINKRATE);//set default blink rate
	if (_textCursorStyle == BLINK) showCursor(false,BLINK); //set default text cursor type and turn off
	setIntFontCoding(DEFAULTINTENCODING);//set default internal font encoding
	setFont(INT);	//set internal font use
	
	//now tft it's ready to go and in [Graphic mode]
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
void RA8875::setActiveWindow(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB){
	if (XR >= _width) XR = _width-1;
	if (YB >= _height) YB = _height-1;
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
uint16_t RA8875::width(void) { return _width; }

/**************************************************************************/
/*!		
		Return the max tft height.
		Note that real size will start from 0
		so you need to subtract 1!
*/
/**************************************************************************/
uint16_t RA8875::height(void) { return _height; }

/************************* Text Mode ***********************************/

/**************************************************************************/
/*!  
		Change the mode between graphic and text
		Parameters:
		m: can be GRAPHIC or TEXT
*/
/**************************************************************************/
void RA8875::changeMode(enum RA8875modes m) {
	writeCommand(RA8875_MWCR0);
	if (m == GRAPHIC){
		if (_currentMode == TEXT){//avoid useless consecutive calls
			 _MWCR0Reg &= ~(1 << 7);
			writeData(_MWCR0Reg);
			_currentMode = GRAPHIC;
		}
	} else {
		if (_currentMode == GRAPHIC){//avoid useless consecutive calls
			_MWCR0Reg |= (1 << 7);
			writeData(_MWCR0Reg);
			_currentMode = TEXT;
		}
	}
}

/**************************************************************************/
/*!   
		Set internal Font Encoding
		Parameters:
		f:ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4
		default:ISO_IEC_8859_1
*/
/**************************************************************************/
void RA8875::setIntFontCoding(enum RA8875fontCoding f) {
	_FNCR0Reg &= ~((1<<1) | (1<<0));// Clear bits 1 and 0
	switch (f){
		case ISO_IEC_8859_1:
			 //do nothing
		break;
		case ISO_IEC_8859_2:
			_FNCR0Reg |= (1 << 0);
		break;
		case ISO_IEC_8859_3:
			_FNCR0Reg |= (1 << 1);
		break;
		case ISO_IEC_8859_4:
			_FNCR0Reg |= ((1<<1) | (1<<0));// Set bits 1 and 0
		break;
	}
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
void RA8875::setExternalFontRom(enum RA8875extRomType ert, enum RA8875extRomCoding erc, enum RA8875extRomFamily erf){
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
			erc = GB12345;//forced
		break;
		case GT23L24M1Z:
			temp &= 0x1F; temp |= 0x60;
		break;
		case GT23L32S4W:
		case GT30H32S4W:
			temp &= 0x1F; temp |= 0x80;
		break;
		default:
			_extFontRom = false;//wrong type, better avoid for future
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
			_extFontRom = false;//wrong coding, better avoid for future
			return;//cannot continue, exit
		}
		_fontFamily = erf;
		switch(erf){	//check rom font family
			case STANDARD:
				temp &= 0xFC;
			break;
			case ARIAL:
				temp &= 0xFC; temp |= 0x01;
			break;
			case ROMAN:
				temp &= 0xFC; temp |= 0x02;
			break;
			case BOLD:
				temp |= ((1<<1) | (1<<0)); // set bits 1 and 0
			break;
			default:
				_fontFamily = STANDARD; temp &= 0xFC;
		}
		_fontRomCoding = erc;
		_extFontRom = true;
		_SFRSETReg = temp;//success, set reg
		//writeReg(RA8875_SFRSET,_SFRSETReg);//0x2F
		//delay(4);
		//return true;
}

/**************************************************************************/
/*!  
		choose from internal/external (if exist) Font Rom
		Parameters:
		s: Font source (INT,EXT)
*/
/**************************************************************************/
void RA8875::setFont(enum RA8875fontSource s) {
	//enum RA8875fontCoding c
	if (s == INT){
		//check the font coding
		if (_extFontRom) {
			//_SFRSETReg = 0b00000000;
			setFontSize(X16,false);
			writeReg(RA8875_SFRSET,0b00000000);//_SFRSETReg
		}
		_FNCR0Reg &= ~((1<<7) | (1<<5));// Clear bits 7 and 5
		writeReg(RA8875_FNCR0,_FNCR0Reg);
		_fontSource = s;
		delay(1);
	} else {
		if (_extFontRom){
			_fontSource = s;
			//now switch
			_FNCR0Reg |= (1 << 5);
			writeReg(RA8875_FNCR0,_FNCR0Reg);//0x21
			delay(1);
			writeReg(RA8875_SFCLR,0x02);//Serial Flash/ROM CLK frequency/2
			setFontSize(X24,false);////X24 size
			//setExternalFontRom(_fontRomType,_fontRomCoding);
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
void RA8875::setFontFullAlign(boolean align) {
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
void RA8875::setFontRotate(boolean rot) {
	rot == true ? _FNCR1Reg |= (1 << 4) : _FNCR1Reg &= ~(1 << 4);
	writeReg(RA8875_FNCR1,_FNCR1Reg);
}

/**************************************************************************/
/*!  
		Set distance between text lines (default off)
		Parameters:
		pix: 0...63 pixels
*/
/**************************************************************************/
void RA8875::setFontInterline(uint8_t pix){
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
		x:horizontal in pixels
		y:vertical in pixels
*/
/**************************************************************************/
void RA8875::setCursor(uint16_t x, uint16_t y) {
	if (!_textWrap){
		if (x >= _width) x = _width-1;
		if (y >= _height) y = _height-1;
	}
	_cursorX = x;
	_cursorY = y;
	writeReg(RA8875_F_CURXL,x & 0xFF);
	writeReg(RA8875_F_CURXH,x >> 8);
	writeReg(RA8875_F_CURYL,y & 0xFF);
	writeReg(RA8875_F_CURYH,y >> 8);
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
void RA8875::getCursor(uint16_t *x, uint16_t *y) {
	uint8_t t1,t2;
	t1 = readReg(RA8875_F_CURXL);
	t2 = readReg(RA8875_F_CURXH);
	_cursorX = (t2 << 8) | (t1 & 0xFF);
	t1 = readReg(RA8875_F_CURYL);
	t2 = readReg(RA8875_F_CURYH);
	_cursorY = (t2 << 8) | (t1 & 0xFF);
	*x = _cursorX;
	*y = _cursorY;
}
/**************************************************************************/
/*!     Show/Hide text cursor
		Parameters:
		cur:(true/false) true:visible, false:not visible
		c: cursor type (NORMAL, BLINK)
*/
/**************************************************************************/
void RA8875::showCursor(boolean cur,enum RA8875tcursor c){
	if (c == BLINK){
		_textCursorStyle = c;
		_MWCR0Reg |= (1 << 5);
	} else {
		_textCursorStyle = NORMAL;
		_MWCR0Reg &= ~(1 << 5);
	}
	bitWrite(_MWCR0Reg,6,cur);//set cursor visibility flag
	writeReg(RA8875_MWCR0,_MWCR0Reg);
}

/**************************************************************************/
/*!     Set cursor property blink and his rate
		Parameters:
		rate:blink speed (fast 0...255 slow)
*/
/**************************************************************************/
void RA8875::setCursorBlinkRate(uint8_t rate){
	// _MWCR0Reg |= (1 << 5);
	//writeReg(RA8875_MWCR0,_MWCR0Reg);
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
void RA8875::setTextColor(uint16_t fColor, uint16_t bColor){
	setForegroundColor(fColor);
	setBackgroundColor(bColor);
	//bitClear(_FNCR1Reg,6);
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

void RA8875::setTextColor(uint16_t fColor){
	setForegroundColor(fColor);
	 _FNCR1Reg |= (1 << 6);
	//bitSet(_FNCR1Reg,6);
	writeReg(RA8875_FNCR1,_FNCR1Reg);
}

/**************************************************************************/
/*!		
		Set the Text size by it's multiple. normal should=0, max is 3 (x4)
		Parameters:
		scale:0..3  -> 0:normal, 1:x2, 2:x3, 3:x4
*/
/**************************************************************************/
void RA8875::setFontScale(uint8_t scale){
	if (scale > 3) scale = 3;
 	_FNCR1Reg &= ~(0xF); // clear bits from 0 to 3
	_FNCR1Reg |= scale << 2;
	_FNCR1Reg |= scale;
	writeReg(RA8875_FNCR1,_FNCR1Reg);
	_textScale = scale; 
}

/**************************************************************************/
/*!		
		Choose between 16x16(8x16) - 24x24(12x24) - 32x32(16x32)
		Parameters:
		ts:X16,X24,X32
		halfSize:true/false (16x16 -> 8x16 and so on...)
*/
/**************************************************************************/
void RA8875::setFontSize(enum RA8875tsize ts,boolean halfSize){
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
		Choose space in pixels between chars
		Parameters:
		spc:0...63pix (default 0=off)
*/
/**************************************************************************/
void RA8875::setFontSpacing(uint8_t spc){//ok
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
		
*/
/**************************************************************************/
void RA8875::textWrite(const char* buffer, uint16_t len) {
	bool goBack = false;
	uint8_t temp;
	uint8_t start = 0;
	if (_currentMode == GRAPHIC){
		changeMode(TEXT);
		goBack = true;
	}
	if (len == 0) len = strlen(buffer);
	if (len > 0 && ((buffer[0] == '\r') && (buffer[1] == '\n'))){//got a println?
		//get current y
		uint8_t t1 = readReg(RA8875_F_CURYL);
		uint8_t t2 = readReg(RA8875_F_CURYH);
		//calc new line y
		uint16_t ny = (t2 << 8) | (t1 & 0xFF);
		//update y
		ny = ny + (16 + (16*_textScale))+_fontInterline;//TODO??
		setCursor(0,ny);
		start = 2;
	}
	writeCommand(RA8875_MRWC);
	for (uint16_t i=start;i<len;i++){
		if (buffer[i] == '\n' || buffer[i] == '\r') {
			//_cursor_y += textsize * 8;
			//_cursor_x  = 0;
		} else {
			writeData(buffer[i]);
			// wait for stuff...
			do {
				temp = readStatus();
			} while ((temp & 0x80) == 0x80);
			// end wait for stuff
		}
#if defined(__AVR__)
		if (_textScale > 1) delay(1);
#elif defined(__arm__)
		if (_textScale > 0) delay(1);//Teensy3 
#endif
	}
	if (goBack) changeMode(GRAPHIC);
}

/**************************************************************************/
/*!
      Sets set the foreground color using 16bit RGB565 color
	  Parameters:
	  color:16bit color RGB565
*/
/**************************************************************************/
void RA8875::setForegroundColor(uint16_t color){
	writeReg(RA8875_FGCR0,((color & 0xF800) >> 11));
	writeReg(RA8875_FGCR1,((color & 0x07E0) >> 5));
	writeReg(RA8875_FGCR2,(color & 0x001F));
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
void RA8875::setForegroundColor(uint8_t R,uint8_t G,uint8_t B){
	writeReg(RA8875_FGCR0,R);
	writeReg(RA8875_FGCR1,G);
	writeReg(RA8875_FGCR2,B);
}
/**************************************************************************/
/*!
      Sets set the background color using 16bit RGB565 color
	  Parameters:
	  color:16bit color RGB565
*/
/**************************************************************************/
void RA8875::setBackgroundColor(uint16_t color){
	writeReg(RA8875_BGCR0,((color & 0xF800) >> 11));
	writeReg(RA8875_BGCR1,((color & 0x07E0) >> 5));
	writeReg(RA8875_BGCR2,(color & 0x001F));
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
void RA8875::setBackgroundColor(uint8_t R,uint8_t G,uint8_t B){
	writeReg(RA8875_BGCR0,R);
	writeReg(RA8875_BGCR1,G);
	writeReg(RA8875_BGCR2,B);
}
/**************************************************************************/
/*!
      Sets set the trasparent background color using 16bit RGB565 color
	  Parameters:
	  color:16bit color RGB565
*/
/**************************************************************************/
void RA8875::setTrasparentColor(uint16_t color){
	writeReg(RA8875_BGTR0,((color & 0xF800) >> 11));
	writeReg(RA8875_BGTR1,((color & 0x07E0) >> 5));
	writeReg(RA8875_BGTR2,(color & 0x001F));
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
void RA8875::setTrasparentColor(uint8_t R,uint8_t G,uint8_t B){
	writeReg(RA8875_BGTR0,R);
	writeReg(RA8875_BGTR1,G);
	writeReg(RA8875_BGTR2,B);
}
/************************* Graphics ***********************************/

/**************************************************************************/
/*!		Set graphic cursor beween 8 different ones.
	    Graphic cursors has to be inserted before use!
		Parameters:
		cur: 0...7
*/
/**************************************************************************/
void RA8875::setGraphicCursor(uint8_t cur) {
	if (cur > 7) cur = 7;
	uint8_t temp = readReg(RA8875_MWCR1);
	temp &= ~(0x70);//clear bit 6,5,4
	temp |= cur << 4;
	temp |= cur;
	writeData(temp);
}

/**************************************************************************/
/*!		Show the graphic cursor
	    Graphic cursors has to be inserted before use!
		Parameters:
		cur: true,false
*/
/**************************************************************************/
void RA8875::showGraphicCursor(boolean cur) {
	uint8_t temp = readReg(RA8875_MWCR1);
	cur == true ? temp |= (1 << 7) : temp &= ~(1 << 7);
	writeData(temp);
}

/**************************************************************************/
/*!
	From Adafruit_RA8875, need to be fixed!!!!!!!!!
*/
/**************************************************************************/
boolean RA8875::waitPoll(uint8_t regname, uint8_t waitflag) {
	while (1) {
		uint8_t temp = readReg(regname);
		if (!(temp & waitflag)) return true;
	}  
	return false; // MEMEFIX: yeah i know, unreached! - add timeout?
}


/**************************************************************************/
/*!		
		Set the position for Graphic Write
		Parameters:
		x:horizontal position
		y:vertical position
*/
/**************************************************************************/
void RA8875::setXY(int16_t x, int16_t y) {
	checkLimitsHelper(x,y);
	writeReg(RA8875_CURH0, x);
	writeReg(RA8875_CURH1, x >> 8);
	writeReg(RA8875_CURV0, y);
	writeReg(RA8875_CURV1, y >> 8);  
}

/**************************************************************************/
/*!		
	  Write n Pixels directly
		Parameters:
		num:pixels
		p:16bit color

*/
/**************************************************************************/
void RA8875::pushPixels(uint32_t num, uint16_t p) {
	startSend();
	SPI.transfer(RA8875_DATAWRITE);
	while (num--) {
		SPI.transfer(p >> 8);
		SPI.transfer(p);
	}
	endSend();
}

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
void RA8875::drawPixel(int16_t x, int16_t y, uint16_t color){
	checkLimitsHelper(x,y);
	setXY(x,y);
	writeCommand(RA8875_MRWC);
	writeData16(color);
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
void RA8875::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color){
	checkLimitsHelper(x0,y0);
	if (x1 >= _width) x1 = _width-1;
	if (y1 >= _height) y1 = _height-1;
	
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
void RA8875::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color){
	if (h < 1) h = 1;
	drawLine(x, y, x, y+h, color);
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
void RA8875::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color){
	if (w < 1) w = 1;
	drawLine(x, y, x+w, y, color);
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
void RA8875::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	rectHelper(x, y, x+w, y+h, color, false);
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
void RA8875::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	rectHelper(x, y, x+w, y+h, color, true);
}

/**************************************************************************/
/*!
      Fill the screen by using a specified RGB565 color
	  Parameters:
	  color: RGB565 color
*/
/**************************************************************************/
void RA8875::fillScreen(uint16_t color){  
	rectHelper(0, 0, _width-1, _height-1, color, true);
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
void RA8875::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color){
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
void RA8875::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color){
	if (r <= 0) return;
	circleHelper(x0, y0, r, color, true);
}

/**************************************************************************/
/*!
      Draw Triangle
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
void RA8875::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
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
void RA8875::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
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
void RA8875::drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color){
	ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, false);
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
void RA8875::fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color){
	ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, true);
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
void RA8875::drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color){
	curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
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
void RA8875::fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color){
	curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
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
void RA8875::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color){
	roundRectHelper(x, y, x+w, y+h, r, color, false);
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
void RA8875::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color){
	roundRectHelper(x, y, x+w, y+h, r, color, true);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//				Helpers functions
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**************************************************************************/
/*!
      helper function for circles
*/
/**************************************************************************/
void RA8875::circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled){
	checkLimitsHelper(x0,y0);
	if (r < 1) r = 1;
	//X
	writeReg(RA8875_DCHR0,x0);
	writeReg(RA8875_DCHR1,x0 >> 8);
	//Y
	writeReg(RA8875_DCVR0,y0);
	writeReg(RA8875_DCVR1,y0 >> 8);	   
	//R
	writeReg(RA8875_DCRR,r); 

	setForegroundColor(color);

	writeCommand(RA8875_DCR);
	filled == true ? writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL) : writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
	waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);//ZzZzz
}

/**************************************************************************/
/*!
		helper function for rects
*/
/**************************************************************************/
void RA8875::rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled){
	checkLimitsHelper(x,y);
	if (w < 1) w = 1;
	if (h < 1) h = 1;
	lineAddressing(x,y,w,h);
	setForegroundColor(color);

	writeCommand(RA8875_DCR);
	filled == true ? writeData(0xB0) : writeData(0x90);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}


/**************************************************************************/
/*!
		common helper for check value limiter
*/
/**************************************************************************/
void RA8875::checkLimitsHelper(int16_t &x,int16_t &y){
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= _width) x = _width - 1;
	if (y >= _height) y = _height -1;
	x = x;
	y = y;
}
/**************************************************************************/
/*!
      helper function for triangles
*/
/**************************************************************************/
void RA8875::triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled){
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
void RA8875::ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled){
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
void RA8875::curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled){
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
void RA8875::roundRectHelper(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color, bool filled){
	if (r < 1) rectHelper(x,y,w,h,color,filled);
	checkLimitsHelper(x,y);
	checkLimitsHelper(w,h);
	lineAddressing(x,y,w,h);
	//P2
	writeReg(RA8875_ELL_A0,r);
	writeReg(RA8875_ELL_A1,r >> 8);
	writeReg(RA8875_ELL_B0,r);
	writeReg(RA8875_ELL_B1,r >> 8);
	setForegroundColor(color);

	writeCommand(RA8875_ELLIPSE);
	filled == true ? writeData(0xE0) :writeData(0xA0);
	waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
		Graphic line addressing helper
*/
/**************************************************************************/
void RA8875::lineAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1){
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
void RA8875::curveAddressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1){
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

/************************* Mid Level ***********************************/

/**************************************************************************/
/*!
		on/off GPIO (basic for Adafruit module
*/
/**************************************************************************/
void RA8875::GPIOX(boolean on) {
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
void RA8875::PWMout(uint8_t pw,uint8_t p) {
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
void RA8875::brightness(uint8_t val) {
	PWMout(1,val);
}

/**************************************************************************/
/*! PRIVATE
		Setup PWM engine
		Parameters:
		pw:pwm selection (1,2)
		on: turn on/off
		clock: the clock setting
*/
/**************************************************************************/
void RA8875::PWMsetup(uint8_t pw,boolean on, uint8_t clock) {
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
	writeReg(reg, set | (clock & 0xF));
}
/**************************************************************************/
/*!
      Enables or disables the on-chip touch screen controller
*/
/**************************************************************************/
void RA8875::touchEnable(boolean on) {
	if (on) {
		/* Enable Touch Panel (Reg 0x70) */
		writeReg(RA8875_TPCR0, RA8875_TPCR0_ENABLE        | 
							   RA8875_TPCR0_WAIT_16384CLK  |
							   RA8875_TPCR0_WAKEDISABLE   | 
							   RA8875_TPCR0_ADCCLK_DIV4); // 10mhz max!
		/* Set Auto Mode      (Reg 0x71) */
		writeReg(RA8875_TPCR1, RA8875_TPCR1_AUTO    | 
							// RA8875_TPCR1_VREFEXT | 
							  RA8875_TPCR1_DEBOUNCE);
		/* Enable TP INT */
		writeReg(RA8875_INTC1, readReg(RA8875_INTC1) | RA8875_INTCx_TP);
	} else {
		/* Disable TP INT */
		writeReg(RA8875_INTC1, readReg(RA8875_INTC1) & ~RA8875_INTCx_TP);
		/* Disable Touch Panel (Reg 0x70) */
		writeReg(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
	}
}

/**************************************************************************/
/*! (from adafruit)
      Checks if a touch event has occured
      
      @returns  True is a touch event has occured (reading it via
                touchRead() will clear the interrupt in memory)
*/
/**************************************************************************/
boolean RA8875::touched(void) {
	if (readReg(RA8875_INTC2) & RA8875_INTCx_TP) return true;
	return false;
}

/**************************************************************************/
/*! (based on adafruit)
      Reads the last touch event
      Parameters:
      x:  ADC value (0...1024) of x
      y:  ADC value (0...1024) of y
	  Based on setting TOUCHINPIXELS it can be:
      x:  position in pix (0...width) of x
      y:  position in pix (0...width) of y
	  It also perform calibrations.
      
      @note Calling this function will clear the touch panel interrupt on
            the RA8875, resetting the flag used by the 'touched' function
*/
/**************************************************************************/
boolean RA8875::touchRead(uint16_t *x, uint16_t *y) {
	uint16_t tx, ty;
	uint8_t temp;
	uint16_t minX = 0;
	uint16_t minY = 0;
	uint16_t maxX = 1024;
	uint16_t maxY = 1024;
	tx = readReg(RA8875_TPXH);
	ty = readReg(RA8875_TPYH);
	temp = readReg(RA8875_TPXYL);
	tx <<= 2;
	ty <<= 2;
	tx |= temp & 0x03;        // get the bottom x bits
	ty |= (temp >> 2) & 0x03; // get the bottom y bits
/* 	uint16_t lx=(480-(((tx-50))*10/18));
	uint16_t ly=(272-(((ty-63))*10/33));
	Serial.println(lx,DEC);
	Serial.print("lx:");
	Serial.print(lx,DEC);
	Serial.print(" - ly:");
	Serial.print(ly,DEC);
	Serial.print(" - olx:");
	Serial.print(map(tx,0,1024,0,_width-1),DEC);
	Serial.print(" - oly:");
	Serial.println(map(ty,0,1024,0,_height-1),DEC); */
	#if defined (INVERTETOUCH_X)
	tx = 1024 - tx;
	#endif
	#if defined (INVERTETOUCH_Y)
	ty = 1024 - ty;
	#endif
	//calibrate???
	  #if (TOUCSRCAL_XLOW != 0)
		minX = TOUCSRCAL_XLOW;
		if (tx < TOUCSRCAL_XLOW) tx = TOUCSRCAL_XLOW;
	  #endif
	  #if (TOUCSRCAL_YLOW != 0)
		minY = TOUCSRCAL_YLOW;
		if (ty < TOUCSRCAL_YLOW) ty = TOUCSRCAL_YLOW;
	  #endif
	  #if (TOUCSRCAL_XHIGH != 0)
		maxX = TOUCSRCAL_XHIGH;
		if (tx > TOUCSRCAL_XHIGH) tx = TOUCSRCAL_XHIGH;
	  #endif
	  #if (TOUCSRCAL_XHIGH != 0)
		maxY = TOUCSRCAL_YHIGH;
		if (ty > TOUCSRCAL_YHIGH) ty = TOUCSRCAL_YHIGH;
	  #endif
	#if defined (TOUCHINPIXELS)
		*x = map(tx,minX,maxX,0,_width-1);
		*y = map(ty,minY,maxY,0,_height-1);
	#else
		#if (TOUCSRCAL_XLOW != 0 || (TOUCSRCAL_XHIGH != 0))
		*x = map(tx,minX,maxX,0,1024);
		#else
		*x = tx;
		#endif
		#if (TOUCSRCAL_YLOW != 0 || (TOUCSRCAL_YHIGH != 0))
		*y = map(ty,minY,maxY,0,1024);
		#else
		*y = ty;
		#endif
	#endif

	// Clear TP INT Status
	writeReg(RA8875_INTC2, RA8875_INTCx_TP);
	return true;
}

/**************************************************************************/
/*!
      turn display on/off
*/
/**************************************************************************/
void RA8875::displayOn(boolean on) {
	if (on) {
		writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
	} else {
		writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
	}
}

/**************************************************************************/
/*!
    Sleep mode on/off (caution! in SPI this need some more code!)
*/
/**************************************************************************/
void RA8875::sleep(boolean sleep) {
	if (sleep) {
		writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
	} else {
		writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF);
	}
}

/************************* Low Level ***********************************/

/**************************************************************************/
/*! PRIVATE
		Write in a register
		Parameters:
		reg: the register
		val: the data
*/
/**************************************************************************/
void  RA8875::writeReg(uint8_t reg, uint8_t val) {
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
uint8_t  RA8875::readReg(uint8_t reg) {
	writeCommand(reg);
	return readData();
}

/**************************************************************************/
/*!
		Write data
		Parameters:
		d: the data
*/
/**************************************************************************/
void  RA8875::writeData(uint8_t d) {
	startSend();
	SPI.transfer(RA8875_DATAWRITE);
	SPI.transfer(d);
	endSend();
}

/**************************************************************************/
/*! 
		Write 16 bit data
		Parameters:
		d: the data (16 bit)
*/
/**************************************************************************/
void  RA8875::writeData16(uint16_t d) {
	startSend();
	SPI.transfer(RA8875_DATAWRITE);
	SPI.transfer(d >> 8);
	SPI.transfer(d);
	endSend();
}

/**************************************************************************/
/*!	PRIVATE

*/
/**************************************************************************/
uint8_t  RA8875::readData(bool stat) {
	#if defined(SPI_HAS_TRANSACTION) && defined(USESPITRANSACTIONS)
	_spiSpeed = MAXSPISPEED/2;
	#else
	SPI.setClockDivider(SPI_CLOCK_DIV8);//2Mhz (3.3Mhz max)
	#endif
	startSend();
	if (stat){
		SPI.transfer(RA8875_CMDREAD);
	} else {
		SPI.transfer(RA8875_DATAREAD);
	}
	uint8_t x = SPI.transfer(0x0);
	endSend();
	#if defined(SPI_HAS_TRANSACTION) && defined(USESPITRANSACTIONS)
	_spiSpeed = MAXSPISPEED;
	#else
	SPI.setClockDivider(SPI_CLOCK_DIV4);//4Mhz (6.6Mhz Max)
	#endif
	return x;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
uint8_t  RA8875::readStatus(void) {
	return readData(true);
}

/**************************************************************************/
/*! PRIVATE
		Write a command
		Parameters:
		d: the command
*/
/**************************************************************************/
void  RA8875::writeCommand(uint8_t d) {
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
void RA8875::startSend(){
#if defined(SPI_HAS_TRANSACTION) && defined(USESPITRANSACTIONS)
	SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
#endif
#if defined(__MK20DX128__) || defined(__MK20DX256__)
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
void RA8875::endSend(){
#if defined(__MK20DX128__) || defined(__MK20DX256__)
	digitalWriteFast(_cs, HIGH);
#else
	digitalWrite(_cs, HIGH);
#endif
#if defined(SPI_HAS_TRANSACTION) && defined(USESPITRANSACTIONS)
	SPI.endTransaction();
#endif
}

