/* Compiles against  Arduino UNO */

/* Main controller Module
 *  
 *  Orchestrates all Elements 
 *  transforming input signals from Buttons, Web IU and Serial Input into Actions and Mode changes
 *  manage output processing
 *  
 */

#include <Adafruit_NeoPixel.h>
#include "lamp.h"
#include "output.h"
#include "mainSettings.h"



#ifdef TRACE_ON
#define TR_SEQUENCE_PROGRESS
//#define TR_STRING_PARSING
//#define TR_WATCH_MODE_CHANGE
#define TR_BUTTON_INPUT

#endif

typedef struct {
    char slot_tag; // 
    byte pattern_id;       // 0-255
    byte preset_speed_id;       // 0-255
    byte color_palette_id;
} t_program_slot;

t_program_slot g_program_slot[MAX_NUMBER_OF_PROGRAM_SLOTS]={ /* IDOL 126 BPM*/
                              {'I',97,STEP_ON_BEAT,0}
                              ,{'A',25,STEP_ON_8TH,0}
                              ,{'B',11,STEP_ON_BEAT,41}
                              ,{'R',10,STEP_ON_16TH,1}
                              ,{'S',97,STEP_ON_16TH,0}
                              ,{PROGRAM_TAG_OFF,0,0,STEP_ON_8TH}
                              ,{PROGRAM_TAG_OFF,0,0,STEP_ON_8TH}
                              ,{PROGRAM_TAG_OFF,0,0,STEP_ON_8TH}
                              }; // String o  I98/4:0  A25/8:0 B11/8:41  R10/16:1 S98/8:0    
      //  Test String o A6/4:20 B10/8:40

int g_program_slot_count=0;                  

typedef struct {
    byte slot_index; // 
    int beats_to_run;       // 0-255
} t_program_sequence;


int g_program_sequence_loop_entry_index=1;
int g_program_sequence_hold_entry_index=0;
int g_program_sequence_length=5;


t_program_sequence g_program_sequence [MAX_NUMBER_OF_PROGRAM_SEQUENCE_STEPS] = {
                      {0,16},{1,64},{2,32},{3,32},{4,32}
                      }; // Full Version: s 126 I88# >A8888 8888 B8888 R8888 S8888 A8888 8888 B8888 R8888 S8888 B8888 R8888 S8888 A8888 

// Pattern Slots: o A6/4:20 B10/4:40 C11/8:40 D65/4:40
// Sequence setting: s80 A1# B44 >C8 D8                     
                        
int g_sequence_entry_count=8;
int g_sequence_index=0;

long g_music_start_time=0;
int g_preset_selected = 7;

int g_tap_track_interval[3];
int g_tap_track_count=0;
int g_tap_track_index=0;
int g_tap_track_sum=0;
unsigned long g_tap_track_prev_time=0L;


enum MODE_OF_OPERATION {
  MODE_SEQUENCE,
  MODE_SEQUENCE_HOLD,
  MODE_FIX_PRESET,
};

MODE_OF_OPERATION mode_of_operation=MODE_SEQUENCE;
#ifdef TR_WATCH_MODE_CHANGE
MODE_OF_OPERATION prev_mode_of_operation=MODE_SEQUENCE;
#endif


void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(115200);
    Serial.println(compile_signature); 
  #endif
  output_setup();
  output_set_bpm(80);
  output_reset_color_palette(HUE_ORANGE,1);
  output_start_pattern(9);  // Heartbeat with low light
  webui_setup();
  input_setup();
  output_reset_color_palette(HUE_LEMON,1);
  mode_of_operation=MODE_FIX_PRESET;
}


/* convert pattern_speed to pattern_speed_id */
int preset_speed_to_id(String pattern_speed) {
    #ifdef TR_STRING_PARSING
 //        Serial.print(F(">TR_STRING_PARSING preset_speed_to_id:"));Serial.println(pattern_speed);
  #endif
      switch(pattern_speed.toInt()) {
        case 2: return STEP_ON_2BEATS; 
        case 4: return STEP_ON_BEAT;
        case 8: return STEP_ON_8TH;
        case 16:  return STEP_ON_16TH;
        case 32:  return STEP_ON_32RD;
        case 64:  return STEP_ON_64TH;
      }
      return -1;
}

void manage_tap_input() {
  int prev_tap_track_interval=g_tap_track_interval[g_tap_track_index];

  if(++g_tap_track_index>=3) g_tap_track_index=0;  // Foreward track array index

  // Determine tap interval
  g_tap_track_interval[g_tap_track_index]=input_getLastPressStartTime()-g_tap_track_prev_time;
  g_tap_track_prev_time=input_getLastPressStartTime();
  g_tap_track_sum+=g_tap_track_interval[g_tap_track_index];

    #ifdef TR_BUTTON_INPUT
            Serial.print(F(">TR_BUTTON_INPUT tap interval: ")); Serial.println(g_tap_track_interval[g_tap_track_index]);
    #endif
  
  if(abs(g_tap_track_interval[g_tap_track_index]-prev_tap_track_interval)>300) {  // no valid lenght compared to previous
    g_tap_track_count=1; // Reset Counter and sum to latest measure
    g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
    #ifdef TR_BUTTON_INPUT
            Serial.print(F(">TR_BUTTON_INPUT Tap prev check failed by ")); Serial.println(abs(g_tap_track_interval[g_tap_track_index]-prev_tap_track_interval));
    #endif
  } else g_tap_track_count++;
  
 
  if(g_tap_track_count>=3) { // We have collected 3 valid lenght
    g_tap_track_count=3;
    int average_tap_lenght=g_tap_track_sum/g_tap_track_count;
     for (int i=0; i<3;i++) {
        if(abs(average_tap_lenght-g_tap_track_interval[i])>300) { // Reset on deviation more then 150 ms to average
                 g_tap_track_count=1;
                 g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
                   #ifdef TR_BUTTON_INPUT
                      Serial.print(F(">TR_BUTTON_INPUT Tap avg check failed by ")); Serial.println(abs(average_tap_lenght-g_tap_track_interval[i]));
                   #endif
                 return; // Get  out in case of fail
        }
     }
     output_set_bpm(60000/average_tap_lenght);
     g_tap_track_count=1; // Reset Counter and sum to latest measure
     g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
  }
     
}

#ifdef TR_SEQUENCE_PROGRESS
void TR_sequence()
{
  Serial.print(F("TR_SEQUENCE_PROGRESS>")); Serial.print(g_sequence_index);
  Serial.print('='); Serial.print(g_program_slot[g_program_sequence[g_sequence_index].slot_index].slot_tag);
  Serial.print(g_program_slot[g_program_sequence[g_sequence_index].slot_index].pattern_id);
  Serial.print('/'); Serial.print(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_speed_id);
  Serial.print(':'); Serial.print(g_program_slot[g_program_sequence[g_sequence_index].slot_index].color_palette_id);
  Serial.print('*'); Serial.print(g_program_sequence[g_sequence_index].beats_to_run);
  Serial.print(F(" mode:"));Serial.println(mode_of_operation);

}
#endif

/* Function to load the slot settings  by parsing a string code 
* Syntax: <letter><pattern_id>/<speed>:<color palette id> ...
* <pattern_id>: declares the pattern id (0-99)
* <speed>: declares the pattern speed as 2 4 8 16 32 64 
* <color palette id>: declares the color palette id (0-255)
*/


void parse_slot_settings(String slot_setting_string)  {
  int slot_index=0;
  int pattern_id=-1;
  int preset_speed_id=-1;
  int color_palette_id;
  int slash_index;
  int colon_index;
  int token_end_index;
  String current_token;
  #ifdef TR_STRING_PARSING
         Serial.print(F(">TR_STRING_PARSING slot settings to parse:"));Serial.println(slot_setting_string);
  #endif
  for(int char_index=0;char_index<slot_setting_string.length();char_index++) {
    if(slot_setting_string.charAt(char_index)>='A') { // new slot letter in string
        for (token_end_index=char_index+1; token_end_index<slot_setting_string.length() && slot_setting_string.charAt(token_end_index)<'A';token_end_index++); // search for end of token
        current_token=slot_setting_string.substring(char_index,token_end_index);
         #ifdef TR_STRING_PARSING
            Serial.print(F(">TR_STRING_PARSING current_token:"));Serial.println(current_token);
          #endif
        slash_index=current_token.indexOf('/');
        if (slash_index>0) { // dash to separate the speed exists
          colon_index=current_token.indexOf(':',slash_index);
          if(colon_index>0) {
            pattern_id=current_token.substring(1).toInt();
            preset_speed_id=preset_speed_to_id(current_token.substring(slash_index+1));
            color_palette_id=current_token.substring(colon_index+1).toInt();
            if(pattern_id>=0 && preset_speed_id>=0 && color_palette_id>=0) {  // Parameters are valid, so store it in array
              g_program_slot[slot_index].slot_tag=slot_setting_string.charAt(char_index);
              g_program_slot[slot_index].pattern_id=pattern_id;
              g_program_slot[slot_index].preset_speed_id=preset_speed_id;
              g_program_slot[slot_index].color_palette_id=color_palette_id;
              #ifdef TR_STRING_PARSING
                Serial.print(F(">TR_STRING_PARSING slot setting:"));
                Serial.print(slot_index);Serial.print("{'");Serial.print(g_program_slot[slot_index].slot_tag);
                Serial.print("',");Serial.print(g_program_slot[slot_index].pattern_id);
                Serial.print(',');Serial.print(g_program_slot[slot_index].preset_speed_id);
                Serial.print(',');Serial.print(g_program_slot[slot_index].color_palette_id);
                Serial.println("},");
              #endif
              pattern_id=-1;
              preset_speed_id=-1;
              if(++slot_index>=MAX_NUMBER_OF_PROGRAM_SLOTS) {
               #ifdef TR_WARNING
                    Serial.print(F("#!# TR_WARNING> max slots reached"));Serial.println(slot_index);
               #endif                    
                break;
              }
            }
          } // end colon found
         #ifdef TR_STRING_PARSING
             else {
              Serial.print(F(">TR_STRING_PARSING bad parsing slash_index, pattern_id, preset_speed_id:"));
              Serial.print(slash_index);Serial.print(',');
              Serial.print(pattern_id);Serial.print(',');
              Serial.println(preset_speed_id);
             }
         #endif
        } // end Slash found
    }  // end letter found
  } // end for char_index
  g_program_slot_count=slot_index;
}

/* Function to load the program sequence by parsing a string code 
* Syntax: <bpmdigit>{2..3}<letter>{1}<beatdigit>{1..n}
* <bpmdigit>: declars the BPM Speed
* <letter>: declares the slot_tag that is addressed 
* <beatdigit>: declares number of beats to hold the slot, consecutive digits will be added (e.g. 248=14)
* Special characters: > Mark Sequence entry to loop back (last entry counts)
*/

void parse_sequence(String sequence_string)  
{
  int newBPM=sequence_string.toInt();
  int sequence_index=0;
  int beat_sum=0;
  int slot_index=-1;
  #ifdef TR_STRING_PARSING
         Serial.print(F(">TR_STRING_PARSING sequence to parse:"));Serial.println(sequence_string);
  #endif
  g_program_sequence_hold_entry_index=-1;
  g_program_sequence_loop_entry_index=0;
  for(int char_index=0;char_index<sequence_string.length();char_index++) {
    if(sequence_string.charAt(char_index)>='A') { // new slot letter in string
      if(beat_sum>0 && slot_index>=0) { // previous phrase is complete and valid
        g_program_sequence[sequence_index].slot_index=slot_index;
        g_program_sequence[sequence_index].beats_to_run=beat_sum;
        #ifdef TR_STRING_PARSING
         Serial.print(F(">TR_STRING_PARSING sequence_index:"));Serial.print(sequence_index);
         Serial.print(F(" slot_index:"));Serial.print(slot_index);
         Serial.print(F(" beat_sum:"));Serial.println(beat_sum);
        #endif
        sequence_index++;
      }
      beat_sum=0;
      for(slot_index=g_program_slot_count-1;slot_index>=0;slot_index--) { // find proper slot
        if(sequence_string.charAt(char_index)==g_program_slot[slot_index].slot_tag) break;
      }
      #ifdef TR_STRING_PARSING
         Serial.print(F(">TR_STRING_PARSING slot_index:"));Serial.println(slot_index);
      #endif
    } // end if letter found 
    else 
    if(sequence_string.charAt(char_index)>='1' && sequence_string.charAt(char_index)<='9') { // beat digit
      beat_sum+=sequence_string.substring(char_index,char_index+1).toInt();
    } else
    if(sequence_string.charAt(char_index)=='>') {  // Loop marker
      g_program_sequence_loop_entry_index=sequence_index+1;
      #ifdef TR_STRING_PARSING
         Serial.print(F(">TR_STRING_PARSING loop entry at:"));Serial.println(g_program_sequence_loop_entry_index);
      #endif
    } else
    if(sequence_string.charAt(char_index)=='#') {  // Hold marker
      g_program_sequence_hold_entry_index=sequence_index;
      #ifdef TR_STRING_PARSING
         Serial.print(F(">TR_STRING_PARSING hold entry at:"));Serial.println(g_program_sequence_loop_entry_index);
      #endif
    }
    
  } // end loop over characters of the string
  if(beat_sum>0 || slot_index>=0) { // previous phrase is complete and valid
        g_program_sequence[sequence_index].slot_index=slot_index;
        g_program_sequence[sequence_index].beats_to_run=beat_sum;
         #ifdef TR_STRING_PARSING
         Serial.print(F(">TR_STRING_PARSING sequence_index:"));Serial.print(sequence_index);
         Serial.print(F(" slot_index:"));Serial.print(slot_index);
         Serial.print(F(" beat_sum:"));Serial.println(beat_sum);
        #endif
        sequence_index++;
      }
  g_program_sequence_length=sequence_index;
  if(g_program_sequence_loop_entry_index>=g_program_sequence_length)g_program_sequence_loop_entry_index=0;  // in case loop entry tag is placed after last valid block
  
  if(newBPM>0) output_set_bpm(newBPM);
  sequence_start();
}

void sequence_load() {
  #ifdef TR_SEQUENCE_PROGRESS
    TR_sequence();
  #endif
  output_set_pattern_speed(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_speed_id);
  output_load_color_palette(g_program_slot[g_program_sequence[g_sequence_index].slot_index].color_palette_id);
  output_start_pattern(g_program_slot[g_program_sequence[g_sequence_index].slot_index].pattern_id);
  if(g_sequence_index==g_program_sequence_hold_entry_index){
    mode_of_operation=MODE_SEQUENCE_HOLD;
 //   #ifdef TR_SEQUENCE_PROGRESS
 //       Serial.println(F(">TR_SEQUENCE_PROGRESS Hold index reached"));
 //   #endif        
  }
  else mode_of_operation=MODE_SEQUENCE;
}

void sequence_start()
{
  #ifdef TR_SEQUENCE_PROGRESS
        Serial.println(F(">TR_SEQUENCE_PROGRESS sequence_start"));
  #endif
  g_sequence_index=0;
  sequence_load();
}

void sequence_next_slot()
{
   #ifdef TR_SEQUENCE_PROGRESS
        Serial.println(F(">TR_SEQUENCE_PROGRESS sequence_next_slot"));
   #endif
   if(++g_sequence_index>=g_program_sequence_length)g_sequence_index=g_program_sequence_loop_entry_index;
   sequence_load();
}



void loop() {
  // Check for input from webui
  webui_loop();
  
  // Manage Button Press
  input_switches_scan_tick();
  if (input_stepGotPressed()) {
      #ifdef TR_BUTTON_INPUT
        Serial.println(F(">TR_BUTTON_INPUT step got pressed"));
      #endif
      output_sync_beat(input_getLastPressStartTime());
      if(mode_of_operation==MODE_SEQUENCE_HOLD) sequence_next_slot();
      manage_tap_input();
  }

  // Manage Serial input
  /*
   * Commands:
   * #<id>               load song preset
   * o <slot settings>   load the slot settings from string syntax
   * s <sequence string> load the sequence from string syntax
   * +                   restart sequence
   * 
   * b <bpm>             set bpm
   * w {2,4,8,16,32,64}  set step waittime
   * 
   * p <pattern id>      start predefined pattern per id (0 Pulse, 10 Whipe, 
   * <id>/{2,4,8,16,32,64}  start predefined pattern per id with waittime
   * 
   * l <pallette id>     load color palette
   * C <hue>,<sat>       reset color palette and set first color
   * c [<i>:]<hue>,<sat> add color to palette or change color on palette index <i>
   * t <id>:<p1>,<p2>    start free pattern: 
   *                     0=PULSE <steps_until_next_color>,<follow_up_tick_count>
   *                     1=WHIPE <over_black>
   *                     2=DOUBLE ORBIT <steps_until_next_color>
   *                     3=COLOR ORBIT <steps_until_next_color>,<color_palette_increment>
   *                     6=RAINBOW <distance>, <increment>
   */
  input_pollSerial();
  if(input_newSerialCommandAvailable()) {
    String command=input_getSerialCommand();
    if(command.startsWith("x")) { //Load sequence from Library
      song_preset_start(command.substring(1).toInt());
    }
    if(command.startsWith("s")) { //Change sequence by string
      parse_sequence(command.substring(1));
    }
    if(command.startsWith("+")) {  // restart sequence mode
      sequence_start();
    }
    if(command.startsWith("b")) {  // b123 <- Change bpm: 
      int bpm=command.substring(1).toInt();
      if(bpm>0) output_set_bpm(bpm);
    }

    if(command.startsWith("w")) {  // w16 <- Change waittime directly
      int preset_speed_id=preset_speed_to_id(command.substring(1));
      if(preset_speed_id>=0)output_set_pattern_speed(preset_speed_id);
      mode_of_operation=MODE_FIX_PRESET;
    }

    if(command.startsWith("l")) { //load color pallette
      int value=command.substring(1).toInt();
      output_load_color_palette(value);
      mode_of_operation=MODE_FIX_PRESET;
    }
    
    if(command.startsWith("p")) { //Change pattern directly
      int value=command.substring(1).toInt();
      output_start_pattern(value);
      mode_of_operation=MODE_FIX_PRESET;
    }
    if(command.startsWith("o")) { // Change all preset slots by string 
      parse_slot_settings(command.substring(1));
      output_set_pattern_speed(g_program_slot[0].preset_speed_id);
      output_load_color_palette(g_program_slot[0].color_palette_id);
      output_start_pattern(g_program_slot[0].pattern_id);      
      mode_of_operation=MODE_FIX_PRESET;
    } else {
      int position_of_slash=command.indexOf('/');  
      if(position_of_slash>0) { //Change preset and waittime directly 3-16
        int pattern_id=command.toInt();
        int preset_speed_id=preset_speed_to_id(command.substring(position_of_slash+1));
        if(preset_speed_id>=0)output_set_pattern_speed(preset_speed_id);
        output_start_pattern(pattern_id);
        mode_of_operation=MODE_FIX_PRESET;
      }
    } 
    if(command.startsWith("c")) { // Color entry "index: Hue, Saturation"
      int position_of_comma=command.indexOf(',');
      if(position_of_comma>0) {
        float saturation=command.substring(position_of_comma+1).toFloat();
        int position_of_colon=command.indexOf(':');
        int palette_entry_index=-1;
        float hue=0.0;
        if(position_of_colon>0) {
          palette_entry_index=command.substring(1).toInt();
          hue=command.substring(position_of_colon+1).toFloat();
          output_set_color_palette_entry(palette_entry_index,hue,saturation);
          mode_of_operation=MODE_FIX_PRESET;
        } else {
          hue=command.substring(1).toFloat();
          output_add_color_palette_entry(hue,saturation);
          mode_of_operation=MODE_FIX_PRESET;
        }
      }
    }
    if(command.startsWith("C")) { // Color palette reset to  "Hue, Saturation"
      int position_of_comma=command.indexOf(',');
      if(position_of_comma>0) {
        float saturation=command.substring(position_of_comma+1).toFloat();
        float hue=command.substring(1).toFloat();
        output_reset_color_palette(hue,saturation);
        mode_of_operation=MODE_FIX_PRESET;
      }
    }
    if(command.startsWith("t")) { // start pattern manually
      int position_of_colon=command.indexOf(':');
      if(position_of_colon>0) {
        mode_of_operation=MODE_FIX_PRESET;
        int pattern=command.substring(1).toInt();
        int position_of_comma=command.indexOf(',');
        switch(pattern){
          case 0:  // PULSE <steps_until_next_color>,<follow_up_tick_count>
                  if(position_of_comma>0) {
                  int steps_until_next_color=command.substring(position_of_colon+1).toInt();
                  int follow_up_tick_count=command.substring(position_of_comma+1).toInt();
                  start_pulse(0.8, // brightness 
                        steps_until_next_color,  // Steps until color increment
                        0.87, // preserve brightnes factor 
                        follow_up_tick_count  );
                  }
                  break;
          case 1: // WHIPE <over_black>
                  {int over_black=command.substring(position_of_colon+1).toInt();
                  start_colorWipe(0.5,over_black);
                  }
                  break;
          case 2: // DOUBLE ORBIT <steps_until_next_color>
                  {int steps_until_next_color=command.substring(position_of_colon+1).toInt();
                  start_doubleOrbit(0.5,steps_until_next_color);
                  }
                  break;
          case 3: // COLOR ORBIT <steps_until_next_color>,<color_palette_increment>
                  if(position_of_comma<0) break;
                  {int steps_until_next_color=command.substring(position_of_colon+1).toInt();
                  int color_palette_increment=command.substring(position_of_comma+1).toInt();
                  start_colorOrbit(0.5,steps_until_next_color,color_palette_increment);
                  }
                  break;
          case 6: // RAINBOW <distance>, <increment>
                  if(position_of_comma<0) break;
                  {float distance_to_neighbour=command.substring(position_of_colon+1).toFloat();
                  float increment=command.substring(position_of_comma+1).toFloat();
                  start_rainbow(0.5,distance_to_neighbour,increment);
                  break;
                  }
            } // end switch
        } // end if colon present
                

      }  // end "t" command

  }  // end Serial command available

  // ------ here starts the final processing 
  
  #ifdef TR_WATCH_MODE_CHANGE
      if(mode_of_operation!=prev_mode_of_operation) {
        Serial.print(F("TR_WATCH_MODE_CHANGE> new mode: "));Serial.println(mode_of_operation);
        prev_mode_of_operation=mode_of_operation;
      }
  #endif
  
  output_determine_beat();
  
  // Manage current sequence 
  if(mode_of_operation==MODE_SEQUENCE) {
    if(output_get_preset_beat_count()>=g_program_sequence[g_sequence_index].beats_to_run) {
      sequence_next_slot();
    }
  }
  
  output_process_pattern();
}



