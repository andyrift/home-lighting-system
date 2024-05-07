#include "esp32-hal-ledc.h"
#include <Arduino.h>

const int frequency = 150;
const int resolution = 16;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  ledcSetup(1, frequency, resolution);
  ledcSetup(2, frequency, resolution);
  ledcAttachPin(26, 1);
  ledcAttachPin(27, 2);
}

uint16_t convertlog(uint8_t input) {
  int max = 65535;
  int mult = 8198;

  int res = log2(input + 1) * mult;
  
  return min(res, max);
}

uint16_t convert(uint8_t input){
  int max = 65535;
  int mult = 4104;

  int res = sqrt(input) * (float)mult / (float)max * convertlog(input);

  return min(res, max);
}

void loop() {
  if (Serial2.available()) {
    String input = Serial2.readStringUntil('\n');
    if (input.substring(0, 2) == "hv") {
      uint8_t val = input.substring(2, input.length()).toInt();
      ledcWrite(1, convert(val));

      // Serial.print("Hot Value: ");
      // Serial.print(val);
      // Serial.print(", ");
      // Serial.print(convert(val));
      // Serial.print("/65535");
      // Serial.println();
    }
    else if (input.substring(0, 2) == "cv") {
      uint8_t val = input.substring(2, input.length()).toInt();
      ledcWrite(2, convert(val));

      // Serial.print("Cold Value: ");
      // Serial.print(val);
      // Serial.print(", ");
      // Serial.print(convert(val));
      // Serial.print("/65535");
      // Serial.println();
    }
    else {
      Serial.print("Unknown command: ");
      Serial.print(input);
      Serial.println();
      Serial2.println("Unknown command");
    }
  }
}