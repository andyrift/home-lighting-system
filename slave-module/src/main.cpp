#include <Arduino.h>
#include "esp32-hal-ledc.h"
#include <WiFi.h>
#include "AsyncUDP.h"
#include <Preferences.h>

#include <WebServer.h>

#include "converters.h"

String ssid = "";
String password = "";
AsyncUDP udp;

WebServer server(80);

Preferences pref;

void initLED();
void initWIFI();
void initUDP();
void initHTTPSetup();
void processInput(String input);

bool setup_mode = false;

bool on;
long switch_ignore_start;
long switch_ignore_length = 1000;

void setup()
{
  Serial.begin(9600);

  pref.begin("pref");
  ssid = pref.getString("ssid");
  password = pref.getString("password");

  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);

  on = !digitalRead(33);
  switch_ignore_start = millis();

  initLED();

  if (digitalRead(32))
  {
    initWIFI();
    initUDP();
  }
  else
  {
    setup_mode = true;
    initHTTPSetup();
  }
  
}

bool checkSwitch()
{
  bool switched = false;
  if (millis() - switch_ignore_start > switch_ignore_length)
  {
    if (digitalRead(33) == on)
    {
      on = !on;
      switched = true;
      switch_ignore_start = millis();
    }
  }
  return switched;
}

void setup_mode_loop()
{
  server.handleClient();
  delay(2);
}

void loop()
{
  if (setup_mode)
  {
    setup_mode_loop();
    return;
  }
  if (checkSwitch())
  {
    if (on)
      Serial.println("Switch ON");
    else
      Serial.println("Switch OFF");
  }
}

void initLED()
{
  const int frequency = 150;
  const int resolution = 16;
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  ledcSetup(1, frequency, resolution);
  ledcSetup(2, frequency, resolution);
  ledcAttachPin(26, 1);
  ledcAttachPin(27, 2);
}

void initWIFI()
{
  Serial.println("Trying to connect to WIFI");
  if (ssid.length() == 0 || ssid.length() > 32 || password.length() == 0)
  {
    Serial.println("Could not connect");
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Could not connect");
  }
  else
  {
    Serial.println("Connected Successfully!");
  }
}

void processPacket(AsyncUDPPacket packet)
{
  processInput(packet.readStringUntil('\n'));
}

void initUDP()
{
  if (udp.listenMulticast(IPAddress(226, 0, 0, 0), 1234))
  {
    udp.onPacket(processPacket);
  }
}

void processInput(String input)
{
  if (input.substring(0, 2) == "hv")
  {
    uint8_t val = input.substring(2, input.length()).toInt();
    ledcWrite(1, convert(val));
  }
  else if (input.substring(0, 2) == "cv")
  {
    uint8_t val = input.substring(2, input.length()).toInt();
    ledcWrite(2, convert(val));
  }
  else if (input.substring(0, 3) == "hcv")
  {
    String nums = input.substring(3, input.length());
    int sep = -1;
    for (int i = 0; i < nums.length(); i++)
    {
      if (nums[i] == ',')
      {
        sep = i;
        break;
      }
    }
    if (sep != -1)
    {
      uint8_t val1 = nums.substring(0, sep).toInt();
      uint8_t val2 = nums.substring(sep + 1, nums.length()).toInt();
      ledcWrite(1, convert(val1));
      ledcWrite(2, convert(val2));
    }
  }
}

void handleSubmit()
{
  String msg;
  if (server.hasArg("ssid") && server.hasArg("password"))
  {
    ssid = server.arg("ssid");
    password = server.arg("password");

    pref.putString("ssid", ssid);
    pref.putString("password", password);
  }
  server.sendHeader("Location", "/");
  server.send(301);
}

void handleRoot()
{
  Serial.println("Enter handleRoot");
  String header;
  String content = "<!DOCTYPE html><html>";
  content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<link rel=\"icon\" href=\"data:,\">";
  content += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
  content += "div { padding: 2px; margin: 2px; }";
  content += "</style></head>";

  content += "<body><h1>ESP32 Web Server</h1><h3>Lighting module setup</h3>";
  
  content += "<form action=\"/submit\"  method=\"GET\">";

  content += "<div><label for=\"ssid\">SSID</label>";
  content += "<input name=\"ssid\" id=\"ssid\" value=\"" + ssid + "\" /></div>";
  content += "<div><label for=\"password\">Password</label>";
  content += "<input name=\"password\" id=\"password\" value=\"" + password + "\" /></div>";

  content += "<div><button>Submit</button></div>";
  content += "</form>";
  content += "</body></html>";
  server.send(200, "text/html", content);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void initHTTPSetup()
{
  Serial.println("Hosting HTTP setup server");
  WiFi.softAP("HLS Setup", "password");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.onNotFound(handleNotFound);
  server.begin();
}