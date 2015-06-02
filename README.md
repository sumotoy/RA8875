RA8875 library
==============

[![video test](http://i.ytimg.com/vi/WbFOsxjFCL8/mqdefault.jpg)](http://www.youtube.com/embed/WbFOsxjFCL8)
<br>Here's a video test that proof the 0.45 version, Teensy3.1 and chinese board for tft.<br>
**Wiki added!** https://github.com/sumotoy/RA8875/wiki<br>
Moved a lot of 'bla bla' in the wiki, readme cleaned.

##### Current Version: 0.69b64 (beta, re-download all library and read changes!!!)<br>

Current beta **tested only with**:
* Teensy 3.1, Stellaris, Arduino UNO, Arduino MEGA2560, Arduino YUN, Arduino DUE
* EastRising RA8875 5"Inch (480x272) with resistive Touch or FT chip capacitive<br>
* EastRising RA8875 7"Inch (800x480) with FT chip capacitive<br>
* EastRising RA8875 5"Inch (800x480) with resistice chip<br>
Check 'Proof of working' in wiky.<br>

NOT tested at all:
* Waveshare displays (if you are able to force SPI mode...)
* Any 640x480

<b>NOTE</b>:If your do not work please ask here.<br>

##### Current version 0.69b60, it's the first preview of the 0.70! Please read!!!.<br>
**0.69b50**<br>
The current version introduce massive changes to fit the 0.70 milestone, some function has changed name,
most of the code has been retouched but more or less this is the definitive scheme of the final library. The only
functions will change a bit it's in the area of rendering text (by using user fonts) and keypad. The current beta it's faster than any previous version<br>
**0.69b60**<br>
Faster optimized code for AVR (checked), faster code for DUE (not checked), introduced **drawArc** (the only missed graphic macro but very flexible and draw in degrees!), hyperfast **getCursor** and library tracking of the current cursor position without any SPI call to RA, **fixed a lot of bugs**, color problems with text, etc, fixed gPrint (for large text and user font) but still not enabled for rotation. New examples, even a **Pong game**.<br>
**0.69b61**<br>
Fixed out-of-range value-check for drawRect and drawRoundRect, RA8875 it's not tolerant for some register to out-of-range values and generate unpredictable bad results, fixed an issue with text in portrait mode, added isPortrait function, fixed Lady Ada examples, they generate heavy  out-of-range values that are clearly useless.<br><br>
Now it start difficult to find bugs, there are still many to dig out of course, I need help from you to find out the remaining ones! The next releases between this an 0.70 will be only optimization and fixing plus some other feature like keypad matrix and final text rendering engine for big fonts.
**0.69b62**<br>
**0.69b63**<br>
Not released, internal changes.<br>
**0.69b64**<br>
Fixed DUE SPI, now can work in normal SPI, Extended SPI and normal with port manipulated CS (the fast one), Teensy LC can use all the SPI alternative pins and the channel 1 SPI (SPI1). I'm try to figure out why Teensu LC it's slow in native SPI mode, Teensy 3.1 it's the faster but I expect good results from LC but not, I will investigate. ( bit arduino uses port manipulated CS and are pretty fast! This release has also many fixes, due complicated SPI code (for many different CPU!) I have to clean a bit but it's pretty usable and most important I HAVE TESTED with Teensy 3.1, Teensy LC, Arduino UNO, Arduino Yun, Arduino MEGA2560, Arduino Nano, Arduino Micro, Arduino Leonardo, Arduino Duemilanove and works (at list with Eastrising 7" but should work with no problem even with 5" one. I have dropped compatibility with 640x480 and 320x240, sorry but I never see a display with this sizes around!<br>
Remember that this library needs the SPI Transaction active for all the library of the devices on the same SPI line of RA8875, this because I use SPI speed modulation to get the best performances but also this chip works with SPI mode 3 and most devices uses SPI mode 0. If you got garbage on screen, try to disconnect other devices, if works, check if your other library uses SPI transaction and in case add to it, you will have only advantages!<br>


##### Beta 0.70 it's on the corner!
The long awaited 0.70 it's almost ready it will introduce many changes:
- **Everithing it's 2x times faster!** Raised up max SPI speed to 20Mhz.
- No more name changes in functions and parameters.
- Optimized code.
- 0 compiler warnings.
- Faster AVR SPI! 8 bit MCU has bottleneck on SPI, I will introduce a faster SPI routine.
- Fixed the Text color bug (hardware bug of RA8875). The RA8875 has global color but new code will solve this.
- Full keypad support.
- A new autocenter parameter for setCursor let you precisely center text inside objects.
- Support for color grandients.
- Fixed initialization problems with some 800x480 5" screens.
- Support for enhanced SPI for DUE.
- Support for alternative enhanched wire library for Teensy 3,3.1 and LC.
- Support for the second SPI channel for Teensy LC.
- **Compatible with ESP8266! Yes! It will work with this tiny chip!!!**
- And more to come....

##### Beta changes:
Be careful since until beta 0.70 things will change a lot so you better not use in your project or you will forced to modify your code! This is necessary because this library use a lot of command (RA8875 it's a really complex chip)<br>

##### Teensy notes:
I love Teensy 3 MCU's, so every library has special features for this micro. You need to read my notes about wiring first. Some examples works only for Teensy 3.<br>

##### Description
A Fast and Optimized library for RAiO RA8875 display driver for Teensy 3.x, Teensy LC and Arduino's (and I hope for other MCU's).<br>
This is the first attemp to create a library that use all the features of this chip _from scratch_ and have nothing to share with the Adafruit_RA8875 library.<br>
As always I provide a lot of examples (check video), more coming soon...<br>

This library will work also with the Adafruit board but was mainly coded for the many TFT displays from china makers that use this chip, some are quite good and cheap, like the EastRising from buydisplay.com, much cheaper than adafruit.<br>I'm not related to EastRising or BuyDisplay, in any way, but I appreciate that I don't have to spend a little fortune for DIY stuff.<br>


####  The goals...
  - All features supported
  - 8 bit and 16 bit color depth support
  - Layer support (now support large screen but only at 256 colors)
  - Scalable complexity, start with basics but you can go trough...
  - Support for the many external ROM Font chip
  - Support for the external SPI FLASH
  - Working with Teensy 3.x, Teensy LC, Arduino's, Energia IDE supported MCU's (stellaris, etc.)
  - Correct use of Print, Write and Println
  - Correct use of setCursor to mimic LiquidCrystal library
  - Compatibility with alternative SPI pinouts on Teensy 3.x,Teensy LC, this will let you use it with Audio board!
  - Can use capacitive and resistive touch screen, the capacitive with gesture and max 5 concurrent touches.
  - Full rotation support.
  - External copyright free fonts support plus large font rendering.
  - Support for Keypad Matrix.
  - A lot of examples.


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



![An FFT snapshot](https://github.com/sumotoy/RA8875/blob/master/documentation/CIMG1886.JPG)
