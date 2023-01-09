#include <ESP8266WiFi.h>
#include "Adafruit_VL6180X.h"

// #define VL6180X_DEFAULT_I2C_ADDR 0x29

#include "IOTV_SH.h"
#include "iot_server.h"

#define BUFFSIZE 32

#define RELE_PIN D5
const uint8 distance = 100;

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";
const char* hostname = "ESP12-switchvl6180";

WiFiServer server(8888);
WiFiClient client;

Adafruit_VL6180X vl = Adafruit_VL6180X();

IOTV_Server iot;
char recivedBuffer[BUFFSIZE]{ 0 }, transmitBuffer[BUFFSIZE]{ 0 };
char* ptrBuf;

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void dataRecived();
void debug();
void vl6180_check();
void setReleState();

void setup() 
{
  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  ptrBuf = recivedBuffer;

  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);

  Serial.begin(115200);

  Serial.println("Adafruit VL6180x test!");
  if (!vl.begin()) 
  {
    Serial.println("Failed to find sensor");
    delay(500);
    ESP.reset();
  }
  Serial.println("Sensor found!");

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname); 
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print('.');
    delay(500);
  }

  server.begin();

  //Сигнал нормальной работы
  pinMode(LED_BUILTIN, OUTPUT);
  for (uint8 i = 0; i < 5; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
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

  vl6180_check();

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
        
        setReleState();
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

void vl6180_check()
{
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (status == VL6180X_ERROR_NONE && range < distance) 
  {
    Serial.println("Range start"); 

    delay(100);

    if (range < distance) 
    {
      iot._readChannel[0] = !iot._readChannel[0];
      setReleState();

      Serial.print("Range: "); 
      Serial.println(range);

      while (vl.readRange() < distance)
        delay(1);

      delay(1000);
    }
  }

  // Some error occurred, print it out!
  
  // if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
  //   Serial.println("System error");
  // }
  // else if (status == VL6180X_ERROR_ECEFAIL) {
  //   Serial.println("ECE failure");
  // }
  // else if (status == VL6180X_ERROR_NOCONVERGE) {
  //   Serial.println("No convergence");
  // }
  // else if (status == VL6180X_ERROR_RANGEIGNORE) {
  //   Serial.println("Ignoring range");
  // }
  // else if (status == VL6180X_ERROR_SNR) {
  //   Serial.println("Signal/Noise error");
  // }
  // else if (status == VL6180X_ERROR_RAWUFLOW) {
  //   Serial.println("Raw reading underflow");
  // }
  // else if (status == VL6180X_ERROR_RAWOFLOW) {
  //   Serial.println("Raw reading overflow");
  // }
  // else if (status == VL6180X_ERROR_RANGEUFLOW) {
  //   Serial.println("Range reading underflow");
  // }
  // else if (status == VL6180X_ERROR_RANGEOFLOW) {
  //   Serial.println("Range reading overflow");
  // }
}

void setReleState()
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
  Serial.print("hostname: ");
  Serial.println(hostname);

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