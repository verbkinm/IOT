#include <ESP8266WiFi.h>
#include <hcsr04.h>


#include "IOTV_SH.h"
#include "iot_server.h"

#define BUFFSIZE 32

#define RELE_PIN D5
#define TRIG_PIN D1
#define ECHO_PIN D2

const uint8 DISTANCE = 70;
const char* SSID = "TP-Link_A6BE";
const char* PASSWORD = "41706831";
const char* HOSTNAME = "ESP12-switchvl6180";

WiFiServer server(8888);
WiFiClient client;

HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 60, 1000); 

IOTV_Server iot;
char recivedBuffer[BUFFSIZE]{ 0 }, transmitBuffer[BUFFSIZE]{ 0 };
char* ptrBuf;

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void dataRecived();
void debug();
void HC_SR04_check();
void switchRele();

void setup() 
{
  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  ptrBuf = recivedBuffer;

  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME); 
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print('.');
    delay(300);
  }

  server.begin();

  //Сигнал нормальной работы
  pinMode(LED_BUILTIN, OUTPUT);
  for (uint8 i = 0; i < 10; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void loop() 
{
  if (!client.connected()) 
  {
    client.flush();
    ptrBuf = recivedBuffer;

    client = server.available();
    if (client)
      Serial.println("Connected client: " + client.remoteIP().toString() + ":" + client.remotePort());
  } 
  else 
  {
    while (client.available())
      dataRecived();
  }

  HC_SR04_check();

  // Проверка подключения Wi-FI
  if (WiFi.status() != WL_CONNECTED) 
  {
    client.flush();
    ptrBuf = recivedBuffer;

    Serial.println("Wifi not connect");
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) 
    {
      Serial.print('.');
      delay(500);
    }
  }
}

void dataRecived() 
{
  uint16_t dataSize = 0;
  *ptrBuf = client.read();
  ptrBuf++;

  if (ptrBuf >= (recivedBuffer + BUFFSIZE))
    ptrBuf = recivedBuffer;

  while (ptrBuf != recivedBuffer) 
  {
    if (recivedBuffer[0] == Protocol_class::QUERY_WAY_BYTE) 
    {
      dataSize = Protocol_class::response_WAY(iot, transmitBuffer);
      client.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
      Serial.println("WAY");
    } 
    else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_READ_BYTE) 
    {
      dataSize = Protocol_class::response_READ(iot, recivedBuffer, ptrBuf, transmitBuffer);
      client.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
      Serial.println("READ");
    } 
    else if (recivedBuffer[0] == Protocol_class::QUERY_PING_BYTE) 
    {
      dataSize = Protocol_class::response_Pong(transmitBuffer);
      client.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
      Serial.println("PING");
    } 
    else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_WRITE_BYTE) 
    {
      // лоакальный dataSize, так как response_WRITE может вернуть -1
      int dataSize = Protocol_class::response_WRITE(iot, recivedBuffer, ptrBuf, transmitBuffer);
      Serial.print("dataSize = ");
      Serial.println(dataSize);
      if (dataSize >= 0) 
      {
        client.write(transmitBuffer, 1);  // ответ на write = 1 байт
        memmove((void*)recivedBuffer, (void*)&recivedBuffer[dataSize + 3], BUFFSIZE - (dataSize + 3));
        ptrBuf -= dataSize + 3;
        Serial.println("WRITE");
        
        switchRele();
      } 
      else
        break;
    } 
    else
      ptrBuf = recivedBuffer;

    if (ptrBuf < recivedBuffer)
      ptrBuf = recivedBuffer;
  }

  debug();
}

void debug() 
{
  Serial.print("Rele state: ");
  Serial.println(iot._readChannel[0]);
}

void HC_SR04_check()
{
  uint8_t range = hcsr04.distanceInMillimeters(); // -1 = 255
  // Serial.println(range);
  if (range < DISTANCE) 
  {
    Serial.println("Range start"); 
    delay(10);

    range = hcsr04.distanceInMillimeters();
    if (range < DISTANCE) 
    {
      iot._readChannel[0] = !iot._readChannel[0];
      Serial.println("SWITCH");
      switchRele();

      Serial.print("Range: "); 
      Serial.println(range);

      while (range < DISTANCE)
      {
        range = hcsr04.distanceInMillimeters();
        Serial.print("Loop, ");
        Serial.println(range);
        delay(50);
      }

      delay(1000);
    }
  }
}

void switchRele()
{
  if (iot._readChannel[0])
    digitalWrite(RELE_PIN, HIGH);
  else
    digitalWrite(RELE_PIN, LOW);
}

void onStationConnected(const WiFiEventStationModeConnected& evt) 
{
  Serial.print("Station connected: ");
  Serial.println(evt.ssid);
  Serial.print("HOSTNAME: ");
  Serial.println(HOSTNAME);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // Serial.print("IP: ");  
  // Serial.println(WiFi.localIP()); 
}

void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) 
{
  Serial.print("Station disconnected: ");
  Serial.println(evt.ssid);
  Serial.print("Code disconnected: ");
  Serial.println(evt.reason);

  client.flush();
  ptrBuf = recivedBuffer;
}