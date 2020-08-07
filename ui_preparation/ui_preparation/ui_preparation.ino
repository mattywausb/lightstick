/* Compile against  NodeMCU 1.0 Profile */

/* Main controller Module
 *  
 *  Orchestrates all Elements 
 *  transforming input signals from Buttons, Web IU and Serial Input into Actions and Mode changes
 *  manage output processing
 *  
 */

#include "mainSettings.h"
#include "webui.h"
#include "output.h"

enum MODE_OF_OPERATION {
  MODE_SEQUENCE,
  MODE_SEQUENCE_HOLD,
  MODE_FIX_PRESET,
};

MODE_OF_OPERATION mode_of_operation=MODE_SEQUENCE;

void setup(void)
{
  Serial.begin(115200);
  char compile_signature[] = "\n\n\n--- START (Build: " __DATE__ " " __TIME__ ") ---";   
  Serial.println(compile_signature);

  webui_setup();
  output_setup();
  input_setup();
  
  output_set_bpm(120);
  #ifdef TRACE_ON
    Serial.println(F(">setup complete "));
  #endif
}


void loop(void)
{
  webui_loop();
}

/*******************  Mockups **************************/

void parse_slot_settings(String slot_setting_string)  {
  #ifdef TRACE_ON
    Serial.print(F("TRACE_ON> MOCK:parse_slot_settings:"));Serial.println(slot_setting_string);
  #endif
}

void parse_sequence(String sequence_string)  {
    #ifdef TRACE_ON
    Serial.print(F("TRACE_ON> MOCK parse_sequence:"));Serial.println(sequence_string);
  #endif
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
