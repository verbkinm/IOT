#include "iot_server.h"

IOTV_Server iot;
char recivedBuffer[BUFFSIZE]{ 0 }, transmitBuffer[BUFFSIZE]{ 0 };
char* ptrBuf;

void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  if(Serial.available())
  {   
    char d = Serial.read();
    // data.push_back(Serial.read());
    if (d == '1')
      digitalWrite(LED_BUILTIN, HIGH);
    else if (d == '2')
      digitalWrite(LED_BUILTIN, LOW);
    // response(data); 
  }
}

// void response(std::vector<uint8_t> &data)
// {    
//     Protocol_class dataType;
    
//     if(data.size())
//       dataType = Protocol_class::checkQueryData(data);

//     if(dataType == Protocol_class::query_type::ERROR)
//     {
//       data.clear();
//       return;
//     }
  
//     if(data.size() == 1 && dataType == Protocol_class::query_type::QUERY_WAY)
//     {
//       Protocol_class::response_WAY(iotServer, data);
//       ArrayToUARTS(data);
//       data.clear();
//     }
//     else if(data.size() == 1 && dataType == Protocol_class::query_type::QUERY_READ)
//     {
//       Protocol_class::response_READ(iotServer, data);
//       ArrayToUARTS(data);
//       data.clear();
//     }
// }

// void ArrayToUARTS(std::vector<uint8_t> &data)
// {
//   for(int i = 0; i < data.size(); i++)
//     Serial.write(data[i]); 
// }
