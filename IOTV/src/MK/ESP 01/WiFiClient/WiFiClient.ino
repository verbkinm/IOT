#include <ESP8266WiFi.h>

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";

WiFiServer server(8888);
WiFiClient client;

void setup() 
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
    delay(500);

  server.begin();
}

void loop() 
{
    if(!client.connected()) 
        client = server.available();
    else 
    {
        if(client.available())
            Serial.write(client.read());
        if(Serial.available())
          client.write(Serial.read());
    }
}
