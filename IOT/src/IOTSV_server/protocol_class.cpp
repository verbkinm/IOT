#include "protocol_class.h"

void Protocol_Class::query_WAY(QByteArray &data)
{
    data.clear();
    data.append(QUERY_WAY);
}

void Protocol_Class::query_READ(QByteArray &data, uint8_t channelNumber)
{
    data.clear();
    char channel = channelNumber << 4;
    data.append(channel | QUERY_READ);
}

void Protocol_Class::query_WRITE(QByteArray &data, uint8_t channelNumber, Raw::RAW writeData)
{
    data.clear();
    char channel = channelNumber << 4;
    data.append(channel | QUERY_WRITE);

    for (uint8_t i = 0; i < Raw::size; i++)
        data.append(writeData.array[i]);
}

void Protocol_Class::response_WAY(IOT_Host *iotHost, const QByteArray &data)
{
    if(checkResponsetData(data) != Response_Type::RESPONSE_WAY)
        return;

    iotHost->channelManager.removeAllSubChannel();
    iotHost->channelManager.eraseAllExpectedResponse();

    iotHost->setId(data.at(1));

    uint16_t descriptionLength = 0;
    descriptionLength = data.at(2);
    descriptionLength <<= 8;
    descriptionLength |= data.at(3);

    uint16_t userDescriptionLength = 0;
    userDescriptionLength = data.at(4);
    userDescriptionLength <<= 8;
    userDescriptionLength |= data.at(5);

    uint8_t channelReadLength = data.at(6) >> 4;
    uint8_t channelWriteLength = data.at(6) & 0x0F;

    iotHost->setDescription(data.mid(7, descriptionLength));
    iotHost->setUser_description(data.mid(7 + descriptionLength, userDescriptionLength));

    int index = 7 + descriptionLength + userDescriptionLength;
    for (uint8_t i = 0; i < channelReadLength; i++)
        iotHost->channelManager.addReadSubChannel(Raw::toDataType(data.at(index++)));

    for (uint8_t i = 0; i < channelWriteLength; i++)
        iotHost->channelManager.addWriteSubChannel(Raw::toDataType(data.at(index++)));
}

void Protocol_Class::response_READ(IOT_Host *iotHost, const QByteArray &data)
{
    if(checkResponsetData(data) != Response_Type::RESPONSE_READ)
        return;

    size_t channelNumber = data.at(0) >> 4;
    Raw::RAW rawData;

    for (uint8_t i = 0; i < Raw::size; i++)
        rawData.array[i] = data.at(i+1);

    iotHost->channelManager.setReadChannelData(channelNumber, rawData);
    iotHost->channelManager.eraseExpectedResponseRead(channelNumber);
}

void Protocol_Class::response_WRITE(IOT_Host *iotHost, const QByteArray &data)
{
    if(checkResponsetData(data) != Response_Type::RESPONSE_WRITE)
        return;

    size_t channelNumber = data.at(0) >> 4;
    iotHost->channelManager.eraseExpectedResponseWrite(channelNumber);
}

Protocol_Class::Response_Type Protocol_Class::checkResponsetData(const QByteArray &data)
{
    if(data.length() == 0)
        return Response_Type::ERROR;

    uint8_t firstByte = data.at(0);

    // Если выставлены оба бита Query и Request. Если бит Request не выставлен
    if( ((firstByte & 0x0C) == 0x0C) || ((firstByte & 0x04) == 0x00) )
        return Response_Type::ERROR;
    // Если выставлен бит R/W, но длина полученных данных меньше 9 байт (firstByte + RAW)
    if( ((firstByte & 0x02) == 0x02) && (data.length() < 9))
        return Response_Type::ERROR;

    firstByte &= 0x0f;

    switch (firstByte)
    {
    case 0x05:
        return Response_Type::RESPONSE_WAY;
        break;
    case 0x06:
        return Response_Type::RESPONSE_READ;
        break;
    case 0x04:
        return Response_Type::RESPONSE_WRITE;
        break;
    default:
        return Response_Type::ERROR;
    }
}
