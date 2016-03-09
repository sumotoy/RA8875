RA8875 library b11p10 [Testing gold release]
===========================================

**Wiki added!** https://github.com/sumotoy/RA8875/wiki<br>
This is the last <b>'development'</b> version, last known <b>stable</b> version is here:<br>
https://github.com/sumotoy/RA8875/tree/0.70<br>
The development release normally contain bug fixes so it's almost safe to use but if you have problems you can switch back to the stable release (I try to keep as stable I can)<br>
Moved a lot of 'bla bla' in the wiki, readme cleaned.

##### Current version 0.70b11, Please read!!!.<br>
**0.70b11p1** changes...<br>
 - New optimized User Font Rendering engine, over 23 times faster.
 - Added template & stepByStep wiki for converting fonts with lcd-image-converter.
 - Changed font file scheme, now just one file and support PROGMEM for low memory cpu's.
 - Added more fonts.
 - Added initial support for many other CPU's (ESP, Chipkit, STM32).
 - Text Rendered now correctly follows wrap rules.
 - Fixed setFontScale(x,y), x and y scale was inverted (this can affect code that uses b10)
 - Fixed a bug when scaling it's on with showUserChar
 - Fixed a bug on println for render fonts.(this can affect code that uses b10)
 - Updated many outdated examples.
 - Changed licence from CC to GNU v3<br>

**0.70b11p2** changes...<br>
 - Try to fix drawPixel and drawPixels color bug in 8bit mode (RA8875 hardware error).
 - Command useLayers(false) re-enable 16bit bpp as well.<br>

**0.70b11p3** changes...<br>
 - Fixed drawPixel and drawPixels for 8bit mode, added _color16To8bpp optimized.<br>

**0.70b11p4** changes...<br>
 - Fixed an error in font rendering, I have to add a new parameter in font file so if someone of you have already converted some font with release prior this one you need to update the template of lcd-image-converter with the new one provided in this release and re-convert, just open the .xlm file e 'Convert', new font file will be updated. Haved to upload again all fonts with new scheme but in the hurry I forget to included licenses so I will provide this tomorrow, sorry to font authors!<br>

**0.70b11p5** changes...<br>
 - Fixed a bug in Font Rendering Line Optimizer that causes loss pixels in some font. Now works as it should and it's also slight faster.<br>

**0.70b11p6** changes...<br>
 - Fixed a stupid error in setFontScale (was inverted), some improvements in several drawings functions, added ER3301_1 font rom.<br>

**0.70b11p7** changes...<br>
 - Added an user option in _settings/RA8875UserSettings.h to use alternative Wire1 on Arduino DUE, experimental since I'm actually don't have a DUE.<br>

**0.70b11p8** changes...(will be downloadable in few hours)<br>
 - Fixed some small bugs (thanks Mr Tom), added some extra support for different CPU's, fixed a couple of examples.
 - Introduced some major changes in initialization, it seems works but better check with differents MCU's before apply as 'gold master'<br>


**0.70b11p9** changes...<br>
 - Fixed compiling compatibility with Particle Spark<br>

**0.70b11p10** changes...<br>
 - Added initial support for incoming Teensy's
 - Some changes from MrTom accepted, should fix an hardware bug in trianles and some other minor things
 - Created _includes folder and moved from settings some file that should never change, this will help users to save their settings between updates.<br>
 

Current beta **tested only with**:
* EastRising RA8875 5"Inch (480x272) with resistive Touch or FT chip capacitive<br>
* EastRising RA8875 7"Inch (800x480) with FT chip capacitive<br>
* EastRising RA8875 5"Inch (800x480) with resistive Touch<br>
 All tested with Teensy 3.0, Teensy 3.1, Teensy LC, Arduino DUE, Arduino UNO, Arduino MEGA<br>
Check 'Proof of working' in wiki.<br>

##### Not Working? Check here!<br>
<b>I have tested 0.70b11 with several CPU and <b>worked</b>:</b><br>
 - Teensy 3.1
 - Teensy 3.0
 - Teensy LC
 - Arduino UNO
 - Arduino Leonardo
 - Arduino NANO
 - Arduino DUE
 - Arduino MEGA
 - Arduino YUN
 - Many arduino variants based on AVR
 <br>
 <b>Display tested:</b><br>
 - EastRising RA8875 5" (480x272)
 - EastRising RA8875 5" (800x480)
 - EastRising RA8875 7" (800x480)
 - Adafruit (480x272) 
 - Adafruit (800x480)<br>
 <b>Planned but still not working:</b>
 - Energia (basic support added but never tested)
 - ESP8266 (compile but several problems with HEAP and PROGMEM to fix)
 - SPARK (early development, NEVER tested)<br>
 <b>Currently not working (needs library changes and support):</b>
 - Arduino ZERO
 - Raspberry (there's alternative libraries for this since many uses python)
 - STM ARM processors (one day, for sure)<br>
 <br>
 If your display not works, please check your connections, supply values, etc.<br>
 Remember it's an SPI connection so there's hardware limitations on some CPU, for example DUE can use as CS pin 4,10   or 52, Teensy can use more but not all, 8 Bit arduino can use almost any, etc..<br>
 Still not working? Check my wiki, **check carefully!**<br>
 When testing, do not connect other devices on the same SPI line! This chip uses SPI mode 3 that it's not compatible
 with most stuff (check my wiki).<br>
 Are you sure you connect everything fine, settings are ok, you are using the correct CS pin, you don't have any other  SPI devices sharing the same line, the display has correct jumper set and you are sure that display it's correct supplied (and the supply it's stable, check also this!) but you still have nothing on screen?<br>
  Ok, in that case you may ask help but you need to double check that you do not make mistakes!<br>Please understand that   I cannot spend time to fix something that it's not a library fault!!!

##### Whoah...Too many commands! I'm lost! <br>
Not really, I provided tons of examples. The library it's scalable so it's really easy to use and as you have learned  basics the most it's done. Most libraries are so basic that you are able to do just basic stuff but with this you have a lot of power for build even complicated interfaces<br>

##### Teensy notes:
I love Teensy 3 MCU's, so every library has special features for this micro. You need to read my notes about wiring first. Some examples works only for Teensy 3.<br>

##### Convert a font
Follow wiki https://github.com/sumotoy/RA8875/wiki/How-to-convert-a-font<br>
I'm not the author of the conversion program so please refere to author site for suggestion or bugs!<br>
So please do not open issues for lcd-image-converter here !!!<br>

##### Description
A Fast and Optimized library for RAiO RA8875 display driver for Teensy 3.x, Teensy LC and Arduino's (and I hope for other MCU's).<br>
This is the first attemp to create a library that use all the features of this chip _from scratch_ and have nothing to share with the Adafruit_RA8875 library.<br>
As always I provide a lot of examples (check video), more coming soon...<br>

This library will work also with the Adafruit board but was mainly coded for the many TFT displays from china makers that use this chip, some are quite good and cheap, like the EastRising from buydisplay.com, much cheaper than adafruit.<br>I'm not related to EastRising or BuyDisplay, in any way, but I appreciate that I don't have to spend a little fortune for DIY stuff.<br>


####  The goals...
  - All features supported
  - 8 bit and 16 bit color depth support (X)
  - Layer support (now support large screen but only at 256 colors) (X)
  - Scalable complexity, start with basics but you can go trough... (X)
  - Support for the many external ROM Font chip (X)
  - Support for the external SPI FLASH (on develop)
  - Working with Teensy 3.x, Teensy LC, Arduino's, Energia IDE supported MCU's (stellaris, etc.)
  - Correct use of Print, Write and Println (X)
  - Correct use of setCursor to mimic LiquidCrystal library (X)
  - Compatibility with alternative SPI pinouts on Teensy 3.x,Teensy LC, this will let you use it with Audio board! (X)
  - Can use capacitive and resistive touch screen, the capacitive with gesture and max 5 concurrent touches. (X)
  - Full rotation support. (X)
  - External copyright free fonts support plus large font rendering. (X)
  - Support for Keypad Matrix. (still working on)
  - Internal support for IRQ's (done for Touch, still develop for other stuff)
  - Use of BTE (almost done)
  - A lot of examples. (X)


#### Wiring with your MCU
I support only _native SPI_.<br>
**MOSI,MISO,SCK** pins can be different between MCU's (UNO and Teensy3 uses 11,12,13) but DUE and other are different so check!)<br>
For **RST** it's your choice, it's really possible use _any_ pin. Apparently only Adafruit board need this, the 2 chinese boards I have here have internal reset circuit so it's optional, but I suggest to use it.<br>
For **CS** pin you have to choose between these pin on **Teensy3.1: 2,6,9,10,15,20,21,22,23**. Arduino UNO,MEGA and almost all 8 bit variants can use any pin, DUE can use as CS only 4,10 or 52 (please check DUE specifications)<br>
You also need another pin for **INT** used by Touch Screen (I used pin 2)<br>
From version 0.6, Energia IDE will be supported so many MCU's can be used but should wait 0.6 and since I have only Stellaris LM4F120XL I cannot be sure of the various MCU's wiring so drop me a note, at list I can add to the documentation!

#### Compatible with PJRC Audio Board! (teensy3.1 only)
Current beta has a optional Teensy instance that can use alternative SPI pinouts, this let you use Audio Board from PJRC that uses the classic SPI pinouts for RX and SD cs. You can test it with Spectrum Analyzer example that uses the Audio Board with a RA8875 TFT screen and thanks to the hardware accelleration of this chip and the use of onchip screen buffer it let you have large screen with touch capabilities with high-end audio manipulation.
see wiky:<br>
https://github.com/sumotoy/RA8875/wiki/Teensy-3.1---Working-with-PJRC-Audio-Board-with-SD-card-and-RA8875

#### Disclaim
First, I'm not affiliated or related to any vendor! I've buy all displays and accessories online as everyone else and received 1 in donation<br>
I'm not responsable to any damage or money loss or problem caused by this library, this is an open source project licensed under GNU General Public License v3.0 so I accept all the GNU rules and you should follow too but for commercial products please it's mandatory you contact me first. This library it's only for civil applications, I do not allow to use for any military purposes of any kind <br>

#### Licensing

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
