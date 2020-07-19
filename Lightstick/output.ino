#include <Adafruit_NeoPixel.h>
#include "lamp.h"

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

Lamp lamp[PIXEL_COUNT];

uint32_t g_color_black=strip.Color(0, 0, 0);


int g_pattern_type_running =0;
boolean g_pattern_needs_init=false;
long g_pattern_start_time=0L;
long g_pattern_previous_step_time=0L;
int g_pattern_step_index=0;


#define WAIT_2BEATS 0
#define WAIT_BEAT   1
#define WAIT_8TH    2
#define WAIT_16TH   3
#define WAIT_32TH   4
#define WAIT_64TH   4

int g_output_waittime[5];
int g_beats_per_minute=120;

void output_setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  digitalWrite(LED_BUILTIN,HIGH);
  output_set_bpm(120);
}

void output_set_bpm(int beats_per_minute) {
  g_beats_per_minute=beats_per_minute;
  g_output_waittime[WAIT_2BEATS]=120000 / g_beats_per_minute;
  g_output_waittime[WAIT_BEAT]=g_output_waittime[WAIT_2BEATS]>>1;
  g_output_waittime[WAIT_8TH]=g_output_waittime[WAIT_BEAT]>>1;
  g_output_waittime[WAIT_16TH]=g_output_waittime[WAIT_8TH]>>1;
  g_output_waittime[WAIT_32TH]=g_output_waittime[WAIT_16TH]>>1;
  g_output_waittime[WAIT_64TH]=g_output_waittime[WAIT_32TH]>>1;
}

void output_start_pattern(int pattern_selected) {
  g_pattern_needs_init=true;
  g_pattern_type_running=pattern_selected;
  output_process_pattern();
}

void output_process_pattern() {
  switch(g_pattern_type_selected){
    case 0: process_colorWipe(strip.Color(10, 10, 0), g_output_waittime[WAIT_BEAT]);    // Very Low
            break;
    case 1: process_colorWipe(strip.Color(255, 0, 0), g_output_waittime[WAIT_8TH]);  // Red
            break;
    case 2: process_colorWipe(strip.Color(0, 255, 0),g_output_waittime[WAIT_16TH]);  // Green
            break;
    case 3: process_colorWipe(strip.Color(0, 0, 255), g_output_waittime[WAIT_32TH]);  // Blue
            break;
    case 4: process_theaterChase(strip.Color(127, 127, 127), g_output_waittime[WAIT_16TH]); // White
            break;
    case 5: process_theaterChase(strip.Color(127,   0,   0), g_output_waittime[WAIT_32TH]); // Red
            break;
    case 6: process_theaterChase(strip.Color(  0,   0, 127), g_output_waittime[WAIT_8TH]); // Blue
            break;
/*    case 7: process_rainbow(20);
            break;
    case 8: process_rainbowCycle(20);
            break;
    case 9: process_theaterChaseRainbow(50);
            break;
            */
  }
}

// Fill the dots one after the other with a color and fill with black afterwards
void process_colorWipe(uint32_t c, int wait) {
  if(g_pattern_needs_init) { // Init pattern when switchting
    g_pattern_needs_init=false;
    g_pattern_type_running=g_pattern_type_selected;
    g_pattern_start_time=millis();
    g_pattern_previous_step_time=0L;
    g_pattern_step_index=0;
    strip.setPixelColor(0,g_color_black); // Switch of center Pixel
  }
  wait/=6;
  long current_step_time=millis();
  if(current_step_time-g_pattern_previous_step_time<wait) return;
  g_pattern_previous_step_time=current_step_time;

  int pixel_index=PIXEL_COUNT-(g_pattern_step_index%(PIXEL_COUNT-1))-1;

  if(g_pattern_step_index<PIXEL_COUNT-1) 
        strip.setPixelColor(pixel_index, c);
  else
        strip.setPixelColor(pixel_index,g_color_black);
  strip.show();

  // Step foreward
  if(++g_pattern_step_index>=(PIXEL_COUNT-1)*2) g_pattern_step_index=0;

}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void process_theaterChase(uint32_t c, long wait) {
  if(g_pattern_needs_init) { // Init pattern when switchting
    g_pattern_needs_init=false;
    g_pattern_type_running=g_pattern_type_selected;
    g_pattern_start_time=millis();
    g_pattern_previous_step_time=0L;
    g_pattern_step_index=0;
  }
  long current_step_time=millis();
  if(current_step_time-g_pattern_previous_step_time<wait) return;
  g_pattern_previous_step_time=current_step_time;

  int angle=g_pattern_step_index%3; // angle, where the pixel is on
  
  for (int pixel_index=1; pixel_index< PIXEL_COUNT; ++pixel_index) {
        if(pixel_index%3==angle) strip.setPixelColor(pixel_index, c);
        else  strip.setPixelColor(pixel_index, g_color_black);
  }

  // center pixel lightens every 4th step
  if(g_pattern_step_index%8==0 || g_pattern_step_index%8==1) strip.setPixelColor(0, c);
  else  strip.setPixelColor(0, g_color_black);

  
  // pattern and step foreward
  strip.show();
  if(++g_pattern_step_index>=24) g_pattern_step_index=0;  // Whole sequence has 24 step
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
