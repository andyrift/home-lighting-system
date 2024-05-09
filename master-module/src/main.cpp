#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include "time.h"

#include "converters.h"
#include "initWIFI.h"
#include "syncTime.h"
#include "printCurrentTime.h"

String ssid = "";
String password = "";
String timeloc = "";

bool connected = false;

AsyncUDP udp;
HTTPClient http;

void processInput(String input, bool print);
void multicast();

int hot = 0;
int cold = 0;

ulong last_send = 0;
ulong last_tick = 0;

time_t lastsync = 0;
time_t syncinterval = 86400;

Preferences pref;

void setup()
{
  Serial.begin(9600);

  pref.begin("pref");
  ssid = pref.getString("ssid");
  password = pref.getString("password");
  // timeloc = pref.getString("timeloc");

  connected = initWIFI(ssid, password);
  connected = initUDP(udp);
  if (connected) {
    syncTime(http);
    lastsync = time(nullptr);
  }
  Serial2.begin(9600, SERIAL_8N1, 16, 17);  
}

void loop()
{

  if (millis() - last_tick >= 1000)
  {
    last_tick = millis();

    if (time(nullptr) - lastsync >= syncinterval)
    {
      syncTime(http);
      lastsync = time(nullptr);
    }
    else
    {
      // printCurrentTime();
      // Serial.print("Since sync: ");
      // Serial.println(time(nullptr) - lastsync);
    }
  }

  if (Serial2.available())
  {
    String input = Serial2.readStringUntil('\n');
    processInput(input, true);
  }
}

void getTimeLoc()
{
  Serial2.println("tl" + timeloc);
}

void setTimeLoc(String input)
{
  timeloc = input;
  // pref.putString("timeloc", timeloc);
}

void getValues()
{
  Serial2.println("hcv" + String(hot) + "," + String(cold));
}

void setWiFi(String input)
{
  int sep = -1;
  for (int i = 0; i < input.length(); i++)
  {
    if (input[i] == '+')
    {
      sep = i;
      break;
    }
  }
  if (sep != -1)
  {
    int ssidlen = sep;
    int passwordlen = input.length() - sep - 1;
    if (ssidlen > 0 && passwordlen > 0)
    {
      ssid = input.substring(0, sep);
      password = input.substring(sep + 1, input.length());

      pref.putString("ssid", ssid);
      pref.putString("password", password);

      Serial.print("ssid: ");
      Serial.println(ssid);
      Serial.print("password: ");
      Serial.println(password);
      connected = initWIFI(ssid, password);
    }
  }
}

void setHotValue(String input, bool print)
{
  uint8_t val = input.toInt();
  hot = val;

  if (print)
  {
    Serial.print("Hot Value: ");
    Serial.print(val);
    Serial.print(", ");
    Serial.print(convert(val));
    Serial.print("/65535");
    Serial.println();
  }
}

void setColdValue(String input, bool print)
{
  uint8_t val = input.toInt();
  cold = val;

  if (print)
  {
    Serial.print("Cold Value: ");
    Serial.print(val);
    Serial.print(", ");
    Serial.print(convert(val));
    Serial.print("/65535");
    Serial.println();
  }
}

void setValues(String input, bool print)
{
  int sep = -1;
  for (int i = 0; i < input.length(); i++)
  {
    if (input[i] == ',')
    {
      sep = i;
      break;
    }
  }
  uint8_t val1 = 0;
  uint8_t val2 = 0;
  if (sep == -1)
  {
    val1 = val2 = input.substring(0, input.length()).toInt();
  }
  else
  {
    val1 = input.substring(0, sep).toInt();
    val2 = input.substring(sep + 1, input.length()).toInt();
  }
  hot = val1;
  cold = val2;
  if (print)
  {
    Serial.print("Hot Value: ");
    Serial.println(val1);
    Serial.print("Cold Value: ");
    Serial.println(val2);
  }
}

void unknownCommand(String input)
{
  Serial.print("Unknown command: ");
  Serial.print(input);
  Serial.println();
  Serial2.println("Unknown command");
}

void processInput(String input, bool print)
{
  if (input.substring(0, 3) == "gtl")
    getTimeLoc();
  else if (input.substring(0, 2) == "tl")
    setTimeLoc(input.substring(2, input.length()));
  else if (input.substring(0, 2) == "gv")
    getValues();
  else if (input.substring(0, 4) == "wifi")
    setWiFi(input.substring(4, input.length()));
  else if (input.substring(0, 2) == "hv")
    setHotValue(input.substring(2, input.length()), true);
  else if (input.substring(0, 2) == "cv")
    setColdValue(input.substring(2, input.length()), true);
  else if (input.substring(0, 3) == "hcv")
    setValues(input.substring(3, input.length()), true);
  else
    unknownCommand(input);
}

void multicast()
{
  if (millis() - last_send >= 4000 && connected)
  {
    last_send = millis();
    udp.print("hcv" + String(hot) + ',' + String(cold));
  }
}