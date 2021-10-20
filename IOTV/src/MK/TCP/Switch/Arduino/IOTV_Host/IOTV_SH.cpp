#include "IOTV_SH.h"

Protocol_class::query_type Protocol_class::checkQueryData(const Array<char> &data)
{
    if(data.size() == 0)
        return query_type::ERROR;

    uint8_t firstByte = data[0];
    //firstByte &= 0x0F;

    if(firstByte == 0x01)
      return query_type::QUERY_WAY;
    else if(firstByte == 0x02)
      return query_type::QUERY_READ;
    else if( (firstByte & 0x0F) == 0 )
    {
      uint16_t length = data[1] | data[2];
      if(data.size() >= 3 + length)
        return query_type::QUERY_WRITE;
    }

    return query_type::ERROR;
}

void Protocol_class::response_WAY(const IOT_Server &iotHost, Array<char> &data)
{
    data.clear();
    uint8_t byte = 0x05;
    data.push_back(byte);

    data.push_back(iotHost._id);

    byte = iotHost._description.length() >> 8;
    data.push_back(byte);
    byte = iotHost._description.length();
    data.push_back(byte);

    byte = READ_CHANNEL_LENGTH << 4;
    byte |= WRITE_CHANNEL_LENGTH;
    data.push_back(byte);

    for (char byte : iotHost._description)
        data.push_back(byte);

    for (uint8_t i = 0; i < READ_CHANNEL_LENGTH; i++)
        data.push_back(Raw::toUInt8(iotHost._readChannelType[i]));

    for (uint8_t i = 0; i < WRITE_CHANNEL_LENGTH; i++)
        data.push_back(Raw::toUInt8(iotHost._writeChannelType[i]));
}

void Protocol_class::response_READ(const IOT_Server &iotHost, Array<char> &data)
{
    uint8_t chNumber = data[0] >> 4;
    data.clear();

    data.push_back((chNumber << 4) | 0x06 );
    if(chNumber >= READ_CHANNEL_LENGTH)
    {
        data.push_back(0x00);
        return;
    }

    data.push_back(0x00);
    data.push_back(0x08);

    Raw::RAW raw = iotHost._readChannel[chNumber];
    for (uint8_t i = 0; i < 8; i++)
        data.push_back(raw.array[i]);
}

void Protocol_class::response_WRITE(IOT_Server &iotHost, Array<char> &data)
{
    uint8_t chNumber = data[0] >> 4;
    uint16_t length = data[1] | data[2];

    if(data.size() < 3 + length)
      return;

    for (size_t i = 0; i < length; i++)
        iotHost._writeChannel[chNumber].array[i] = data[i + 3];

    iotHost.update();

    data.clear();
    data.push_back((chNumber << 4) | 0x04 );
}