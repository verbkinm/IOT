#include "IOTV_SH.h"
#include "iot_server.h"

#define RELE_PIN 10

IOT_Server iotServer;

Array<char> arr;

void response(Array<char> &data);
void ArrayToUARTS(Array<char> &data);

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELE_PIN, OUTPUT);
  //Serial.setTimeout(1000);
}

void loop() 
{
  if(Serial.available() > 0)
  {    
    arr.push_back(Serial.read());
    response(arr); 
  }

  if(iotServer._readChannel[0].ui8 == 0)
  {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(RELE_PIN, LOW);
  }
  else if(iotServer._readChannel[0].ui8 == 1)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(RELE_PIN, HIGH);
  }
}

void response(Array<char> &data)
{    
    Protocol_class::query_type dataType;
    
    if(data.size() > 0)
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
    else if(data.size() > 3 && dataType == Protocol_class::query_type::QUERY_WRITE)
    {
      uint16_t dataLength = (data[1] << 8) | data[2];
      if(data.size() == (3 + dataLength))
      {
        Protocol_class::response_WRITE(iotServer, data);
        ArrayToUARTS(data);
        data.clear();
      }
    }
}

void ArrayToUARTS(Array<char> &data)
{
  for(int i = 0; i < data.size(); i++)
    Serial.write(data[i]); 
}
