#include <ESP8266WiFi.h>

#include "IOTV_SH.h"
#include "iot_server.h"

#define BUFFSIZE 64

#define PLAY_PIN D5
#define LED_PIN D7
#define MODE_1_PIN D1
#define MODE_2_PIN D6
#define MODE_3_PIN D2
#define IS_PLAYING_PIN A0

uint8_t modeArr[3] = { MODE_1_PIN, MODE_2_PIN, MODE_3_PIN };

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";

//const char* ssid = "vm";
//const char* password = "12345678";

WiFiServer server(8888);
WiFiClient client;

IOTV_Server iot;
char recivedBuffer[BUFFSIZE]{ 0 }, transmitBuffer[BUFFSIZE]{ 0 };
char* ptrBuf;

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void dataRecived();
void debug();

void setup() {
  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  ptrBuf = recivedBuffer;

  pinMode(PLAY_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MODE_1_PIN, INPUT);
  pinMode(MODE_2_PIN, INPUT);
  pinMode(MODE_3_PIN, INPUT);

  // digitalWrite(PLAY_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(MODE_1_PIN, LOW);
  digitalWrite(MODE_2_PIN, LOW);
  digitalWrite(MODE_3_PIN, LOW);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  server.begin();
}

void loop() 
{
  iot.setPlaing(isPlaying());
  if (!client.connected()) 
  {
    client.flush();
    ptrBuf = recivedBuffer;

    client = server.available();
    if (client)
      Serial.println("Connected client: " + client.remoteIP().toString() + ":" + client.remotePort());
  } else {
    while (client.available())
      dataRecived();
  }

 iot.setPlaing(isPlaying());

  if (iot.repeate() && !iot.isPlaing())
  { 
    delay(1000);
    if (!isPlaying())
      iot.setPlayStop(1);
  }

  if (iot.playStop()) 
  {
    bool tmp = isPlaying();

    pinMode(PLAY_PIN, OUTPUT);
    digitalWrite(PLAY_PIN, HIGH);
    if (tmp)
      delay(3500);
    else
      delay(500);
    digitalWrite(PLAY_PIN, LOW);
    pinMode(PLAY_PIN, INPUT);

    iot.setPlayStop(0);
  } 
  else if (iot.mode() > 0 && iot.mode() < 4) 
  {
    Serial.print("set mode: ");
    Serial.println(iot.mode());

    pinMode(modeArr[iot.mode() - 1], OUTPUT);
    digitalWrite(modeArr[iot.mode() - 1], HIGH);
    delay(500);
    digitalWrite(modeArr[iot.mode() - 1], LOW);
    pinMode(modeArr[iot.mode() - 1], INPUT);

    if ((iot.mode() != 3 && iot.isPlaing()) || (iot.mode() == 3))
      iot.setMode(0);
  }
  //!!!
  // digitalWrite(LED_PIN, iot.led());

  

  if (WiFi.status() != WL_CONNECTED) {
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

void dataRecived() {
  uint16_t dataSize = 0;
  *ptrBuf = client.read();
  ptrBuf++;

  if (ptrBuf >= (recivedBuffer + BUFFSIZE))
    ptrBuf = recivedBuffer;

  while (ptrBuf != recivedBuffer) {
    if (recivedBuffer[0] == Protocol_class::QUERY_WAY_BYTE) {
      dataSize = Protocol_class::response_WAY(iot, transmitBuffer);
      client.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
      Serial.println("WAY");
      debug();
    } else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_READ_BYTE) {
      dataSize = Protocol_class::response_READ(iot, recivedBuffer, ptrBuf, transmitBuffer);
      client.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
      Serial.println("READ");
      debug();
    } else if (recivedBuffer[0] == Protocol_class::QUERY_PING_BYTE) {
      dataSize = Protocol_class::response_Pong(transmitBuffer);
      client.write(transmitBuffer, dataSize);
      memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
      ptrBuf--;
      Serial.println("PING");
      debug();
    } else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_WRITE_BYTE) {
      // лоакальный dataSize, так как response_WRITE может вернуть -1
      Serial.println("debug WRITE");
      Serial.println(recivedBuffer[0], HEX);

      // for (int i = 0; i < 16; i++)
      //   Serial.print(recivedBuffer[i], HEX);
      // Serial.println();

      int dataSize = Protocol_class::response_WRITE(iot, recivedBuffer, ptrBuf, transmitBuffer);
      Serial.print("dataSize = ");
      Serial.println(dataSize);
      if (dataSize >= 0) {
        client.write(transmitBuffer, 1);  // ответ на write = 1 байт
        memmove((void*)recivedBuffer, (void*)&recivedBuffer[dataSize + 3], BUFFSIZE - (dataSize + 3));
        ptrBuf -= dataSize + 3;
        Serial.println("WRITE");
        debug();
      } else
        break;
    } else
      ptrBuf = recivedBuffer;

    if (ptrBuf < recivedBuffer)
      ptrBuf = recivedBuffer;
  }
}

void debug() {
  Serial.print("Play: ");
  Serial.print(iot.playStop());
  Serial.print(", isPlaying: ");
  Serial.print(isPlaying());
  Serial.print(", Led: "); 
  Serial.print(iot.led());
  Serial.print(", Repeate: ");
  Serial.print(iot.repeate());
  Serial.print(", Mode: ");
  Serial.println(iot.mode());
  Serial.print(", ADC: ");
  Serial.println(analogRead(IS_PLAYING_PIN));
  // Serial.print("Play: " + iot.play());
  // Serial.print(", Led: " + iot.led());
  // Serial.print(", Repeate: " + iot.repeate());
  // Serial.print(", Mode: " + iot.mode());
}

void onStationConnected(const WiFiEventStationModeConnected& evt) {
  Serial.print("Station connected: ");
  Serial.println(evt.ssid);
}

void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) {
  Serial.print("Station disconnected: ");
  Serial.println(evt.ssid);
  Serial.print("Code disconnected: ");
  Serial.println(evt.reason);

  client.flush();
  ptrBuf = recivedBuffer;
}

bool isPlaying()
{
  int value = 0;
  for (uint8_t i = 0; i < 5; i++)
  {
    value += analogRead(IS_PLAYING_PIN);
    delay(10);
  }

  return (value / 5) > 20;
}