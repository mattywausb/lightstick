/* Compile against  NodeMCU 1.0 Profile */

/*
 * Demonstrate using an http server and an HTML form to control an LED.
 * The http server runs on the ESP8266.
 *
 * Connect to "http://esp8266WebForm.local" or "http://<IP address>"
 * to bring up an HTML form to control the LED connected GPIO#0. This works
 * for the Adafruit ESP8266 HUZZAH but the LED may be on a different pin on
 * other breakout boards.
 *
 * Imperatives to turn the LED on/off using a non-browser http client.
 * For example, using wget.
 * $ wget http://esp8266webform.local/ledon
 * $ wget http://esp8266webform.local/ledoff
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "wifi_credentials.h"
#include "webui.h"
#include "mainSettings.h"

#ifdef TRACE_ON
#define TRACE_WEBUI
// #define TRACE_WEBUI_PAGE_GENERATION
#endif

/* Macro to declare Strings with reserved buffer */
#define DECLARE_PREALLOCATED_STRING(varname, alloc_size) String varname((const char*)nullptr); varname.reserve(alloc_size)

constexpr unsigned SMALL_STR = 64-1;
constexpr unsigned MED_STR = 256-1;
constexpr unsigned LARGE_STR = 512-1;
constexpr unsigned XLARGE_STR = 1024-1;

//  SSID and password are defined in header file that will not be in the git repo
const char* ssid = STASSID;
const char* password = STAPSK;

MDNSResponder mdns;

ESP8266WebServer server(80);

t_webui_command webui_command=NONE;
int webui_data_bpm=0;

boolean webui_has_new_command() { return webui_command!=NONE; }

t_webui_command webui_read_command() { 
  t_webui_command current_command=webui_command;
  webui_command=NONE;
  return current_command;
 }

int webui_data_get_bpm() { return webui_data_bpm;}

/* HTML Header */ 

const char WEB_PAGE_HEADER[] PROGMEM = "<!DOCTYPE html><html>"
"<head> <title>Lightstick</title> "
"<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
"<meta name=\"viewport\" content=\"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<link href=\"default.css\" rel=\"stylesheet\" type=\"text/css\">"
"</head><body>";

const char TXT_CONTENT_TYPE_TEXT_HTML[] PROGMEM = "text/html; charset=utf-8";

void sendStylesheet() {
   server.send(200, "text/css", F(
    "body {   font-family: \"Open Sans\", \"Arial\", \"mono\";  color:#ffe036;  background-color: #18006a;  font-size: 14px;  text-align: left; }"
"h1 {   text-align: center; }"
"label {  float: left;     margin: 2px 2px;     padding: 10px 10px; }"
"input[type=\"submit\"] {   background-color: #4CAF50;   border-radius: 4px;   border: none;   color:white;   margin: 2px 2px;   padding: 16px 16px;   width:100% }"
"input[type=\"number\"], select{   background-color: #432e8b;   border-radius: 4px;   border: none;   color:#ffe036;   box-sizing: border-box;   margin: 2px 2px;   padding: 10px 10px; }  "
".col-c {    width: 50%;   float:center; }"
".col-1 {    width: 50%; }"
".col-2, .col-3 {    width: 20%; }"
".row:after {   content: \"\";   display: table;   clear: both; }"));
}


static void send_html_header() {
 
  DECLARE_PREALLOCATED_STRING(s, LARGE_STR);
  s = FPSTR(WEB_PAGE_HEADER);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), s);
}

/* End of Webpage */
const char WEB_PAGE_FOOTER[] PROGMEM = "</body></html>\r\n";


static void send_html_body(String& body_content) {
  if (body_content.length()) {
    server.sendContent(body_content);
  } else {
    server.sendContent(F("<h1>Ooops. There was no/nada/nix content assembled for the html page body</h1>"));
  }
  server.sendContent_P(WEB_PAGE_FOOTER);
  #ifdef TRACE_WEBUI
    Serial.println(F(">TRACE_WEBUI- Page sent "));
  #endif
      
}

const char PRESET_NAME_LIST_STR[] PROGMEM ="-off-|" 
                                          "Alarm Gelb|"
                                          "Alarm Rot/Orange/Rosa|"
                                          "Fade Grün/Cyan/Lemon|"
                                          "Fade Blau/Weiß|"
                                          "Orbit Blau/Cyan|"
                                          "Orbit Gelb/Lila/Rot/Grün|"
                                          "Orbit Lemon/Cyan/Pink/Orange|"
                                          "Rainbow 60° Step|"
                                          "Rainbow 1° Step|"
                                          "1/4 Rainbow 1° Step|"
                                          "1/4 Rainbow 10° Step|"
                                          "Einfarbig 100° Farbstep|";

#define PRESET_NAME_COUNT 13

const char speed_name_0[] PROGMEM ="2 Beats";
const char speed_name_1[] PROGMEM ="1 Beat";
const char speed_name_2[] PROGMEM ="8th Note";
const char speed_name_3[] PROGMEM ="16th Note";
const char speed_name_4[] PROGMEM ="32nd Note";
const char speed_name_5[] PROGMEM ="64th Note";


const char * const speed_name_table[] PROGMEM  = {speed_name_0
                                                 ,speed_name_1
                                                 ,speed_name_2
                                                 ,speed_name_3
                                                 ,speed_name_4
                                                 ,speed_name_5};
#define SPEED_NAME_COUNT 6
                                                 

void send_main_page() {
  DECLARE_PREALLOCATED_STRING(body_content,XLARGE_STR);

  String value_as_string;
  
  send_html_header() ;

  // Heading of page 
  body_content+=F("<h1>Show Control</h1> <div>");
  body_content+=F("<form action=\"/\" method=\"post\">"); 
  body_content+=F("<div class=\"row\">");

  // BPM input
  #ifdef TRACE_WEBUI_PAGE_GENERATION
    Serial.println(F(">TRACE_WEBUI_PAGE_GENERATION- BPM Input"));
  #endif
  body_content+=F("<label for=\"BPM\" class=\"col-1\"> Beats per minute</label>");
  value_as_string= String(output_get_bpm());
  body_content+=F("<input type=\"number\" id=\"BPM\" name=\"BPM\" value=\"");
  body_content+=value_as_string;
  body_content+=F("\" class=\"col-2\">");

  
  // Pattern Setup Table

  for(int sequence_step_index=0;sequence_step_index<MAX_NUMBER_OF_PRESETS_IN_SEQUENCE;sequence_step_index++) {
        body_content+=F("</div><div class=\"row\">");
        // Preset selection
        #ifdef TRACE_WEBUI_PAGE_GENERATION
          Serial.print(F(">TRACE_WEBUI_PAGE_GENERATION- sequence step #"));
          Serial.println(sequence_step_index);
        #endif
        value_as_string= "Preset"+String(sequence_step_index);
        body_content+=F("<select id=\"");
        body_content+=value_as_string;
        body_content+=F("\" name=\"");
        body_content+=value_as_string;
        body_content+=F("\" class=\"col-1\" >");  
        String element_list=FPSTR(PRESET_NAME_LIST_STR);
        int element_end_index=element_list.indexOf('|');
        int element_start_index=0;
        byte preset_id=0;
        for(int select_index=0;select_index<PRESET_NAME_COUNT;select_index++) {
           body_content+=F("<option value=\"");
           preset_id=select_index>0?select_index-1:PRESET_ID_OFF;  
           body_content+=String(preset_id);
           if(preset_id==g_preset_sequence[sequence_step_index].preset_id) body_content+=F("\" selected>");
           else body_content+=F("\">");
           body_content+=element_list.substring(element_start_index,element_end_index);
           body_content+=F("</option>");

           // Foreward to next list element
           element_start_index=element_end_index+1;
           if(element_start_index>=element_list.length()) element_start_index =0;
           element_end_index=element_list.indexOf('|',element_start_index);
        }
        body_content+=F("</select>");
        
        // Preset Speed selection 
        
        char string_buffer[50];
        #ifdef TRACE_WEBUI_PAGE_GENERATION
          Serial.println(F(">TRACE_WEBUI_PAGE_GENERATION- preset speed selection"));
        #endif
        value_as_string= "Speed"+String(sequence_step_index);
        body_content+=F("<select id=\"");
        body_content+=value_as_string;
        body_content+=F("\" name=\"");
        body_content+=value_as_string;
        body_content+=F("\" class=\"col-2\" >");
        for(int speed_index=0;speed_index<SPEED_NAME_COUNT;speed_index++) {
           strcpy_P(string_buffer, (char*)pgm_read_dword(&(speed_name_table[speed_index])));
           body_content+=F("<option value=\"");
           body_content+=String(speed_index);
           if(speed_index==g_preset_sequence[sequence_step_index].preset_speed_id) body_content+=F("\" selected>");
           else body_content+=F("\">");
           body_content+=string_buffer;
           body_content+=F("</option>");
        }
        body_content+=F("</select>");

        // preset beat duration input 
        
        #ifdef TRACE_WEBUI_PAGE_GENERATION
          Serial.println(F(">TRACE_WEBUI_PAGE_GENERATION- preset duration input"));
        #endif
        value_as_string= "Duration"+String(sequence_step_index);
        body_content+=F("<input type=\"number\" name=\"");
        body_content+=value_as_string;;
        body_content+=F("\" value=\"");
        body_content+=String(g_preset_sequence[sequence_step_index].beats_to_run);
        body_content+=F("\" class=\"col-3\">");
  }// end of loop over preset parameter rows 
  
  // Submit button 
  body_content+=F("</div><div class=\"row\">");
  body_content+=F("<input type=\"submit\" value=\"Start\">");

  // End of Form
  body_content+=F("</div></form>");

  /*Now get it all out to the user */
  send_html_body(body_content);
}

void parseFormData()
{
  String FormArgument;

  if (server.hasArg("BPM")) {
      FormArgument = server.arg("BPM");
      webui_data_bpm=FormArgument.toInt();
      webui_command=SET_BPM;
  }

  String argument_name;
  
  for(int sequence_step_index=0;sequence_step_index<MAX_NUMBER_OF_PRESETS_IN_SEQUENCE;sequence_step_index++){
     // Get preset id
     argument_name="Preset"+String(sequence_step_index);
     if (server.hasArg(argument_name)) {
      FormArgument = server.arg(argument_name);
      g_preset_sequence[sequence_step_index].preset_id=FormArgument.toInt();
      webui_command=SET_SEQUENCE;
     }

     // Get speed
     argument_name="Speed"+String(sequence_step_index);
     if (server.hasArg(argument_name)) {
      FormArgument = server.arg(argument_name);
      g_preset_sequence[sequence_step_index].preset_speed_id=FormArgument.toInt();
      webui_command=SET_SEQUENCE;
     }

     // Get duration
     argument_name="Duration"+String(sequence_step_index);
     if (server.hasArg(argument_name)) {
      FormArgument = server.arg(argument_name);
      g_preset_sequence[sequence_step_index].beats_to_run=FormArgument.toInt();
      webui_command=SET_SEQUENCE;
     }
    
  } // end of loop over sequence step index
}

void handleRoot()
{
  #ifdef TRACE_WEBUI
    Serial.println(F(">TRACE_WEBUI- handleRoot started"));
  #endif
  if (server.hasArg("BPM")) {
    parseFormData();
    process_webui_command();
  }
  send_main_page() ;
}



void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}



void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}

/*
 * Imperative to turn the LED on using a non-browser http client.
 * For example, using wget.
 * $ wget http://esp8266webform/ledon
 */
void handleLEDon()
{
  //writeLED(true);
  returnOK();
}

/*
 * Imperative to turn the LED off using a non-browser http client.
 * For example, using wget.
 * $ wget http://esp8266webform/ledoff
 */
void handleLEDoff()
{
  //writeLED(false);
  returnOK();
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



void webui_setup(void)
{

  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("lightstick", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/default.css", sendStylesheet);
  server.on("/ledon", handleLEDon);
  server.on("/ledoff", handleLEDoff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connect to http://lightstick.local or http://");
  Serial.println(WiFi.localIP());
  #ifdef TRACE_ON
    Serial.println(F(">webui_setup complete "));
  #endif
}

void webui_loop(void)
{
  server.handleClient();
}
