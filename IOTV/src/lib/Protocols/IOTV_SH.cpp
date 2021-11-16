#include "IOTV_SH.h"

void IOTV_SH::query_WAY(QByteArray &data)
{
    data.clear();
    data.append(QUERY_WAY_BYTE);
}

qint64 IOTV_SH::query_READ(Base_Host &host, uint8_t channelNumber)
{
    QByteArray data;

    char channel = channelNumber << 4;
    data.append(channel | QUERY_READ_BYTE);

    if(host.insertExpectedResponseRead(channelNumber))
        return host.writeToServer(data);

    return -1;
}

qint64 IOTV_SH::query_WRITE(Base_Host &host, uint8_t channelNumber, Raw::RAW &rawData)
{
    QByteArray data;

    char channel = channelNumber << 4;
    data.append(channel | QUERY_WRITE_BYTE);

    if(host.getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR && rawData.str != nullptr)
    {
        quint16 strLength = strlen(rawData.str);
        data.append(strLength >> 8);
        data.append(strLength);

        for (quint16 i = 0; i < strLength; i++)
            data.append(rawData.str[i]);
    }
    else
    {
        data.append(Raw::size >> 8);
        data.append(Raw::size);

        for (quint16 i = 0; i < Raw::size; i++)
            data.append(rawData.array[i]);
    }

    if(host.insertExpectedResponseWrite(channelNumber, rawData))
        return host.writeToServer(data);
    else
        delete[] rawData.str;

    return -1;
}

void IOTV_SH::response_WAY(Base_Host &iotHost, const QByteArray &data)
{
    if(checkResponsetData(data) != Response_Type::RESPONSE_WAY)
        return;

    iotHost.removeAllSubChannel();
    iotHost.eraseAllExpectedResponse();

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
        char *ptr = new char[dataLength + 1]; // удаляется

        for (quint16 i = 0; i < dataLength; i++)
            ptr[i] = buf.at(i);
        ptr[dataLength] = '\0';

        if(iotHost.getReadChannelData(channelNumber).str != nullptr && strcmp(ptr, iotHost.getReadChannelData(channelNumber).str) == 0) //если строковые данные уже есть в хосте и они равны тем, что получили, то удаляем то, что получили
        {
            rawData.str = iotHost.getReadChannelData(channelNumber).str;
            delete[] ptr;
        }
        else
        {
            // удаляем строковые данные в хосте и записываем новые
            delete[] iotHost.getReadChannelData(channelNumber).str;
            rawData.str = ptr;
        }
    }
    else
    {
        for (uint8_t i = 0; i < dataLength; i++)
            rawData.array[i] = buf.at(i);
    }

    iotHost.setReadChannelData(channelNumber, rawData);
    iotHost.eraseExpectedResponseRead(channelNumber);
}

void IOTV_SH::response_WRITE(Base_Host &iotHost, const QByteArray &data)
{
    if(checkResponsetData(data) != Response_Type::RESPONSE_WRITE)
        return;

    uint8_t channelNumber = data.at(0) >> 4;
    iotHost.eraseExpectedResponseWrite(channelNumber);
}

IOTV_SH::Response_Type IOTV_SH::checkResponsetData(const QByteArray &data)
{
    if(data.length() == 0)
        return Response_Type::ERROR;

    uint8_t firstByte = data.at(0);

    if(firstByte == RESPONSE_WAY_BYTE)
        return Response_Type::RESPONSE_WAY;

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
    else if(dataSize > 5 && dataType == Response_Type::RESPONSE_WAY)
    {
        quint16 descriptionLength = (data[2] << 8) | data[3];
        uint8_t readChannelCount = data[4] >> 4;
        uint8_t writeChannelCount = data[4] & 0x0F;
        uint32_t packetSize = 5 + descriptionLength + readChannelCount + writeChannelCount;
        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize > 3 && dataType == Response_Type::RESPONSE_READ)
    {
        quint16 dataLength = (data[1] << 8) | data[2];
        if(dataSize >= static_cast<uint32_t>(3 + dataLength))
            return {true, static_cast<uint32_t>(3 + dataLength)};
    }
    else if(dataSize > 1 && dataType == Response_Type::RESPONSE_WRITE)
        return {true, 1};
    else if(dataSize > 256)
        return {false, 0};

    return {true, 0};
}
