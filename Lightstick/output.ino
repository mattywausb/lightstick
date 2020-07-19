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
  ST_DOUBLE_ORBIT
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
int g_pattern_step_wait_interval=1000;
float g_pattern_value = 0.5;
float g_master_light_value = 0.5;

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
         start_colorWipe(0.05,g_output_waittime[WAIT_2BEATS]);
         break;
    case 1:
         g_color_palette[0].h=HUE_RED;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_ORANGE;g_color_palette[1].s=1.0;
         g_color_palette[2].h=HUE_RED;g_color_palette[2].s=0.8;
         g_color_palette_lenght=3;
         start_colorWipe(0.5,g_output_waittime[WAIT_8TH]);
         break;
    case 2:
         g_color_palette[0].h=HUE_GREEN;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_CYAN;g_color_palette[1].s=1.0; 
         g_color_palette[2].h=HUE_GREEN;g_color_palette[2].s=1.0;
         g_color_palette[3].h=HUE_LEMON;g_color_palette[3].s=1.0;
         g_color_palette_lenght=4;
         start_colorWipe(0.5,g_output_waittime[WAIT_16TH]);
         break;
    case 3:
         g_color_palette[0].h=HUE_BLUE;g_color_palette[0].s=1.0;
         g_color_palette[1].h=HUE_CYAN;g_color_palette[1].s=0.1; // WHITE
         g_color_palette_lenght=2;
         start_colorWipe(0.5,g_output_waittime[WAIT_BEAT]);
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
  }
  output_process_pattern();
}

// Following function must be called in the loop
void output_process_pattern() {
  switch (g_current_stepper_type) {
    case ST_COLOR_WIPE: process_colorWipe();      break;
    case ST_DOUBLE_ORBIT: process_doubleOrbit(); break;
  }
}

/*
 *   Color Wipe Stepper
 *   Light up one dot in the ring every step. Then sets them off step by step.
 */
// 
void start_colorWipe(float lamp_value, int wait){
  // init all globales for the pattern
  g_pattern_start_time = millis();
  g_current_stepper_type=ST_COLOR_WIPE;
  g_pattern_step_wait_interval=wait/6;
  g_pattern_previous_step_time = 0L;
  g_pattern_step_index = 0;

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
  if (++g_pattern_step_index >= (LAMP_COUNT - 1) * 2) {
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
 *  Theatre-style crawling lights with rainbow effect
 */

void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}


/*
 *   Rainbow stepper
 */
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
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

