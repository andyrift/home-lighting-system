#include <Arduino.h>
#include "time.h"

void printCurrentTime()
{
  time_t now = time(nullptr);
  struct tm *timestr;
  timestr = localtime(&now);
  Serial.print("Time Now: ");
  Serial.println(timestr);
}