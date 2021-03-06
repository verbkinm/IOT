#include <ESP8266WiFi.h>
#include "DHT.h"

#include "IOTV_SH.h"
#include "iot_server.h"

#define DHTPIN 2
#define DHTTYPE DHT22

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";

WiFiServer server(8888);
WiFiClient client;

IOT_Server iotServer;
DHT dht(DHTPIN, DHTTYPE);
Array<char> arr;

void response(Array<char> &data);
void ArrayToUARTS(Array<char> &data);

void setup() 
{
  Serial.begin(115200);
  dht.begin();

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
    {
      arr.push_back(client.read());
      response(arr);
    }
  }

  iotServer._readChannel[0].f = dht.readTemperature();
  iotServer._readChannel[1].f = dht.readHumidity();

  while(isnan(iotServer._readChannel[0].f) ||  isnan(iotServer._readChannel[1].f))
  {
    delay(2000);
    iotServer._readChannel[0].f = dht.readTemperature();
    iotServer._readChannel[1].f = dht.readHumidity();
  }
}

void response(Array<char> &data)
{    
    Protocol_class::query_type dataType;
    Array<char> dataTmp(data);
    
    if(data.size())
      dataType = Protocol_class::checkQueryData(data);
    else
      return;

    if(dataType == Protocol_class::query_type::ERROR)
    {
      data.clear();
      return;
    }
  
    if(dataType == Protocol_class::query_type::QUERY_WAY)
    {
      Protocol_class::response_WAY(iotServer, dataTmp);
      ArrayToUARTS(dataTmp);
      data.remove(0, 1);
    }
    else if(dataType == Protocol_class::query_type::QUERY_READ)
    {
      Protocol_class::response_READ(iotServer, dataTmp);
      ArrayToUARTS(dataTmp);
      data.remove(0, 1);
    }
    else if(dataType == Protocol_class::query_type::QUERY_PING)
    {
      Protocol_class::response_PONG(dataTmp);
      ArrayToUARTS(dataTmp);
      data.remove(0, 1);
    }
}

void ArrayToUARTS(Array<char> &data)
{
  for(int i = 0; i < data.size(); i++)
    client.write(data[i]); 
}
