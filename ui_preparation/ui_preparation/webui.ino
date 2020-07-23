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

//  SSID and password are defined in header file that will not be in the git repo
const char* ssid = STASSID;
const char* password = STAPSK;

MDNSResponder mdns;

ESP8266WebServer server(80);

// GPIO#0 is for Adafruit ESP8266 HUZZAH board. Your board LED might be on 13.
const int LEDPIN = LED_BUILTIN ;

t_webui_command webui_command=NONE;
int webui_data_bpm=0;

boolean webui_has_new_command() { return webui_command!=NONE; }

t_webui_command webui_read_command() { 
  t_webui_command current_command=webui_command;
  webui_command=NONE;
  return current_command;
 }

int webui_data_get_bpm() { return webui_data_bpm;}

void handleRoot()
{
  if (server.hasArg("BPM")) {
    parseFormData();
    process_webui_command();
  }
  sendMainPage() ;
}

void sendMainPage() {
  String BPM_as_string= String(output_get_bpm());
  String send_part1= "<!DOCTYPE html>"
"<html><head>"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
"<meta name=\"viewport\" content=\"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>Lightstick</title>"
"<link href=\"default.css\" rel=\"stylesheet\" type=\"text/css\">"
"</head>"
"<BODY class=\"body\">"
"<body><h1>Show Control</h1>"
"<div>"
"<form action=\"/\" method=\"post\">"
"<div class=\"row\">"
"<label for=\"BPM\" class=\"col-1\"> Beats per minute</label>"
"<input type=\"number\" id=\"BPM\" name=\"BPM\" value=\"";

String send_part2="\" class=\"col-2\">"
"</div>"
"<div class=\"row\">"
" <select id=\"Preset1\" name=\"Preset1\" class=\"col-1\" >"
  "<option value=\"-1\">- off -</option>"
  "<option value=\"0\">Alarm Gelb</option>"
  "<option value=\"1\">Alarm Rot/Orange/Rosa</option>"
  "<option value=\"2\">Fade Grün/Cyan/Lemon</option>"
  "<option value=\"3\">Fade Blau/Weiß</option>"
  "<option value=\"4\">Orbit Blau/Cyan</option>"
  "<option value=\"5\">Orbit Gelb/Lila/Rot/Grün</option>"
  "<option value=\"6\">Orbit Lemon/Cyan/Pink/Orange</option>"
  "<option value=\"7\" selected>Rainbow 60° Step</option>"
  "<option value=\"8\">Rainbow 1° Step</option>"
  "<option value=\"9\">1/4 Rainbow 1° Step</option>"
  "<option value=\"10\">1/4 Rainbow 10° Step</option>"
  "<option value=\"11\">Einfarbig 100° Farbstep</option>"
"</select>" 
" <select id=\"Speed1\" name=\"Speed1\" class=\"col-2\">"
  "<option value=\"0\">2 Beats (Half Note)</option>"
  "<option value=\"1\" selected>1 Beat (Quater Note)</option>"
  "<option value=\"2\">8th Note</option>"
  "<option value=\"3\">16th Note</option>"
  "<option value=\"4\">32th Note</option>"
  "<option value=\"5\">64th Note</option>"
"</select>"
"<input type=\"number\" name=\"duration1\" value=\"4\" class=\"col-3\">"
"</div>"
"<input type=\"submit\" value=\"Start\"> </form>"
"</div>"
"</body></html>";
    server.send(200, "text/html", send_part1 + BPM_as_string + send_part2);
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
