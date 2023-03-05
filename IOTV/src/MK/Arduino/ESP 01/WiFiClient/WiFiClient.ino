#include <ESP8266WiFi.h>

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";
const char* HOSTNAME = "ESP01-switch";

WiFiServer server(8888);
WiFiClient client;

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void connectToWifi();

void setup() 
{
  Serial.begin(115200);

  stationConnectedHandler = WiFi.onStationModeConnected(&onStationConnected);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationDisconnected);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setHostname(HOSTNAME);

  connectToWifi();

  // pinMode(LED_BUILTIN, OUTPUT);
  // for (uint8 i = 0; i < 10; i++)
  // {
  //   digitalWrite(LED_BUILTIN, HIGH);
  //   delay(100);
  //   digitalWrite(LED_BUILTIN, LOW);
  //   delay(100);
  // }

  server.begin();
}

void loop() 
{
  if(!client.connected()) 
    client = server.available();
  else 
  {
    while(client.available())
      Serial.write(client.read());

    while(Serial.available())
      client.write(Serial.read());
  }

  if (WiFi.status() != WL_CONNECTED) 
    connectToWifi();
}

void onStationConnected(const WiFiEventStationModeConnected& evt) 
{
  // Serial.print("Station connected: ");
  // Serial.println(evt.ssid);
  // Serial.print("HOSTNAME: ");
  // Serial.println(HOSTNAME);

  // Serial.print("MAC: ");
  // Serial.println(WiFi.macAddress());
}

void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) 
{
  // Serial.print("Station disconnected: ");
  // Serial.println(evt.ssid);
  // Serial.print("Code disconnected: ");
  // Serial.println(evt.reason);

  client.flush();
}

void connectToWifi()
{
  while (WiFi.status() != WL_CONNECTED) 
    delay(500);
}
