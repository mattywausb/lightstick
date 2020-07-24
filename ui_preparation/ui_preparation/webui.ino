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
#define TRACE_WEBUI_PAGE_GENERATION
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

const char preset_name_off[] PROGMEM ="-off-";
const char preset_name_0[] PROGMEM ="Alarm Gelb";
const char preset_name_1[] PROGMEM ="Alarm Rot/Orange/Rosa";
const char preset_name_2[] PROGMEM ="Fade Grün/Cyan/Lemon";
const char preset_name_3[] PROGMEM ="Fade Blau/Weiß";
const char preset_name_4[] PROGMEM ="Orbit Blau/Cyan";
const char preset_name_5[] PROGMEM ="Orbit Gelb/Lila/Rot/Grün";
const char preset_name_6[] PROGMEM ="Orbit Lemon/Cyan/Pink/Orange";
const char preset_name_7[] PROGMEM ="Rainbow 60° Step";
const char preset_name_8[] PROGMEM ="Rainbow 1° Step";
const char preset_name_9[] PROGMEM ="1/4 Rainbow 1° Step";
const char preset_name_10[] PROGMEM ="1/4 Rainbow 10° Step";
const char preset_name_11[] PROGMEM ="Einfarbig 100° Farbstep";

const char * const preset_name_table[] PROGMEM =  {preset_name_off
                                                  ,preset_name_0
                                                  ,preset_name_1
                                                  ,preset_name_2
                                                  ,preset_name_3
                                                  ,preset_name_4
                                                  ,preset_name_5
                                                  ,preset_name_6
                                                  ,preset_name_7
                                                  ,preset_name_8
                                                  ,preset_name_9
                                                  ,preset_name_10
                                                  ,preset_name_11};

#define PRESET_NAME_COUNT 12

const char speed_name_0[] PROGMEM ="2 Beats";
const char speed_name_1[] PROGMEM ="1 Beat";
const char speed_name_2[] PROGMEM ="8th Note";
const char speed_name_3[] PROGMEM ="16th Note";
const char speed_name_4[] PROGMEM ="32rd Note";
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
  value_as_string= String(output_get_bpm());
  body_content+=F("<input type=\"number\" id=\"BPM\" name=\"BPM\" value=\"");
  body_content+=value_as_string;
  body_content+=F("\" class=\"col-2\">");

  
  // Pattern Setup Table
  char string_buffer[30];
  for(int preset_step_index=0;preset_step_index<4;preset_step_index++) {
        body_content+=F("</div><div class=\"row\">");
        // Preset selection
        #ifdef TRACE_WEBUI_PAGE_GENERATION
          Serial.print(F(">TRACE_WEBUI_PAGE_GENERATION- preset selection #"));
          Serial.println(preset_step_index);
        #endif
        value_as_string= "Preset"+String(preset_step_index+1);
        body_content+=F("<select id=\"");
        body_content+=value_as_string;
        body_content+=F("\" name=\"");
        body_content+=value_as_string;
        body_content+=F("\" class=\"col-1\" >");
        for(int select_index=0;select_index<PRESET_NAME_COUNT;select_index++) {
           Serial.print(">");
           strcpy_P(string_buffer, (char*)pgm_read_word(&(preset_name_table[select_index])));
           Serial.println("<");
           body_content+=F("<option value=\"");
           body_content+=String(select_index-1);
           // 2do add selected tag here for selected option
           body_content+=F("\">");

           body_content+=string_buffer;

           body_content+=F("</option>");
        }
        body_content+=F("</select>");
        /*
        // Speed selection 
        #ifdef TRACE_WEBUI_PAGE_GENERATION
          Serial.println(F(">TRACE_WEBUI_PAGE_GENERATION- Speed selection"));
        #endif
        value_as_string= "Speed"+String(preset_step_index+1);
        body_content+=F("<select id=\"");
        body_content+=value_as_string;
        body_content+=F("\" name=\"");
        body_content+=value_as_string;
        body_content+=F("\" class=\"col-2\" >");
        for(int speed_index=0;speed_index<SPEED_NAME_COUNT;speed_index++) {
          strcpy_P(string_buffer, (char*)pgm_read_word(&(speed_name_table[speed_index])));
           body_content+=F("<option value=\"");
           body_content+=String(speed_index-1);
           body_content+=F("\">");
           // 2do add selected tag here for selected option
           body_content+=string_buffer;
           body_content+=F("</option>");
        }
        body_content+=F("</select>");

        // preset beat duration input 
        value_as_string= "duration"+String(preset_step_index+1);
        body_content+=F("<input type=\"number\" name=\"");
        body_content+=value_as_string;;
        body_content+=F("\" value=\"4\" class=\"col-3\">");
        */
        
  }// end of loop over preset parameter rows 
  
  // Submit button 
  body_content+=F("</div><div class=\"row\">");
  body_content+=F("<input type=\"submit\" value=\"Start\">");

  // End of Form
  body_content+=F("</div></form>");

  /*Now get it all out to the user */
  send_html_body(body_content);
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

void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void parseFormData()
{
  String FormArgument;

  if (server.hasArg("BPM")) {
      FormArgument = server.arg("BPM");
      webui_data_bpm=FormArgument.toInt();
      webui_command=SET_BPM;
  }
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
