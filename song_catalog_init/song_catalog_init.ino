/* Example showing timestamp support in LittleFS */
/* Released into the public domain. */
/* Earle F. Philhower, III <earlephilhower@yahoo.com> */

#include <FS.h>
#include <LittleFS.h>
#include <time.h>

#define SONG_DIR_NAME "/songs"
#define SONG_FILE_SEPARATOR_CHAR '&'
#define SONG_LOAD_BUFFER_LENGTH 200

// MAX STRING LENGTH                         0--------10--------20--------30--------40--------50--------60--------70--------80--------90--------100-------10--------20--------30--------40--------50--------60--------70--------80--------90--------200-------90--------90--------90--------90--------9-254
const char song_name_CYPHER4[] PROGMEM ="Cypher 4 (BTS)";
const char song_slots_CYPHER4[] PROGMEM ="I8/4:100 J0/4:100 K25/4:21 R44/16:22";
const char song_sequence_CYPHER4[] PROGMEM ="120 I88# >J888888 K8888 8888 8888 8888 R8887J1 K8888 8888 8888 8888 8888 R8887J1 K8888 8888 8888 R8888 8887J1";

const char song_name_BURNINGUP[] PROGMEM ="Burning Up (BTS)";
const char song_slots_BURNINGUP[] PROGMEM ="I0/4:44 R54/32:130 K108/16:44 Y100/32:44 P10/16:44 Q10/32:44 X0/4:44 L21/16:44 B10/8:44 T54/64:130";
const char song_sequence_BURNINGUP[] PROGMEM ="100 I88# R8888 K88 88 P888Q6X2 R8888 L88 K84Y2X2  P888Q6X2 R8888 B8884Q3X1 T8888 R8888";


const char song_name_DNA[] PROGMEM ="DNA (BTS)";
const char song_slots_DNA[] PROGMEM ="I27/4:40 J22/2:41 A24/4:40  B23/4:40 R10/4:40 S10/8:40 T11/8:41 X1:/4:40";
const char song_sequence_DNA[] PROGMEM ="130 I488 # J88 A8888 B8888 R8888  S8888 T8888  X4 A8888 8888 R8888 S8888 T8888 X8 A8888 X8 R8888 8888";

const char song_name_FAKE_LOVE[] PROGMEM ="FAKE LOVE (BTS)";
const char song_slots_FAKE_LOVE[] PROGMEM ="I11/8:23 A10/8:23 C32/8:23 L23/8:23 M23/16:23 R32/8:23 Z:9/4:23";
const char song_sequence_FAKE_LOVE[] PROGMEM ="78 I88 #A88 R4444 L4444 M4444 A4444 4444 C4444 4444 R4444 L4444 M4444 A4444 4444 C4444 4444 R4444 A4444 4444 >Z8";

const char song_name_IDOL[] PROGMEM ="IDOL (BTS)";
const char song_slots_IDOL[] PROGMEM ="I2/4:1 J44/4:1  A24/8:6 B11/8:41  R11/16:7 S95/8:0";
const char song_sequence_IDOL[] PROGMEM ="126 I88# J88 A8888 8888 B8888 R8888 S8888 A8888 8888 B8888 R8888 S8888 B8888 R8888 S8888";

const char song_name_NOT_TODAY[] PROGMEM ="Not Today (BTS)";
const char song_slots_NOT_TODAY[] PROGMEM ="I2/4:11010 J2/4:29100 H1/8:29100 X:10/4:1 K11/16:2 P62/32:2 O68/8:2 Q68/16:2 R55/16:2";
const char song_sequence_NOT_TODAY[] PROGMEM ="110 I88# J84H2X2 K8888 K8888 P888O4Q4 R8888 8888 K8888 P888O4Q4 R8888 8888 I8884H4 R8888 8888";

const char song_name_MAGIC_SHOP[] PROGMEM ="Magic Shop (BTS)";
const char song_slots_MAGIC_SHOP[] PROGMEM ="I2/4:42 J34/4:42  A10/2:42 K25/16:42 R10/8:42 U10/16:42 S10/8:120 B10/32:1202";
const char song_sequence_MAGIC_SHOP[] PROGMEM ="75 I4444# J4444 K4444 4444 R4444 44 U44 S4444 4444 A4444 K4444 4444 R4444 44U44 S4444 4444 R4444 444B4 S4444 R4444 4444 4444";

const char song_name_DOPY[] PROGMEM ="Dope (BTS)";
const char song_slots_DOPY[] PROGMEM ="I3/4:11010 J0/4:102 L1/4:102 M102/4:102 K44/16:43 P63/8:0 Q68/8:0";
const char song_sequence_DOPY[] PROGMEM ="154 I88# J88 J8888 K8888 8888 8888 P8888 88Q88 K8888 8888 M8888 K4L4K4L4K8L8 P8888 88Q88 K8888 8888 P8888 88Q88 K8888 M8";

const char song_name_NO[] PROGMEM ="N.O (BTS)";
const char song_slots_NO[] PROGMEM ="I0/4:80 J0/4:81 K34/4:45 L32/4:45 R104/16:45 S10/4:45 X11/8:45 Y100/4:45";
const char song_sequence_NO[] PROGMEM ="147 I88 #J88 K8888 L8888 8888 R8888 S8888 8888 K8888 L8888 8888 R8888 S8888 8888 Y88 X86R2 X86R2 X86I2 X86I2 Y8888";

const char song_name_ON[] PROGMEM ="ON (BTS)";
const char song_slots_ON[] PROGMEM ="I2/4:100 J23/16:3 A23/4:5 B22/4:5 R23/16:3 S11/16:4 C10/2:3";
const char song_sequence_ON[] PROGMEM ="106 I88 #J881 A8888 B8888 R8888 S8888 B8888 8888 R8888 S8888 C8 C8888 R8888 S8888 S8888";

const char song_name_DEMO__[] PROGMEM ="-- full demo --";
const char song_slots_DEMO__[] PROGMEM ="A1/4:0 B5/4:0 C10/4:0 D11/8:0 D24/4:0 E34/4:0 F68/8:0";
const char song_sequence_DEMO__[] PROGMEM ="120 A8888 B8888 C8888 D8888 E8888 F8888";


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



String song_catalog_status_message="";
boolean song_catalog_has_error=false;

String song_catalog_current_name="";
String song_catalog_current_slots="";
String song_catalog_current_sequence="";

Dir song_catalog_dir;


void setup() {
  Serial.begin(115200);
  song_catalog_setup() ;
  if(song_catalog_has_error) {
    Serial.print("song_catalog_error:");
    Serial.println(song_catalog_status_message);
  } else {
    listDirToSerial("/");
  }
}


// -------------- Song catalog functions ---------------------

void song_catalog_setup() {
Serial.println("Mount LittleFS");
  if (!LittleFS.begin()) {
    song_catalog_status_message="Could not open filesystem";
    song_catalog_has_error=true;
    Serial.println("LittleFS mount failed");
    return;
  }
}


void song_catalog_goto_start() {
  song_catalog_dir = LittleFS.openDir(SONG_DIR_NAME);
}

boolean song_catalog_next() {
  if (song_catalog_dir.next()) {
    song_catalog_current_name=song_catalog_dir.fileName();
    return true;
  } 
  return false;
}

boolean song_catalog_save_song(String song_name, boolean do_overwrite) {
  song_catalog_status_message="";
  song_catalog_has_error=false;
    
  if(song_name.length()==0) {
    song_catalog_status_message="Song name missing";
    song_catalog_has_error=true;
    return false;
  }
  if(song_catalog_current_slots.length()>=SONG_LOAD_BUFFER_LENGTH-1) {
    song_catalog_status_message="Slot descritpions too long";
    song_catalog_has_error=true;
    return false;
  }
  if(song_catalog_current_sequence.length()>=SONG_LOAD_BUFFER_LENGTH-1) {
    song_catalog_status_message="Sequence descritpion too long";
    song_catalog_has_error=true;
    return false;
  }  
  song_catalog_current_name=song_name;
  
  String path=String(SONG_DIR_NAME)+"/"+song_catalog_current_name;

  if (LittleFS.exists(path) && ! do_overwrite) {
    song_catalog_status_message="Song already exists.";
    song_catalog_has_error=true;
    return false;
  }

  File file = LittleFS.open(path, "w");
  if(!file) {
    song_catalog_status_message="Song file "+path+" could not be opened";
    song_catalog_has_error=true;
    return false;
  }
  
  file.print(song_catalog_current_slots);
  file.print(String((char) SONG_FILE_SEPARATOR_CHAR));
  file.print(song_catalog_current_sequence);
  file.close();
  song_catalog_status_message="Saved sucessfully";
  return true;
}

boolean song_catalog_load_song(String song_name) {
    song_catalog_status_message="";
    song_catalog_has_error=false;
    
    if(song_name.length()==0) {
      song_catalog_status_message="Song name missing";
      song_catalog_has_error=true;
    return false;
    }
    
    char load_buffer[SONG_LOAD_BUFFER_LENGTH+1];
    load_buffer[SONG_LOAD_BUFFER_LENGTH]=0; // ensure string termination at end of buffer
    int read_count=0;

    song_catalog_current_name=song_name;
    String path=String(SONG_DIR_NAME) + "/" + song_catalog_current_name;

    File file = LittleFS.open(path, "r");
    if(!file) {
      song_catalog_status_message="Song file "+path+" could not be opened";
      song_catalog_has_error=true;
      return false;
    }
    int separator_position=-1;
    read_count=file.read((uint8_t*)load_buffer,SONG_LOAD_BUFFER_LENGTH);
    for(int i=0; i<SONG_LOAD_BUFFER_LENGTH && separator_position==-1;i++) {
      if(load_buffer[i]==SONG_FILE_SEPARATOR_CHAR) separator_position=i;
    }
     if(separator_position==-1) {
        song_catalog_status_message="Separator not in first data block";
        song_catalog_has_error=true;
        return false;
     }
    load_buffer[separator_position]=0; // Terminate the string at separator position
    song_catalog_current_slots=load_buffer;
    file.seek(separator_position+1,SeekSet);

    read_count=file.read((uint8_t*) load_buffer,SONG_LOAD_BUFFER_LENGTH);
    load_buffer[read_count]=0; // Terminate the string at end of stream
    song_catalog_current_sequence=load_buffer;

    file.close();
    song_catalog_status_message="Loaded sucessfully";
    return true;
}    

 
    
boolean song_catalog_delete_song(String song_name) {
    song_catalog_status_message="";
    song_catalog_has_error=false;
    
    if(song_name.length()==0) {
      song_catalog_status_message="Song name missing";
      song_catalog_has_error=true;
    return false;
    }

    String path=String(SONG_DIR_NAME)+"/"+song_name;

    if(!LittleFS.exists(path)) {
      song_catalog_status_message="Unknown song name";
      song_catalog_has_error=true;      
      return false;
    }
    
    LittleFS.remove(path);
    return true;
}

    

void song_catalog_to_serial() {
  Serial.print("song_catalog_current_name:");  Serial.println(song_catalog_current_name);
  Serial.print("song_catalog_current_slots:");  Serial.println(song_catalog_current_slots);
  Serial.print("song_catalog_current_sequence:");  Serial.println(song_catalog_current_sequence);
}


// ############## Preset manage functions #####################


void preset_store(int preset_id, boolean do_overwrite) {
  char string_buffer[SONG_LOAD_BUFFER_LENGTH];
  
  if(preset_id<0 && preset_id>song_catalog_count) return;  // Bail out, if input is bad



  strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[preset_id].song_slot_definition)));
  song_catalog_current_slots=string_buffer;

  strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[preset_id].song_sequence_definition)));
  song_catalog_current_sequence=string_buffer;

  strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[preset_id].song_name)));
  Serial.print(F(">TRACE_PRESET_API_CALL song_preset_run:"));Serial.println(string_buffer);

  song_catalog_save_song(string_buffer,do_overwrite);
  if (song_catalog_has_error) {
        Serial.print(F("ERROR:"));
        }
  
  Serial.println(song_catalog_status_message);  
}


// -------------- File System helper functions ---------------------

void listDirToSerial(const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: '");
    Serial.print(root.fileName());
    Serial.print("'  SIZE: ");
    Serial.print(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
    Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
}


void readSongToSerial(String song_name) {

  String path=String(SONG_DIR_NAME)+"/"+song_name;
  Serial.printf("Reading file: %s\n", path.c_str());

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

// ##################  UI ##################################

String input_serialBuffer="";
String input_currentSerialCommand="";
boolean input_serialCommand_isNew=false;

void input_pollSerial() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      input_serialCommand_isNew=true;
      input_currentSerialCommand=input_serialBuffer;
      input_serialBuffer="";
      #ifdef TR_INP
          Serial.print(F("TR_INP>Fetched serial command:"));
          Serial.println(input_currentSerialCommand);
      #endif
    } else input_serialBuffer += inChar;
  }
}


// ############ LOOP ######################
void loop() { 

  input_pollSerial();
  if (input_serialCommand_isNew) {
    input_serialCommand_isNew=false;
    if(input_currentSerialCommand.startsWith("l")) {   // L ist directory
      listDirToSerial(SONG_DIR_NAME);
    }
    if(input_currentSerialCommand.startsWith("c")) {   // C atalog list
      Serial.println("\nCurrent Catalog:");
      song_catalog_goto_start();
      while (song_catalog_next()) {
        Serial.print("'"+song_catalog_current_name+"' - ");
        song_catalog_current_name.replace(" ","_");
        Serial.println("'"+song_catalog_current_name+"'");
      }
      Serial.println("------");
    }
    if(input_currentSerialCommand.startsWith("x")){    // X port song
      int preset_id=input_currentSerialCommand.substring(1).toInt();
      preset_store(preset_id,false);
    }

    if(input_currentSerialCommand.startsWith("y")){    // X port song
      int preset_id=input_currentSerialCommand.substring(1).toInt();
      preset_store(preset_id,true);
    }

    if(input_currentSerialCommand.startsWith("z")){    // X port all songs
      for(int i=0;i<song_catalog_count;i++)   preset_store(i,true);
    }


    if(input_currentSerialCommand.startsWith("r")){  // R ead song
      song_catalog_load_song(input_currentSerialCommand.substring(2));
       if (song_catalog_has_error) {
        Serial.print(F("ERROR:"));
        }
      Serial.println(song_catalog_status_message);  
      if (!song_catalog_has_error) {
          song_catalog_to_serial(); 
      }
    }
    
    if(input_currentSerialCommand.startsWith("p")){  // P rint song file
      readSongToSerial(input_currentSerialCommand.substring(2));
    }

    if(input_currentSerialCommand.startsWith("F")){  // F ormat File System
      LittleFS.end();
      LittleFS.format();
      if (!LittleFS.begin()) {
        Serial.println("LittleFS mount after format failed");
      } else {   Serial.println("LittleFS formatted"); }
    }

    if(input_currentSerialCommand.startsWith("d")){  // D elete song file
      song_catalog_delete_song(input_currentSerialCommand.substring(2));
      Serial.println("\nCatalog after deletion:");
      song_catalog_goto_start();
      while (song_catalog_next()) {
        Serial.println(song_catalog_current_name);
      }
      Serial.println("------");
    }
  }
  
}

