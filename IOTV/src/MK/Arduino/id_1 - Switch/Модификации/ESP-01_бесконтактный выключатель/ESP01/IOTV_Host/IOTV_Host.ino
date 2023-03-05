#include <ESP8266WiFi.h>

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

#define BUFSIZ 256  // по умолчанию, после компиляции, BUFSIZ = 128

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = 20;
uint64_t cutDataSize = 0;
bool error = false;

uint8_t readType[1] = {DATA_TYPE_BOOL};

struct IOTV_Server_embedded iot = {
    .id = 1,
    .name = "vl6180",
    .description = "ESP-01 + vl6180 + rele",
    .numberReadChannel = 1,
    .readChannel = NULL,
    .readChannelType = readType,
    .numberWriteChannel = 1,
    .writeChannelType = readType,
    .state = 1,
    .nameSize = static_cast<uint8_t>(strlen(iot.name)),
    .descriptionSize = static_cast<uint8_t>(strlen(iot.description)),
};

WiFiServer server(8888);
WiFiClient client;

void dataRecived(char ch);
void clearData();
void connectToWifi();

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void setup() 
{
  // Wi-Fi Handlers
  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  connectToWifi();

  // TCP сервер - старт
  server.begin();

  // Выделения памяти для iot структуры
  iot.readChannel = (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * iot.numberReadChannel);

  for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
  {
    iot.readChannel[i].dataSize = dataSizeonDataType(readType[i]);
    iot.readChannel[i].data = (char *)malloc(iot.readChannel[i].dataSize);
  }

  // LED
  pinMode(LED_BUILTIN, OUTPUT);
  // Сигнализируем о нормальном запуске МК
  for (uint8_t i = 0; i < 10; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  // UART 
  Serial.begin(115200);
}

void loop() 
{
  // Запросы:
  // -1 - запрос состояния реле
  // 0 - выключить реле
  // 1 - включить реле

  if (Serial.available())
  {
    char releState = Serial.read();
    if (releState != -1)
      *(bool*)iot.readChannel[0].data = releState;
  }
  
  if(!client.connected()) 
  {
    clearData();
    client = server.available();
    // if (client)
      // Serial.println("Connected client: " + client.remoteIP().toString() + ":" + client.remotePort());
  }
  else 
  {
    while(client.available())
      dataRecived(client.read());
  }

  if (WiFi.status() != WL_CONNECTED) 
    connectToWifi();
}

void dataRecived(char ch)
{
  //страховка
  if (realBufSize >= BUFSIZ)
  {
    realBufSize = 0;
    expextedDataSize = 0;
    cutDataSize = 0;
    return;
  }

  recivedBuffer[realBufSize] = ch;
  ++realBufSize;

  if (realBufSize < expextedDataSize)
        return;

  while (realBufSize > 0)
  {
    struct Header* header = createPkgs((uint8_t *)recivedBuffer, realBufSize, &error, &expextedDataSize, &cutDataSize);

    if (error == true)
    {
        realBufSize = 0;
        expextedDataSize = 0;
        cutDataSize = 0;
        return;
    }

    if (expextedDataSize > 0)
        return;

    if (header->type == Header::HEADER_TYPE_REQUEST)
    {
      if (header->assignment == Header::HEADER_ASSIGNMENT_IDENTIFICATION)
      {
        uint64_t size = responseIdentificationData(transmitBuffer, BUFSIZ, &iot);
        client.write(transmitBuffer, size);
      }
      else if(header->assignment == Header::HEADER_ASSIGNMENT_READ)
      {
        uint64_t size = responseReadData(transmitBuffer, BUFSIZ, &iot, header);
        client.write(transmitBuffer, size);
      }
      else if(header->assignment == Header::HEADER_ASSIGNMENT_WRITE)
      {
        uint64_t size = responseWriteData(transmitBuffer, BUFSIZ, &iot, header);
        client.write(transmitBuffer, size);
        // *(bool*)iot.readChannel[0].data = *(bool*)header->readWrite->data;
        Serial.write(*(bool*)header->readWrite->data);
      }
      else if(header->assignment == Header::HEADER_ASSIGNMENT_PING_PONG)
      {
        uint64_t size = responsePingData(transmitBuffer, BUFSIZ);
        client.write(transmitBuffer, size);
      }
      else if(header->assignment == Header::HEADER_ASSIGNMENT_STATE)
      {
        uint64_t size = responseStateData(transmitBuffer, BUFSIZ, &iot);
        client.write(transmitBuffer, size);
      }
    }

    memcpy(recivedBuffer, &recivedBuffer[cutDataSize], BUFSIZ - cutDataSize);
    realBufSize -= cutDataSize; // тут всегда должно уходить в ноль, если приём идёт по 1 байту!
    // realBufSize = 0;

    clearHeader(header);
  }
}

void onStationConnected(const WiFiEventStationModeConnected& evt) 
{
  // Serial.print("\nStation connected: ");
  // Serial.println(evt.ssid);
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  // Serial.print("MAC address: ");
  // Serial.println(WiFi.macAddress());
}

void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) 
{
  // Serial.print("Station disconnected: ");
  // Serial.println(evt.ssid);
  // Serial.print("Code disconnected: ");
  // Serial.println(evt.reason);

  clearData();
}

void connectToWifi()
{
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    // Serial.print('.');
    delay(500);
  }
  // Serial.print("\n");
}

void clearData()
{
  client.flush();
  realBufSize = 0;
  expextedDataSize = 0;
  cutDataSize = 0;
}