#include <Wire.h>
#include "Adafruit_VL6180X.h"
#include "IOTV_SH.h"
#include "iot_server.h"

IOT_Server iotServer;
Array<char> arr;

const byte     ledPin            = 13;    // номер вывода со светодиодом
byte           ledState          = LOW;
const int      relePin           = 10;      //пин для реле
byte           releState         = LOW;
bool           powerOn           = false;

Adafruit_VL6180X vl = Adafruit_VL6180X();

void powerShift(int _delay = 500);
void response(Array<char> &data);
void postResponse(Array<char> &data);

void debug_printArray(Array<char> &data);

void setup() 
{
  Serial.begin(115200);

  vl.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(relePin, OUTPUT);

  digitalWrite(ledPin, HIGH);
  
  for(int i = 0; i < 19; i++)
  {
    ledState = !ledState;
    digitalWrite(ledPin, ledState); 
    delay(50);
  }
  
  digitalWrite(relePin,releState);
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
  {
    powerOn = true;
    powerShift();
  }
  else if(iotServer._readChannel[0].ui8 == 1)
  {
    powerOn = false;
    powerShift();
  }
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
    {
      if(tmpData.size() < ((data[1] | data[2]) + 3))
        return;
      Protocol_class::response_WRITE(iotServer, tmpData);
    }
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

void powerShift(int _delay = 500)
{
  powerOn = !powerOn;
  if(powerOn)
  {
    ledState = LOW;
    releState = HIGH;
  }
  else 
  {
    ledState = HIGH;
    releState = LOW;
  }
  
  digitalWrite(ledPin,  ledState);
  digitalWrite(relePin, releState);
    
  delay(_delay);
}
