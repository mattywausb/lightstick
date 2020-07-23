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

void process_webui_command() {
  switch (webui_read_command())
    {
      case SET_BPM: 
              int new_bpm=webui_data_get_bpm();
              output_set_demo_led(new_bpm%2);
              output_set_bpm(new_bpm);
              break;
    }
}

void loop(void)
{
  webui_loop();
}
