// This is code creates small light show, using neo pixel


#include <Adafruit_NeoPixel.h>
#include "lamp.h"
#include "output.h"


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
int g_preset_selected = 7;



void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  input_setup();
  output_setup();
  output_start_preset(g_preset_selected);
}

void loop() {
  // Manage Button Press
  input_switches_scan_tick();
  if (input_stepGotPressed()) {
      if (++g_preset_selected > 11) g_preset_selected=0;
      output_start_preset(g_preset_selected);
      #ifdef TRACE_ON 
       Serial.print("#startpattern ");Serial.println(g_preset_selected);
      #endif
  }

  // Manage Serial input
  if(input_newSerialCommandAvailable()) {
    String command=input_getSerialCommand();
    if(command.startsWith("b")) {
      int bpm=command.substring(2).toInt();
      if(bpm>0) output_set_bpm(bpm);
    }
    if(command.startsWith("w")) {
      int value=command.substring(1).toInt();
      switch(value) {
        case 2: output_set_pattern_speed(STEP_ON_2BEATS); break;
        case 4: output_set_pattern_speed(STEP_ON_BEAT); break;
        case 8: output_set_pattern_speed(STEP_ON_8TH); break;
        case 16:  output_set_pattern_speed(STEP_ON_16TH); break;
        case 32:  output_set_pattern_speed(STEP_ON_32TH); break;
        case 64:  output_set_pattern_speed(STEP_ON_64TH); break;
      }
    }

  }
  output_process_pattern();
}

