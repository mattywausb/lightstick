#include <Adafruit_NeoPixel.h>
#include "lamp.h"
#include "output.h"
#include "mainSettings.h"

#ifdef TRACE_ON
//#define TR_OUT_PIXEL_RESULT
//#define TR_OUT_TIMING
//#define TR_OUT_PATTERN_BEAT
//#define TR_OUT_API_CALL
//#define TR_COLOR_PALETTE_SETTING
//#define TR_COLOR_PRESET_PALETTE_SETTING
//#define TR_PATTERN_SETTING
//#define TR_COLOR_ORBIT_COLOR
//#define TR_OUT_BEAT_TRACKING
//#define TR_OUT_SPARKLE_DETAILS
#endif

#define PIXEL_PIN    12    // Digital IO pin connected to the NeoPixels. D6 on ESP8266 / Node MCU

#define PIXEL_COUNT 7

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define LAMP_COUNT 7
Lamp lamp[LAMP_COUNT];

typedef struct {
    float h;       // angle in degrees
    float s;       // a fraction between 0 and 1
} t_color_hs;

#define COLOR_PALETTE_MAX_ENTRIES  22
t_color_hs patconf_color_palette[COLOR_PALETTE_MAX_ENTRIES];
uint8_t patconf_color_palette_lenght=0;
uint8_t patvar_color_palette_index=0;

enum STEPPER_TYPE {
  ST_COLOR_WIPE,
  ST_DOUBLE_ORBIT,
  ST_COLOR_ORBIT,
  ST_PULSE,
  ST_RAINBOW,
  ST_SPARKLE
};

STEPPER_TYPE output_current_stepper_type=ST_COLOR_WIPE;


float output_general_color[]={HUE_RED,    //0
                             HUE_ORANGE,  //1
                             HUE_YELLOW,  //2
                             HUE_LEMON,   //3
                             HUE_GREEN,   //4
                             HUE_CYAN,    //5
                             HUE_SKYBLUE, //6
                             HUE_BLUE,    //7
                             HUE_PURPLE,  //8
                             HUE_PINK} ;   //9

int output_waittime[6];
int output_beats_per_minute = 120;
float output_master_light_value = 0.5;
long output_beat_sync_time=0L;
int output_preset_beat_start_beat=0;
int output_preset_beat_count=0;

boolean output_beat_sync_happened=false;

#define PATCONF_FADE_FRAMETIME 25
#define PATCONF_PULSE_SATURATION_INCREMENT 0.15

int patconf_speed_id=STEP_ON_BEAT;
uint8_t patconf_steps_until_color_change=0;
uint8_t patconf_color_palette_increment=1;
int patconf_max_step_count=1000;
float patconf_pattern_lamp_value = 0.5;
float patconf_fade_factor=0.9;
float patconf_rainbow_step_angle_increment=1.0;
int patconf_step_0_waittime=0;
int patconf_step_1_waittime=0;
byte patconf_follow_up_ticks=4;

int patvar_current_step_index = 0;
uint8_t patvar_step_in_color_index=0;
long patvar_previous_step_time = 0L;
long patvar_previous_fade_time = 0L;


/* 
 *  API Functions
 */

// Following function must be called in the setup
void output_setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  digitalWrite(LED_BUILTIN, LOW);
  output_set_bpm(40);
  output_set_pattern_speed(STEP_ON_BEAT);
  randomSeed(1);
}


// Manage color palette
void output_reset_color_palette(float hue, float saturation){
  patconf_color_palette[0].h=hue;
  patconf_color_palette[0].s=saturation; 
  patconf_color_palette_lenght=1;
  #ifdef TR_COLOR_PALETTE_SETTING
    dump_color_palette_to_serial();
  #endif
}

void output_add_color_palette_entry(float hue, float saturation){
  if(patconf_color_palette_lenght<COLOR_PALETTE_MAX_ENTRIES) {
    patconf_color_palette[patconf_color_palette_lenght].h=hue;
    patconf_color_palette[patconf_color_palette_lenght].s=saturation; 
    patconf_color_palette_lenght++;
  }
  #ifdef TR_COLOR_PALETTE_SETTING
    dump_color_palette_to_serial();
  #endif
}

void output_set_color_palette_entry(int index,float hue, float saturation){
  if(index<0 || index>patconf_color_palette_lenght) return;
  patconf_color_palette[index].h=hue;
  patconf_color_palette[index].s=saturation; 
  #ifdef TR_COLOR_PALETTE_SETTING
    dump_color_palette_to_serial();
  #endif
}

void output_saturate_color_palette(int entry_count) {
  for(int i=0; i<entry_count;i++) patconf_color_palette[i].s=1.0;
}

// patch color palette with AAAA BBBB CCCC DDDD pattern
// first color index can be >9 to trigger white with follow up color
void output_generate_4step_palette( byte step1_index, byte step2_index,byte step3_index, byte step4_index)
{
  for(int i=0;i<4;i++){
       patconf_color_palette[i].h=output_general_color[step1_index];
       patconf_color_palette[i+4].h=output_general_color[step2_index];
       patconf_color_palette[i+8].h=output_general_color[step3_index];
       patconf_color_palette[i+12].h=output_general_color[step4_index];
  }
}


// patch color palette with AAA BBB CCC DDD pattern
// first color index can be >9 to trigger white with follow up color
void output_generate_3step_palette( byte step1_index, byte step2_index,byte step3_index, byte step4_index)
{
  for(int i=0;i<3;i++){
       patconf_color_palette[i].h=output_general_color[step1_index];
       patconf_color_palette[i+3].h=output_general_color[step2_index];
       patconf_color_palette[i+6].h=output_general_color[step3_index];
       patconf_color_palette[i+9].h=output_general_color[step4_index];
  }
}


// patch color palette with color on interval  pattern
// first color index can be 10 to trigger white with follow up color
void output_patch_palette_hue( float hue,  byte start,byte stepping )
{
  for(int i=start;i<COLOR_PALETTE_MAX_ENTRIES;i+= stepping){
       patconf_color_palette[i].h=hue;
  }
}

// patch color palette with white saturation on interval  pattern
void output_patch_palette_white( byte start,byte stepping )
{
  for(int i=start;i<COLOR_PALETTE_MAX_ENTRIES;i+= stepping){
       patconf_color_palette[i].s=0.0;
  }
}


void output_load_color_palette(long palette_id)
{
  #ifdef TR_OUT_API_CALL
      Serial.print(F("TR_OUT_API_CALL> output_load_color_palette:"));Serial.println(palette_id);
  #endif 
  if(palette_id<10000) {
    output_saturate_color_palette(COLOR_PALETTE_MAX_ENTRIES);
    switch(palette_id) {
      // broad  section
      case 0:                 // Prime Colors and Yellow
           palette_id=12704;
           break;
      case 1:                 // Police USA b wb r wr
           palette_id=30700;
           break;
      case 2:                 // Red blue variant (w,sb,r,b,o,sb,r,b) "not today"
           patconf_color_palette[0].h=HUE_SKYBLUE;patconf_color_palette[0].s=0.0; //WHITE
           patconf_color_palette[1].h=HUE_SKYBLUE;
           patconf_color_palette[2].h=HUE_RED;
           patconf_color_palette[3].h=HUE_BLUE; 
           patconf_color_palette[4].h=HUE_ORANGE;
           patconf_color_palette[5].h=HUE_SKYBLUE;
           patconf_color_palette[6].h=HUE_RED;
           patconf_color_palette[7].h=HUE_BLUE;
           patconf_color_palette_lenght=8;
           break;
      case 3:                 // b g y
           palette_id=20742;
           break;
      case 4:                 // Yellow pulse with blue and green change
           palette_id=42274;
           break;
      case 5:                 //  y g b c 
           palette_id=15247;
           break;
      case 6:                // Mixed rainbow
           patconf_color_palette_lenght=8;
           patconf_color_palette[0].h=HUE_RED;
           patconf_color_palette[1].h=HUE_CYAN;
           patconf_color_palette[2].h=HUE_YELLOW;
           patconf_color_palette[3].h=HUE_PURPLE; 
           patconf_color_palette[4].h=HUE_GREEN;
           patconf_color_palette[5].h=HUE_PINK;
           patconf_color_palette[6].h=HUE_ORANGE;
           patconf_color_palette[7].h=HUE_BLUE;
           break;
      case 7:                // w B g y r p  rainbow
           patconf_color_palette_lenght=8;
           patconf_color_palette[0].h=HUE_BLUE;patconf_color_palette[0].s=0.0; //WHITE
           patconf_color_palette[1].h=HUE_BLUE;
           patconf_color_palette[2].h=HUE_CYAN;
           patconf_color_palette[3].h=HUE_GREEN;
           patconf_color_palette[4].h=HUE_YELLOW;
           patconf_color_palette[5].h=HUE_ORANGE;
           patconf_color_palette[6].h=HUE_RED;
           patconf_color_palette[7].h=HUE_PINK;
           break;
      
     /// ---------- Cold section       
     case 20:                // blue green  
           palette_id=14747;
           break;
     case 21:                 // blue white cyan purpl
           patconf_color_palette[0].h=HUE_BLUE;
           patconf_color_palette[1].h=HUE_BLUE;patconf_color_palette[1].s=0.0; //WHITE
           patconf_color_palette[2].h=HUE_CYAN;
           patconf_color_palette[3].h=HUE_PURPLE;
           patconf_color_palette_lenght=4;
           break;
     case 22:                 // blue white(on blue) cyan green 
           patconf_color_palette[0].h=HUE_GREEN;
           patconf_color_palette[1].h=HUE_BLUE;
           patconf_color_palette[2].h=HUE_BLUE;patconf_color_palette[2].s=0.0; //WHITE
           patconf_color_palette[3].h=HUE_PURPLE;
           patconf_color_palette[4].h=HUE_BLUE;
           patconf_color_palette[5].h=HUE_CYAN;patconf_color_palette[5].s=SAT_LCYAN;
           patconf_color_palette_lenght=6;
           break;
     case 23:                 // blue sky lblue cyan
           patconf_color_palette[0].h=HUE_BLUE;
           patconf_color_palette[1].h=HUE_SKYBLUE;
           patconf_color_palette[2].h=HUE_LBLUE;patconf_color_palette[2].s=SAT_LBLUE;
           patconf_color_palette[3].h=HUE_CYAN;
           patconf_color_palette_lenght=4;
           break;
     /// -- Warm Section
     case 40:                   // yellow, skyblue, orange, pink
           palette_id=12619;
           break;
     case 41:                   // red, orange, rose
           patconf_color_palette[0].h=HUE_RED;
           patconf_color_palette[1].h=HUE_ORANGE;
           patconf_color_palette[2].h=HUE_RED;patconf_color_palette[2].s=SAT_ROSE;
           patconf_color_palette_lenght=3;
           break;
     case 42:                   // pu pi wpu pi
           palette_id=35899;
           break;
     case 43:                   // o wo o wo r wo r wo 
           palette_id=42100;
           break;
     case 44:                                                   // fire
           patconf_color_palette[0].h=20;
           patconf_color_palette[1].h=10;
           patconf_color_palette[2].h=25;
           patconf_color_palette[3].h=5; 
           patconf_color_palette[4].h=15;
           patconf_color_palette[5].h=7;
           patconf_color_palette_lenght=6;
           break; 
     case 45:                           // r wr
           palette_id=20000;
           break; 
     case 46:                           //  r r wr r r r
           palette_id=24000;
           break; 
   // -- Single Color + White Overlay version
     case 80:         // pure red
           palette_id=20000;
           break; 
     case 81:         // pure red over white  
           palette_id=29000;
           break; 
           
           
                     
   // -- Pulse specials  Section
     case 100:                                        // blue green 
           palette_id=17744; 
           break;
    case 101:                                        // r w+r oe w+o
           palette_id=30011; 
           break;
    case 102:                                        // o wo r wo wo
           patconf_color_palette[0].h=HUE_ORANGE;
           patconf_color_palette[1].h=HUE_ORANGE;patconf_color_palette[1].s=0.0; 
           patconf_color_palette[2].h=HUE_RED;
           patconf_color_palette[3].h=HUE_ORANGE;patconf_color_palette[3].s=0.0; 
           patconf_color_palette[4].h=HUE_ORANGE;patconf_color_palette[4].s=0.0; 
           patconf_color_palette_lenght=5;
           break;
    // -- Whipe specials Section
    case 120:                                        // pu pu pi pi wpu wpu pi pi
           patconf_color_palette[0].h=HUE_PURPLE;
           patconf_color_palette[1].h=HUE_PURPLE;
           patconf_color_palette[2].h=HUE_PINK;
           patconf_color_palette[3].h=HUE_PINK;
           patconf_color_palette[4].h=HUE_PURPLE;patconf_color_palette[4].s=0.0; 
           patconf_color_palette[5].h=HUE_PURPLE;patconf_color_palette[5].s=0.0; 
           patconf_color_palette[6].h=HUE_PINK;
           patconf_color_palette[7].h=HUE_PINK;
           patconf_color_palette_lenght=8;
           break;
   // -- Color Orb specials Section 
    case 130:
                                                     // fire with White center 3 steps
           patconf_color_palette[0].h=20;        
           patconf_color_palette[1].h=HUE_YELLOW; patconf_color_palette[1].s=0.0; // WHITE
           patconf_color_palette[2].h=15;        
           patconf_color_palette[3].h=10;        
           patconf_color_palette[4].h=HUE_YELLOW; patconf_color_palette[4].s=0.0; // WHITE
           patconf_color_palette[5].h= 5;         
           patconf_color_palette[6].h=25;         
           patconf_color_palette[7].h=HUE_YELLOW; patconf_color_palette[7].s=0.0; // WHITE 
           patconf_color_palette[8].h=19;      
           patconf_color_palette[9].h= 6;          
           patconf_color_palette[10].h=HUE_YELLOW;patconf_color_palette[10].s=0.0; // WHITE
           patconf_color_palette[11].h= 8;      
           patconf_color_palette_lenght=12;
           break; 
            
            
     #ifdef TR_WARNING
     default:
          Serial.print(F("#!# TR_WARNING> unkown color pallette:"));Serial.println(palette_id);
     #endif         
    } // Switch
  } // <10000
  
  if(palette_id>=10000 && palette_id<100000) {
    byte digit5=palette_id%10;
    byte digit4=(palette_id/10)%10;
    byte digit3=(palette_id/100)%10;
    byte digit2=(palette_id/1000)%10;
    
    
    if(palette_id<20000) { // 10000-19999
    output_saturate_color_palette(4);  
    patconf_color_palette[0].h=output_general_color[digit2];     
    patconf_color_palette[1].h=output_general_color[digit3];   
    patconf_color_palette[2].h=output_general_color[digit4];        
    patconf_color_palette[3].h=output_general_color[digit5];     
    patconf_color_palette_lenght=4;
    } else { 
      int generator_id=palette_id/1000;
      output_saturate_color_palette(COLOR_PALETTE_MAX_ENTRIES);       
      if(digit3==digit5 && digit4==digit5) {    // One Color special Pattern
        #ifdef TR_COLOR_PRESET_PALETTE_SETTING
              Serial.println(F("TR_COLOR_PRESET_PALETTE_SETTING> 1 color special"));
        #endif

        switch(generator_id){
          case 20:                                                   // x wx
                  patconf_color_palette_lenght=2;
                  patconf_color_palette[0].h=output_general_color[digit3];
                  patconf_color_palette[1].h=output_general_color[digit3];patconf_color_palette[1].s=0.0; 
                  break;
          case 21:                                                  
                  patconf_color_palette_lenght=2;                   // wx x
                  patconf_color_palette[0].h=output_general_color[digit3];patconf_color_palette[0].s=0.0; 
                  patconf_color_palette[1].h=output_general_color[digit3];
                  break;
          case 22:
                  patconf_color_palette_lenght=3;                   // wx x x
                  patconf_color_palette[0].h=output_general_color[digit3];patconf_color_palette[0].s=0.0; 
                  patconf_color_palette[1].h=output_general_color[digit3];
                  patconf_color_palette[2].h=output_general_color[digit3];
                  break;
          case 23:                                                   // wx x x x  
                  patconf_color_palette_lenght=4;
                  output_generate_4step_palette(digit3,digit3,digit3,digit3);
                  patconf_color_palette[0].s=0.0; 
                  break;
          case 24:
                  patconf_color_palette_lenght=6;                   // x x x wx x x 
                  output_generate_3step_palette(digit3,digit3,digit3,digit3);
                  patconf_color_palette[2].s=0.0; 
                  break;
          case 29:
                  patconf_color_palette_lenght=1;                   // wx
                  patconf_color_palette[0].h=output_general_color[digit3];patconf_color_palette[0].s=0.0; 
           default:                                                 // BAD ID
                  patconf_color_palette_lenght=1;
                  patconf_color_palette[0].h=HUE_MAGENTA;                  
         } //switch
      } else {
        if(digit4==digit5) {    // two Color special Pattern
          #ifdef TR_COLOR_PRESET_PALETTE_SETTING
              Serial.println(F("TR_COLOR_PRESET_PALETTE_SETTING> 2 color special"));
          #endif
          switch(generator_id){
            case 20:                                                 // x y y
            case 21:                                                 // x x y
            case 22:                                                 // wx x y
            case 23:                                                 // x wx y
            case 24:                                                 // x x wy
                      #ifdef TR_COLOR_PRESET_PALETTE_SETTING
                          Serial.println(F("TR_COLOR_PRESET_PALETTE_SETTING> Block 20-24"));
                      #endif
                    patconf_color_palette_lenght=3;
                    patconf_color_palette[0].h=output_general_color[digit3];
                    patconf_color_palette[2].h=output_general_color[digit4];
                    if(generator_id==20)  patconf_color_palette[1].h=output_general_color[digit4];
                    if(generator_id==21)  patconf_color_palette[1].h=output_general_color[digit3];
                    if(generator_id>=22) patconf_color_palette[generator_id-22].s=0.0; 
                    break;
           case 25:                                                 // wx  x  y  y
           case 26:                                                 //  x wx  y  y
           case 27:                                                 //  x  x wy  y
           case 28:                                                 //  x  x  y wy
           case 29:                                                 // wx  x wy  y
           case 30:                                                 //  x wx  y wy
           case 31:                                                 // wx wx wy wy
                    patconf_color_palette_lenght=4;
                    patconf_color_palette[0].h=output_general_color[digit3];
                    patconf_color_palette[1].h=output_general_color[digit3];
                    patconf_color_palette[2].h=output_general_color[digit4];
                    patconf_color_palette[3].h=output_general_color[digit4];
                    switch (generator_id) {
                      case 29: patconf_color_palette[0].s=0.0;
                               patconf_color_palette[2].s=0.0;
                               break;
                      case 30: patconf_color_palette[1].s=0.0;
                               patconf_color_palette[3].s=0.0;
                               break;
                      case 31: output_patch_palette_white(0,1);break;
                      default:
                               patconf_color_palette[generator_id-25].s=0.0;
                      
                    }// end switch
                    break;  // 25-31
            case 32:                                                 // wx wy 
                    patconf_color_palette_lenght=2;
                    patconf_color_palette[0].h=output_general_color[digit3];
                    patconf_color_palette[1].h=output_general_color[digit4];
                    patconf_color_palette[0].s=0.0;
                    patconf_color_palette[1].s=0.0;
                    break;
            case 33:                                                 // wx  y  x  y
            case 34:                                                 //  x wy  x  y
            case 35:                                                 //  x  y wx  y
            case 36:                                                 //  x  y  x wy
                    patconf_color_palette_lenght=4;
                    patconf_color_palette[0].h=output_general_color[digit3];
                    patconf_color_palette[1].h=output_general_color[digit4];
                    patconf_color_palette[2].h=output_general_color[digit3];
                    patconf_color_palette[3].h=output_general_color[digit4];
                    patconf_color_palette[generator_id-33].s=0.0;
                    break;
            case 40:                                                 // wx  x  x  x wy  y  y  y 
            case 41:                                                 // wx  x wx  x wy  y wy  y 
            case 42:                                                 //  x wx  x wx  y wy  y wy 
                    patconf_color_palette_lenght=8;
                    output_generate_4step_palette(digit3,digit4,digit3,digit4);
                    output_patch_palette_white(generator_id==42?1:0,generator_id==40?4:2);
                    break;
            case 43:                                                 // wx  x  x  x wx  x  x  x wy  y  y  y wy  y  y  y 
                    patconf_color_palette_lenght=generator_id=16;
                    output_generate_4step_palette(digit3,digit3,digit4,digit4);
                    output_patch_palette_white(0,4);
                    break;
            
            case 70:                                                 //  x  x  x  y  y  y 
            case 71:                                                 //  x wx  x  y wy  y 
            case 72:                                                 //  x  y  x  y  x  y 
                    patconf_color_palette_lenght=6;
                    output_generate_3step_palette(digit3,digit4,digit3,digit4);
                    if(generator_id==71) output_patch_palette_white(1,3);
                    if(generator_id==72) {
                              patconf_color_palette[1].h=output_general_color[digit4];
                              patconf_color_palette[4].h=output_general_color[digit3];      
                    }
                    break;
            case 73:                                                 //  x  x  x  y  x  x 
                    patconf_color_palette_lenght=6;
                    output_generate_3step_palette(digit3,digit3,digit3,digit4);
                    patconf_color_palette[2].h=output_general_color[digit4];      
                    break;
           default:                                                 // BAD ID
                  patconf_color_palette_lenght=1;
                  patconf_color_palette[0].h=HUE_MAGENTA;
          } //switch
        } else { // three Color special Pattern
         #ifdef TR_COLOR_PRESET_PALETTE_SETTING
              Serial.println(F("TR_COLOR_PRESET_PALETTE_SETTING> 3 color special"));
          #endif
          switch(generator_id){
           case 20:                                                   // x y z
           case 21:                                                  // wx wy wz
                  patconf_color_palette_lenght=3;
                  patconf_color_palette[0].h=output_general_color[digit3];
                  patconf_color_palette[1].h=output_general_color[digit4];
                  patconf_color_palette[2].h=output_general_color[digit5];
                  if(generator_id==20)break;
                  output_patch_palette_white(0,1);
                  break;
           case 25:                                                  //wx y x z
           case 26:                                                  //x wy x z
           case 27:                                                  //x y wx z
           case 28:                                                  //x y x wz
           case 29:                                                  //wx wy wx wz
                  patconf_color_palette_lenght=4;
                  patconf_color_palette[0].h=output_general_color[digit3];  
                  patconf_color_palette[1].h=output_general_color[digit4];
                  patconf_color_palette[2].h=output_general_color[digit3];
                  patconf_color_palette[3].h=output_general_color[digit5];
                  if(generator_id<29) patconf_color_palette[generator_id-25].s=0.0;
                  else output_patch_palette_white(0,1);
                  break;
           case 40:                                                  // x y y y x z z z 
           case 41:                                                  // x y x y x z x z 
           case 42:                                                  // y x y x z x z x
                  patconf_color_palette_lenght=8;
                  output_generate_4step_palette(digit4,digit5,digit5,digit3);
                  output_patch_palette_hue(output_general_color[digit3],generator_id==42?1:0,generator_id==40?4:2);
                  break;
           case 43:                                                  // x y y y x y y y  x z z z x z z z 
           case 44:                                                  // x y x y x y x y  x z x z x z x z 
           case 45:                                                  // y x y x y x y x  z x z x z x z x
                  patconf_color_palette_lenght=16;
                  output_generate_4step_palette(digit4,digit4,digit5,digit5);
                  output_patch_palette_hue(output_general_color[digit3],generator_id==45?1:0,generator_id==43?4:2);
                  break;
           case 46:                                                  // wx  x  x  x wy  y  y  y wz  z  z  z 
           case 47:                                                  // wx  x wx  x wy  y wy  y wz  z wz  z 
           case 48:                                                  // wx  x wx  x  y  x  y  x  z  x  z  x  
           case 49:                                                  //  x  y  x  y  z  y  z  y  x  z  x  z 
                  patconf_color_palette_lenght=12;
                  output_generate_4step_palette(digit3,digit4,digit5,digit3);
                  switch (generator_id) {
                    case 46: output_patch_palette_white(0,4); break;
                    case 47: output_patch_palette_white(0,2); break;
                    case 48: output_patch_palette_hue(output_general_color[digit3],1,2); 
                             patconf_color_palette[0].s=0.0; patconf_color_palette[2].s=0.0; 
                              break;
                    case 49: patconf_color_palette[1].h=output_general_color[digit4];patconf_color_palette[3].h=output_general_color[digit4];
                             patconf_color_palette[4].h=output_general_color[digit5];patconf_color_palette[6].h=output_general_color[digit5];
                             patconf_color_palette[8].h=output_general_color[digit3];patconf_color_palette[10].h=output_general_color[digit3];
                             break;
                  }
                  break;

           case 70:                                                  // x  x  x  y  y  y  z  z  z
           case 71:                                                  // x wx  x  y wy  y  z wz  z
           case 72:                                                  // x  y  x  y  z  y  z  x  z
                  patconf_color_palette_lenght=9;
                  output_generate_3step_palette(digit3,digit4,digit5,digit4);
                  if(generator_id==71) output_patch_palette_white(1,3);
                  if(generator_id==72) {
                    patconf_color_palette[1].h=output_general_color[digit4];
                    patconf_color_palette[4].h=output_general_color[digit5];
                    patconf_color_palette[7].h=output_general_color[digit3];
                  }
                  break;       
           case 73:                                                  // y  x  y  z  x  z  
                  patconf_color_palette_lenght=6;
                  output_generate_3step_palette(digit4,digit5,digit5,digit4);
                  patconf_color_palette[1].h=output_general_color[digit3];
                  patconf_color_palette[4].h=output_general_color[digit3];
                  break;
           default:                                                 // BAD ID
                  patconf_color_palette_lenght=1;
                  patconf_color_palette[0].h=HUE_MAGENTA;
          } //switch
        } // not 2 Color Pattern
      } // not 1  Color Pattern
    } // end not <20000
  }// End >=10000
  
  #ifdef TR_COLOR_PRESET_PALETTE_SETTING
  #ifdef TR_COLOR_PALETTE_SETTING
    dump_color_palette_to_serial();
  #endif  
  #endif
}


// Change bpm
void output_set_bpm(int beats_per_minute) {
  #ifdef TR_OUT_API_CALL
      Serial.print(F("TR_OUT_API_CALL> output_set_bpm:"));Serial.println(beats_per_minute);
  #endif 
  output_beats_per_minute = beats_per_minute;
  output_set_waittimes(60000/output_beats_per_minute);
  output_sync_beat(millis());
};

int output_get_bpm() { return output_beats_per_minute;}

void output_set_waittimes(int time_for_beat) {
  output_waittime[STEP_ON_2BEATS] =time_for_beat*2;
  output_waittime[STEP_ON_BEAT] = time_for_beat;
  output_waittime[STEP_ON_8TH] = output_waittime[STEP_ON_BEAT] /2;
  output_waittime[STEP_ON_16TH] = output_waittime[STEP_ON_8TH] /2;
  output_waittime[STEP_ON_32RD] = output_waittime[STEP_ON_16TH] /2;
  output_waittime[STEP_ON_64TH] = output_waittime[STEP_ON_32RD] /2;
  #ifdef TR_OUT_TIMING
    Serial.print(F(">TR_OUT_TIMING calculated BPM ")); Serial.println(60000/time_for_beat);
    Serial.print(F(">TR_OUT_TIMING STEP_ON_BEAT ")); Serial.println(output_waittime[STEP_ON_BEAT]);
    //Serial.print(F(">TR_OUT_TIMING STEP_ON_32RD ")); Serial.println(output_waittime[STEP_ON_32RD]);
    //Serial.print(F(">TR_OUT_TIMING STEP_ON_64TH ")); Serial.println(output_waittime[STEP_ON_64TH]);
  #endif
}

int output_get_beat_number_since_sync() {
  long runtime_since_sync=millis()-output_beat_sync_time;
  return runtime_since_sync/output_waittime[STEP_ON_BEAT];
}

long output_get_current_beat_start_time()  {
   return output_get_beat_number_since_sync()*output_waittime[STEP_ON_BEAT]; 
}

int output_get_preset_beat_count() {
  return output_preset_beat_count;
}

void output_sync_beat(long sync_event_time) {
  output_beat_sync_time=sync_event_time;
  output_beat_sync_happened=true;
}

// Change Pattern speed (relative to bpm)
void output_set_pattern_speed(int wait_index)
{
  #ifdef TR_OUT_API_CALL
      Serial.print(F("TR_OUT_API_CALL> output_set_pattern_speed:"));Serial.println(wait_index);
  #endif 
  if(wait_index>=STEP_ON_2BEATS && wait_index<=STEP_ON_64TH) patconf_speed_id=wait_index;
}

// Select and start a pattern
void output_start_pattern(int pattern_id) {
  #ifdef TR_OUT_API_CALL
      Serial.print(F("TR_OUT_API_CALL> output_start_pattern:"));Serial.println(pattern_id);
  #endif 
  output_preset_beat_start_beat=output_get_beat_number_since_sync();
  output_preset_beat_count=0;
  switch (pattern_id/10) {
    case 0:                       // PULSE  0-3: Regular, 4-7: Heartbeat (2,4,8,16 Steps) 8 Regular low light
         
         if( pattern_id<8) {
          int followup_tick=pattern_id<4?4:6;  
          start_pulse(1, // brightness 
                      1<<(1+(pattern_id%4)),  // Steps until color increment
                      0.85, // preserve brightnes factor 
                      followup_tick  ); // follow up tick count (4= equally spaced)
         } else {
         if(pattern_id==8)  
          start_pulse(0.05, // brightness 
                      8,  // Steps until color increment
                      0.9, // preserve brightnes factor 
                      4  ); // follow up tick count (4= equally spaced)
         else                      
          start_pulse(0.05, // brightness 
                      8,  // Steps until color increment
                      0.9, // preserve brightnes factor 
                      2  ); // follow up tick count (4= equally spaced)
         }
         break;
    case 1:            // WHIPE  10 = direct , 11= over black, 12= direct low light, 13 = over black low light
         start_colorWipe(0.8/1+(pattern_id/2),pattern_id%2);  // brightness, over_black flag
         break;
    case 2:            // DOUBLE ORBIT 20-26: Color step 1,2,4,8,16,32,64
          start_doubleOrbit(0.8,1<<(pattern_id-20));  // brightness, Steps until color increment
          break;
    case 3:           // DOUBLE COLOR ORBIT 30-36 palette increment 1 
         start_colorOrbit(0.8,1<<(pattern_id-30),1);  // brightness, Steps until color increment, color palette increment   
         break;
    case 4:           // DOUBLE COLOR ORBIT 40-46 palette increment 2 
         start_colorOrbit(0.8,1<<(pattern_id-40),2);  // brightness, Steps until color increment, color palette increment   
         break;
    case 5:           // DOUBLE COLOR ORBIT 50-56 palette increment 3 
         start_colorOrbit(0.8,1<<(pattern_id-50),3);  // brightness, Steps until color increment, color palette increment   
         break;
    case 6:           // RAINBOW FULL 60-69  increment  6.0° to 60° 
         start_rainbow(0.5,60.0,(pattern_id-59)*6.0); // brightness, hue angle distance to neighbour, hue step increment   // Full span, hart stepping
         break;
    case 7:           // RAINBOW HALF 70-79  increment  6.0° to 60° 
         start_rainbow(0.5,30.0,(pattern_id-69)*6.0); // brightness, hue angle distance to neighbour, hue step increment   // Full span, hart stepping
         break;
    case 8:           // RAINBOW QUATER 80-89  increment  6.0° to 60° 
         start_rainbow(0.5,15.0,(pattern_id-79)*6.0); // brightness, hue angle distance to neighbour, hue step increment   // Full span, hart stepping
         break;
    case 9:           // RAINBOW FLAT 90-99  increment  1,6,12,18,24 ,55,85,115,145,175
         if(pattern_id==90) {
               start_rainbow(0.5,0.0,1.0); 
         } else
         if(pattern_id<95) {
               start_rainbow(0.5,0.0,6*(pattern_id-90)); 
         }
         else {
               start_rainbow(0.5,0.0,30*(pattern_id-94)+25); 
         }
         break;
    case 10:           // SPARKLE 100-106 palette increment 1 
         start_sparkle(0.8,1<<(pattern_id-100),1);  // brightness, Steps until color increment, color palette increment   
         break;
    case 11:           // SPARKLE 110-116 palette increment 2 
         start_sparkle(0.8,1<<(pattern_id-110),2);  // brightness, Steps until color increment, color palette increment   
         break;
    case 12:           // SPARKLE 120-126 palette increment 3 
         start_sparkle(0.8,1<<(pattern_id-120),3);  // brightness, Steps until color increment, color palette increment   
         break;  
  }
  output_process_pattern();
}

void output_determine_beat()
{
  if(output_preset_beat_start_beat==output_get_beat_number_since_sync())  return;
  #ifdef TR_OUT_PATTERN_BEAT
    Serial.print(F("TR_OUT_PATTERN_BEAT> Beat = ")); Serial.println(output_preset_beat_count);
  #endif 
  output_preset_beat_start_beat=output_get_beat_number_since_sync();
  digitalWrite(LED_BUILTIN, !(output_preset_beat_count%2));
  output_preset_beat_count++;
}

// Following function must be called in the loop at last
void output_process_pattern() {
  switch (output_current_stepper_type) {
    case ST_COLOR_WIPE: process_colorWipe();      break;
    case ST_DOUBLE_ORBIT: process_doubleOrbit(); break;
    case ST_COLOR_ORBIT: process_colorOrbit(); break;
    case ST_PULSE: process_pulse(); break;
    case ST_RAINBOW: process_rainbow();break;
    case ST_SPARKLE: process_sparkle();break;
  }
  output_beat_sync_happened=false;      // Agents had their chance to react, now we set it back
}


/*
 *   SPARKLE 
 *   Igniting random leds with random color from 3 of palette
 *   lamp_value: general brightness
 *   steps_per_color: number of steps to change color index (index alway increments by 2, so you must double a color in palette for uni color effect)
 *   follow_up_time_shift: number of 1/4 waittime between center and ring pulse
 */
void start_sparkle(float lamp_value, int steps_per_color, int color_palette_increment){
  #ifdef TR_PATTERN_SETTING
      Serial.print(F("TR_PATTERN_SETTING> start_sparkle:"));
      Serial.print(steps_per_color);Serial.print(',');
      Serial.println(color_palette_increment);
  #endif
  // init all globales for the pattern
  output_current_stepper_type=ST_SPARKLE;
  patvar_previous_step_time = output_get_current_beat_start_time();
  patvar_previous_fade_time = 0L;
  patvar_color_palette_index=0;
  
  patconf_pattern_lamp_value = lamp_value;
  patconf_steps_until_color_change=steps_per_color;
  patconf_color_palette_increment=color_palette_increment;
  patconf_fade_factor=0.90;

  for (int lamp_index = 0; lamp_index < LAMP_COUNT; ++lamp_index){
           lamp[lamp_index].set_value(0);
  }
  lamp[random(0,PIXEL_COUNT)].set_hsv(patconf_color_palette[patvar_color_palette_index].h,patconf_color_palette[patvar_color_palette_index].s,patconf_pattern_lamp_value);

  output_push_lamps_to_pixels();
}

void process_sparkle() {

  long current_time = millis();
  boolean pixel_update_necessary=false;

  if (current_time - patvar_previous_fade_time >= PATCONF_FADE_FRAMETIME) {  // calculate fading effect
    patvar_previous_fade_time=current_time;
    pixel_update_necessary=true;
     for (int lamp_index = 0; lamp_index < LAMP_COUNT; ++lamp_index){
       if(lamp[lamp_index].get_saturation()<1.0) {
         lamp[lamp_index].add_saturation(0.1);
        } 
        lamp[lamp_index].multiply_value(patconf_fade_factor);
      } // loop over lamp ring
  }  // end of fading calculation

   
   if ((current_time - patvar_previous_step_time >= output_waittime[patconf_speed_id])||   
        output_beat_sync_happened){                                                        // next step
          
      if(output_beat_sync_happened) {    //clean init of step 
          patvar_previous_step_time=output_beat_sync_time;  // new Start
          patvar_current_step_index=0;                      // Start with even step
          patvar_step_in_color_index=patconf_steps_until_color_change; // Trigger color change
      }else {
          patvar_previous_step_time = current_time;
          ++patvar_current_step_index;   // Step index will not be reset (would take 32000 steps to overflow)
      }
      
      if (++patvar_step_in_color_index>=patconf_steps_until_color_change) {  // Forward color pallette if necessary
            patvar_step_in_color_index=0;
            patvar_color_palette_index+=patconf_color_palette_increment;
            while(patvar_color_palette_index>=patconf_color_palette_lenght) patvar_color_palette_index-=patconf_color_palette_lenght;
      } 

      byte random_component=random((patconf_color_palette_lenght>3?3:patconf_color_palette_lenght));
      #ifdef TR_OUT_SPARKLE_DETAILS
        Serial.print(F("TR_OUT_SPARKLE_DETAILS> random_component:"));
        Serial.println(random_component);
      #endif
      byte random_color_index=patvar_color_palette_index+random_component;
      while(random_color_index>=patconf_color_palette_lenght) random_color_index-=patconf_color_palette_lenght;
      lamp[random(0,PIXEL_COUNT)].set_hsv(patconf_color_palette[random_color_index].h,patconf_color_palette[random_color_index].s,patconf_pattern_lamp_value);
   }

   // Finally display the result
   if(pixel_update_necessary) output_push_lamps_to_pixels();       
}


/*
 *   Pulse: alternating "Ignite" of ring and center, ramping up saturation first and then fade to black
 *   Keeping ring step on 2nd subbeat (depend on patconv_speed-id)
 *   lamp_value: general brightness
 *   steps_per_color: number of steps to change color index (index alway increments by 2, so you must double a color in palette for uni color effect)
 *   fade_factor: brightnes to keep after 50ms
 *   follow_up_time_shift: number of 1/4 waittime between center and ring pulse
 */
void start_pulse(float lamp_value, int steps_per_color, float fade_factor, int follow_up_ticks){
  #ifdef TR_PATTERN_SETTING
      Serial.print(F("TR_PATTERN_SETTING> start_pulse:"));
      Serial.print(steps_per_color);Serial.print(',');
      Serial.print(fade_factor);Serial.print(',');
      Serial.println(follow_up_ticks);
  #endif
  // init all globales for the pattern
  output_current_stepper_type=ST_PULSE;
  patvar_previous_step_time = output_get_current_beat_start_time();
  patvar_previous_fade_time = 0L;
  
  patconf_pattern_lamp_value = lamp_value;
  patconf_fade_factor = fade_factor;
  patconf_steps_until_color_change=steps_per_color;
  patconf_follow_up_ticks=follow_up_ticks;
 
  patvar_current_step_index = 0;
  patvar_color_palette_index=0;
  patvar_step_in_color_index=0;
  
  lamp[0].set_value(0.0);
  for (int lamp_index = 1; lamp_index < LAMP_COUNT; ++lamp_index){
           lamp[lamp_index].set_hsv(patconf_color_palette[patvar_color_palette_index].h,patconf_color_palette[patvar_color_palette_index].s,patconf_pattern_lamp_value);
  }
  patconf_step_0_waittime=follow_up_ticks*output_waittime[patconf_speed_id]/4;
  patconf_step_1_waittime=output_waittime[patconf_speed_id]*2-patconf_step_0_waittime;
  #ifdef TR_OUT_TIMING
    Serial.print(F(">TR_OUT_TIMING patconf_step_0_waittime ")); Serial.println(patconf_step_0_waittime);
    Serial.print(F(">TR_OUT_TIMING patconf_step_1_waittime ")); Serial.println(patconf_step_1_waittime);
  #endif
  output_push_lamps_to_pixels();
}

void process_pulse() {

  long current_time = millis();
  boolean pixel_update_necessary=false;

  if (current_time - patvar_previous_fade_time >= PATCONF_FADE_FRAMETIME) {  // calculate fading effect
    patvar_previous_fade_time=current_time;
    pixel_update_necessary=true;
     for (int lamp_index = 0; lamp_index < LAMP_COUNT; ++lamp_index){
       if(lamp[lamp_index].get_saturation()<1.0) {
         lamp[lamp_index].add_saturation(0.1);
        } 
        lamp[lamp_index].multiply_value(patconf_fade_factor);
      } // loop over lamp ring
  }  // end of fading calculation

   
   if ((patvar_current_step_index%2==0 && (current_time - patvar_previous_step_time >= patconf_step_0_waittime))||   
       (patvar_current_step_index%2==1 && (current_time - patvar_previous_step_time >= patconf_step_1_waittime))||
        output_beat_sync_happened){                                                                                       // next step

      if(output_beat_sync_happened) {    //clean init of step 
          patvar_previous_step_time=output_beat_sync_time;  // new Start
          patvar_current_step_index=0;                      // Start with even step
          patvar_step_in_color_index=patconf_steps_until_color_change; // Trigger color change
      }else {
          patvar_previous_step_time = current_time;
          ++patvar_current_step_index;   // Step index will not be reset (would take 32000 steps to overflow)
      }
      
      pixel_update_necessary=true;
      if (++patvar_step_in_color_index>=patconf_steps_until_color_change) {  // Forward color pallette if necessary
            patvar_step_in_color_index=0;
            patvar_color_palette_index+=2;
            if (patvar_color_palette_index>=patconf_color_palette_lenght) {
              patvar_color_palette_index-=(patconf_color_palette_lenght);
              if(patvar_color_palette_index>=patconf_color_palette_lenght)patvar_color_palette_index=0;
            }
      }

      if(patvar_current_step_index%2==0) {  // ignite ring
         for (int lamp_index = 1; lamp_index < LAMP_COUNT; ++lamp_index){
            lamp[lamp_index].set_hsv(patconf_color_palette[patvar_color_palette_index].h,patconf_color_palette[patvar_color_palette_index].s,patconf_pattern_lamp_value*0.6);
         }
         // Recalculate waittimes if speed changed
         patconf_step_0_waittime=patconf_follow_up_ticks*output_waittime[patconf_speed_id]/4;
         patconf_step_1_waittime=output_waittime[patconf_speed_id]*2-patconf_step_0_waittime;
      } else { // ignite center
         int follow_up_palette_entry= patvar_color_palette_index+1;
         if( follow_up_palette_entry>=patconf_color_palette_lenght)follow_up_palette_entry-=patconf_color_palette_lenght;
         lamp[0].set_hsv(patconf_color_palette[follow_up_palette_entry].h,patconf_color_palette[follow_up_palette_entry].s,patconf_pattern_lamp_value);
      }
   }
     
  if(pixel_update_necessary) output_push_lamps_to_pixels();  
}

/*
 *   Color Wipe Stepper
 *   Light up one dot in the ring every step. Then sets them off step by step.
 */
// 
void start_colorWipe(float lamp_value, boolean over_black){
  // init all globales for the pattern
  #ifdef TR_PATTERN_SETTING
      Serial.print(F("TR_PATTERN_SETTING> start_colorWipe:"));
      Serial.println(over_black);
  #endif
  output_current_stepper_type=ST_COLOR_WIPE;

  patvar_previous_step_time = output_get_current_beat_start_time() ;
  patvar_current_step_index = 0;
  patconf_max_step_count=over_black ?  (LAMP_COUNT - 1) * 2 : (LAMP_COUNT - 1) ; 

  patconf_pattern_lamp_value = lamp_value;
  patvar_color_palette_index=0;

  // init all lamps
  lamp[0].set_value(0.0); // Switch of center lamp
  // init lamps
  for (int i = 1; i < PIXEL_COUNT; i++) if (over_black) lamp[i].set_hsv(patconf_color_palette[patvar_color_palette_index].h, patconf_color_palette[patvar_color_palette_index].s, 0.0);
                                        else lamp[i].set_hsv(patconf_color_palette[patconf_color_palette_lenght-1].h, patconf_color_palette[patconf_color_palette_lenght-1].s, 1.0);
  output_push_lamps_to_pixels(); 
}

void process_colorWipe() {
  long current_step_time = millis();
  int wait_interval= patconf_speed_id==STEP_ON_2BEATS? output_waittime[patconf_speed_id]:output_waittime[patconf_speed_id]/3;

  if ((current_step_time - patvar_previous_step_time > wait_interval)||output_beat_sync_happened) {

    if(output_beat_sync_happened) {    //clean init of step 
      #ifdef TR_OUT_BEAT_TRACKING
          Serial.println(F("TR_OUT_BEAT_TRACKING> process_colorWipe has beat_sync"));
      #endif 
        patvar_previous_step_time=output_beat_sync_time;  // new Start
        patvar_current_step_index=0; // Start with next step
        if (++patvar_color_palette_index>=patconf_color_palette_lenght) patvar_color_palette_index=0; // Trigger color change
        patvar_step_in_color_index=0; 
    }else {
        patvar_previous_step_time = current_step_time;
    }
  
    // Determine the lamp, this step will change
    int lamp_index = LAMP_COUNT - (patvar_current_step_index % (LAMP_COUNT - 1)) - 1;
  
    // Depending on phase, switch on or off
    if (patvar_current_step_index < LAMP_COUNT - 1)
      lamp[lamp_index].set_hsv(patconf_color_palette[patvar_color_palette_index].h, patconf_color_palette[patvar_color_palette_index].s, patconf_pattern_lamp_value);
    else
      lamp[lamp_index].set_value(0.0);
  
    // Pattern complete
    output_push_lamps_to_pixels();
    
    // Step foreward
    if (++patvar_current_step_index >= patconf_max_step_count) {
      patvar_current_step_index = 0;
      // Change color after complete cycle
      if (++patvar_color_palette_index>=patconf_color_palette_lenght) patvar_color_palette_index=0;
    }
  }  // end if step time reached
}

/*
 *   Double Orbit
 */
void start_doubleOrbit(float lamp_value,  int steps_per_color){
    #ifdef TR_PATTERN_SETTING
      Serial.print(F("TR_PATTERN_SETTING> start_doubleOrbit:"));
      Serial.println(steps_per_color);
  #endif
  // init all globales for the pattern
  output_current_stepper_type=ST_DOUBLE_ORBIT;
  patvar_previous_step_time = output_get_current_beat_start_time();
  patvar_current_step_index = 0;
  patvar_step_in_color_index=0;

  patconf_steps_until_color_change=steps_per_color;

  patconf_pattern_lamp_value = lamp_value;
  patvar_color_palette_index=0;

  // init all lamps
  lamp[0].set_value(0.0); // Switch of center lamp
  // Preset color, but set value to 0
  for (int i = 1; i < PIXEL_COUNT; i++) lamp[i].set_hsv(patconf_color_palette[patvar_color_palette_index].h, patconf_color_palette[patvar_color_palette_index].s, 0.0);
  output_push_lamps_to_pixels(); 
}

void process_doubleOrbit() {

  long current_step_time = millis();
  if (current_step_time - patvar_previous_step_time < output_waittime[patconf_speed_id]) return;
  patvar_previous_step_time = current_step_time;

  int angle = patvar_current_step_index % 3; // angle, where the pixel is on

  for (int lamp_index = 1; lamp_index < LAMP_COUNT; ++lamp_index) {
    if (lamp_index % 3 == angle) lamp[lamp_index].set_hsv(patconf_color_palette[patvar_color_palette_index].h, patconf_color_palette[patvar_color_palette_index].s, patconf_pattern_lamp_value);
    else  lamp[lamp_index].set_value(0.0);
  }

  // center pixel lightens every 4th step for 2 ticks
  if (patvar_current_step_index % 8 == 0 || patvar_current_step_index % 8 == 1) lamp[0].set_hsv(patconf_color_palette[patvar_color_palette_index].h, patconf_color_palette[patvar_color_palette_index].s, patconf_pattern_lamp_value);
  else  lamp[0].set_value(0.0);

  output_push_lamps_to_pixels();

  // step foreward;
  
  if (++patvar_current_step_index >= 24) patvar_current_step_index = 0; // Whole sequence has 24 step because of the blinking
  if (++patvar_step_in_color_index>=patconf_steps_until_color_change) {
        patvar_step_in_color_index=0;
        if (++patvar_color_palette_index>=patconf_color_palette_lenght) patvar_color_palette_index=0;
  }
}

/*
 *   Color Orbit (triple coik
 *   
 */
void start_colorOrbit(float lamp_value,  int steps_per_color, int color_palette_increment){
  #ifdef TR_PATTERN_SETTING
      Serial.print(F("TR_PATTERN_SETTING> start_colorOrbit:"));
      Serial.print(steps_per_color);Serial.print(',');
      Serial.println(color_palette_increment);
  #endif
  // init all globales for the pattern
  output_current_stepper_type=ST_COLOR_ORBIT;
  patconf_steps_until_color_change=steps_per_color;
  patvar_previous_step_time = output_get_current_beat_start_time();
  patvar_current_step_index = 0;
  patvar_step_in_color_index=0;
  patconf_color_palette_increment=color_palette_increment;

  patconf_pattern_lamp_value = lamp_value;
  patvar_color_palette_index=0;

  // init all lamps
  lamp[0].set_value(0.0); // Switch of center lamp
  // Preset color, but set value to 0
  for (int i = 1; i < PIXEL_COUNT; i++) lamp[i].set_hsv(patconf_color_palette[patvar_color_palette_index].h, patconf_color_palette[patvar_color_palette_index].s, 0.0);
}

void process_colorOrbit() {

  long current_step_time = millis();
  if (current_step_time - patvar_previous_step_time < output_waittime[patconf_speed_id]) return;
  patvar_previous_step_time = current_step_time;

  int angle_a = patvar_current_step_index % 6; // angle, where the first color is shown
  int angle_b = (angle_a + 3)%6; // angle where the second color is shown
  int color_index_c=patvar_color_palette_index+1;
  if(color_index_c>=patconf_color_palette_lenght)color_index_c=0;
  int color_index_b=color_index_c+1;
  if(color_index_b>=patconf_color_palette_lenght)color_index_b=0;

  #ifdef TR_COLOR_ORBIT_COLOR
   Serial.print(F("TR_COLOR_ORBIT_COLOR> colorr index (c,a,b):"));
   Serial.print(color_index_c);Serial.print(',');
   Serial.print(patvar_color_palette_index);Serial.print(',');
   Serial.println(color_index_b);
  #endif

  // center pixel lightens every 4th step for 2 ticks with 1st color
  if (patvar_current_step_index % 8 == 0 || patvar_current_step_index % 8 == 1) {
    lamp[0].set_hsv(patconf_color_palette[color_index_c].h, patconf_color_palette[color_index_c].s, patconf_pattern_lamp_value);
  }
  else  lamp[0].set_value(0.0);

  // Orbit pixels user 2rd and 3rd color in palette order
  for (int lamp_index = 1; lamp_index < LAMP_COUNT; ++lamp_index) {
    if ((lamp_index-1) == angle_a) lamp[lamp_index].set_hsv(patconf_color_palette[patvar_color_palette_index].h, patconf_color_palette[patvar_color_palette_index].s, patconf_pattern_lamp_value);
    else if ((lamp_index-1) == angle_b) lamp[lamp_index].set_hsv(patconf_color_palette[color_index_b].h, patconf_color_palette[color_index_b].s, patconf_pattern_lamp_value);
    else  lamp[lamp_index].set_value(0.0);
  }



  output_push_lamps_to_pixels();

  // step foreward;
  
  if (++patvar_current_step_index >= 24) patvar_current_step_index = 0; // Whole sequence has 24 step because of the blinking
  if (++patvar_step_in_color_index>=patconf_steps_until_color_change) {
        patvar_step_in_color_index=0;
        patvar_color_palette_index+=patconf_color_palette_increment;
        while(patvar_color_palette_index>=patconf_color_palette_lenght) patvar_color_palette_index-=patconf_color_palette_lenght;
  }
}

/*
 *  rainbow
 */

 void start_rainbow(float lamp_value,  float angle_difference, float angle_step){
  // init all globales for the pattern
  #ifdef TR_PATTERN_SETTING
      Serial.print(F("TR_PATTERN_SETTING> start_rainbow:"));
      Serial.print(angle_difference);Serial.print(',');
      Serial.println(angle_step);
  #endif
  
  output_current_stepper_type=ST_RAINBOW;
     patvar_previous_step_time = output_get_current_beat_start_time();
 
  patconf_rainbow_step_angle_increment=angle_step;
   
  patconf_pattern_lamp_value = lamp_value;
  
  // init all lamps
  lamp[0].set_value(0.0); // Switch of center lamp
  // Preset color, but set value to 0
   float hue=0;
  for (int i = 1; i < PIXEL_COUNT; i++) 
  {
    lamp[i].set_hsv(hue, 1.0, 1.0);
     hue+=angle_difference;
  }
}

void process_rainbow() {

  long current_step_time = millis();
  if (current_step_time - patvar_previous_step_time <  output_waittime[patconf_speed_id]) return;
  patvar_previous_step_time = current_step_time;

  for (int lamp_index = 1; lamp_index < LAMP_COUNT; ++lamp_index) {
    lamp[lamp_index].add_hue_angle(patconf_rainbow_step_angle_increment);
  }

  output_push_lamps_to_pixels();

}

/*
 *  Helper functions
 */

void output_push_lamps_to_pixels()
{
  t_color_rgb_int lamp_color;
  for (int p = 0; p < PIXEL_COUNT; p++) {
    lamp_color = lamp[p].get_color_rgb( output_master_light_value);
    strip.setPixelColor(p, strip.Color(lamp_color.r, lamp_color.g, lamp_color.b ));
    
#ifdef TR_OUT_PIXEL_RESULT
    Serial.print(F("TR_OUT_PIXEL_RESULT> hsv "));
    lamp[p].trace_hsv();
    Serial.print(F("\nTR_OUT_PIXEL_RESULT> rgb "));
    Serial.print(lamp_color.r); Serial.print(F(","));
    Serial.print(lamp_color.g); Serial.print(F(","));
    Serial.println(lamp_color.b);
#endif

  }
  strip.show();
}

#ifdef TR_COLOR_PALETTE_SETTING
void dump_color_palette_to_serial()
{
  Serial.println(F("TR_COLOR_PALETTE_SETTING>")); 
  for(int i=0;i<patconf_color_palette_lenght;i++) {
    Serial.print('(');Serial.print(patconf_color_palette[i].h);
    Serial.print(',');Serial.print(patconf_color_palette[i].s);
    Serial.println(')');
  }
}
#endif

