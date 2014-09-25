RA8875 library
==============

[![video test](http://i.ytimg.com/vi/WbFOsxjFCL8/mqdefault.jpg)](http://www.youtube.com/embed/WbFOsxjFCL8)

A Fast and Optimized library for RAiO RA8875 display driver for Teensy 3.x and Arduino's (and I hope for other MCU's).
This is the first attemp to create a library that use all the features of this chip and I'm tring to optimize as much I can.
Actually it's in alpha stage, working but only SPI (4 wires), please read the notes in .h file.
I provide a lot of examples, coming soon...
Please note that there's some remaining code from the Adafruit_RA8875 that I've use to test some basic functionalities but it's 100%
stealed from the RAiO application note so in that case it's quite difficult talk about copyrights!
As always, thanks to Lady Ada and his team to provide some code for developers, a good start point!

This library will work with the Adafruit board but was made expecially for the many tft on china market that use this chip, some are quite good and cheap, like
the EastRising at buydisplay.com, well made and much cheaper than adafruit.

## PLEASE, READ THIS
Until I reached a stable beta (around version 0.9) it's normal that something doesn't go in the right way!
At this time I'm also redesign the entire library so many functions change name, parameters or disappear, it's not
safe to use in an project!!!! If you help me testing it's always a great idea, I'm actually working with Teensy3 and buyDisplay version of the TFT (not the adafruit) but plan to switch between MCU's around version 0.8 when function names and overall structure will be fixed.
Thanks for your attention....

####  The goals...
  
  - All features supported
  - All communication protocols supported
  - Support for the many external ROM Font chip
  - Support for the external SPI FLASH
  - Suitable for Teensy 3 and Arduino's
  - Correct use of Print, Write and Println
  - Correct use of setCursor to mimic LiquidCrystal library

This early beta version uses the new SPI Transaction library from Paul Stoffregen that it's included in Teensy 3 1.0.5 R20/2 IDE and will be prolly adapted for Arduino 1.0.5 as well pretty soon, but can automatically downgrade to normal SPI library.
The beta release will use a main section for all chip stuff and several other libs for the protocols (as my LiquidCrystalNew).
I should have a stable and workable version in a couple of days with many examples included...


#### About RA8875 chip
This is amazing device, if you read the capabilities you will shocked but all come at a price...
All communicate with this chip can be a problem, remember that it's not a classic driver where you have basic drawing commands and you build up everithing else, this one use kinda macros and the chip will take care of all drawings so you have tons of registers and commands! 
Another issue... when you sent a command you will prolly need to check if the chip has terminated job before send another istruction, this is important since your MCU it's really fast to send commands and it's prolly faster than RA8875 so you can easily going messed up! 
The chip it's prone to freeze if you send data out of range, this not happen with all istructions but I'm investigating.
The documentation of this chip it's hudge so prepare spend several hours to study over and over....
I'm started with 4 wire SPI that is probably the best choice, just 4 wires (maybe 5 or 6) and you can have a full color interface without lose all your MCU pins, it's pretty damn fast but I spent hours to figure out that the best way it's use SPI transactions since Write commands can be max 6.6Mhz but Read ones works only at 3.2Mhz max! If you are using another SPI chip on the same bus remember to modify his library for use SPI transactions and you save the day...

RA8875 will shine even with a small and slow MCU even if you are driving a 800x400 65K color tft!
You have onboard touch screen controller, keypad controller, PWM's 2GPIO's, has BTE graphic engine and DMA capabilities and much much more, really impressive.

#### Connections
It's an Early beta, only SPI so it uses native SPI.
MOSI,MISO,SCK pins will be differ between MCU's (UNO and Teensy3 uses 11,12,13) but DUE and other may differ)
CS and RST it's your choice, I'm using the standard UNO,TEENSY3 CS pin (10), as RST it's really possible use any pin.
You also need another 2 PINS, uno for Touch Screen and another for SD cs if your board it's full featured, let's talk about this later.


