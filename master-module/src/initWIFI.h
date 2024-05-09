#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

bool initWIFI(String ssid, String password)
{
  bool connected = false;
  Serial.println("Trying to connect to WiFi");
  if (ssid.length() == 0 || ssid.length() > 32 || password.length() == 0)
  {
    Serial.println("Wrong credentials length");
    return false;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    connected = false;
    Serial.print("WiFi Failed ");
    Serial.println(WiFi.status());
    Serial2.println("WiFi Connection Failed");
  }
  else
  {
    connected = true;
    Serial.println("WiFi Connection Success");
  }
  return connected;
}

bool initUDP(AsyncUDP& udp)
{
  bool connected = true;
  if (udp.listenMulticast(IPAddress(226, 0, 0, 0), 1234))
  {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    connected = false;
    Serial.println("UDP Listen Failed");
  }
  return connected;
}