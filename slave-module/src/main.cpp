#include <Arduino.h>
#include "esp32-hal-ledc.h"
#include <WiFi.h>
#include "AsyncUDP.h"

#include "converters.h"

String ssid = "";
String password = "";
AsyncUDP udp;

void initLED();
void initWIFI();
void processInput(String input, bool print);

void setup()
{
  Serial.begin(9600);
  initLED();
  initWIFI();
}

void loop()
{
  
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

void processPacket(AsyncUDPPacket packet)
{
  processInput(packet.readStringUntil('\n'), false);
}

void initWIFI()
{
  if (ssid.length() == 0 || ssid.length() > 32 || password.length() == 0)
  {
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    
  }
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