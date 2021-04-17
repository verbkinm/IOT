#include "protocol_class.h"

Protocol_class::query_type Protocol_class::checkQueryData(const char *data)
{
    uint8_t length = 0;
    const char *buffData = data;
    while (*buffData)
    {
        length++;
        buffData++;
    }

    if(length == 0)
        return query_type::ERROR;

     uint8_t firstByte = data[0];
     firstByte &= 0x0F;

     switch (firstByte)
     {
     case 0x09:
         return query_type::QUERY_WAY;
         break;
     case 0x0A:
         return query_type::QUERY_READ;
         break;
     case 0x08:
         return query_type::QUERY_WRITE;
         break;
     default:
         return query_type::ERROR;
     }
    }

uint8_t Protocol_class::getChannelNumber(char data)
{
    data >>= 4;
    return data;
}
