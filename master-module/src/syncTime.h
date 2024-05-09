#include <HTTPClient.h>
#include "ArduinoJson.h"
#include "time.h"

void syncTime(HTTPClient &http)
{
  http.begin("http://worldtimeapi.org/api/timezone/Etc/UTC");
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0)
  {
    Serial.print("Retrieved time. Response code: ");
    Serial.println(httpResponseCode);

    String payload = http.getString();

    JsonDocument json;
    deserializeJson(json, payload);
    time_t unixtime = json["unixtime"];

    struct timeval tv;
    tv.tv_sec = unixtime;

    settimeofday(&tv, NULL);
  }
  else
  {
    Serial.print("Error retrieving time. Code: ");
    Serial.println(httpResponseCode);

    String payload = http.getString();
    Serial.print("Response:");
    Serial.println(payload);
  }
  http.end();
}