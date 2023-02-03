#include <ESP8266WiFi.h>

#include <creatorpkgs.h>
#include <IOTV_SH.h>
#include <iotv_server_embedded.h>

#define PLAY_PIN D5
// #define LED_PIN D7
#define MODE_1_PIN D1
#define MODE_2_PIN D6
#define MODE_3_PIN D2
#define IS_PLAYING_PIN A0

int8_t modeArr[3] = { MODE_1_PIN, MODE_2_PIN, MODE_3_PIN };

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";

WiFiServer server(8888);
WiFiClient client;

//**************************************************
//               IOTVP_embedded

char recivedBuffer[BUFSIZ], transmitBuffer[BUFSIZ];
uint64_t realBufSize = 0;

uint64_t expextedDataSize = 0;
uint64_t cutDataSize = 0;
bool error = false;

/*
Каналы чтения/записи:
0 - ADC. Запись в этот канал ничего не делает
1 - R. Флаг повтора, при выключении музыки
2 - M. Режим воспроизведения. Если в канале чтения -1, то ничего не происходит. 0, 1, 2 - запуск одного из режимов.
3 - T. Флаг переключения, если стоит значение 1, то необходимо включить/выключить воспроизведение музыки.
*/
#define ADC 0
#define REPEATE 1
#define MODE 2
#define TRIGGER 3
#define ADC_BORDER 20 // граничное значение ADC для определения воспроизведения музыки

const uint8_t NUMBER_READ_CHANNEL = 4;
const uint8_t NUMBER_WRITE_CHANNEL = NUMBER_READ_CHANNEL;

const uint8_t readType[NUMBER_READ_CHANNEL] = { 
                                                DATA_TYPE_INT_16, 
                                                DATA_TYPE_BOOL, 
                                                DATA_TYPE_INT_8, 
                                                DATA_TYPE_BOOL
                                              };
struct IOTV_Server_embedded iot = {
    .id = 4,
    .name = "Cloud",
    .description = "Облачко - музыкальный ночник для Евы-Королевы",
    .numberReadChannel = NUMBER_READ_CHANNEL,
    .readChannel = NULL,
    .readChannelType = (uint8_t *)readType,
    .numberWriteChannel = NUMBER_WRITE_CHANNEL,
    .writeChannelType = (uint8_t *)readType,
    .state = 1,
    .nameSize = static_cast<uint8_t>(strlen(iot.name)),
    .descriptionSize = static_cast<uint8_t>(strlen(iot.description)),
};

//               IOTVP_embedded
//**************************************************

void dataRecived(char ch);
uint8_t avrADC();
void connectToWifi();
void debug();

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void setup() 
{
  // Wi-Fi Handlers
  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  // Настройка портов ввода/вывода
  pinMode(PLAY_PIN, INPUT);
  pinMode(MODE_1_PIN, INPUT);
  pinMode(MODE_2_PIN, INPUT);
  pinMode(MODE_3_PIN, INPUT);

  digitalWrite(MODE_1_PIN, LOW);
  digitalWrite(MODE_2_PIN, LOW);
  digitalWrite(MODE_3_PIN, LOW);

  // UART 
  Serial.begin(115200);

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
    iot.readChannel[i].data = (char *)calloc(iot.readChannel[i].dataSize, sizeof(char));
  }
}

void loop() 
{
  if(!client.connected()) 
  {
    client = server.available();
    if (client)
      Serial.println("Connected client: " + client.remoteIP().toString() + ":" + client.remotePort());
  }
  else 
  {
    if(client.available())
      dataRecived(client.read());
  }

  if (WiFi.status() != WL_CONNECTED) 
    connectToWifi();

  // Записываем сренее значение ADC в канал чтения ADC
  int16_t resADC = avrADC();
  memcpy(iot.readChannel[ADC].data, &resADC, iot.readChannel[ADC].dataSize);

  // Если установлен флаг переключения состояния воспроизведения
  if (*(bool *)(iot.readChannel[TRIGGER].data)) 
  {
    // Подаём логическую единицу на ножку PLAY_PIN
    pinMode(PLAY_PIN, OUTPUT);
    digitalWrite(PLAY_PIN, HIGH);
    
    // Если мелодия воспроизводится, делаем задержку на 3000 мс (для выключения), иначе на 500 мс для включения
    if (*(int16_t *)iot.readChannel[ADC].data > ADC_BORDER)
      delay(3000);
    else
      delay(500);

    // Подаём логический ноль на ножку PLAY_PIN и переключаем режим на INPUT
    digitalWrite(PLAY_PIN, LOW);
    pinMode(PLAY_PIN, INPUT);

    // Снимаем флаг переключения состояния воспроизведения
    *(bool *)iot.readChannel[TRIGGER].data = false;
  } 
  // Если установлен режим в диапазоне -1 < mode < 3
  else if ((*(int8_t *)iot.readChannel[MODE].data > -1) && (*(int8_t *)iot.readChannel[MODE].data < 3)) 
  {
    // Устанавливаем логическую единицу на ножке соответствующей переданному режиму
    pinMode(modeArr[*(uint8_t *)iot.readChannel[MODE].data], OUTPUT);
    digitalWrite(modeArr[*(uint8_t *)iot.readChannel[MODE].data], HIGH);
    delay(500);
    // Устанавливаем логический ноль на ножке соответствующей переданному режиму и переключаем режим на INPUT
    digitalWrite(modeArr[*(uint8_t *)iot.readChannel[MODE].data], LOW);
    pinMode(modeArr[*(uint8_t *)iot.readChannel[MODE].data], INPUT);

    // Убираем присутствие переключения режима
    *iot.readChannel[MODE].data = -1;
  }
  // Если установлен флаг повтора и мелодия не воспроизводится
  else if (*(bool *)iot.readChannel[REPEATE].data && (*(int16_t *)iot.readChannel[ADC].data < ADC_BORDER)) // iot.readChannel[ADC].data > ADC_BORDER) - идёт воспроизведение
  {
    static uint8_t repeateCounter;
    ++repeateCounter;
    // Делаем задержку 
    delay(1000);

    if (repeateCounter > 4)
    {
      *(bool *)(iot.readChannel[TRIGGER].data) = true;
      repeateCounter = 0;
    }
  }

  // delay(500);
  // debug();
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
  Serial.print("Station connected: ");
  Serial.println(evt.ssid);
}

void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) 
{
  Serial.print("Station disconnected: ");
  Serial.println(evt.ssid);
  Serial.print("Code disconnected: ");
  Serial.println(evt.reason);

  client.flush();
  realBufSize = 0;
  expextedDataSize = 0;
  cutDataSize = 0;
}

uint8_t avrADC()
{
  int value = 0;
  for (uint8_t i = 0; i < 5; i++)
  {
    value += analogRead(IS_PLAYING_PIN);
    delay(10);
  }

  return (value / 5);
}

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

void debug() 
{
  Serial.print("CH0 (ADC): ");
  Serial.println(*(int16_t *)iot.readChannel[ADC].data);
  Serial.print("CH1, (Repeate): ");
  Serial.print(*(bool *)iot.readChannel[REPEATE].data);
  Serial.print("CH2 (Mode): ");
  Serial.println(*(int8_t *)iot.readChannel[MODE].data);
  Serial.print("CH3 (Trigger): ");
  Serial.println(*(bool *)iot.readChannel[TRIGGER].data);
}