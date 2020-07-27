// This is code creates small light show, using neo pixel


#include <Adafruit_NeoPixel.h>
#include "lamp.h"
#include "output.h"
#include "mainSettings.h"


#ifdef TRACE_ON
#define TRACE_SEQUENCE_PROGRESS
#define TRACE_BUTTON_INPUT
#define TRACE_STRING_PARSING
#endif

typedef struct {
    char slot_tag; // 
    byte preset_id;       // 0-255
    byte preset_speed_id;       // 0-255
} t_program_slot;

t_program_slot g_program_slot[MAX_NUMBER_OF_PROGRAM_SLOTS]={ /* IDOL 126 BPM*/
                               {'R',3,STEP_ON_16TH}
                              ,{'A',5,STEP_ON_8TH}
                              ,{'B',1,STEP_ON_BEAT}
                              ,{'S',5,STEP_ON_16TH}
                              ,{'I',0,STEP_ON_BEAT}
                              ,{PROGRAM_TAG_OFF,0,STEP_ON_8TH}
                              ,{PROGRAM_TAG_OFF,0,STEP_ON_8TH}
                              ,{PROGRAM_TAG_OFF,0,STEP_ON_8TH}
                              };
                  

typedef struct {
    byte slot_index; // 
    int beats_to_run;       // 0-255
} t_program_sequence;


int g_program_sequence_loop_entry_index=1;
int g_program_sequence_length=5;


t_program_sequence g_program_sequence [MAX_NUMBER_OF_PROGRAM_SEQUENCE_STEPS] = {
                      {5,16},{1,64},{2,32},{0,32},{3,32}
                      }; // Extended Version to S126 I88 >A8888 8888 B8888 R8888 S8888 A8888 8888 B8888 R8888 S8888 B8888 R8888 S8888 A8888 

// Other test: S80 A22 B4 >C8 D4                     
                        
int g_sequence_entry_count=8;
int g_sequence_index=0;

long g_music_start_time=0;
int g_preset_selected = 7;

int g_tap_track_interval[3];
int g_tap_track_count=0;
int g_tap_track_index=0;
int g_tap_track_sum=0;
long g_tap_track_prev_time=0L;


enum MODE_OF_OPERATION {
  MODE_SEQUENCE,
  MODE_FIX_PRESET
};

MODE_OF_OPERATION mode_of_operation=MODE_SEQUENCE;

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  input_setup();
  output_setup();
  output_set_bpm(126);
  sequence_start();
}

/* Tranlates input over serial into step speed index */
void change_waittime_for_input(int value) {
        switch(value) {
        case 2: output_set_pattern_speed(STEP_ON_2BEATS); break;
        case 4: output_set_pattern_speed(STEP_ON_BEAT); break;
        case 8: output_set_pattern_speed(STEP_ON_8TH); break;
        case 16:  output_set_pattern_speed(STEP_ON_16TH); break;
        case 32:  output_set_pattern_speed(STEP_ON_32RD); break;
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
    #ifdef TRACE_BUTTON_INPUT
            Serial.print(F(">TRACE_BUTTON_INPUT Tap prev check failed by ")); Serial.println(abs(g_tap_track_interval[g_tap_track_index]-prev_tap_track_interval));
    #endif
  } else g_tap_track_count++;
  
 
  if(g_tap_track_count>=3) { // We have collected 3 valid lenght
    g_tap_track_count=3;
    int average_tap_lenght=g_tap_track_sum/g_tap_track_count;
     for (int i=0; i<3;i++) {
        if(abs(average_tap_lenght-g_tap_track_interval[i])>300) { // Reset on deviation more then 150 ms to average
                 g_tap_track_count=1;
                 g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
                   #ifdef TRACE_BUTTON_INPUT
                      Serial.print(F(">TRACE_BUTTON_INPUT Tap avg check failed by ")); Serial.println(abs(average_tap_lenght-g_tap_track_interval[i]));
                   #endif
                 return; // Get  out in case of fail
        }
     }
     output_set_bpm(60000/average_tap_lenght);
     g_tap_track_count=1; // Reset Counter and sum to latest measure
     g_tap_track_sum=g_tap_track_interval[g_tap_track_index];
  }
     
}

#ifdef TRACE_SEQUENCE_PROGRESS
void trace_sequence()
{
  Serial.print(F(">TRACE_SEQUENCE_PROGRESS index:")); Serial.print(g_sequence_index);
  Serial.print(F(" preset_id: ")); Serial.print(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_id);
  Serial.print(F(" preset_speed_id: ")); Serial.print(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_speed_id);
  Serial.print(F(" beats_to_run: ")); Serial.println(g_program_sequence[g_sequence_index].beats_to_run);
}
#endif

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
  int char_index=0;
  int sequence_index=0;
  int beat_sum=0;
  int slot_index=-1;
  g_program_sequence_loop_entry_index=0;

  #ifdef TRACE_STRING_PARSING
         Serial.print(F(">TRACE_STRING_PARSING string to parse:"));Serial.println(sequence_string);
  #endif
  
  for(char_index=0;char_index<sequence_string.length();char_index++) {
    if(sequence_string.charAt(char_index)>='A') { // new slot letter in string
      if(beat_sum>0 && slot_index>=0) { // previous phrase is complete and valid
        g_program_sequence[sequence_index].slot_index=slot_index;
        g_program_sequence[sequence_index].beats_to_run=beat_sum;
        #ifdef TRACE_STRING_PARSING
         Serial.print(F(">TRACE_STRING_PARSING sequence_index:"));Serial.print(sequence_index);
         Serial.print(F(" slot_index:"));Serial.print(slot_index);
         Serial.print(F(" beat_sum:"));Serial.println(beat_sum);
        #endif
        sequence_index++;
      }
      beat_sum=0;
      for(slot_index=MAX_NUMBER_OF_PROGRAM_SLOTS-1;slot_index>=0;slot_index--) { // find proper slot
        if(sequence_string.charAt(char_index)==g_program_slot[slot_index].slot_tag) break;
      }
      #ifdef TRACE_STRING_PARSING
         Serial.print(F(">TRACE_STRING_PARSING slot_index:"));Serial.println(slot_index);
      #endif
    } else 
    if(sequence_string.charAt(char_index)>='1' && sequence_string.charAt(char_index)<='9') { // beat digit
      beat_sum+=sequence_string.substring(char_index,char_index+1).toInt();
    } else
    if(sequence_string.charAt(char_index)>='>') {  // Loop marker
      g_program_sequence_loop_entry_index=sequence_index+1;
      #ifdef TRACE_STRING_PARSING
         Serial.print(F(">TRACE_STRING_PARSING loop entry at:"));Serial.println(g_program_sequence_loop_entry_index);
      #endif
    }
  } // end loop over characters of the string
  if(beat_sum>0 || slot_index>=0) { // previous phrase is complete and valid
        g_program_sequence[sequence_index].slot_index=slot_index;
        g_program_sequence[sequence_index].beats_to_run=beat_sum;
         #ifdef TRACE_STRING_PARSING
         Serial.print(F(">TRACE_STRING_PARSING sequence_index:"));Serial.print(sequence_index);
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

void sequence_start()
{
  g_sequence_index=0;
  output_set_pattern_speed(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_speed_id);
  output_start_preset(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_id);
  #ifdef TRACE_SEQUENCE_PROGRESS
    trace_sequence();
  #endif
}

void sequence_next_slot()
{
    if(++g_sequence_index>=g_program_sequence_length)g_sequence_index=g_program_sequence_loop_entry_index;
    output_set_pattern_speed(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_speed_id);
    output_start_preset(g_program_slot[g_program_sequence[g_sequence_index].slot_index].preset_id);   
    #ifdef TRACE_SEQUENCE_PROGRESS
      trace_sequence();
    #endif
}



void loop() {
  // Manage Button Press
  input_switches_scan_tick();
  if (input_stepGotPressed()) {
      #ifdef TRACE_BUTTON_INPUT
        Serial.println(F(">TRACE_BUTTON_INPUT step got pressed"));
      #endif
      output_sync_beat();
      manage_tap_input();
  }

  // Manage Serial input
  input_pollSerial();
  if(input_newSerialCommandAvailable()) {
    String command=input_getSerialCommand();
    if(command.startsWith("S")) { //Change sequence by string
      parse_sequence(command.substring(1));
      mode_of_operation=MODE_SEQUENCE;
    }
    if(command.startsWith("s")) {  // restart sequence mode
      mode_of_operation=MODE_SEQUENCE;
      sequence_start();
    }
    if(command.startsWith("b")) {  // b123 <- Change bpm: 
      int bpm=command.substring(1).toInt();
      if(bpm>0) output_set_bpm(bpm);
    }
    if(command.startsWith("w")) {  // w16 <- Change waittime
      int wait_value=command.substring(1).toInt();
      change_waittime_for_input(wait_value);
      mode_of_operation=MODE_FIX_PRESET;
    }
    if(command.startsWith("p")) { //Change preset
      int value=command.substring(1).toInt();
      output_start_preset(value);
      mode_of_operation=MODE_FIX_PRESET;
    }
    int position_of_dash=command.indexOf('-');  
    if(position_of_dash>0) { //Change preset and waittime
      int preset_id=command.toInt();
      int wait_value=command.substring(position_of_dash+1).toInt();
       change_waittime_for_input(wait_value); 
       output_start_preset(preset_id);
        mode_of_operation=MODE_FIX_PRESET;
    }
  }

  // Manage current sequence 
  if(mode_of_operation==MODE_SEQUENCE) {
    if(output_get_preset_beat_count()>=g_program_sequence[g_sequence_index].beats_to_run) {
      sequence_next_slot();
    }
  }
  
  output_process_pattern();
}



