#include <ESP8266WiFi.h>
#include "DHT.h"

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

#define DHTPIN 2
#define DHTTYPE DHT22

// const char* ssid = "TP-Link_A6BE";
// const char* password = "41706831";
const char* ssid = "vm";
const char* password = "12345678";

WiFiServer server(8888);
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = 0;
uint64_t cutDataSize = 0;
bool error = false;

const uint8_t NUMBER_READ_CHANNEL = 2;
uint8_t readType[NUMBER_READ_CHANNEL] = {DATA_TYPE_FLOAT_32, DATA_TYPE_FLOAT_32};

struct IOTV_Server_embedded iot = {
    .id = 2,
    .name = "ESP-OLD",
    .description = "ESP-01 и DHT-22",
    .numberReadChannel = NUMBER_READ_CHANNEL,
    .readChannel = NULL,
    .readChannelType = readType,
    .numberWriteChannel = 0,
    .writeChannelType = NULL,
    .state = 1,
    .nameSize = static_cast<uint8_t>(strlen(iot.name)),
    .descriptionSize = static_cast<uint8_t>(strlen(iot.description)),
};

void dataRecived(char ch);
void connectToWifi();

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void setup() 
{
    // Wi-Fi Handlers
  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  connectToWifi();

  server.begin();

  iot.readChannel = (struct RawEmbedded *)malloc(sizeof(struct RawEmbedded) * iot.numberReadChannel);

  for (uint8_t i = 0; i < iot.numberReadChannel; ++i)
  {
    iot.readChannel[i].dataSize = dataSizeonDataType(readType[i]);
    iot.readChannel[i].data = (char *)malloc(iot.readChannel[i].dataSize);
  }
}

void loop() 
{
  if(!client.connected()) 
    client = server.available();
  else 
  {
    if(client.available())
      dataRecived(client.read());
  }

  if (WiFi.status() != WL_CONNECTED) 
    connectToWifi();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  memcpy(iot.readChannel[0].data, &temp, iot.readChannel[0].dataSize);
  memcpy(iot.readChannel[1].data, &hum, iot.readChannel[1].dataSize);

  delay(100);
  // while(isnan(iotServer._readChannel[0].f) ||  isnan(iotServer._readChannel[1].f))
  // {
  //   delay(2000);
  //   iotServer._readChannel[0].f = dht.readTemperature();
  //   iotServer._readChannel[1].f = dht.readHumidity();
  // }
}

void dataRecived(char ch)
{
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
    }

  //страховка
  if (realBufSize >= BUFSIZ)
  {
    realBufSize = 0;
    expextedDataSize = 0;
    cutDataSize = 0;
  }
}

void onStationConnected(const WiFiEventStationModeConnected& evt) 
{
  // Serial.print("Station connected: ");
  // Serial.println(evt.ssid);
}

void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) 
{
  // Serial.print("Station disconnected: ");
  // Serial.println(evt.ssid);
  // Serial.print("Code disconnected: ");
  // Serial.println(evt.reason);

  client.flush();
  realBufSize = 0;
  expextedDataSize = 0;
  cutDataSize = 0;
}

void connectToWifi()
{
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
    delay(500);
}