RA8875 library [gold version]
==============================

**Wiki added!** https://github.com/sumotoy/RA8875/wiki<br>
Moved a lot of 'bla bla' in the wiki, readme cleaned.

##### Current Version: 0.70b6(preview 2) <br>

Current beta 0.70 **tested only with**:
* EastRising RA8875 5"Inch (480x272) with resistive Touch or FT chip capacitive<br>
* EastRising RA8875 7"Inch (800x480) with FT chip capacitive<br>
* EastRising RA8875 5"Inch (800x480) with resistive Touch<br>
Check 'Proof of working' in wiky.<br>

A couple of users tested also with:
* Arduino DUE
* Adafruit RA8875 board with 7" (800x600) and 5" (480x272) displays

<b>NOTE</b>:If your do not work please ask here but remember that this is a preview to test new capabilities!<br>

##### Current version 0.70b5p1, it's the first preview of the 0.70b5! Please read!!!.<br>
**0.70b5**<br>
massive changes, lot of fixing,rendering fonts,some command changed!<br>
**0.70b6**<br>
changes in the FONT ROM code, added GT30L16U2W support, fixed an IRQ error.<br>

##### Rendering Font Note
This library support user fonts but the utility I'm using to convert fonts it's not released since it's a modified copy from another author. The reason it's simple, it's the only software I've found that it's able to precisely define fonts parameters and format the font in clever way (intead of the classic monster array with another index array).
The utility it's also able to compress fonts, write 16 or 32 bit lenght strings (useful for large fonts),I cannot find any program that let me do that so I have decided to use this.<br>
However, since it's not my code, I will release the app only when author give me the rights to do that (or incorporate my changes in the next release).<br>
If someone of you have some font proposal to convert (in the meantime) please let me know, I will convert and incorporate in the library but be sure it's copyrights permit this!<br><br>

##### Teensy notes:
I love Teensy 3 MCU's, so every library has special features for this micro. You need to read my notes about wiring first. Some examples works only for Teensy 3.<br>

##### Description
A Fast and Optimized library for RAiO RA8875 display driver for Teensy 3.x, Teensy LC and Arduino's (and I hope for other MCU's).<br>
This is the first attemp to create a library that use all the features of this chip _from scratch_ and have nothing to share with the Adafruit_RA8875 library.<br>
As always I provide a lot of examples (check video), more coming soon...<br>

This library will work also with the Adafruit board but was mainly coded for the many TFT displays from china makers that use this chip, some are quite good and cheap, like the EastRising from buydisplay.com, much cheaper than adafruit.<br>I'm not related to EastRising or BuyDisplay, in any way, but I appreciate that I don't have to spend a little fortune for DIY stuff.<br>


####  The goals...
  - All features supported
  - 8 bit and 16 bit color depth support (done)
  - Layer support (now support large screen but only at 256 colors) (done)
  - Scalable complexity, start with basics but you can go trough... (done)
  - Support for the many external ROM Font chip (done)
  - Support for the external SPI FLASH (on develop)
  - Working with Teensy 3.x, Teensy LC, Arduino's, Energia IDE supported MCU's (stellaris, etc.)
  - Correct use of Print, Write and Println (done)
  - Correct use of setCursor to mimic LiquidCrystal library (done)
  - Compatibility with alternative SPI pinouts on Teensy 3.x,Teensy LC, this will let you use it with Audio board! (done)
  - Can use capacitive and resistive touch screen, the capacitive with gesture and max 5 concurrent touches. (done)
  - Full rotation support. (done)
  - External copyright free fonts support plus large font rendering. (done)
  - Support for Keypad Matrix. (still working on)
  - Internal support for IRQ's (done for Touch, still develop for other stuff)
  - Use of BTE (almost done)
  - A lot of examples. (done)


#### Wiring with your MCU
I support only _native SPI_.<br>
**MOSI,MISO,SCK** pins can be different between MCU's (UNO and Teensy3 uses 11,12,13) but DUE and other are different so check!)<br>
For **RST** it's your choice, it's really possible use _any_ pin. Apparently only Adafruit board need this, the 2 chinese boards I have here have internal reset circuit so it's optional, but I suggest to use it.<br>
For **CS** pin you have to choose between these pin on **Teensy3.1: 2,6,9,10,15,20,21,22,23**. Arduino UNO,MEGA and almost all 8 bit variants can use any pin, DUE maybe need special pins (please check DUE specifications)<br>
You also need another pin for **INT** used by Touch Screen (I used pin 2)<br>
From version 0.6, Energia IDE will be supported so many MCU's can be used but should wait 0.6 and since I have only Stellaris LM4F120XL I cannot be sure of the various MCU's wiring so drop me a note, at list I can add to the documentation!

#### Compatible with PJRC Audio Board! (teensy3.1 only)
Current beta has a optional Teensy instance that can use alternative SPI pinouts, this let you use Audio Board from PJRC that uses the classic SPI pinouts for RX and SD cs. You can test it with Spectrum Analyzer example that uses the Audio Board with a RA8875 TFT screen and thanks to the hardware accelleration of this chip and the use of onchip screen buffer it let you have large screen with touch capabilities with high-end audio manipulation.
see wiky:<br>
https://github.com/sumotoy/RA8875/wiki/Teensy-3.1---Working-with-PJRC-Audio-Board-with-SD-card-and-RA8875
