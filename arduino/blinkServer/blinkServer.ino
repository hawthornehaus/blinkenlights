/*
  Blink Server
  Chris Ertel
  
  Web server for the barduino--runs the lightboard.
  
  Code adapted from WebServer AjaxRGB sample code.  
 */

/* Web_AjaxRGB.pde - example sketch for Webduino library */

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"
#include "WS2801.h"

// CHANGE THIS TO YOUR OWN UNIQUE VALUE
static uint8_t mac[6] = { 0x42, 0x00, 0x00, 0x00, 0x00, 0x01 };

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
const static int dataPin = 34;
const static int clockPin = 30;
int needsUpdate = 0;
// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// CHANGE THIS TO MATCH YOUR HOST NETWORK

/* all URLs on this server will start with /rgb because of how we
 * define the PREFIX value.  We also will listen on port 80, the
 * standard HTTP service port */
#define PREFIX "/rgb"
WebServer webserver(PREFIX, 80);

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
  0x00ff0000,  0x00ff0000,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ffff00,  0x00ffff00,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ff0000,  0x00ffff00,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ffff00,  0x00ffff00,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,  0x00ff0000,
};
//*/

uint32_t chara[40] = {
  0x1, 0x1, 0x1, 0x1, 0x0,
  0x1, 0x0, 0x0, 0x0, 0x1,
  0x1, 0x1, 0x1, 0x1, 0x0,
  0x1, 0x0, 0x0, 0x0, 0x1,
  0x0, 0x1, 0x1, 0x1, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 
};

uint32_t num_0[40] = {
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

uint32_t num_1[40] = {
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
};

uint32_t num_2[40] = {
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
};

uint32_t num_3[40] = {
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

uint32_t num_4[40] = {
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
};

uint32_t num_5[40] = {
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

uint32_t num_6[40] = {
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

uint32_t num_6_new[40] = {
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

uint32_t num_7[40] = {
  0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000,
};

uint32_t num_8[40] = {
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

uint32_t num_9[40] = {
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF,
  0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
};

uint32_t num_10[40] = {
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00000000, 0x00FFFFFF,
  0x00FFFFFF, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
};

void drawFrameBuffer()
{
  //int i,j;
  // blit the framebuffer
  //for (i = 0; i < nx; i+=2)
  //{
  //  for (j = 0; j < ny; j++)
  //  {
  //    strip.setPixelColor(i*ny+ j, frameBuffer[i*ny + j].u);    
  //  }
  //}
  
  //for (i = nx -1 ; i >= 0; i-=2)
  //{
  //  for (j = ny - 1; j >= 0; j--)
  //  {
  //    strip.setPixelColor((i)*ny+j, frameBuffer[(i+1)*ny - j - 1].u);
  //  }
  //}
  int i;
  for (i = 0; i < nx * ny; i++)
  {
     strip.setPixelColor((99 - i) + ((i/10)%2) * (((i % 10) * 2) - 9), frameBuffer[i].u); // Goddamn magic.   
  }
  
  strip.show();
}
void drawChar(uint32_t tl_loc, uint32_t size_x, uint32_t size_y, uint32_t * char_image, uint32_t color)
{
  for (int i = 0; i < size_x * size_y; i++)
    frameBuffer[tl_loc + i % size_x + ((i / size_x) * nx)].u = char_image[i] & color;
  
  return;
}

void drawBackgroundChar(uint32_t tl_loc, uint32_t size_x, uint32_t size_y, uint32_t * char_image)
{
  for (int i = 0; i < size_x * size_y; i++)
    frameBuffer[tl_loc + i % size_x + ((i / size_x) * nx)].u =
        char_image[i] & frameBuffer[tl_loc + i % size_x + ((i / size_x) * nx)].u;
  
  return;
}

void drawFontChar(uint32_t tl_loc, uint32_t * char_image, uint32_t color)
{
  drawChar(tl_loc, 5, 8, char_image, color); 
}

void drawBackgroundFontChar(uint32_t tl_loc, uint32_t * char_image)
{
  drawBackgroundChar(tl_loc, 5, 8, char_image); 
}

void drawBackground(uint32_t color)
{
  int i;
  for (i = 0; i < nx * ny; i++)
    frameBuffer[i].u = color;
  
}
void Countdown(uint32_t start, uint32_t time_delay)
{
  int i;
  for (i = start; i >= 0; i--)
  {
    displayNumber(i);
    drawFrameBuffer();
    delay(time_delay);
  }
  // Clear the LEDs.
  drawBackground(0x00000000);
}

// 2-digit Maximum
void displayNumber(uint32_t num)
{
   uint32_t * tens, * ones;
   tens = number_switch(num / 10);
   ones = number_switch(num % 10);
   
   drawFontChar(20, tens, 0x00FFFFFF);
   drawFontChar(25, ones, 0x00FFFFFF);
}

uint32_t * number_switch(uint32_t num)
{
  switch(num)
  {
    case 0:
      return num_0;
    case 1:
      return num_1;
    case 2:
      return num_2;
    case 3:
      return num_3;
    case 4:
      return num_4;
    case 5:
      return num_5;
    case 6:
      return num_6;
    case 7:
      return num_7;
    case 8:
      return num_8;
    case 9:
      return num_9;
  } 
}

int red = 0;            //integer for red darkness
int blue = 0;           //integer for blue darkness
int green = 0;          //integer for green darkness

/* This command is set as the default command for the server.  It
 * handles both GET and POST requests.  For a GET, it returns a simple
 * page with some buttons.  For a POST, it saves the value posted to
 * the red/green/blue variable, affecting the output of the speaker */
void rgbCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);

      /* this is a standard string comparison function.  It returns 0
       * when there's an exact match.  We're looking for a parameter
       * named red/green/blue here. */
      if (strcmp(name, "red") == 0)
      {
	/* use the STRing TO Unsigned Long function to turn the string
	 * version of the color strength value into our integer red/green/blue
	 * variable */
        red = strtoul(value, NULL, 10);
        if (!needsUpdate)
          needsUpdate = 1;
      }
      if (strcmp(name, "green") == 0)
      {
        green = strtoul(value, NULL, 10);
        if (!needsUpdate)
          needsUpdate = 1;
      }
      if (strcmp(name, "blue") == 0)
      {
        blue = strtoul(value, NULL, 10);
        if (!needsUpdate)
          needsUpdate = 1;
      }
    } while (repeat);
      if (needsUpdate) 
  {

//      colorWipe(Color(red, green, blue), 0);
    colorSet(Color(red,green,blue));

  }
    
    // after procesing the POST data, tell the web browser to reload
    // the page using a GET method. 
    server.httpSeeOther(PREFIX);

    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    /* store the HTML in program memory using the P macro */
    P(message) = 
"<!DOCTYPE html><html><head>"
  "<title>Webduino AJAX RGB Example</title>"
  "<link href='http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/themes/base/jquery-ui.css' rel=stylesheet />"
  "<script src='http://ajax.googleapis.com/ajax/libs/jquery/1.6.4/jquery.min.js'></script>"
  "<script src='http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/jquery-ui.min.js'></script>"
  "<style> body { background: black; } #red, #green, #blue { margin: 10px; } #red { background: #f00; } #green { background: #0f0; } #blue { background: #00f; } </style>"
  "<script>"

// change color on mouse up, not while sliding (causes much less traffic to the Arduino):
    "function changeRGB(event, ui) { var id = $(this).attr('id'); if (id == 'red') $.post('/rgb', { red: ui.value } ); if (id == 'green') $.post('/rgb', { green: ui.value } ); if (id == 'blue') $.post('/rgb', { blue: ui.value } ); } "
    "$(document).ready(function(){ $('#red, #green, #blue').slider({min: 0, max:255, change:changeRGB}); });"
    
  "</script>"
"</head>"
"<body style='font-size:62.5%;'>"
  "<div id=red></div>"
  "<div id=green></div>"
  "<div id=blue></div>"
"</body>"
"</html>";

    server.printP(message);
  }
}

void setup()
{
  // setup the Ehternet library to talk to the Wiznet board
  Ethernet.begin(mac);

  /* register our default command (activated with the request of
   * http://x.x.x.x/rgb */
  webserver.setDefaultCommand(&rgbCmd);

  /* start the server to wait for connections */
  webserver.begin();

  // Update LED contents, to start they are all 'off'
  strip.begin();
  strip.show();
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();
}

void colorWipe(uint32_t c, uint8_t wait)
{
 int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
  needsUpdate = 0;
}
void colorSet(uint32_t c)
{
 int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);

  }
  strip.show();
  needsUpdate = 0;
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c = 0;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

