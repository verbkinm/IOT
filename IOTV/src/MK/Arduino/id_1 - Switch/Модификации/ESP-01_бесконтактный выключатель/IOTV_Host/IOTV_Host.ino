#include "Adafruit_VL6180X.h"

#define RELE_PIN 10

volatile bool releState = false;
const uint8_t DISTANCE = 150;

Adafruit_VL6180X vl = Adafruit_VL6180X();

void setup() 
{
  Serial.begin(115200);
  vl.begin();

  pinMode(RELE_PIN, OUTPUT); 
  pinMode(LED_BUILTIN, OUTPUT);

  // Сигнализируем о нормальном запуске МК
  for (uint8_t i = 0; i < 10; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void loop() 
{
  Serial.write(releState);

  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) 
  {
    delay(100);
    range = vl.readRange();
    if (range < DISTANCE)
    {
      releState = !releState;
      digitalWrite(RELE_PIN, releState);
      uint16_t counter = 0;
      while (vl.readRange() < DISTANCE)
      {
        if (++counter > 500) // чтобы избежать зависания в цикле. Для ATmega168 (16 МГц) ~ 6 сек Для ATmega328 (8 МГц) ~ 3 сек. По хорошому испоьлзовать таймер!
          break;
      }
    }
    delay(500);
  }

  if (Serial.available())
  {
    int newState = Serial.read();
    if (newState != -1)
    {
        releState = newState;
        digitalWrite(RELE_PIN, releState);
    }
  }

  // delay(100);
}
