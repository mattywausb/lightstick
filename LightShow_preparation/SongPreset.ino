/* This module keeps all song data, that is available as ready to run preset in the firmware */

#include "mainSettings.h"

#ifdef TRACE_ON
#define TRACE_PRESET_API_CALL
#endif

// MAX STRING LENGTH                   0--------90--------90--------90--------90--------90--------90--------90--------90--------90--------9100------90--------90--------90--------90--------90--------90--------90--------90--------90--------9200------90--------90--------90--------90--------9-254
const char song_name_0[] PROGMEM ="Cypher 4 (BTS)";
const char song_slots_0[] PROGMEM ="P I0/4:100 K25/4:21 R44/16:22";
const char song_sequence_0[] PROGMEM ="120 I88 8888 K8888 8888 8888 8888 R8888 K8888 8888 8888 8888 8888 R8888 K8888 8888 8888 R8888 8888";


const char song_name_1[] PROGMEM ="DNA (BTS)";
const char song_slots_1[] PROGMEM ="P J23/2:40 A24/4:40  B23/4:40 R10/4:40 S44/8:40 T11/8:41 X1:/4:40";
const char song_sequence_1[] PROGMEM ="130 J8888# A8888 B8888 R8888 S8888 T8888 X4 A8888 8888 R8888 S8888 T8888 X8 A8888 X8 R8888 A8888 ";

#define MAX_DEFINITON_STRING_LENGTH 150

struct catalog {
  const char *song_name;
  const char *song_slot_definition;
  const char *song_sequence_definition;
} const song_catalog[] PROGMEM = {
   {song_name_0,song_slots_0,song_sequence_0}
  ,{song_name_1,song_slots_1,song_sequence_1}
};

int song_catalog_count=2;

void song_preset_start(int preset_id) {
  char string_buffer[MAX_DEFINITON_STRING_LENGTH];
  
  if(preset_id<0 && preset_id>song_catalog_count) return;  // Bail out, if input is bad

  #ifdef TRACE_PRESET_API_CALL
    strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[preset_id].song_name)));
    Serial.print(F(">TRACE_PRESET_API_CALL song_preset_run:"));Serial.println(string_buffer);
  #endif

  strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[preset_id].song_slot_definition)));
  parse_slot_settings(string_buffer);
  strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[preset_id].song_sequence_definition)));
  parse_sequence(string_buffer);
}

