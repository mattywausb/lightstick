#include "output.h"

int g_beats_per_minute=120;
int g_output_waittime[6];
int g_pattern_step_wait_index;
long g_pattern_previous_step_time;

long output_beat_sync_time=0L;
int output_preset_beat_count=0;
int output_preset_beat_start_beat=0;

/*
 * 
 *      ATTENTION: Use pin D6 (GPIO12) for LED Data
 * 
 * 
 */

void output_setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  #ifdef TRACE_ON
  Serial.println(F(">output_setup complete "));
  #endif
}

// Change bpm
void output_set_bpm(int beats_per_minute) {
  g_beats_per_minute = beats_per_minute;
  output_set_waittimes(60000/g_beats_per_minute);
  output_sync_beat();
};

int output_get_bpm() { return g_beats_per_minute;}

void output_set_waittimes(int time_for_beat) {
  g_output_waittime[STEP_ON_2BEATS] =time_for_beat*2;
  g_output_waittime[STEP_ON_BEAT] = time_for_beat;
  g_output_waittime[STEP_ON_8TH] = g_output_waittime[STEP_ON_BEAT] /2;
  g_output_waittime[STEP_ON_16TH] = g_output_waittime[STEP_ON_8TH] /2;
  g_output_waittime[STEP_ON_32TH] = g_output_waittime[STEP_ON_16TH] /2;
  g_output_waittime[STEP_ON_64TH] = g_output_waittime[STEP_ON_32TH] /2;
  #ifdef TRACE_OUTPUT_TIMING
    Serial.print(F(">TRACE_OUTPUT_TIMING calculated BPM ")); Serial.println(60000/time_for_beat);
    Serial.print(F(">TRACE_OUTPUT_TIMING STEP_ON_BEAT ")); Serial.println(g_output_waittime[STEP_ON_BEAT]);
    Serial.print(F(">TRACE_OUTPUT_TIMING STEP_ON_32TH ")); Serial.println(g_output_waittime[STEP_ON_32TH]);
    Serial.print(F(">TRACE_OUTPUT_TIMING STEP_ON_64TH ")); Serial.println(g_output_waittime[STEP_ON_64TH]);
  #endif
}

int output_get_beat_number_since_sync() {
  long runtime_since_sync=millis()-output_beat_sync_time;
  return runtime_since_sync/g_output_waittime[STEP_ON_BEAT];
}

long output_get_current_beat_start_time()  {
   return output_get_beat_number_since_sync()*g_output_waittime[STEP_ON_BEAT]; 
}

int output_get_preset_beat_count() {
  return output_preset_beat_count;
}

void output_sync_beat() {
  output_beat_sync_time=millis();
  g_pattern_previous_step_time=output_get_current_beat_start_time();
}

// Change Pattern speed (relative to bpm)
void output_set_pattern_speed(int wait_index)
{
  if(wait_index>=STEP_ON_2BEATS && wait_index<=STEP_ON_64TH) g_pattern_step_wait_index=wait_index;
}

// Select and start a pattern
void output_start_preset(int preset_id) {
  /* MOCKUP */
}

void output_set_demo_led(boolean value) {
  digitalWrite(LED_BUILTIN, value);
}

// Following function must be called in the loop
void output_process_pattern() {
  /* MOCKUP */

  
  if(output_preset_beat_start_beat==output_get_beat_number_since_sync())  return;
  #ifdef TRACE_OUTPUT_PATTERN_BEAT
    Serial.print(F(">TRACE_OUTPUT_PATTERN_BEAT Beat = ")); Serial.println(output_preset_beat_count);
  #endif   output_preset_beat_start_beat=output_get_beat_number_since_sync();
  //digitalWrite(LED_BUILTIN, output_preset_beat_count%2);
  output_preset_beat_count++;
}


