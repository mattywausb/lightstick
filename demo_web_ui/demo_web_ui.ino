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

//  SSID and password are defined in header file that will not be in the git repo
const char* ssid = STASSID;
const char* password = STAPSK;

MDNSResponder mdns;

ESP8266WebServer server(80);
const char DEFAULT_CSS[] =
"body {   font-family: \"Open Sans\", \"Arial\", \"mono\";  color:#ffe036;  background-color: #18006a;  font-size: 14px;  text-align: left; }"
"h1 {   text-align: center; }"
"label {  float: left;     margin: 2px 2px;     padding: 10px 10px; }"
"input[type=\"submit\"] {   background-color: #4CAF50;   border-radius: 4px;   border: none;   color:white;   margin: 2px 2px;   padding: 16px 16px;   width:100% }"
"input[type=\"number\"], select{   background-color: #432e8b;   border-radius: 4px;   border: none;   color:#ffe036;   box-sizing: border-box;   margin: 2px 2px;   padding: 10px 10px; }  "
".col-c {    width: 50%;   float:center; }"
".col-1 {    width: 50%; }"
".col-2, .col-3 {    width: 20%; }"
".row:after {   content: \"\";   display: table;   clear: both; }";

const char INDEX_HTML[] =
"<!DOCTYPE html>"
"<html><head>"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=windows-1252\">"
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
"<input type=\"number\" id=\"BPM\" name=\"BPM\" value=\"120\" class=\"col-2\">"
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

// GPIO#0 is for Adafruit ESP8266 HUZZAH board. Your board LED might be on 13.
const int LEDPIN = LED_BUILTIN ;

void handleRoot()
{
  if (server.hasArg("BPM")) {
    handleSubmit();
  }
  else {
    server.send(200, "text/html", INDEX_HTML);
  }
}

void sendStylesheet() {
   server.send(200, "text/css", DEFAULT_CSS);
}

void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void handleSubmit()
{
  String BPMArgument;
  int BPM;

  if (!server.hasArg("BPM")) return returnFail("BAD ARGS");
  BPMArgument = server.arg("BPM");
  BPM=BPMArgument.toInt();
  if (BPM%2==1) {
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
  }
  else  {
    writeLED(false);
    server.send(200, "text/html", INDEX_HTML);
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
  writeLED(true);
  returnOK();
}

/*
 * Imperative to turn the LED off using a non-browser http client.
 * For example, using wget.
 * $ wget http://esp8266webform/ledoff
 */
void handleLEDoff()
{
  writeLED(false);
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

void writeLED(bool LEDon)
{
  // Note inverted logic for Adafruit HUZZAH board
  if (LEDon)
    digitalWrite(LEDPIN, 0);
  else
    digitalWrite(LEDPIN, 1);
}

void setup(void)
{
  pinMode(LEDPIN, OUTPUT);
  writeLED(false);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

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

  if (mdns.begin("esp8266WebForm", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/default.css", sendStylesheet);
  server.on("/ledon", handleLEDon);
  server.on("/ledoff", handleLEDoff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connect to http://esp8266WebForm.local or http://");
  Serial.println(WiFi.localIP());
}

void loop(void)
{
  server.handleClient();
}
