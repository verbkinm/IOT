#include "WiFi.h"
#include "Adafruit_VL6180X.h"
#include "esp_wifi.h"

#include "creatorpkgs.h"
#include "IOTV_SH.h"
#include "iotv_server_embedded.h"

#define BUFSIZ 256  // по умолчанию, после компиляции, BUFSIZ = 128 для esp01 и esp32
#define RELE_PIN 23 //36 //GPIO23 //d23

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";
const char* HOSTNAME = "ESP32-id-1";

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = 20;
uint64_t cutDataSize = 0;
bool error = false;

bool releState = false;
const uint8_t DISTANCE = 150;

uint8_t readType[1] = {DATA_TYPE_BOOL};

struct IOTV_Server_embedded iot = {
    .id = 1,
    .name = "vl6180",
    .description = "ESP-32 + vl6180 + rele",
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
Adafruit_VL6180X vl = Adafruit_VL6180X();

void dataRecived(char ch);
void clearData();
void connectToWifi();

// WiFiEventHandler stationConnectedHandler;
// WiFiEventHandler stationDisconnectedHandler;

void setup() 
{
  // UART 
  Serial.begin(115200);
  vl.begin();

  pinMode(RELE_PIN, OUTPUT); 
  // // pinMode(LED_BUILTIN, OUTPUT);

  // // Wi-Fi Handlers
  // // stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  // // stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setHostname(HOSTNAME);

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

  // // LED
  // pinMode(LED_BUILTIN, OUTPUT);
  // // Сигнализируем о нормальном запуске МК
  // for (uint8_t i = 0; i < 10; i++)
  // {
  //   digitalWrite(LED_BUILTIN, HIGH);
  //   delay(100);
  //   digitalWrite(LED_BUILTIN, LOW);
  //   delay(100);
  // }
}

void loop() 
{
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
      Serial.print("Rele switch to ");
      Serial.println(releState);

      uint32_t counter = 0;
      while (vl.readRange() < DISTANCE)
      {
        if (++counter > 300) // чтобы избежать зависания в цикле. Для ESP32 ~ 2 сек. По хорошому испоьлзовать таймер!
        {
          Serial.println("break");
          break;
        }
      }
    }
    delay(500);
  }

  *(bool*)iot.readChannel[0].data = releState;
  
  if(!client.connected()) 
  {
    clearData();
    client = server.available();
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

        releState = *(bool *)iot.readChannel[0].data;
        digitalWrite(RELE_PIN, releState);
        Serial.print("Rele switch to ");
        Serial.println(releState);
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

// void onStationConnected(const WiFiEventStationModeConnected& evt) 
// {
//   Serial.print("\nStation connected: ");
//   Serial.println(evt.ssid);
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
//   Serial.print("MAC address: ");
//   Serial.println(WiFi.macAddress());
// }

// void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) 
// {
//   Serial.print("Station disconnected: ");
//   Serial.println(evt.ssid);
//   Serial.print("Code disconnected: ");
//   Serial.println(evt.reason);

//   clearData();
// }

void connectToWifi()
{
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print('.');
    delay(500);
  }
  Serial.print("\n");
}

void clearData()
{
  client.flush();
  realBufSize = 0;
  expextedDataSize = 0;
  cutDataSize = 0;
}
