#include <ESP8266WiFi.h>

#include "IOTV_SH.h"
#include "iot_server.h"

#define BUFFSIZE 36

//D0 == GPIO_16
//D1 == GPIO_5
//D2 == GPIO_4
//D3 == GPIO_0
//D4 == GPIO_2
//D5 == GPIO_14
//D6 == GPIO_12
//D7 == GPIO_13
//D8 == GPIO_15
#define PLAY_PIN 5
#define LED_PIN 4
#define MODE_1_PIN 14
#define MODE_2_PIN 12
#define MODE_3_PIN 13
#define IS_PLAYING_PIN 16

uint8_t modeArr[3] = {MODE_1_PIN, MODE_2_PIN, MODE_3_PIN};

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";

//const char* ssid = "vm";
//const char* password = "12345678";

WiFiServer server(8888);
WiFiClient client;

IOTV_Server iot;
char recivedBuffer[BUFFSIZE] {0}, transmitBuffer[BUFFSIZE] {0};
char *ptrBuf; 

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void dataRecived();
void debug();

void setup() 
{
  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
  }

  server.begin();

  ptrBuf = recivedBuffer;

  pinMode(PLAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);  
  pinMode(MODE_1_PIN, OUTPUT);
  pinMode(MODE_2_PIN, OUTPUT);  
  pinMode(MODE_3_PIN, OUTPUT);  
  pinMode(IS_PLAYING_PIN, INPUT);  

  digitalWrite(PLAY_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(MODE_1_PIN, LOW);
  digitalWrite(MODE_2_PIN, LOW);
  digitalWrite(MODE_3_PIN, LOW);
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

  if (iot.repeate() && !iot.isPlaing())
    iot.setPlayStop(1);

  if (iot.playStop())
  {
    bool tmpState = digitalRead(IS_PLAYING_PIN);

    digitalWrite(PLAY_PIN, HIGH);
    delay(1000);
    digitalWrite(PLAY_PIN, LOW);

    iot.setPlaing(digitalRead(IS_PLAYING_PIN));
    if (iot.isPlaing() != tmpState)
      iot.setPlayStop(0);
  }
  else if (iot.mode() > 0 && iot.mode() < 4)
  {
    Serial.print("set mode: ");
    Serial.println(iot.mode());

    digitalWrite(modeArr[iot.mode() - 1], HIGH);
    delay(1000);
    digitalWrite(modeArr[iot.mode() - 1], LOW);

    iot.setPlaing(digitalRead(IS_PLAYING_PIN));
    if (iot.isPlaing())
      iot.setMode(0);
  }

  digitalWrite(LED_PIN, iot.led());

  //!!!
  iot.setPlaing(digitalRead(IS_PLAYING_PIN));

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
       debug();
     } 
     else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_READ_BYTE) 
     {
       dataSize = Protocol_class::response_READ(iot, recivedBuffer, ptrBuf, transmitBuffer);
       client.write(transmitBuffer, dataSize);
       memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
       ptrBuf--;
       Serial.println("READ");
       debug();
     } 
     else if (recivedBuffer[0] == Protocol_class::QUERY_PING_BYTE) 
     {
       dataSize = Protocol_class::response_Pong(transmitBuffer);
       client.write(transmitBuffer, dataSize);
       memmove((void*)recivedBuffer, (void*)&recivedBuffer[1], BUFFSIZE - 1);
       ptrBuf--;
       Serial.println("PING");
       debug();
     } 
     else if ((recivedBuffer[0] & 0x0F) == Protocol_class::QUERY_WRITE_BYTE) 
     {
       // лоакальный dataSize, так как response_WRITE может вернуть -1
        Serial.println("debug WRITE");
        Serial.println(recivedBuffer[0], HEX);

        for (int i = 0; i < 16; i++)
          Serial.print(recivedBuffer[i], HEX);
        Serial.println();
       
       int dataSize = Protocol_class::response_WRITE(iot, recivedBuffer, ptrBuf, transmitBuffer);
       Serial.print("dataSize = ");
       Serial.println(dataSize);
       if (dataSize >= 0) 
       {
         client.write(transmitBuffer, 1);  // ответ на write = 1 байт
         memmove((void*)recivedBuffer, (void*)&recivedBuffer[dataSize + 3], BUFFSIZE - (dataSize + 3));
         ptrBuf -= dataSize + 3;
         Serial.println("WRITE");
         debug();
       }
       else
        break;
     } 
     else
       ptrBuf = recivedBuffer;

     if (ptrBuf < recivedBuffer)
       ptrBuf = recivedBuffer;
   }
}

void debug() 
{
  Serial.print("Play: ");
  Serial.print(iot.playStop());
  Serial.print(", isPlaying: ");
  Serial.print(iot.isPlaing());
  Serial.print(", Led: ");
  Serial.print(iot.led());
  Serial.print(", Repeate: ");
  Serial.print(iot.repeate());
  Serial.print(", Mode: ");
  Serial.println(iot.mode());
  // Serial.print("Play: " + iot.play());
  // Serial.print(", Led: " + iot.led());
  // Serial.print(", Repeate: " + iot.repeate());
  // Serial.print(", Mode: " + iot.mode());
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
  ptrBuf = recivedBuffer;
}
