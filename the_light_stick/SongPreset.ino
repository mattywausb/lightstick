/* This module keeps all song data, that is available as ready to run preset in the firmware */

#include "mainSettings.h"

#ifdef TRACE_ON
#define TRACE_PRESET_API_CALL
#endif

// MAX STRING LENGTH                   0--------90--------90--------90--------90--------90--------90--------90--------90--------90--------9100------90--------90--------90--------90--------90--------90--------90--------90--------90--------9200------90--------90--------90--------90--------9-254
const char song_name_0[] PROGMEM ="Cypher 4 (BTS)";
const char song_slots_0[] PROGMEM ="I0/4:100 K25/4:21 R44/16:22";
const char song_sequence_0[] PROGMEM ="120 I8# I8888 K8888 8888 8888 8888 R8888 K8888 8888 8888 8888 8888 R8888 K8888 8888 8888 R8888 8888";


const char song_name_1[] PROGMEM ="DNA (BTS)";
const char song_slots_1[] PROGMEM =" J23/2:40 A24/4:40  B23/4:40 R10/4:40 S44/8:40 T11/8:41 X1:/4:40";
const char song_sequence_1[] PROGMEM ="130 J8888  # A8888 B8888 R8888  S8888 T8888  X4 A8888 8888 R8888 S8888 T8888 X8 A8888 X8 R8888 A8888";

const char song_name_2[] PROGMEM ="FAKE LOVE (BTS)";
const char song_slots_2[] PROGMEM ="A10/8:23 C10/32:23 L24/16:23 R44/16:23 Z:9/4:23";
const char song_sequence_2[] PROGMEM ="78 A8# R4444 4444 L4444 A4444 4444 C4444 4444 R4444 L4444 4444 A4444 4444 C4444 4444 R4444 A4444 4444 >Z8";

const char song_name_3[] PROGMEM ="IDOL (BTS)";
const char song_slots_3[] PROGMEM ="I44/4:1  A25/8:0 B11/8:41  R10/16:1 S98/8:0";
const char song_sequence_3[] PROGMEM ="126 I8# A8888 8888 B8888 R8888 S8888 A8888 8888 B8888 R8888 S8888 B8888 R8888 S8888";

const char song_name_4[] PROGMEM ="Not Today (BTS)";
const char song_slots_4[] PROGMEM ="I2/4:101 J1/8:101 X:10/4:1 K11/16:2 P62/32:2 R55/16:2";
const char song_sequence_4[] PROGMEM ="110 I8# I84J2X2 >K8888 8888 P8888 R8888 8888 K8888 P8888 R8888 8888 I8884J4 R8888 8888 P8888";

const char song_name_5[] PROGMEM ="Magic Shop (BTS)";
const char song_slots_5[] PROGMEM ="A10/2:42 K25/16:42 R10/8:42 U10/16:42 S10/8:120 B10/32:120 X24/2:42";
const char song_sequence_5[] PROGMEM ="75 A4444# A4444 K4444 4444 R4444 44 U44 S4444 4444 A4444 K4444 4444 R4444 44U44 S4444 4444 R4444 444B4 S4444 R4444 4444 X4444";

const char song_name_6[] PROGMEM ="Dope (BTS)";
const char song_slots_6[] PROGMEM ="I0/2:102 L1/4:102 K44/16:43 P63/8:0 Q68/8:0";
const char song_sequence_6[] PROGMEM ="154 I8888# I8888 K8888 8888 8888 P8888 88Q88 K8888 8888 L8888 K4L4K4L4K8L8 P8888 88Q88 K8888 8888 P8888 88Q88 K8888 8";

const char song_name_7[] PROGMEM ="N.O (BTS)";
const char song_slots_7[] PROGMEM ="TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD";
const char song_sequence_7[] PROGMEM ="TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD ";

const char song_name_8[] PROGMEM ="ON (BTS)";
const char song_slots_8[] PROGMEM ="TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD";
const char song_sequence_8[] PROGMEM ="TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD TBD ";

const char song_name_9[] PROGMEM ="-- full demo --";
const char song_slots_9[] PROGMEM ="A1/4:0 B5/4:0 C10/4:0 D11/8:0 D24/4:0 E34/4:0 F68/8:0";
const char song_sequence_9[] PROGMEM ="120 A8888 B8888 C8888 D8888 E8888 F8888";


#define MAX_DEFINITON_STRING_LENGTH 150

struct catalog {
  const char *song_name;
  const char *song_slot_definition;
  const char *song_sequence_definition;
} const song_catalog[] PROGMEM = {
   {song_name_0,song_slots_0,song_sequence_0}
  ,{song_name_1,song_slots_1,song_sequence_1}
  ,{song_name_2,song_slots_2,song_sequence_2}
  ,{song_name_3,song_slots_3,song_sequence_3}
  ,{song_name_4,song_slots_4,song_sequence_4}
  ,{song_name_5,song_slots_5,song_sequence_5}
  ,{song_name_6,song_slots_6,song_sequence_6}
  ,{song_name_7,song_slots_7,song_sequence_7}
  ,{song_name_8,song_slots_8,song_sequence_8}
  ,{song_name_9,song_slots_9,song_sequence_9}
};

int song_catalog_count=10;

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

