/* This module keeps all song data, that is available as ready to run preset in the firmware */

#include "mainSettings.h"

#ifdef TRACE_ON
#define TRACE_PRESET_API_CALL
#endif

// MAX STRING LENGTH                   0--------90--------90--------90--------90--------90--------90--------90--------90--------90--------9100------90--------90--------90--------90--------90--------90--------90--------90--------90--------9200------90--------90--------90--------90--------9-254
const char song_name_CYPHER4[] PROGMEM ="Cypher 4 (BTS)";
const char song_slots_CYPHER4[] PROGMEM ="I8/4:100 J0/4:100 K25/4:21 R44/16:22";
const char song_sequence_CYPHER4[] PROGMEM ="120 I88# >J888888 K8888 8888 8888 8888 R8887J1 K8888 8888 8888 8888 8888 R8887J1 K8888 8888 8888 R8888 8887J1";

const char song_name_BURNINGUP[] PROGMEM ="Burning Up (BTS)";
const char song_slots_BURNINGUP[] PROGMEM ="I0/4:44 R54/32:130 K35/32:44 Y36/64:44 P10/16:44 Q10/32:44 X0/4:44 L21/16:44 B10/8:44 T54/64:130";
const char song_sequence_BURNINGUP[] PROGMEM ="100 I88# R8888 K88 88 P888Q6X2 R8888 L88 K84Y2X2  P888Q6X2 R8888 B8884Q3X1 T8888 R8888";


const char song_name_DNA[] PROGMEM ="DNA (BTS)";
const char song_slots_DNA[] PROGMEM ="I27/4:40 J22/2:41 A24/4:40  B23/4:40 R10/4:40 S44/8:40 T11/8:41 X1:/4:40";
const char song_sequence_DNA[] PROGMEM ="130 I488 # J88 A8888 B8888 R8888  S8888 T8888  X4 A8888 8888 R8888 S8888 T8888 X8 A8888 X8 R8888 A8888";

const char song_name_ON[] PROGMEM ="ON (BTS)";
const char song_slots_ON[] PROGMEM ="I2/4:100 J23/16:3 A23/4:5 B22/4:5 R23/16:3 S11/16:4 C10/2:3";
const char song_sequence_ON[] PROGMEM ="106 I88 #J881 A8888 B8888 R8888 S8888 B8888 8888 R8888 S8888 C8 C8888 R8888 S8888 S8888";

#define MAX_DEFINITON_STRING_LENGTH 150

struct catalog {
  const char *song_name;
  const char *song_slot_definition;
  const char *song_sequence_definition;
} const song_catalog[] PROGMEM = {
   {song_name_BURNINGUP,song_slots_BURNINGUP,song_sequence_BURNINGUP}
  ,{song_name_CYPHER4,song_slots_CYPHER4,song_sequence_CYPHER4}
  ,{song_name_DNA,song_slots_DNA,song_sequence_DNA}
  ,{song_name_ON,song_slots_ON,song_sequence_ON}
};

int song_catalog_count=4;

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

