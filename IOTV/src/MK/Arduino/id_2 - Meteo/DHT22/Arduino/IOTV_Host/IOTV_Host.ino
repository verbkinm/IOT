#include "DHT.h"

#include "IOTV_SH.h"
#include "iot_server.h"

#define DHTPIN 2
#define DHTTYPE DHT22

IOT_Server iotServer;
DHT dht(DHTPIN, DHTTYPE);
Array<char> arr;

void response(Array<char> &data);
void ArrayToUARTS(Array<char> &data);

void setup() 
{
  Serial.begin(115200);
  dht.begin();
  delay(500);
}

void loop() 
{
  if(Serial.available())
  {    
    arr.push_back(Serial.read());
    response(arr); 
  }

  delay(2000);
  iotServer._readChannel[0].f = dht.readTemperature();
  iotServer._readChannel[1].f = dht.readHumidity();
  iotServer._readChannel[2].f = 0;
}

void response(Array<char> &data)
{    
    Protocol_class::query_type dataType;
    
    if(data.size())
      dataType = Protocol_class::checkQueryData(data);

    if(dataType == Protocol_class::query_type::ERROR)
    {
      data.clear();
      return;
    }
  
    if(data.size() == 1 && dataType == Protocol_class::query_type::QUERY_WAY)
    {
      Protocol_class::response_WAY(iotServer, data);
      ArrayToUARTS(data);
      data.clear();
    }
    else if(data.size() == 1 && dataType == Protocol_class::query_type::QUERY_READ)
    {
      Protocol_class::response_READ(iotServer, data);
      ArrayToUARTS(data);
      data.clear();
    }
}

void ArrayToUARTS(Array<char> &data)
{
  for(int i = 0; i < data.size(); i++)
    Serial.write(data[i]); 
}
