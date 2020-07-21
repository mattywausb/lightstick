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

int g_tap_track_interval[3];
int g_tap_track_count=0;
int g_tap_track_index=0;
int g_tap_track_sum=0;
long g_tap_track_prev_time=0L;



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


void change_waittime_for_input(int value) {
        switch(value) {
        case 2: output_set_pattern_speed(STEP_ON_2BEATS); break;
        case 4: output_set_pattern_speed(STEP_ON_BEAT); break;
        case 8: output_set_pattern_speed(STEP_ON_8TH); break;
        case 16:  output_set_pattern_speed(STEP_ON_16TH); break;
        case 32:  output_set_pattern_speed(STEP_ON_32TH); break;
        case 64:  output_set_pattern_speed(STEP_ON_64TH); break;
      }
}

void manage_tap_input() {
  int prev_tap_track_interval=g_tap_track_interval[g_tap_track_index];
  long current_millis=millis();

  if(++g_tap_track_index>=3) g_tap_track_index=0;  // Foreward track array index

  // Determine tap interval
  g_tap_track_interval[g_tap_track_index]=current_millis-g_tap_track_prev_time;
  g_tap_track_prev_time=current_millis;
  g_tap_track_sum+=g_tap_track_interval[g_tap_track_index];
  
  if(abs(g_tap_track_interval[g_tap_track_index]-prev_tap_track_interval)>300) {  // no valid lenght compared to previous
    g_tap_track_count=1; // Reset Counter and sum to latest measure
    g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
    #ifdef TRACE_ON
            Serial.print(F(">TRACE_ON Tap prev check failed by ")); Serial.println(abs(g_tap_track_interval[g_tap_track_index]-prev_tap_track_interval));
    #endif
  } else g_tap_track_count++;
  
 
  if(g_tap_track_count>=3) { // We have collected 3 valid lenght
    g_tap_track_count=3;
    int average_tap_lenght=g_tap_track_sum/g_tap_track_count;
     for (int i=0; i<3;i++) {
        if(abs(average_tap_lenght-g_tap_track_interval[i])>300) { // Reset on deviation more then 150 ms to average
                 g_tap_track_count=1;
                 g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
                   #ifdef TRACE_ON
                      Serial.print(F(">TRACE_ON Tap avg check failed by ")); Serial.println(abs(average_tap_lenght-g_tap_track_interval[i]));
                   #endif
                 return; // Get  out in case of fail
        }
     }
     output_set_waittimes(average_tap_lenght);
     g_tap_track_count=1; // Reset Counter and sum to latest measure
     g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
  }
     
}

void loop() {
  // Manage Button Press
  input_switches_scan_tick();
  if (input_stepGotPressed()) {
      output_sync_beat() ;
      manage_tap_input();
  }

  // Manage Serial input
  if(input_newSerialCommandAvailable()) {
    String command=input_getSerialCommand();
    if(command.startsWith("b")) {  // b123 <- Change bpm: 
      int bpm=command.substring(1).toInt();
      if(bpm>0) output_set_bpm(bpm);
    }
    if(command.startsWith("w")) {  // w16 <- Change waittime
      int wait_value=command.substring(1).toInt();
      change_waittime_for_input(wait_value);
    }
    if(command.startsWith("p")) { //Change preset
      int value=command.substring(1).toInt();
      output_start_preset(value);
    }
    int position_of_dash=command.indexOf('-');  
    if(position_of_dash>0) { //Change preset and waittime
      int preset_id=command.toInt();
      int wait_value=command.substring(position_of_dash+1).toInt();
       change_waittime_for_input(wait_value); 
       output_start_preset(preset_id);
    }
  }
  output_process_pattern();
}



