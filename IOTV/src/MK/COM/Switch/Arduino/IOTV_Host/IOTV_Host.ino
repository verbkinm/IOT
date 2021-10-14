#include "IOTV_SH.h"
#include "iot_server.h"

IOT_Server iotServer;

Array<char> arr;

void response(Array<char> &data);
void postResponse(Array<char> &data);

void debug_printArray(Array<char> &data);

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  //Serial.setTimeout(1000);
}

void loop() 
{
  if(Serial.available() > 0)
  {
    String buffer = Serial.readString();

    if(buffer.length() < 256)
    {
      arr.clear();
      for(int i = 0; i < buffer.length(); i++)
        arr.push_back((char)buffer[i]);
  
      //debug_printArray(arr);
      buffer = "";
      
      response(arr); 
    }
  }

  if(iotServer._readChannel[0].ui8 == 0)
    digitalWrite(LED_BUILTIN, LOW);
  else if(iotServer._readChannel[0].ui8 == 1)
    digitalWrite(LED_BUILTIN, HIGH);
  
}

void response(Array<char> &data)
{    
    Array<char> tmpData; // если написать Array<char> tmpData = data, происходит сбой!!!
    tmpData = data;

    Protocol_class::query_type dataType = Protocol_class::checkQueryData(tmpData);

    if(dataType == Protocol_class::query_type::QUERY_WAY)
      Protocol_class::response_WAY(iotServer, tmpData);
    else if(dataType == Protocol_class::query_type::QUERY_READ)
      Protocol_class::response_READ(iotServer, tmpData);
    else if(dataType == Protocol_class::query_type::QUERY_WRITE)
      Protocol_class::response_WRITE(iotServer, tmpData);
   else
      return;

  debug_printArray(tmpData);

  postResponse(data);
}

void postResponse(Array<char> &data)
{
    Protocol_class::query_type dataType = Protocol_class::checkQueryData(data);

    if(dataType == Protocol_class::query_type::QUERY_WAY)
        data.remove(0, 1);
    else if(dataType == Protocol_class::query_type::QUERY_READ)
        data.remove(0, 1);
    else if(dataType == Protocol_class::query_type::QUERY_WRITE)
        data.remove(0, (data[1] | data[2]) + 3);
    else if(dataType == Protocol_class::query_type::ERROR)
        data.clear();
        
    if(data.size())
        response(data);
}

void debug_printArray(Array<char> &data)
{
  for(int i = 0; i < data.size(); i++)
    Serial.print(data[i]); 

  Serial.print("\r\n");
}
