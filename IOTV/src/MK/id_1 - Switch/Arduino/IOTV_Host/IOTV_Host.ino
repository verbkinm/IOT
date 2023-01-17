#include "Adafruit_VL6180X.h"
#include "iot_server.h"

#define BUFFSIZE 32
#define RELE_PIN 10

IOTV_Server iot;
Adafruit_VL6180X vl = Adafruit_VL6180X();

char recivedBuffer[BUFFSIZE], transmitBuffer[BUFFSIZE];
char* ptrBuf;
const uint8_t DISTANCE = 150;

void dataRecived();
void checkDistance();

void setup() 
{
  Serial.begin(115200);
  vl.begin(); 

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELE_PIN, OUTPUT);

  ptrBuf = recivedBuffer;

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
  if(Serial.available() > 0)
    dataRecived();    

  checkDistance();
}

void dataRecived() 
{
  uint16_t dataSize = 0;
  *ptrBuf = Serial.read();
  ptrBuf++;

  if (ptrBuf >= (recivedBuffer + BUFFSIZE)) // проверка выхода за пределы буфера данных recivedBuffer[BUFFSIZE]
    ptrBuf = recivedBuffer;

  while (ptrBuf != recivedBuffer) 
  {
    if (recivedBuffer[0] == Protocol_class::QUERY_WAY_BYTE) 
    {
      dataSize = Protocol_class::response_WAY(iot, transmitBuffer);
      Serial.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1); // !!! (void*) убрать?
      ptrBuf--;
    } 
    else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_READ_BYTE) 
    {
      dataSize = Protocol_class::response_READ(iot, recivedBuffer, ptrBuf, transmitBuffer);
      Serial.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
    } 
    else if (recivedBuffer[0] == Protocol_class::QUERY_PING_BYTE) 
    {
      dataSize = Protocol_class::response_Pong(transmitBuffer);
      Serial.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
    } 
    else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_WRITE_BYTE) 
    {
      // лоакальный dataSize, так как response_WRITE может вернуть -1
      int dataSize = Protocol_class::response_WRITE(iot, recivedBuffer, ptrBuf, transmitBuffer);
      if (dataSize >= 0) 
      {
        Serial.write(transmitBuffer, 1);  // ответ на write = 1 байт
        memmove((void*)recivedBuffer, (void*)&recivedBuffer[dataSize + 3], BUFFSIZE - (dataSize + 3));
        ptrBuf -= dataSize + 3;
        digitalWrite(RELE_PIN, iot._readChannel[0]);
      } 
      else
        break;
    } 
    else
      ptrBuf = recivedBuffer;

    // Если ушел левее буфера
    if (ptrBuf < recivedBuffer)
      ptrBuf = recivedBuffer;
  }
}

void checkDistance()
{
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE) 
  {
    delay(100);
    range = vl.readRange();
    if (range < DISTANCE)
    {
      iot._readChannel[0] = !iot._readChannel[0];
      digitalWrite(RELE_PIN, iot._readChannel[0]);
      while (vl.readRange() < DISTANCE);
    }
    delay(500);
  }
}