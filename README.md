RA8875 library
==============

[![video test](http://i.ytimg.com/vi/WbFOsxjFCL8/mqdefault.jpg)](http://www.youtube.com/embed/WbFOsxjFCL8)
<br>Here's a video test that proof the 0.45 version, Teensy3.1 and chinese board for tft.<br>
**Wiki added!**

##### Current Version: 0.60 (beta, re-download all library and read changes!!!)<br>
Current beta **tested only with**:

* Teensy 3.1, Stellaris
* Chinese RA8875 5"Inch (480x272) in SPI mode

A couple of users tested also with:
* Arduino UNO, Arduino YUN
* Adafruit RA8875 board w 800x600 and 480x272 displays

##### Beta changes:

* 0.49b1:<s>Fixed 800x480 initialization</s>
* 0.49b7:Complete rebuilded touch screen stuff, now much easier and super easy to calibrate
(note: this version have different Touch Screen approach so commands changed! Examples where updated to reflect changes)
* 0.49b8:new command writeTo very useful for write to layers (1,2), pattern,cgram,cursor,etc. A great example of how using layers!
* 0.49b9:Fixed a silly error in display identifier that not enable GPIOx when adafruit stuff is used. Another attemp (I hope the last!) to get 800x480 initialization fixed.
* 0.49b10:Added support for user custom fonts and example.
* 0.49b11:Changed name for user char upload/show, custom font can be designed wide as needed, better example for that, officially fixed 800x480 compatibility and examples verified with arduino uno.
* 0.49b12:Added a softReset workaround if the rst pin is not used. Fixed bubbles example in AVR. Say ByeBye to useless command softReset();
* 0.51:First compatibility test with Energia IDE (stellaris,tiva,etc.) passed but not tested!
* 0.55:Tested and worked (all examples except SD) with **Stellaris** and **Energia 0013 IDE**. All examples now works with any MCU (stellaris included). Due recently changes in Arduino 1.0.6 IDE haved to change all examples.
* 0.60:Introduced compatibility if used width [PJRC Audio Board!](http://www.pjrc.com/store/teensy3_audio.html). <s>Still fixing this but plan to release fully compatible for next release in these days.</s> Tested and working really fast with PJRC Audio Board! I was able to visualize almost in realtime 118 bands from the fft but probably I can get much more, and with touch screen and 5x4 matrix switch support as well. Examples coming soon (only for Teensy3.x and Audio Board).

###### Upcoming beta 0.7 release
A major release upcoming these days, <s>have to fix a silly but important bug on drawPixel color weirdness</s> then I will release the next beta that support SD, full BTE, full DMA, lot of examples, better cursor tracking, almost finished text support and much more!<br>

##### description
A Fast and Optimized library for RAiO RA8875 display driver for Teensy 3.x and Arduino's (and I hope for other MCU's).
This is the first attemp to create a library that use all the features of this chip and I'm tring to optimize as much I can.
Actually it's in early beta stage, working, but only SPI (4 wires), please read the notes in .h file.
I provide a lot of examples (check video), more coming soon...
Please note that there's some remaining code from the Adafruit_RA8875 that I've use to test some basic functionalities but it's 100% stealed from the RAiO application note so in that case it's quite difficult talk about copyrights!
As always, thanks to Lady Ada and his team to provide some code for developers, a good start point!

This library will work with the Adafruit board but was made expecially for the many tft on china market that use this chip, some are quite good and cheap, like
the EastRising at buydisplay.com, well made and much cheaper than adafruit.

### PLEASE, READ THIS
Until I reached a stable beta (around version 0.9) it's normal that something doesn't go in the right way!
At this time I'm also redesign the entire library so many functions change name, parameters or disappear, it's not
safe to use in an project!!!! If you help me testing it's always a great idea, I'm actually working with Teensy3 and buyDisplay version of the TFT (not the adafruit) but plan to switch between MCU's around version 0.8 when function names and overall structure will be fixed.
Thanks for your attention....

####  The goals...
  
  - All features supported
  - Layer support
  - Scalable complexity, start with basics but you can go trough...
  - All communication protocols supported
  - Support for the many external ROM Font chip
  - Support for the external SPI FLASH
  - Working with Teensy 3.x, Arduino's, Energia IDE supported MCU's (stellaris, etc.)
  - Correct use of Print, Write and Println
  - Correct use of setCursor to mimic LiquidCrystal library
  - Compatibility with alternative SPI pinouts on Teensy 3.x, this will let you use it with Audio board!

This beta version uses the new SPI Transaction library from Paul Stoffregen that it's included in Teensy 3 1.0.5 R20/2 IDE and <s>will be prolly</s> adapted for Arduino 1.5.8 as well <s>pretty soon</s>, but can automatically downgrade to normal SPI library if not supported or force disable inside RA8875.h file.
The beta release will use a main section for all chip stuff and several other libs for the protocols (as my LiquidCrystalNew).
I should have a stable and workable version in a couple of days with many examples included...


#### About RA8875 chip
This is amazing device, if you read the capabilities you will shocked but all come at a price...<br>
It cannot act as framebuffer device so forget video and fast loading images even using it's 16 bit bus, it have almost every drawing functions internally hardware accellerated so it's perfectly suitable for drawing interfaces and text mixed to graphics fast and without using cpu resources.<br>
Since all drawing commands are mostly macros, it has a MASSIVE amount of registers, prolly one of the biggest resource driven chip I ever see, really hudge datasheet, it takes a lot of time go deep inside it.
Do you think that driving with 8 or 16 bit interface will be much faster than Serial? I really don't think so!
The problem is that chip needs time to perform a command! In brief, the scenario it's something like this:<br>
You tell it to draw a rect by send rect macros command and colors, the chip starts it's job but you cannot send another command until has finished it's job so you have to check it's register or the WAIT pin before send another command.
This almost for every command. Also drawing bitmap images it's a slow job, there's not a way to send chunks of data, at list I haven't find a working way, the only fast way to get a picture fast on screen is use internal DMA and a optional SPI Flash memory pre-programmed and controlled directly by the chip!<br>
Since it's not a great advantage to use it with 8/16 parallel interface I choosed 4 Wire SPI because it's prolly the best choice for this chip, with datasheet on hand this chip have a limit of 6.6Mhz for write and 3.2Mhz for read so it's perfectly for SPI Transactions (thanks to Paul Stoffregen for this) that is supported officially in the last IDE for Teensy (1.0.6 release 1.20) and Arduino (1.5.8).<br>

#### RA8875 in short

* full accellerated drawing functions.
* support for TFT till 800x480.
* onboard resistive touch screen driver.
* onboard 4x5 keypad controller driver.
* internal font chip.
* external optional font chip.
* external optional SPI flash chip.
* 2 x PWM generators.
* 2 x GPIOX.
* BTE hardware engine.
* DMA access to font rom and SPI flash.
* 256 user configurable fonts.
* Patterns support.
* user configurable graphic cursors.
* Layers support (not for screen larger than 480x272).
* configurable INT pin.


#### RA8875 chip bugs!
I discovered several bugs in the chip.<br>
Register **0x10** (SYSR), setting bit 3 to 1 should set the 65K color feature.<br>
In real life this set apparently set almost all drawing functions to 65K color BUT _drawing single pixel it result in a 256 color!_. I spent a lot of time to discover that I need to set bit 3,2 to 1 to solve the problem, sent a note to RAiO to correct datasheet.<br>
Register **0xB3** (should be SSAR3), part of the 32 bit addressing of the DMA start address... Was purposely erased on all last datasheet, still present in many application notes, what the hell I have to do to address 32bit data?<br>
_The chip it's prone to freeze if you send out-of-range data_, this forced me to surrond code by data-limits-check.<br>

#### Wiring with your MCU
It's an Early beta, only SPI for now so it uses _native SPI_.<br>
**MOSI,MISO,SCK** pins will be differ between MCU's (UNO and Teensy3 uses 11,12,13) but DUE and other are different so check!)<br>
For **RST** it's your choice, it's really possible use _any_ pin.<br>
For **CS** pin you have to choose between these pin on Teensy3:2,6,9,10,15,20,21,22,23. On UNO or DUE refer to they documents.<br>
You also need another 2 PINS, **INT** for Touch Screen (I used pin 2) and a **CS** for SD (I used pin 21).<br>If your board don't have SD slot (Adafruit don't) just forget the SD example (btw you can use any SD card holder and use it)<br>
From version 0.6, Energia IDE will be supported so many MCU's can be used but should wait 0.6 and since I have only Stellaris LM4F120XL I cannot be sure of the various MCU's wiring so drop me a note, at list I can add to the documentation!

#### Compatible with PJRC Audio Board! (teensy3.x only)
Current beta has a new designed instance that can use alternative SPI pinouts, this let you use Audio Board from PJRC that uses the classic SPI pinouts for RX and SD cs. You can test it with Spectrum Analyzer example that uses the Audio Board with a RA8875 TFT screen and thanks to the hardware accelleration of this chip and the use of onchip screen buffer it let you have large screen with touch capabilities with high-end audio manipulation.


![An FFT snapshot](https://github.com/sumotoy/RA8875/blob/master/documentation/CIMG1886.JPG)
