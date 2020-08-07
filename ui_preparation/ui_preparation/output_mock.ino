#include "output.h"


/*
 * 
 *      ATTENTION: Use pin D6 (GPIO12) for NEOPIXEL Data
 * 
 * 
 */

void output_setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  #ifdef TRACE_ON
  Serial.println(F("TRACE_ON>output_setup complete "));
  #endif
}

void output_set_demo_led(boolean value) {
  digitalWrite(LED_BUILTIN, value);
}

void output_set_bpm(int beats_per_minute) {
  #ifdef TRACE_ON
    Serial.print(F("TRACE_ON> MOCK:output_set_bpm:"));Serial.println(beats_per_minute);
  #endif
}


void output_set_pattern_speed(int wait_index)
{
  #ifdef TRACE_ON
    Serial.print(F("TRACE_ON> MOCK:output_set_pattern_speed:"));Serial.println(wait_index);
  #endif
}


void output_start_pattern(int pattern_id) {
    #ifdef TRACE_ON
    Serial.print(F("TRACE_ON> MOCK:output_start_pattern:"));Serial.println(pattern_id);
  #endif
}

void output_reset_color_palette(float hue, float saturation){
    #ifdef TRACE_ON
    Serial.print(F("TRACE_ON> MOCK:output_reset_color_palette:"));Serial.print(hue);Serial.print(' ');Serial.println(saturation);
  #endif
}

void output_add_color_palette_entry(float hue, float saturation){
    #ifdef TRACE_ON
    Serial.print(F("TRACE_ON> MOCK:output_add_color_palette_entry:"));Serial.print(hue);Serial.print(' ');Serial.println(saturation);
  #endif
}


