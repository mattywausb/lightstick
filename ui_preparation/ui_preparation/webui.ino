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
"body{font-family:\"OpenSans\",\"Arial\",\"mono\";color:#f7d53d;background-color:#193c3d;font-size:16px ;text-align:left;}"
"h1{margin:2px 2px ;font-size:18px ;padding:2px 2px ;}"
"label{float:left;margin:2px 2px ;font-size:18px ;padding:2px 2px ;}"
"input[type=\"submit\"]{background-color:#773172;border-radius:4px ;border:none;color:#f7d53d;margin:2px 2px ;padding:16px 16px ;font-size:16px ;cursor:pointer; width:100%}"
"input[type=\"submit\"]:hover{background-color:#a6449f;color:#fff;}"
"textarea{background-color:#317577;border-radius:4px ;border:none;color:#f7d53d;box-sizing:border-box;margin:2px 2px ;padding:10px 10px ;}"
".link_button{display:block;background-color:#773172;border-radius:4px ;border:none;color:#f7d53d;margin:8px 2px ;padding:10px 10px ;font-size:16px ;cursor:pointer;text-decoration:none;width:100%}"
".link_button:hover{background-color:#a6449f;color:#fff;}"
".annotations{font-size:10px ;margin:2px 2px ;}"));
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
  #ifdef TR_WEBUI
    Serial.println(F("TR_WEBUI> Page sent "));
  #endif
      
}

                                              

void send_main_page() {
  DECLARE_PREALLOCATED_STRING(body_content,XLARGE_STR);

  String value_as_string;
  
  send_html_header() ;

  // Heading of page 
  body_content+=F("<h1>Show Control</h1> <div>");
  body_content+=F("<form action=\"/\" method=\"post\">"); 
  body_content+=F("<div class=\"row\">");

  body_content+=F("<div> <form action=\"/\" method=\"post\">");
  body_content+=F("<div> <label for=\"Sequence\" >Song Sequence</label>");
  body_content+=F("<textarea  id=\"Sequence\" name=\"Sequence\" style=\"width:100%; height:3;\" >");

  body_content+="### put current SEQUENCE String  here ###";
  
  body_content+=F("</textarea> </div>");
  body_content+=F("<div class=\"annotations\">");
    body_content+=F("Syntax: &lt;bpm&gt; &lt;Part Letter&gt; &lt;Beats&gt;&lt;Beats&gt;... &lt;Part Letter&gt;...<br/>");
  body_content+=F("# = Forward immediatly to next on button press <br/>");
  body_content+=F("&gt; = Jump back here after end<br/>");
  body_content+=F("Part Letter is defined in Song part configuration<br/>");
  body_content+=F("Beats are one digit numbers, that get accumulated: 8888=32 Beats <br/>");
  body_content+=F("248 =14 Beats");
  body_content+=F("</div>");
  
  body_content+=F("<div > <label for=\"Parts\" >Song Parts</label>");
  body_content+=F("<textarea  id=\"Parts\" name=\"Parts\" style=\"width:100%; height:1; \" >");

  body_content+="### put current PART String  here ###";

  body_content+=F("</textarea> </div>");
  body_content+=F("<div class=\"annotations\">");
  body_content+=F("Syntax:  &lt;Part Letter&gt;&lt;Pattern&gt;/&lt;Speed&gt;:&lt;color&gt;... <br/>");
  body_content+=F("Pattern: identifies the pattern of light change (see pattern catalog)<br/>");
  body_content+=F("Speed: Speed of the pattern 2=Haft Beat 4=Beat 8=8th, 16,32,64 <br/>");
  body_content+=F("Color: defines the color palette (see color catalog)");
  body_content+=F("</div>");
  body_content+=F("<input type=\"submit\" value=\"Start\">");
  body_content+=F("</form>");
  body_content+=F("<hr/>");
  body_content+=F("<h1>Song Presets</h1>");

  
  char string_buffer[MAX_DEFINITON_STRING_LENGTH];

  for(int song_index=0;song_index<song_catalog_count;song_index++) {
        body_content+=F("<a class=\"link_button\"  href=\"song?song_index=");
        body_content+=song_index;
        body_content+=F("\">");
        strcpy_P(string_buffer, (char*)pgm_read_dword(&(song_catalog[song_index].song_name)));
        body_content+=string_buffer;
        body_content+=F("</a>");
  } // Loop over song catalog

  /*Now get it all out to the user */
  send_html_body(body_content);
}

void parseFormData()
{
  String FormArgument;
   if (server.hasArg("Parts")) {
      FormArgument = server.arg("Parts");
      // Parse Parts here
  }

  if (server.hasArg("Sequence")) {
      FormArgument = server.arg("Sequence");
      // Parse and start Sequence here
  }
}

void handleRoot()
{
  #ifdef TR_WEBUI
    Serial.println(F("TR_WEBUI> handleRoot started"));
  #endif
  if (server.hasArg("Parts")) {
    parseFormData();
    process_webui_command();
  }
  send_main_page() ;
}

void handleSong()
{
  #ifdef TR_WEBUI
    Serial.println(F("TR_WEBUI> handleSong started"));
  #endif
  
  String urlParameterValue;
  
  if (server.hasArg("song_index")) {
    urlParameterValue=server.arg("song_index");
     #ifdef TR_WEBUI
       Serial.print(F("TR_WEBUI> song_index:"));Serial.println(urlParameterValue);
    #endif   
    /* ########## Load song here ############# */
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
