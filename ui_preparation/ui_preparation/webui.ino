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
#define TR_WEBUI
#define TR_WEBUI_PAGE_GENERATION
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

String webui_song_sequence_textarea="120 I8#>A8888 B8888 R8888 8888";

String webui_song_parts_textarea="I8/2:0 A10/4:0 B11/4:0 R68/8:0";




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
"body{font-family:\"OpenSans\",\"Arial\",\"mono\";color:#f7d53d;background-color:#193c3d;font-size:16px ;text-align:left;}"
"h1{margin:2px 2px ;font-size:18px ;padding:2px 2px ;}"
"label{float:left;margin:2px 2px ;font-size:18px ;padding:2px 2px ;}"
"input[type=\"submit\"]{background-color:#773172;border-radius:4px ;border:none;color:#f7d53d;margin:2px 2px ;padding:16px 16px ;font-size:16px ;cursor:pointer; width:100%}"
"input[type=\"submit\"]:hover{background-color:#a6449f;color:#fff;}"
"textarea{background-color:#317577;border-radius:4px ;border:none;color:#f7d53d;box-sizing:border-box;margin:2px 2px ;padding:10px 10px ;}"
".lb{display:block;background-color:#773172;border-radius:4px ;border:none;color:#f7d53d;padding:10px 10px ;font-size:16px ;cursor:pointer;text-decoration:none}"
".lb:hover{background-color:#a6449f;color:#fff;}"
".lb_box{padding:2px 2px;}"
".anntn{font-size:10px;margin:2px 2px;}"));
}

/* Switchgrid constants */
const char pattx_pulse[] PROGMEM ="Pulse";
const char pattx_whipe[] PROGMEM ="Whipe";
const char pattx_wave[] PROGMEM ="Wave";
const char pattx_orbit[] PROGMEM ="Orbit";
const char pattx_disco[] PROGMEM ="Disco";
const char pattx_rainbow[] PROGMEM ="Rainbow";
const char pattx_quater[] PROGMEM ="Quater";
const char pattx_flat[] PROGMEM ="Flat";

typedef struct pattern_button {
  const char *label;
  int   pattern_id;
} t_pattern_button;

t_pattern_button webui_pattern_button[] {
  {pattx_pulse,2},
  {pattx_whipe,10},
  {pattx_wave,11},
  {pattx_orbit,24},
  {pattx_disco,44},
  {pattx_rainbow,68},
  {pattx_quater,88},
  {pattx_flat,92}
};

const char coltx_red[] PROGMEM ="Red";
const char coltx_pink[] PROGMEM ="Pink";
const char coltx_blue[] PROGMEM ="Blue";
const char coltx_cyan[] PROGMEM ="Cyan";
const char coltx_green[] PROGMEM ="Green";
const char coltx_lemon[] PROGMEM ="Lemon";
const char coltx_yellow[] PROGMEM ="Yellow";
const char coltx_orange[] PROGMEM ="Orange";
const char coltx_purple[] PROGMEM ="Purple";
const char coltx_white[] PROGMEM ="White";


typedef struct color_button {
  const char *label;
  int hue;
  byte saturation;
} t_color_button;

t_color_button webui_color_button [] {
  {coltx_red,0,1}     ,{coltx_pink,340,1},
  {coltx_blue, 240,1} ,{coltx_cyan,180,1},
  {coltx_green, 120,1} ,{coltx_lemon,95,1},
  {coltx_yellow, 60,1} ,{coltx_orange,15,1},
  {coltx_purple, 250,1} ,{coltx_white,180,0}
};

#define WEBUI_COLOR_BUTTON_ROW_COUNT 5




static void send_html_header() {
 
  DECLARE_PREALLOCATED_STRING(s, LARGE_STR);
  s = FPSTR(WEB_PAGE_HEADER);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, FPSTR(TXT_CONTENT_TYPE_TEXT_HTML), s);
}

/* End of Webpage */
const char WEB_PAGE_FOOTER[] PROGMEM = "</body></html>\r\n";


static void send_html_footer() {
  server.sendContent_P(WEB_PAGE_FOOTER);
  #ifdef TR_WEBUI
    Serial.println(F("TR_WEBUI> Page sent "));
  #endif
      
}

const char WEB_PAGE_BUTTON_START[] PROGMEM =
      "<div>" 
      "<table width=100%>"
      "<tr>"
      " <td><h1>Pattern</h1></td><td></td><td><h1>Color<h1></td><td></td><td></td><td></td>"
      "</tr>"; 

const char WEB_PAGE_BUTTON_END[] PROGMEM = "</table></div>";


const char WEB_PAGE_FORM_SECTION_START[] PROGMEM ="<hr/><div><form action=\"/\" method=\"post\">"; 

const char WEB_PAGE_SEQ_PART_START[] PROGMEM =
      "<div> <label for=\"Sequence\" >Song Sequence</label>"
      "<textarea  id=\"Sequence\" name=\"Sequence\" style=\"width:100%; height:3;\" >";
  

const char WEB_PAGE_SEQ_PART_END[] PROGMEM =
      "</textarea> </div>"
      "<div class=\"anntn\">"
      "Syntax: &lt;bpm&gt; &lt;Part Letter&gt; &lt;Beats&gt;&lt;Beats&gt;... &lt;Part Letter&gt;...<br/>"
      "# = Forward immediatly to next on button press <br/>"
      "&gt; = Jump back here after end<br/>"
      "Part Letter is defined in Song part configuration<br/>"
      "Beats are one digit numbers, that get accumulated: 8888=32 Beats <br/>"
      "248 =14 Beats"
      "</div>";

const char WEB_PAGE_SONG_PART_START[] PROGMEM =
      "<div > <label for=\"Parts\" >Song Parts</label>"
      "<textarea  id=\"Parts\" name=\"Parts\" style=\"width:100%; height:1; \" >";

const char WEB_PAGE_SONG_PART_END[] PROGMEM =
      "</textarea> </div>"
      "<div class=\"anntn\">"
      "Syntax:  &lt;Part Letter&gt;&lt;Pattern&gt;/&lt;Speed&gt;:&lt;color&gt;... <br/>"
      "Pattern: identifies the pattern of light change (see pattern catalog)<br/>"
      "Speed: Speed of the pattern 2=Haft Beat 4=Beat 8=8th, 16,32,64 <br/>"
      "Color: defines the color palette (see color catalog)"
      "</div>";

const char WEB_PAGE_FORM_END[] PROGMEM =
      "<input type=\"submit\" value=\"Start\">"
      "</form>";

const char WEB_PRESET_SECTION_START[] PROGMEM =
      "<hr/>"
      "<h1>Song Presets</h1>";

void send_main_page() {


  String value_as_string;
  
  send_html_header() ;

  char string_buffer[MAX_DEFINITON_STRING_LENGTH];
  
  DECLARE_PREALLOCATED_STRING(content_element,XLARGE_STR);
  
  // ******************* Send Button Grid ***************************
  server.sendContent_P(WEB_PAGE_BUTTON_START);
  
  for(int row_index=0;row_index<WEBUI_COLOR_BUTTON_ROW_COUNT+1;row_index++) {
    content_element="<tr>";

    for(int col=0;col<(row_index<WEBUI_COLOR_BUTTON_ROW_COUNT?1:3);col++){ // on last row fill it up by yourself
      content_element+=F("<td><div class=\"lb_box\"><a class=\"lb\"  href=\"/switch?p=");
      content_element+=webui_pattern_button[row_index+col].pattern_id;
      content_element+=F("&w=4\">");
      strcpy_P(string_buffer, (char*)pgm_read_dword(&(webui_pattern_button[row_index+col].label)));
      content_element+=string_buffer;
      content_element+=F("</a></div></td>");
  
      content_element+=F("<td><div class=\"lb_box\"><a class=\"lb\"  href=\"/switch?p=");
      content_element+=webui_pattern_button[row_index+col].pattern_id;
      content_element+=F("&w=8\">8th");
      content_element+=F("</a></div></td>");
  }

    if(row_index<WEBUI_COLOR_BUTTON_ROW_COUNT) {
      for(int col=0;col<2;col++) {
        content_element+=F("<td><div class=\"lb_box\"><a class=\"lb\"  href=\"/switch?i=1&h=");
        content_element+=webui_color_button[2*row_index+col].hue;
        content_element+=F("&s=");
        content_element+=webui_color_button[2*row_index+col].saturation;
        content_element+="\">";
        strcpy_P(string_buffer, (char*)pgm_read_dword(&(webui_color_button[2*row_index+col].label)));
        content_element+=string_buffer;
        content_element+=F("</a></div></td>");
        content_element+=F("<td><div class=\"lb_box\"><a class=\"lb\"  href=\"/switch?h=");
        content_element+=webui_color_button[2*row_index+col].hue;
        content_element+=F("&s=");
        content_element+=webui_color_button[2*row_index+col].saturation;
        content_element+="\">+";
        content_element+=F("</a></div></td>");
      }
    }
    content_element+="</tr>";
    server.sendContent(content_element);
  }

   server.sendContent_P(WEB_PAGE_BUTTON_END);

 
  // ******************* Send Sequence Section Form ****************
  server.sendContent_P(WEB_PRESET_SECTION_START);
  
 
  
  content_element="";
  for(int song_index=0;song_index<song_catalog_count;song_index++) {
        content_element+=F("<div class=\"lb_box\"><a class=\"lb\"  href=\"song?song_index=");
        content_element+=(song_index);  
        content_element+=F("\">");
        strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[song_index].song_name)));
        content_element+=string_buffer;
        content_element+=F("</a></div>");
  } // Loop over song catalog

  /*Now get it all out to the user */
  server.sendContent(content_element);

  // ******************* Send Sequence Section Form ****************
  // Sequence
  server.sendContent_P(WEB_PAGE_FORM_SECTION_START);
  server.sendContent_P(WEB_PAGE_SEQ_PART_START);
  server.sendContent(webui_song_sequence_textarea);
  server.sendContent_P(WEB_PAGE_SEQ_PART_END);


  // Parts
  server.sendContent_P(WEB_PAGE_SONG_PART_START);
  server.sendContent(webui_song_parts_textarea);
  server.sendContent_P(WEB_PAGE_SONG_PART_END);
  
  // End of Part/ Sequence Form
  server.sendContent_P(WEB_PAGE_FORM_END);

  send_html_footer();
}

void parseFormData()
{
  if (server.hasArg("Parts")) {
      webui_song_parts_textarea = server.arg("Parts");
  }

  if (server.hasArg("Sequence")) {
      webui_song_sequence_textarea = server.arg("Sequence");
  }
  parse_slot_settings( webui_song_parts_textarea);
  parse_sequence (webui_song_sequence_textarea);
}

void handleRoot()
{
  #ifdef TR_WEBUI
    Serial.println(F("TR_WEBUI> handleRoot started"));
  #endif
  if (server.hasArg("Parts")) {
    parseFormData();
  }
  send_main_page() ;
}

void handleSong()
{
  #ifdef TR_WEBUI
    Serial.println(F("TR_WEBUI> handleSong started"));
  #endif
  
  String song_index_parameter;
  char string_buffer[MAX_DEFINITON_STRING_LENGTH];
   
  if (server.hasArg("song_index")) {
     song_index_parameter=server.arg("song_index");
     #ifdef TR_WEBUI
       Serial.print(F("TR_WEBUI> song_index:"));Serial.println(song_index_parameter);
    #endif   
    int song_index=song_index_parameter.toInt();  // Bad input will result in song 0 (can happen from saved urls, not from urls of own form)
    song_preset_start(song_index_parameter.toInt());

    strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[song_index].song_slot_definition)));
    webui_song_parts_textarea=string_buffer;

    strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[song_index].song_sequence_definition)));
    webui_song_sequence_textarea=string_buffer;
  }
  send_main_page() ;
}

void handleSwitch()
{
  #ifdef TR_WEBUI
    Serial.println(F("TR_WEBUI> handleSwitch started"));
  #endif
  String url_parameter_value;
  if (server.hasArg("p")) {  // Pattern switch
    url_parameter_value=server.arg("p");
    output_start_pattern(url_parameter_value.toInt());
    mode_of_operation=MODE_FIX_PRESET;
  }
  if (server.hasArg("w")) {  // Pattern switch
    url_parameter_value=server.arg("w");
    int preset_speed_id=preset_speed_to_id(server.arg("w"));
    if(preset_speed_id>=0)output_set_pattern_speed(preset_speed_id);
    mode_of_operation=MODE_FIX_PRESET;
  }
  if (server.hasArg("h")&&server.hasArg("s")) {
    float hue = server.arg("h").toFloat();
    float saturation = server.arg("s").toFloat();
    if(server.hasArg("i")) output_reset_color_palette(hue,saturation);
    else output_add_color_palette_entry(hue,saturation);
    mode_of_operation=MODE_FIX_PRESET;
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

void handleNotFound()
{
  String message = "Not a valid request\n";
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
  //message += "<a href=\"/\">Get Back to start</a>\n";
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
  server.on("/song", handleSong);
  server.on("/switch", handleSwitch);
  server.on("/default.css", sendStylesheet);
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
