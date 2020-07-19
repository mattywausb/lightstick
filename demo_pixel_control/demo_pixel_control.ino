// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN   12    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 7

#define TRACE_ON

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int g_show_type_selected = 0;
int g_show_type_running =0;
long g_show_start_time=0L;
long g_show_previous_step_time=0L;
int g_show_step_index=0;

int g_beats_per_minute=120;
long g_music_start_time=0;

long g_quater_note,g_eigths_note,g_16th_note,g_32th_note;

uint32_t g_color_black=strip.Color(0, 0, 0);

void setup() {
  #ifdef TRACE_ON 
    char compile_signature[] = "--- START (Build: " __DATE__ " " __TIME__ ") ---";   
    Serial.begin(9600);
    Serial.println(compile_signature); 
  #endif
  input_setup();
  calculate_musical_waittimes();
  pinMode(LED_BUILTIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  digitalWrite(LED_BUILTIN,HIGH);
}

void calculate_musical_waittimes() {
  g_quater_note=60000 / g_beats_per_minute;
  g_eigths_note=30000 / g_beats_per_minute;
  g_16th_note=15000 / g_beats_per_minute;
  g_32th_note=7500 / g_beats_per_minute;
}


void loop() {
  // Get current button state.
  input_switches_scan_tick();
  if (input_stepGotPressed()) {
      if (++g_show_type_selected > 6) g_show_type_selected=0;
      #ifdef TRACE_ON 
       Serial.print("#startShow ");Serial.println(g_show_type_selected);
      #endif
  }
  processShow();
}

void processShow() {
  switch(g_show_type_selected){
    case 0: process_colorWipe(strip.Color(10, 10, 0), g_quater_note);    // Very Low
            break;
    case 1: process_colorWipe(strip.Color(255, 0, 0), g_eigths_note);  // Red
            break;
    case 2: process_colorWipe(strip.Color(0, 255, 0), g_16th_note);  // Green
            break;
    case 3: process_colorWipe(strip.Color(0, 0, 255), g_32th_note);  // Blue
            break;
    case 4: process_theaterChase(strip.Color(127, 127, 127), g_16th_note); // White
            break;
    case 5: process_theaterChase(strip.Color(127,   0,   0), g_32th_note); // Red
            break;
    case 6: process_theaterChase(strip.Color(  0,   0, 127), g_eigths_note); // Blue
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

// Fill the dots one after the other with a color and fill with back afterwards
void process_colorWipe(uint32_t c, long wait) {
  if(g_show_type_running!=g_show_type_selected) { // Init Show when switchting
    g_show_type_running=g_show_type_selected;
    g_show_start_time=millis();
    g_show_previous_step_time=0L;
    g_show_step_index=0;
    strip.setPixelColor(0,g_color_black); // Switch of center Pixel
  }
  wait/=6;
  long current_step_time=millis();
  if(current_step_time-g_show_previous_step_time<wait) return;
  g_show_previous_step_time=current_step_time;

  int pixel_index=PIXEL_COUNT-(g_show_step_index%(PIXEL_COUNT-1))-1;

  if(g_show_step_index<PIXEL_COUNT-1) 
        strip.setPixelColor(pixel_index, c);
  else
        strip.setPixelColor(pixel_index,g_color_black);
  strip.show();

  // Step foreward
  if(++g_show_step_index>=(PIXEL_COUNT-1)*2) g_show_step_index=0;

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
  if(g_show_type_running!=g_show_type_selected) { // Init Show when switchting
    g_show_type_running=g_show_type_selected;
    g_show_start_time=millis();
    g_show_previous_step_time=0L;
    g_show_step_index=0;
  }
  long current_step_time=millis();
  if(current_step_time-g_show_previous_step_time<wait) return;
  g_show_previous_step_time=current_step_time;

  int angle=g_show_step_index%3; // angle, where the pixel is on
  
  for (int pixel_index=1; pixel_index< PIXEL_COUNT; ++pixel_index) {
        if(pixel_index%3==angle) strip.setPixelColor(pixel_index, c);
        else  strip.setPixelColor(pixel_index, g_color_black);
  }

  // center pixel lightens every 4th step
  if(g_show_step_index%8==0 || g_show_step_index%8==1) strip.setPixelColor(0, c);
  else  strip.setPixelColor(0, g_color_black);

  
  // Show and step foreward
  strip.show();
  if(++g_show_step_index>=24) g_show_step_index=0;  // Whole sequence has 24 step
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
