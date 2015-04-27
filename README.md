RA8875 library
==============

[![video test](http://i.ytimg.com/vi/WbFOsxjFCL8/mqdefault.jpg)](http://www.youtube.com/embed/WbFOsxjFCL8)
<br>Here's a video test that proof the 0.45 version, Teensy3.1 and chinese board for tft.<br>
**Wiki added!** https://github.com/sumotoy/RA8875/wiki

##### Current Version: 0.69b27 (beta, re-download all library and read changes!!!)<br>
Current beta **tested only with**:

* Teensy 3.1, Stellaris
* EastRising RA8875 5"Inch (480x272) in SPI mode<br>
* EastRising RA8875 7"Inch (800x480) in SPI mode<br>
<b>NOTE</b>:If your do not work please ask here, I've <b>VERIFIED</b> that works.<br>

A couple of users tested also with:
* Arduino UNO, Arduino YUN
* Arduino MEGA
* Adafruit RA8875 board w 800x600 and 480x272 displays

##### Teensy notes:
I love Teensy 3 MCU's, so every library has special features for this micro. You need to read my notes about wiring first. Some examples works only for Teensy 3.<br>

##### Beta changes:

Be careful since until beta 0.70 things will change a lot so you better not use in your project or you will forced to modify your code! This is necessary because this library use a lot of command (RA8875 it's a really complex chip)<br><br>
From *0.68b7* I have finally breaked the SPI speed bottleneck and fixed SPIMode so better you test if it's still working for you, please note that high SPI speeds needs short and firmly soldered cables, if your project have not this needed features you may have to tune the *SPI_MULT* parameter in the R8875.h file from the actual value to a lower one, even at 1 (that act as was before the SPI speedup)<br>
<br>

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
* 0.60:Introduced compatibility if used width [PJRC Audio Board!](http://www.pjrc.com/store/teensy3_audio.html). <s>Still fixing this but plan to release fully compatible for next release in these days.</s> Tested and working really fast with PJRC Audio Board! I was able to visualize almost in realtime 238 bands from the fft with touch screen and 5x4 matrix switch support as well. Examples coming soon (only for Teensy3.x and Audio Board).
* 0.64:High optimizations for Teensy3, now uses DMA SPI and several drawing commands optimized. Not deeply tested btw.
* 0.65:Fix a typo for DUE (thanks for point me to this). Now compile with IDE 1.5.8 and DUE but still untested (wait for user feedbacks!).
* 0.66: Github app gived me some code sincronization problems so code online failed to update. The actual fix for DUE should works (Thanks DrewJaworskiRIS to point me this issue).
* 0.67:Added compatibility with Teensyduino 1.21b and IDE 1.6.x
* 0.68:Again changes for Teensyduino 1.22b, in theory it should work as is in Teensyduino 1.20.
* 0.69 (pre-release): A lot of changes, dropped useless alternative SPI pins config (can be easily done in setup),
corrected some errors in drawFastVLine,drawFastHLine,drawRect,fillRect,drawRoundRect,fillRoundRect, dropped hyperdrive since it's useless for a 6.6Mhz SPI,support for 16 bit transfer, starting support for alternate SPI1 for Teensy LC, etc. etc. This version has major changes so if you currently using an old version you probably have small differences.
If you have troubles the old version it's in the folder OldVersions.
* 0.69b2: Full 8 bit color support! This allow user with screen over 272*480 size to use layers!.
* 0.69b3: Small fixes.
* 0.69b4: (Probably) better 800x480 initialization (someone confirm, please!), fixed 640x480 compatibility.
* 0.69b5: RA8875 needs SPIMode3 and not SPIMode0, I've fixed this.
* 0.69b6: Another fix on 800x480 initialization sequence.
* 0.69b7: Dropped SPI speed bottleneck! Now I can reach the hardware limit of the chip (from 6.6Mhz to 21Mhz!) Thanks Christoph to point me to the SystemClock that I was think is the Xtal but at the end it's not!
* 0.69b8: Fixed some typo, rst pin not working with Teensy (now yes), cleaned up a little.
* 0.69b9:b8 was broken, b9 have many fixes and separate user settings file, still try to fix 8 bit layers on 800x480
* 0.69b10: text it's faster than ever, changed useLayer (now not needed to be called), rebuilded showCursors (now easier with 4 different cursors, check demo basicTextFunctions,fixed Triangle, faster SPI.
* 0.69b11: internal changes
* 0.69b12: changeMode function it's now automatic and will be private soon.
* 0.69b15: fixed several errors and added more functionalities by M.Sanderscock. Confirmed 800x480 works.
* 0.69b18: FIXED 8 BIT COLOR! fixed sleep sequence, added a couple of functions.
* 0.69b19: Fixed a couple of examples, memoryClear now works as it should.
* 0.69b21: All rotation issues seems fixed!!! You can run any example in any rotation mode, no issues.
* 0.69b22: fixed println in portrait, fixed getCursor in portrait
* 0.69b23: fixed scroll in portrait mode plus other small things...
* 0.69b24: fixed clearScreen functionality,added getActiveWindow,some other fixes.
* 0.69b25: faster text/graphic change, support the capacitive touchscreen based on FT5206, included gesture.
* 0.69b26: Now can succesfully send block of pixels fast!Added conversion from 24 bit to 16 bit.
* 0.69b27: Font rendering alpha test included (plus several test fonts). It's an alpha so don't complain.


##### Description
A Fast and Optimized library for RAiO RA8875 display driver for Teensy 3.x, Teensy LC and Arduino's (and I hope for other MCU's).<br>
This is the first attemp to create a library that use all the features of this chip _from scratch_ and have NOTHING to do or share with the Adafruit_RA8875 library.<br>
As always I provide a lot of examples (check video), more coming soon...<br>

This library will work also with the Adafruit board but was mainly coded for the many TFT displays from china makers that use this chip, some are quite good and cheap, like the EastRising from buydisplay.com, much cheaper than adafruit.<br>I'm not related to EastRising or BuyDisplay, in any way, but I appreciate that I don't have to spend a little fortune for DIY stuff.<br>


####  The goals...
  
  - All features supported
  - 8 bit and 16 bit color depth support
  - Layer support (now support large screen but only at 256 colors)
  - Scalable complexity, start with basics but you can go trough...
  - All communication protocols supported
  - Support for the many external ROM Font chip
  - Support for the external SPI FLASH
  - Working with Teensy 3.x, Teensy LC, Arduino's, Energia IDE supported MCU's (stellaris, etc.)
  - Correct use of Print, Write and Println
  - Correct use of setCursor to mimic LiquidCrystal library
  - Compatibility with alternative SPI pinouts on Teensy 3.x,Teensy LC, this will let you use it with Audio board!


#### About RA8875 chip
This is amazing device, if you read the capabilities you will shocked but all come at a price...<br>
It cannot act as framebuffer device so forget video and fast loading images even using it's 16 bit bus, it have almost every drawing functions internally hardware accellerated so it's perfectly suitable for drawing interfaces and text mixed to graphics fast and without using cpu resources.<br>
Since all drawing commands are mostly macros, it has a MASSIVE amount of registers, prolly one of the biggest resource driven chip I ever see, really hudge datasheet, it takes a lot of time go deep inside it.<br>
Do you think that driving with 8 or 16 bit interface will be much faster than Serial? I really don't think so!
The problem is that chip needs time to perform a command! In brief, the scenario it's something like this:<br>
You tell it to draw a rect by send rect macros command and colors, the chip starts it's job but you cannot send another command until has finished it's job so you have to check it's register or the WAIT pin before send another command.<br>
This almost for every command. Also drawing bitmap images it's a slow job, there's not a way to send chunks of data, at list I haven't find a working way, the only fast way to get a picture fast on screen is use internal DMA and a optional SPI Flash memory pre-programmed and controlled directly by the chip!<br>
Since it's not a great advantage to use it with 8/16 parallel interface I choosed 4 Wire SPI because it's prolly the best choice for this chip.<br>

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
* Layers support <s>(not for screen larger than 480x272)</s> (limited to 256 colors for screens > 480x272).
* configurable INT pin.


#### RA8875 chip bugs!
I discovered several bugs in the chip.<br><br>
Register **0x10** (SYSR). Changing bit 3 to 1 should set (accordly datasheet) the 65K color feature.<br>
In real life this set apparently set almost all drawing functions to 65K color BUT _drawing single pixel it result in a 256 color!_. I spent a lot of time to discover that I need to set bit 3,2 to 1 to solve the problem, sent a note to RAiO to correct datasheet.<br><br>
Register **0xB3** (should be SSAR3), part of the 32 bit addressing of the DMA start address... Was purposely erased on all last datasheet, still present in many application notes, what the hell I have to do to address 32bit data?<br><br>

**IF YOU OWN A EASTRISING from BuyDisplay please read this!!**<br>
Thanks to the help of The Experimentalist was discovered that those display have pullups on any SPI output! This cause any type of malfunction with other SPI devices and even damage processor pin in some case. The display affected are the 5" and 7", black PCB, see this page<br>

https://github.com/sumotoy/RA8875/wiki/EastRising-and-Buydisplay-SPI-configuration-and-wiring

<br>
There's another **hardware issue on MISO** that's a problem only if you are not planning to use any other SPI devices together with RA8875 (example, the SD card holder!), Paul Stoffregen discover the MISO bug that it's not tristate:<br>
https://github.com/sumotoy/RA8875/wiki/Fix-compatibility-with-other-SPI-devices<br><br>
The chip it's **NOT out-of-range-values tolerant!** (in contrast of the 90% of the other commercial drivers) If a value it's out of range you can experience various screen weirdness like garbage, white screen or chip freeze! This forced me to carefully surround many function with data range checks.<br><br>
Memory Clear full should clear all layers memory? Accordly datasheet seems yes but it doesn't work as it should. On Datasheet, memory clear can clear the current screen or the entire memory but there's very obscure commands like memory start clear and memory stop clear that are not documented... Grrr<br>
During the fix of setRotation issues I found another bug, the RA8875 forget some parameter previously assigned when you touch the scan registers...Again solved by some library workaround<br>

#### Wiring with your MCU
I support only _native SPI_.<br>
**MOSI,MISO,SCK** pins will be differ between MCU's (UNO and Teensy3 uses 11,12,13) but DUE and other are different so check!)<br>
For **RST** it's your choice, it's really possible use _any_ pin. Apparently only Adafruit board need this, the 2 chinese boards I've tested have internal reset circuit so it's optional<br>
For **CS** pin you have to choose between these pin on **Teensy3.1: 2,6,9,10,15,20,21,22,23**. Arduino UNO,MEGA and almost all 8 bit variants can use any pin, DUE maybe need special pins (please check DUE datasheet)<br>
You also need another 2 PINS, **INT** for Touch Screen (I used pin 2) and a **CS** for SD (I used pin 21).<br>If your board don't have SD slot (Adafruit don't) just forget the SD example (btw you can use any SD card holder and use it)<br>
From version 0.6, Energia IDE will be supported so many MCU's can be used but should wait 0.6 and since I have only Stellaris LM4F120XL I cannot be sure of the various MCU's wiring so drop me a note, at list I can add to the documentation!

#### Compatible with PJRC Audio Board! (teensy3.1 only)
Current beta has a new designed instance that can use alternative SPI pinouts, this let you use Audio Board from PJRC that uses the classic SPI pinouts for RX and SD cs. You can test it with Spectrum Analyzer example that uses the Audio Board with a RA8875 TFT screen and thanks to the hardware accelleration of this chip and the use of onchip screen buffer it let you have large screen with touch capabilities with high-end audio manipulation.
see wiky:<br>
https://github.com/sumotoy/RA8875/wiki/Teensy-3.1---Working-with-PJRC-Audio-Board-with-SD-card-and-RA8875



![An FFT snapshot](https://github.com/sumotoy/RA8875/blob/master/documentation/CIMG1886.JPG)
