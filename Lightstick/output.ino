#include <Adafruit_NeoPixel.h>
#include "lamp.h"

#ifdef TRACE_ON
// #define TRACE_OUPUT_PIXEL_RESULT
#endif

#define BUTTON_PIN   12    // Digital IO pin connected to the button.  This will be
// driven with a pull-up resistor so the switch should
// pull the pin to ground momentarily.  On a high -> low
// transition the button press logic will execute.

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.

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

#define COLOR_PALETTE_MAX_ENTRIES  4 
t_color_hs g_color_palette[COLOR_PALETTE_MAX_ENTRIES];
uint8_t g_color_palette_lenght=0;
uint8_t g_color_palette_index=0;

enum STEPPER_TYPE {
  ST_COLOR_WIPE,
  ST_DOUBLE_ORBIT,
  ST_RAINBOW
};

STEPPER_TYPE g_current_stepper_type=ST_COLOR_WIPE;

const uint32_t g_color_black = strip.Color(0, 0, 0);


int g_pattern_type_running = 0;
boolean g_pattern_needs_init = false;
long g_pattern_start_time = 0L;
long g_pattern_previous_step_time = 0L;
int g_pattern_step_index = 0;
uint8_t g_steps_per_color=0;
uint8_t g_step_in_color_index=0;
int g_max_step_count=1000;
int g_pattern_step_wait_interval=1000;
float g_pattern_value = 0.5;
float g_master_light_value = 0.5;
float g_rainbow_step_angle_increment=1.0;

#define WAIT_2BEATS 0
#define WAIT_BEAT   1
#define WAIT_8TH    2
#define WAIT_16TH   3
#define WAIT_32TH   4
#define WAIT_64TH   4

int g_output_waittime[5];
int g_beats_per_minute = 120;

/* 
 *  API Functions
 */

// Following function must be called in the setup
void output_setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  digitalWrite(LED_BUILTIN, HIGH);
  output_set_bpm(120);
}

// Change bpm
void output_set_bpm(int beats_per_minute) {
  g_beats_per_minute = beats_per_minute;
  g_output_waittime[WAIT_2BEATS] = 120000 / g_beats_per_minute;
  g_output_waittime[WAIT_BEAT] = g_output_waittime[WAIT_2BEATS] >> 1;
  g_output_waittime[WAIT_8TH] = g_output_waittime[WAIT_BEAT] >> 1;
  g_output_waittime[WAIT_16TH] = g_output_waittime[WAIT_8TH] >> 1;
  g_output_waittime[WAIT_32TH] = g_output_waittime[WAIT_16TH] >> 1;
  g_output_waittime[WAIT_64TH] = g_output_waittime[WAIT_32TH] >> 1;
}

// Select and start a pattern
void output_start_pattern(int pattern_selected) {
  g_pattern_type_running = pattern_selected;
  switch (g_pattern_type_selected) {
    case 0:
         g_color_palette[0].h=HUE_YELLOW;g_color_palette[0].s=1.0;
         g_color_palette_lenght=1;
         start_colorWipe(0.05,WAIT_BEAT,true);
         break;
    case 1:
         g_color_palette[0].h=HUE_RED;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_ORANGE;g_color_palette[1].s=1.0;
         g_color_palette[2].h=HUE_RED;g_color_palette[2].s=0.8;
         g_color_palette_lenght=3;
         start_colorWipe(0.5,WAIT_8TH,true);
         break;
    case 2:
         g_color_palette[0].h=HUE_GREEN;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_CYAN;g_color_palette[1].s=1.0; 
         g_color_palette[2].h=HUE_GREEN;g_color_palette[2].s=1.0;
         g_color_palette[3].h=HUE_LEMON;g_color_palette[3].s=1.0;
         g_color_palette_lenght=4;
         start_colorWipe(0.5,WAIT_2BEATS,false);
         break;
    case 3:
         g_color_palette[0].h=HUE_BLUE;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_CYAN;g_color_palette[1].s=0.1; // WHITE
         g_color_palette_lenght=2;
         start_colorWipe(0.5,WAIT_16TH,false);
         break;
    case 4:
         g_color_palette[0].h=HUE_BLUE;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_CYAN;g_color_palette[1].s=1.0; // WHITE
         g_color_palette_lenght=2;
         start_doubleOrbit(0.5,g_output_waittime[WAIT_BEAT],8);
         break;
    case 5:
         g_color_palette[0].h=HUE_YELLOW;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_BLUE;g_color_palette[1].s=1.0;
         g_color_palette[2].h=HUE_RED;g_color_palette[2].s=1.0; 
         g_color_palette[3].h=HUE_GREEN;g_color_palette[3].s=1.0; 
         g_color_palette_lenght=4;
         start_doubleOrbit(0.5,g_output_waittime[WAIT_16TH],32);
         break;
    case 6:
         g_color_palette[0].h=HUE_LEMON;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_CYAN;g_color_palette[1].s=1.0;
         g_color_palette[2].h=HUE_PINK;g_color_palette[2].s=0.7;  // PINK
         g_color_palette[3].h=HUE_ORANGE;g_color_palette[3].s=1.0; 
         g_color_palette_lenght=4;
         start_doubleOrbit(0.5,g_output_waittime[WAIT_32TH],32);
         break;
    case 7:
         start_rainbow(0.5,g_output_waittime[WAIT_BEAT],60.0,60.0); // Full span, hart stepping
         break;
    case 8:
         start_rainbow(0.5,g_output_waittime[WAIT_32TH],-60.0,1.0); // Full span, soft stepping
         break;
    case 9:
         start_rainbow(0.5,g_output_waittime[WAIT_32TH],10.0,1.0); // 1/6  span, soft stepping
         break;
    case 10:
         start_rainbow(0.5,g_output_waittime[WAIT_8TH],20.0,-15.0); // 1/3  cycle, medium reverse stepping
         break;
    case 11:
         start_rainbow(0.5,g_output_waittime[WAIT_2BEATS],0.0,100.0); // 1/3  cycle, medium reverse stepping
         break;
  }
  output_process_pattern();
}

// Following function must be called in the loop
void output_process_pattern() {
  switch (g_current_stepper_type) {
    case ST_COLOR_WIPE: process_colorWipe();      break;
    case ST_DOUBLE_ORBIT: process_doubleOrbit(); break;
    case ST_RAINBOW: process_rainbow();break;
  }
}

/*
 *   Color Wipe Stepper
 *   Light up one dot in the ring every step. Then sets them off step by step.
 */
// 
void start_colorWipe(float lamp_value, int wait_index, boolean over_black){
  // init all globales for the pattern
  g_pattern_start_time = millis();
  g_current_stepper_type=ST_COLOR_WIPE;
  if(wait_index==WAIT_2BEATS) g_pattern_step_wait_interval=g_output_waittime[WAIT_BEAT];
  else  g_pattern_step_wait_interval=g_output_waittime[wait_index]/3;
  g_pattern_previous_step_time = 0L;
  g_pattern_step_index = 0;
  g_max_step_count=over_black ?  (LAMP_COUNT - 1) * 2 : (LAMP_COUNT - 1) ; 

  g_pattern_value = lamp_value;
  g_color_palette_index=0;

  // init all lamps
  lamp[0].set_value(0.0); // Switch of center lamp
  // Preset color, but set value to 0
  for (int i = 1; i < PIXEL_COUNT; i++) lamp[i].set_hsv(g_color_palette[g_color_palette_index].h, g_color_palette[g_color_palette_index].s, 0.0);
}

void process_colorWipe() {
  long current_step_time = millis();
  if (current_step_time - g_pattern_previous_step_time < g_pattern_step_wait_interval) return;
  g_pattern_previous_step_time = current_step_time;

  // Determine the lamp, this step will change
  int lamp_index = LAMP_COUNT - (g_pattern_step_index % (LAMP_COUNT - 1)) - 1;

  // Depending on phase, switch on or off
  if (g_pattern_step_index < LAMP_COUNT - 1)
    lamp[lamp_index].set_hsv(g_color_palette[g_color_palette_index].h, g_color_palette[g_color_palette_index].s, g_pattern_value);
  else
    lamp[lamp_index].set_value(0.0);

  // Pattern complete
  output_push_lamps_to_pixels();
  
  // Step foreward
  if (++g_pattern_step_index >= g_max_step_count) {
    g_pattern_step_index = 0;
    // Change color after complete cycle
    if (++g_color_palette_index>=g_color_palette_lenght) g_color_palette_index=0;
  }
}

/*
 *   Double Orbit
 */
void start_doubleOrbit(float lamp_value, int wait, int steps_per_color){
  // init all globales for the pattern
  g_pattern_start_time = millis();
  g_current_stepper_type=ST_DOUBLE_ORBIT;
  g_pattern_step_wait_interval=wait;
  g_pattern_previous_step_time = 0L;
  g_pattern_step_index = 0;
  g_steps_per_color=steps_per_color;
  g_step_in_color_index=0;

  g_pattern_value = lamp_value;
  g_color_palette_index=0;

  // init all lamps
  lamp[0].set_value(0.0); // Switch of center lamp
  // Preset color, but set value to 0
  for (int i = 1; i < PIXEL_COUNT; i++) lamp[i].set_hsv(g_color_palette[g_color_palette_index].h, g_color_palette[g_color_palette_index].s, 0.0);
}

void process_doubleOrbit() {

  long current_step_time = millis();
  if (current_step_time - g_pattern_previous_step_time < g_pattern_step_wait_interval) return;
  g_pattern_previous_step_time = current_step_time;

  int angle = g_pattern_step_index % 3; // angle, where the pixel is on

  for (int lamp_index = 1; lamp_index < LAMP_COUNT; ++lamp_index) {
    if (lamp_index % 3 == angle) lamp[lamp_index].set_hsv(g_color_palette[g_color_palette_index].h, g_color_palette[g_color_palette_index].s, g_pattern_value);
    else  lamp[lamp_index].set_value(0.0);
  }

  // center pixel lightens every 4th step for 2 ticks
  if (g_pattern_step_index % 8 == 0 || g_pattern_step_index % 8 == 1) lamp[0].set_hsv(g_color_palette[g_color_palette_index].h, g_color_palette[g_color_palette_index].s, g_pattern_value);
  else  lamp[0].set_value(0.0);

  output_push_lamps_to_pixels();

  // step foreward;
  
  if (++g_pattern_step_index >= 24) g_pattern_step_index = 0; // Whole sequence has 24 step because of the blinking
  if (++g_step_in_color_index>=g_steps_per_color) {
        g_step_in_color_index=0;
        if (++g_color_palette_index>=g_color_palette_lenght) g_color_palette_index=0;
  }
}

/*
 *  rainbow
 */

 void start_rainbow(float lamp_value, int wait, float angle_difference, float angle_step){
  // init all globales for the pattern
  g_pattern_start_time = millis();
  g_current_stepper_type=ST_RAINBOW;
  g_pattern_step_wait_interval=wait;
  g_pattern_previous_step_time = 0L;
 
  g_rainbow_step_angle_increment=angle_step;
   
  g_pattern_value = lamp_value;
  
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
  if (current_step_time - g_pattern_previous_step_time < g_pattern_step_wait_interval) return;
  g_pattern_previous_step_time = current_step_time;

  for (int lamp_index = 1; lamp_index < LAMP_COUNT; ++lamp_index) {
    lamp[lamp_index].add_hue_angle(g_rainbow_step_angle_increment);
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
    lamp_color = lamp[p].get_color_rgb( g_master_light_value);
    strip.setPixelColor(p, strip.Color(lamp_color.r, lamp_color.g, lamp_color.b ));
    
#ifdef TRACE_OUPUT_PIXEL_RESULT
    Serial.print(F(">TRACE_OUPUT_PIXEL_RESULT hsv "));
    lamp[p].trace_hsv();
    Serial.print(F("\n>TRACE_OUPUT_PIXEL_RESULT rgb "));
    Serial.print(lamp_color.r); Serial.print(F(","));
    Serial.print(lamp_color.g); Serial.print(F(","));
    Serial.println(lamp_color.b);
#endif

  }
  strip.show();
}

