#include "SPI.h"
#include "WS2801.h"

/*****************************************************************************
Example sketch for driving WS2801 pixels
*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int dataPin = 34;
int clockPin = 30;
// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply


// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
WS2801 strip = WS2801(100, dataPin, clockPin);

typedef struct 
{
  uint32_t g:8;
  uint32_t r:8;
  uint32_t p:8;
  uint32_t b:8;
} Color_t;

typedef union Pixel
{
  Color_t c;
  uint32_t u;
} Pixel;

const int nx = 10;
const int ny = 10;

Pixel frameBuffer[nx*ny];
///*
uint32_t image[100] = {
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ffff00,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ffff00,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ffff00,  0x00ffff00,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,    
};
//*/

void setup() {
    
  // init framebuffer
  for (int i = 0; i < 100; i++) {
    frameBuffer[i].u= image[i];
//    frameBuffer[i].u = 0;
  }
  
  frameBuffer[11].c.p = 1;

  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();
}


void drawFrameBuffer()
{
  int i,j;
  // blit the framebuffer
  for (i = 0; i < nx; i+=2)
  {
    for (j = 0; j < ny; j++)
    {
      strip.setPixelColor(i*ny+ j, frameBuffer[i*ny + j].u);    
    }
  }
  
  for (i = nx -1 ; i >= 0; i-=2)
  {
    for (j = ny - 1; j >= 0; j--)
    {
      strip.setPixelColor((i)*ny+j, frameBuffer[(i+1)*ny - j - 1].u);
    }
  }
  strip.show();

}

void loop() {
//    rainbow(1);

  drawFrameBuffer();
  delay(450);
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
