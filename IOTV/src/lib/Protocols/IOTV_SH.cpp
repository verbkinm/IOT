#include "IOTV_SH.h"

QByteArray IOTV_SH::query_WAY()
{
    QByteArray data;

    data.clear();
    data.append(QUERY_WAY_BYTE);

    return data;
}

QByteArray IOTV_SH::query_READ(uint8_t channelNumber)
{
    QByteArray data;

    char channel = channelNumber << 4;
    data.append(channel | QUERY_READ_BYTE);

    return data;
}

QByteArray IOTV_SH::query_WRITE(const Base_Host &host, uint8_t channelNumber, const Raw::RAW &rawData)
{        
    QByteArray data;

    // если устройство не зарегистрировано, id == 0, то и не может быть произведена в него запись.
    if(!host.isOnline())
        return data;

    char channel = channelNumber << 4;
    data.append(channel | QUERY_WRITE_BYTE);

    if(host.getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR && rawData.str != nullptr)
    {
        quint16 strLength = strlen(rawData.str);
        data.append(strLength >> 8);
        data.append(strLength);
        data.append(rawData.str);
    }
    else
    {
        data.append(Raw::size >> 8);
        data.append(Raw::size);

        for (quint16 i = 0; i < Raw::size; i++)
            data.append(rawData.array[i]);
    }

    return data;
}

QByteArray IOTV_SH::query_PING()
{
    QByteArray data;
    data.push_back(QUERY_PING_BYTE);

    return data;
}

void IOTV_SH::response_WAY(Base_Host &iotHost, const QByteArray &data)
{
    //!!! проверка длины data
    if(checkResponsetData(data) != Response_Type::RESPONSE_WAY)
        return;

    iotHost.removeAllSubChannel();

    iotHost.setId(data.at(1));

    quint16 descriptionLength = 0;
    descriptionLength = data.at(2);
    descriptionLength <<= 8;
    descriptionLength |= data.at(3);

    uint8_t channelReadLength = data.at(4) >> 4;
    uint8_t channelWriteLength = data.at(4) & 0x0F;

    iotHost.setDescription(data.mid(5, descriptionLength));

    int index = 5 + descriptionLength;
    for (uint8_t i = 0; i < channelReadLength; i++)
        iotHost.addReadSubChannel(Raw::toDataType(data.at(index++)));

    for (uint8_t i = 0; i < channelWriteLength; i++)
        iotHost.addWriteSubChannel(Raw::toDataType(data.at(index++)));
}

void IOTV_SH::response_READ(Base_Host &iotHost, const QByteArray &data)
{
    if(checkResponsetData(data) != Response_Type::RESPONSE_READ)
        return;

    size_t channelNumber = data.at(0) >> 4;
    quint16 dataLength = (data.at(1) << 8) | data.at(2);

    QByteArray buf = data.mid(3);
    Raw::RAW rawData;

    if(iotHost.getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR)
    {
        rawData.str = new char[dataLength]; // удаляется в ~Base_Host()

        for (quint16 i = 0; i < dataLength; i++)
            rawData.str[i] = buf.at(i);

        // удаляем строковые данные в хосте и в конце метода мы их туда запишем
        Raw::RAW raw = (iotHost.getReadChannelData(channelNumber));
        if(raw.str != nullptr)
        {
            delete[] raw.str;
            raw.str = nullptr;
        }
    }
    else
    {
        for (uint8_t i = 0; i < dataLength; i++)
            rawData.array[i] = buf.at(i);
    }

    iotHost.setReadChannelData(channelNumber, rawData);
}

void IOTV_SH::response_WRITE(const Base_Host &iotHost, const QByteArray &data)
{
    Q_UNUSED(iotHost);
    Q_UNUSED(data);

    if(checkResponsetData(data) != Response_Type::RESPONSE_WRITE)
        return;
}

void IOTV_SH::response_PONG(QByteArray &data)
{
    data.clear();
    data.push_back(RESPONSE_PONG_BYTE);
}

IOTV_SH::Response_Type IOTV_SH::checkResponsetData(const QByteArray &data)
{
    if(data.length() == 0)
        return Response_Type::ERROR;

    uint8_t firstByte = data.at(0);

    if(firstByte == RESPONSE_WAY_BYTE)
    {
        if(data.size() > 6)
        {
            quint16 descriptionLength = 0;
            descriptionLength = data.at(2);
            descriptionLength <<= 8;
            descriptionLength |= data.at(3);

            uint8_t channelReadLength = data.at(4) >> 4;
            uint8_t channelWriteLength = data.at(4) & 0x0F;

            quint16 expectedLength = 5 + descriptionLength + channelReadLength + channelWriteLength;
            if(data.size() < expectedLength)
                return Response_Type::ERROR;
        }
        else
            return Response_Type::ERROR;

        return Response_Type::RESPONSE_WAY;
    }
    else if(firstByte == RESPONSE_PONG_BYTE)
        return Response_Type::RESPONSE_PONG;


    firstByte &= 0x0F;

    switch (firstByte)
    {
    case RESPONSE_READ_BYTE:
        return Response_Type::RESPONSE_READ;
        break;
    case RESPONSE_WRITE_BYTE:
        return Response_Type::RESPONSE_WRITE;
        break;
    default:
        return Response_Type::ERROR;
    }
}

uint8_t IOTV_SH::channelNumber(char byte)
{
    return (byte >> 4);
}

std::pair<bool, int> IOTV_SH::accumPacket(const QByteArray &data)
{
    Response_Type dataType = checkResponsetData(data);
    uint32_t dataSize = static_cast<uint32_t>(data.size());

    if(dataType == Response_Type::ERROR)
        return {false, 0};
    else if(dataSize >= 5 && dataType == Response_Type::RESPONSE_WAY)
    {
        quint16 descriptionLength = (data[2] << 8) | data[3];
        uint8_t readChannelCount = data[4] >> 4;
        uint8_t writeChannelCount = data[4] & 0x0F;
        uint32_t packetSize = 5 + descriptionLength + readChannelCount + writeChannelCount;
        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize >= 1 && dataType == Response_Type::RESPONSE_PONG)
    {
        return {true, 1};
    }
    else if(dataSize >= 3 && dataType == Response_Type::RESPONSE_READ)
    {
        quint16 dataLength = (data[1] << 8) | data[2];
        if(dataSize >= static_cast<uint32_t>(3 + dataLength))
            return {true, static_cast<uint32_t>(3 + dataLength)};
    }
    else if(dataSize >= 1 && dataType == Response_Type::RESPONSE_WRITE)
        return {true, 1};
    else if(dataSize > 256)
        return {false, 0};

    return {false, 0};
}
