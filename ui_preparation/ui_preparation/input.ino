/* Functions to handle all input elements */

#include "mainSettings.h"


// Activate general trace output

#ifdef TRACE_ON
#define TR_INP 
 #define TR_INP_HIGH
//#define TR_INP_TIMING 
#endif



/* Button constants */ 
const byte switch_pin_list[] = {4,    // Mode Select  D2 on ESP8266 / Node MCU
                                5    // Step          D1 on ESP8266 / Node MCU
                               };
                                
#define INPUT_PORT_COUNT sizeof(switch_pin_list)

#define INPUT_BUTTON_COOLDOWN 10   // Milliseconds until we accept another state change of the buttons



/* Button variables */

unsigned long buttons_last_read_time=0;  // Millis of last butto ncheck
unsigned int button_raw_state=0;         // state of buttons in last button check
unsigned int button_tick_state = 0;      // current and historized state in the actual tick



/* Generic button bit pattern (Button 0) */

#define INPUT_0_BITS 0x0003
#define INPUT_0_IS_ON_PATTERN 0x0003
#define INPUT_0_SWITCHED_ON_PATTERN 0x0001
#define INPUT_0_SWITCHED_OFF_PATTERN 0x0002


/* Element specific  button bit pattern */
/*                                         76543210 */

#define INPUT_BUTTON_A_BITS              0x0003
#define INPUT_BUTTON_A_IS_PRESSED_PATTERN   0x0003
#define INPUT_BUTTON_A_GOT_PRESSED_PATTERN   0x0001
#define INPUT_BUTTON_A_GOT_RELEASED_PATTERN  0x0002


#define INPUT_BUTTON_B_BITS                  0x000c
#define INPUT_BUTTON_B_IS_PRESSED_BIT        0x0004
#define INPUT_BUTTON_B_GOT_PRESSED_PATTERN   0x0004
#define INPUT_BUTTON_B_GOT_RELEASED_PATTERN  0x0008

#define INPUT_ALL_BUTTON_STATE_MASK 0x000f


/* history handling */

#define INPUT_CURRENT_BITS 0x0005
#define INPUT_PREVIOUS_BITS 0x000a


unsigned long input_last_press_start_time=0;  //Last System time, a button got pressed
unsigned long input_last_press_end_time=0;  // Last System time, a button got releasd
bool input_enabled=true;

/* General state variables */
volatile bool setupComplete = false;
unsigned long input_last_change_time = 0;

/* Serial input management */
String input_serialBuffer="";
String input_currentSerialCommand="";
boolean input_serialCommand_isNew=false;

/* ********************************************************************************************************** */
/*               Interface functions                                                                          */

/* ------------- General Information --------------- */

int input_getSecondsSinceLastEvent() {
  unsigned long timestamp_difference = (millis() - input_last_change_time) / 1000;
  if (timestamp_difference > 255) return 255;
#ifdef TR_INP_TIMING
  Serial.print(F("TR_INP_TIMING>input last interaction:"));
  Serial.println(timestamp_difference);
#endif
  return timestamp_difference;
}

/* ------------- Button events --------------- */

bool input_selectGotPressed()
{

  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_GOT_PRESSED_PATTERN);
}

bool input_selectIsPressed()
{
  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_IS_PRESSED_PATTERN); 
}

byte input_selectGotReleased()
{
  return input_enabled && ((button_tick_state & INPUT_BUTTON_A_BITS) == INPUT_BUTTON_A_GOT_RELEASED_PATTERN); 
}

bool input_stepGotPressed()
{

  return input_enabled && ((button_tick_state & INPUT_BUTTON_B_BITS) == INPUT_BUTTON_B_GOT_PRESSED_PATTERN);
}

bool input_stepIsPressed()
{
  return input_enabled && (button_tick_state & INPUT_BUTTON_B_IS_PRESSED_BIT ); 
}

byte input_stepGotReleased()
{
  return input_enabled && ((button_tick_state & INPUT_BUTTON_B_BITS) == INPUT_BUTTON_B_GOT_RELEASED_PATTERN); 
}

long input_getCurrentPressDuration()
{
  #ifdef TR_INP_TIMING
    Serial.print(F("TR_INP_TIMING>input CurrentPressDuration:"));
    Serial.println(millis()-input_last_press_start_time);
  #endif
    
  return millis()-input_last_press_start_time;
}

long input_getLastPressDuration()
{
  return  input_last_press_end_time-input_last_press_start_time;
}

unsigned long input_getLastPressStartTime()
{
  return  input_last_press_start_time;
}

unsigned long input_getLastPressEndTime()
{
  return  input_last_press_end_time;
}


/* ---- Serial Data Input ---- */

boolean input_newSerialCommandAvailable() {return input_serialCommand_isNew;}

String input_getSerialCommand() 
{
    input_serialCommand_isNew=false;
    return input_currentSerialCommand;
}

/* ------------- Operations ----------------- */



/* Disable input until all buttons have been released */

void input_IgnoreUntilRelease()
{
  input_enabled=false;
}

/* *************************** internal implementation *************************** */

 



/* ************************************* TICK ***********************************************
   translate the state of buttons into the ticks of the master loop
   Must be called by the master loop for every cycle to provide valid event states of
   all input devices.
*/

void input_switches_scan_tick()
{
  bool change_happened=false;
  long frame_time=millis();
  
  /* regular button scan  */
  if (frame_time - buttons_last_read_time > INPUT_BUTTON_COOLDOWN)
  {
    byte isPressed=0;
    int analog_value=0;
    buttons_last_read_time = frame_time;
    
    for (int i = 0; i <INPUT_PORT_COUNT; i++) { // for all input ports configured
      isPressed=!digitalRead(switch_pin_list[i]);
      bitWrite(button_raw_state,i*2,isPressed);
    }

  }

  /* copy previous tick  state to history bits  and take raw pattern as new value */
  button_tick_state = (button_tick_state & INPUT_CURRENT_BITS) << 1
               | (button_raw_state & INPUT_CURRENT_BITS);

  
  /* Track pressing time */
  for (int i =0;i<INPUT_PORT_COUNT;i++)
  {
    if((button_tick_state & (INPUT_0_BITS<<(i*2))) == INPUT_0_SWITCHED_ON_PATTERN<<(i*2)) 
    {
      change_happened=true;
      input_last_press_end_time =input_last_press_start_time=frame_time;
      #ifdef TR_INP_HIGH
        Serial.print(("TR_INP_HIGH>press of "));Serial.println(i);
      #endif     
    }
    if((button_tick_state & (INPUT_0_BITS<<(i*2))) == INPUT_0_SWITCHED_OFF_PATTERN<<(i*2)) 
    {
      input_last_press_end_time=frame_time;
      change_happened=true;
      #ifdef TR_INP_HIGH
        Serial.print(("TR_INP_HIGH>release of "));Serial.println(i);
      #endif     
    }
  }

  if((button_tick_state & INPUT_ALL_BUTTON_STATE_MASK) ==0x00)  input_enabled=true; // enable input when all is released and settled

  if(change_happened)input_last_change_time =frame_time; // Reset the globel age of interaction


} // void input_switches_tick()

/* ************** Manage Serial input ***************** */

void input_pollSerial() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    input_serialBuffer += inChar;
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
    }
  }
}

/* ***************************       S E T U P           ******************************
*/

void input_setup() {

  /* Initialize switch pins and raw_state_register array */
  for (byte switchIndex = 0; switchIndex < INPUT_PORT_COUNT ; switchIndex++) {
    pinMode(switch_pin_list[switchIndex], INPUT_PULLUP);
  }
  delay(INPUT_BUTTON_COOLDOWN*2);  // ensure, systemtime is behind Button Cooldown
  setupComplete = true;
}

