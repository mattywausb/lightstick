// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>
#include "lamp.h"



#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 7

#define TRACE_ON

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick



long g_music_start_time=0;
int g_pattern_type_selected = 0;



void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  input_setup();
  output_setup();
  output_start_pattern(g_pattern_type_selected);
}

void loop() {
  // Get current button state.
  input_switches_scan_tick();
  if (input_stepGotPressed()) {
      if (++g_pattern_type_selected > 6) g_pattern_type_selected=0;
      output_start_pattern(g_pattern_type_selected);
      #ifdef TRACE_ON 
       Serial.print("#startpattern ");Serial.println(g_pattern_type_selected);
      #endif
  }
  output_process_pattern();
}

