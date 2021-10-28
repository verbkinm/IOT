#include <BME280I2C.h>
#include <Wire.h>

#include "IOTV_SH.h"
#include "iot_server.h"

BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
                  
IOT_Server iotServer;

Array<char> arr;

void response(Array<char> &data);
void ArrayToUARTS(Array<char> &data);

void setup() 
{
  Serial.begin(115200);
  Wire.begin();

  while(!bme.begin())
    delay(100);
}

void loop() 
{
  if(Serial.available())
  {    
    arr.push_back(Serial.read());
    response(arr); 
  }

  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(pres, temp, hum, tempUnit, presUnit);
   
  iotServer._readChannel[0].f = temp;
  iotServer._readChannel[1].f = hum;
  iotServer._readChannel[2].f = pres / 133.3224;
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
