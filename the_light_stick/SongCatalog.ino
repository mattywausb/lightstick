/* This module manages song data to store and retreive from the filesystem */

#include "mainSettings.h"
#include <FS.h>
#include <LittleFS.h>

#ifdef TRACE_ON
#define TRACE_CATALOG_API_CALL
#endif

#define SONG_DIR_NAME "/songs"
#define SONG_FILE_SEPARATOR_CHAR '&'
#define SONG_LOAD_BUFFER_LENGTH 200

String song_catalog_status_message="";
boolean song_catalog_has_error=false;

String song_catalog_list_name=" ";
String song_catalog_current_name=" ";
String song_catalog_current_parts=" ";
String song_catalog_current_sequence=" ";

Dir song_catalog_dir;


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
    song_catalog_list_name=song_catalog_dir.fileName();
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
  if(song_catalog_current_parts.length()>=SONG_LOAD_BUFFER_LENGTH-1) {
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
  
  file.print(song_catalog_current_parts);
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
    song_catalog_current_parts=load_buffer;
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
    song_catalog_status_message="Song deleted";
    return true;
}

    

void song_catalog_to_serial() {
  Serial.print("song_catalog_current_name:");  Serial.println(song_catalog_current_name);
  Serial.print("song_catalog_current_parts:");  Serial.println(song_catalog_current_parts);
  Serial.print("song_catalog_current_sequence:");  Serial.println(song_catalog_current_sequence);
}


