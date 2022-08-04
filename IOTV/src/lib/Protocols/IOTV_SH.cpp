#include "IOTV_SH.h"

QByteArray IOTV_SH::query_WAY()
{
    return {1, QUERY_WAY_BYTE};
}

QByteArray IOTV_SH::query_READ(uint8_t channelNumber)
{
    QByteArray data;

    uint8_t channel = channelNumber << 4;
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
        uint16_t strLength = strlen(rawData.str);
        data.append(strLength >> 8);
        data.append(strLength);
        data.append(rawData.str);
    }
    else
    {
        data.append(Raw::size >> 8);
        data.append(Raw::size);

        for (uint16_t i = 0; i < Raw::size; i++)
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

    uint16_t descriptionLength = 0;
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
    uint16_t dataLength = (data.at(1) << 8) | static_cast<uint8_t>(data.at(2)); //  cast - чтобы не было отрицательного значения dataLength

    QByteArray buf = data.mid(3);
    Raw::RAW rawData {0};

    if(iotHost.getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR)
    {
        Raw::RAW rawHost = iotHost.getReadChannelData(channelNumber);
        if(rawHost.str != nullptr)
        {
            std::string sHost {rawHost.str};
            std::string sBuf {buf.data()};
            if (*rawHost.str != *buf.data())
//            if (sHost != sBuf)
            {
                delete[] rawHost.str;
                rawHost.str = nullptr;

                rawData.str = new char[dataLength]; // удаляется в ~Base_Host()
                memcpy(rawData.str, buf.data(), dataLength);
            }
            else
                rawData = rawHost;
        }
        else
        {
            rawData.str = new char[dataLength]; // удаляется в ~Base_Host()
            memcpy(rawData.str, buf.data(), dataLength);
        }
    }
    else
        memcpy(rawData.array, buf.data(), dataLength);

    iotHost.setReadChannelData(channelNumber, rawData);
}

void IOTV_SH::response_WRITE(const Base_Host &iotHost, const QByteArray &data)
{
    //!!!
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
            uint16_t descriptionLength = 0;
            descriptionLength = data.at(2);
            descriptionLength <<= 8;
            descriptionLength |= data.at(3);

            uint8_t channelReadLength = data.at(4) >> 4;
            uint8_t channelWriteLength = data.at(4) & 0x0F;

            uint16_t expectedLength = 5 + descriptionLength + channelReadLength + channelWriteLength;
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

uint8_t IOTV_SH::channelNumber(uint8_t byte)
{
    return (byte >> 4);
}

std::pair<bool, uint32_t> IOTV_SH::accumPacket(const QByteArray &data)
{
    Response_Type dataType = checkResponsetData(data);
    uint32_t dataSize = data.size();

    if(dataType == Response_Type::ERROR)
        return {false, 0};
    else if(dataSize >= 5 && dataType == Response_Type::RESPONSE_WAY)
    {
        uint16_t descriptionLength = (data[2] << 8) | data[3];
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
        uint16_t dataLength = (data[1] << 8) | data[2];
        if(dataSize >= (3u + dataLength))
            return {true, 3 + dataLength};
    }
    else if(dataSize >= 1 && dataType == Response_Type::RESPONSE_WRITE)
        return {true, 1};
    else if(dataSize > 256) // максимальный размер пакета данных
        return {false, 0};

    return {false, 0};
}
