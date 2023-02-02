#include <ESP8266WiFi.h>

#include "IOTV_SH.h"
#include "iot_server.h"

#define RELE_PIN 2
#define TOUCH_PIN 1

volatile bool releState = true;

const char* ssid = "TP-Link_A6BE";
const char* password = "41706831";

WiFiServer server(8888);
WiFiClient client;

IOT_Server iotServer;

Array<char> arr;

void response(Array<char> &data);
void ArrayToUARTS(Array<char> &data);

void setup() 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

//  while (WiFi.status() != WL_CONNECTED) 
    delay(2000);

  server.begin();

  pinMode(RELE_PIN, OUTPUT); 
  pinMode(TOUCH_PIN, INPUT); 

  digitalWrite(RELE_PIN, HIGH);
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

  if(!digitalRead(TOUCH_PIN))
  {
    delay(200);
    if(!digitalRead(TOUCH_PIN))
    {
      releState = !releState;
      digitalWrite(RELE_PIN, releState);
    }
    while(!digitalRead(TOUCH_PIN))
      delay(100);
  }

  iotServer._readChannel[0].b = !releState;  
  digitalWrite(RELE_PIN, releState);
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
    else if((dataType == Protocol_class::query_type::QUERY_WRITE) && (dataTmp.size() >= 3))
    {
      uint16_t len = (dataTmp.at(1) << 8 | dataTmp.at(2));
      if(dataTmp.size() < (3 + len))
        return;
        
      Protocol_class::response_WRITE(iotServer, dataTmp);
      if(iotServer._readChannel[0].b == releState)
        releState = !releState;
        
      data.remove(0, 3 + len);
    }
}

void ArrayToUARTS(Array<char> &data)
{
  for(int i = 0; i < data.size(); i++)
    client.write(data[i]); 
}
