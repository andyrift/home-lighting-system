#include <Arduino.h>

uint16_t convertlog(uint8_t input)
{
  int max = 65535;
  int mult = 9843;

  int res = log2(input + 1) * mult;

  return min(res, max);
}

uint16_t convert(uint8_t input)
{
  int max = 65535;
  int mult = 6554;

  int res = sqrt(input) * (float)mult / (float)max * convertlog(input);

  return min(res, max);
}