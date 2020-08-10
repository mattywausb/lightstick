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

const char song_name_FAKE_LOVE[] PROGMEM ="FAKE LOVE (BTS)";
const char song_slots_FAKE_LOVE[] PROGMEM ="I11/8:23 A10/8:23 C10/32:23 L24/16:23 M23/16:23 R43/16:23 Z:9/4:23";
const char song_sequence_FAKE_LOVE[] PROGMEM ="78 I88 #A88 R4444 L4444 M4444 A4444 4444 C4444 4444 R4444 L4444 M4444 A4444 4444 C4444 4444 R4444 A4444 4444 >Z8";

const char song_name_IDOL[] PROGMEM ="IDOL (BTS)";
const char song_slots_IDOL[] PROGMEM ="I2/4:1 J44/16:1  A25/8:0 B11/8:41  R10/16:1 S98/8:0";
const char song_sequence_IDOL[] PROGMEM ="126 I88# J88 A8888 8888 B8888 R8888 S8888 A8888 8888 B8888 R8888 S8888 B8888 R8888 S8888";

const char song_name_NOT_TODAY[] PROGMEM ="Not Today (BTS)";
const char song_slots_NOT_TODAY[] PROGMEM ="I2/4:101 J1/8:101 X:10/4:1 K11/16:2 P62/32:2 R55/16:2";
const char song_sequence_NOT_TODAY[] PROGMEM ="110 I88# I84J2X2 >K8888 8888 P8888 R8888 8888 K8888 P8888 R8888 8888 I8884J4 R8888 8888 P8888";

const char song_name_MAGIC_SHOP[] PROGMEM ="Magic Shop (BTS)";
const char song_slots_MAGIC_SHOP[] PROGMEM ="I2/4:42 J34/4:42  A10/2:42 K25/16:42 R10/8:42 U10/16:42 S10/8:120 B10/32:120 X24/2:42";
const char song_sequence_MAGIC_SHOP[] PROGMEM ="75 I4444# J4444 K4444 4444 R4444 44 U44 S4444 4444 A4444 K4444 4444 R4444 44U44 S4444 4444 R4444 444B4 S4444 R4444 4444 X4444";

const char song_name_DOPY[] PROGMEM ="Dope (BTS)";
const char song_slots_DOPY[] PROGMEM ="I3/4:102 J0/4:102 L1/4:102 K44/16:43 P63/8:0 Q68/8:0";
const char song_sequence_DOPY[] PROGMEM ="154 I88# J88 J8888 K8888 8888 8888 P8888 88Q88 K8888 8888 L8888 K4L4K4L4K8L8 P8888 88Q88 K8888 8888 P8888 88Q88 K8888 8";

const char song_name_NO[] PROGMEM ="N.O (BTS)";
const char song_slots_NO[] PROGMEM ="I0/4:80 J0/4:81 K34/4:45 L32/4:45 R55/16:46 S10/4:45 X11/8:45";
const char song_sequence_NO[] PROGMEM ="147 I88 #J88 K8888 L8888 8888 R8888 S8888 8888 K8888 L8888 8888 R8888 S8888 8888 X88 8888 8888";

const char song_name_ON[] PROGMEM ="ON (BTS)";
const char song_slots_ON[] PROGMEM ="I2/4:100 J23/16:3 A23/4:5 B22/4:5 R23/16:3 S11/16:4 C10/2:3";
const char song_sequence_ON[] PROGMEM ="106 I88 #J881 A8888 B8888 R8888 S8888 B8888 8888 R8888 S8888 C8 C8888 R8888 S8888 S8888";

const char song_name_DEMO__[] PROGMEM ="-- full demo --";
const char song_slots_DEMO__[] PROGMEM ="A1/4:0 B5/4:0 C10/4:0 D11/8:0 D24/4:0 E34/4:0 F68/8:0";
const char song_sequence_DEMO__[] PROGMEM ="120 A8888 B8888 C8888 D8888 E8888 F8888";


#define MAX_DEFINITON_STRING_LENGTH 150

struct catalog {
  const char *song_name;
  const char *song_slot_definition;
  const char *song_sequence_definition;
} const song_catalog[] PROGMEM = {
   {song_name_BURNINGUP,song_slots_BURNINGUP,song_sequence_BURNINGUP}
  ,{song_name_CYPHER4,song_slots_CYPHER4,song_sequence_CYPHER4}
  ,{song_name_DNA,song_slots_DNA,song_sequence_DNA}
  ,{song_name_DOPY,song_slots_DOPY,song_sequence_DOPY}
  ,{song_name_FAKE_LOVE,song_slots_FAKE_LOVE,song_sequence_FAKE_LOVE}
  ,{song_name_IDOL,song_slots_IDOL,song_sequence_IDOL}
  ,{song_name_MAGIC_SHOP,song_slots_MAGIC_SHOP,song_sequence_MAGIC_SHOP}
  ,{song_name_NOT_TODAY,song_slots_NOT_TODAY,song_sequence_NOT_TODAY}
  ,{song_name_NO,song_slots_NO,song_sequence_NO}
  ,{song_name_ON,song_slots_ON,song_sequence_ON}
  ,{song_name_DEMO__,song_slots_DEMO__,song_sequence_DEMO__}
};

int song_catalog_count=11;

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

