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

typedef struct {
    byte preset_id;       // 0-255
    byte preset_speed;       // 0-255
    byte beats_to_run;       // 0-255
} t_sequence_entry;

t_sequence_entry g_preset_sequence[MAX_NUMBER_OF_PRESETS_IN_SEQUENCE] = {
                               {1, 4,16}
                              ,{1, 8,16}
                              ,{7, 4, 8}
                              ,{3,16, 8}
                              ,{6,16,32}
                              ,{7, 4, 8}
                              ,{3,16, 4}
                              ,{8,32, 4}
                              };

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

