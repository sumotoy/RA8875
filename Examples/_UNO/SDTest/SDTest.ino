/*
	Grab bmp image from an sd card.
 	It reads column by column and send each RA instead of pixels
	Look inside the folder RA8875/examples/SDTest_800x480
	there's a folder, copy the content in a formatted FAT32 SD card
	Note: Fixed the garbage introduced with Teensy3 optimized SD library
	You can use USE_TEENSY3_OPTIMIZED_CODE now.
 */
#include <SPI.h>
#include <RA8875.h>
#include <SD.h>


/*
Teensy3.x
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x
 MISO:  12//Teensy3.x
 SCK:   13//Teensy3.x
 the rest of pin below:
 */

#define SDCSPIN      6//for SD
#define RA8875_CS 10 //see below...
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
*/
#define RA8875_RESET 9//any pin or nothing!

#if defined USE_TEENSY3_OPTIMIZED_CODE
#define BUFFPIXEL 120//do not change this! It's a workaround to SD caching artifact introduced in SD optimized
#else
#define BUFFPIXEL 20//I've experiment a little with this but not so many differences
#endif

//instances
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET); //Teensy3/arduino's
File     bmpFile;


void setup()
{
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("RA8875 start");

  tft.begin(RA8875_480x272);
  if (!SD.begin(SDCSPIN)) {
    Serial.println("SD failed!");
    return;
  }
  Serial.println("OK!");
  bmpDraw("alert.bmp", 0, 0);//copy the enclosed image in a SD card (check the folder!!!)
}

void loop()
{

}



void bmpDraw(const char *filename, uint16_t x, uint16_t y) {

  
  uint16_t      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint16_t  buffidx = 0; 
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int16_t      w, h, row, col;
  uint32_t pos = 0, startTime = millis();
  buffidx = sizeof(sdbuffer);// Current position in sdbuffer
  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == 0) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        uint16_t rowBuffer[w];
        if (((w - 1)+x) >= tft.width())  w = tft.width()  - x;
        if (((h - 1)+y) >= tft.height()) h = tft.height() - y;

        for (row = 0; row < h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
            
            rowBuffer[col] = tft.Color565(sdbuffer[buffidx++], sdbuffer[buffidx++], sdbuffer[buffidx++]);
          } // end pixel
          tft.setY(y + row);
          tft.drawPixels(rowBuffer, w, x, y + row);
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) {
    Serial.println("BMP format not recognized.");
  } else {
    Serial.println("end...");
  }
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

void writePixb(int16_t x, uint16_t color) {
  tft.setX(x);
  tft.writeCommand(RA8875_MRWC);
  tft.writeData16(color);
}

uint16_t read16(File &f) {
  uint16_t result = 0;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result = 0;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

