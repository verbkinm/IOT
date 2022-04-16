#include "iotv_sc.h"

void IOTV_SC::query_Device_List(QByteArray &data)
{
    data.clear();
    data.append(0x01);
}

bool IOTV_SC::query_STATE(QByteArray &data, const QString &deviceName)
{
    data.clear();

    if(deviceName.length() > MAX_LENGTH_DEVICE_NAME)
        return false;

    data.append(deviceName.length() << 3);
    data.append(0x10);
    data.append(deviceName.toUtf8());

    return true;
}

qint64 IOTV_SC::query_READ(Base_Host &host, const QString &deviceName, uint8_t channelNumber)
{
    QByteArray data;

    if(deviceName.length() > MAX_LENGTH_DEVICE_NAME || channelNumber > 0x0f)
        return false;

    data.append( (deviceName.length() << 3) | 0x02);
    data.append(channelNumber);
    data.append(deviceName.toUtf8());

    return host.writeToServer(data);
}

qint64 IOTV_SC::query_WRITE(Base_Host &host, const QString &deviceName, uint8_t channelNumber, Raw::RAW &rawData)
{
    QByteArray data;

    if(deviceName.length() > MAX_LENGTH_DEVICE_NAME || channelNumber > 0x0f)
        return -1;

    data.append(deviceName.length() << 3);
    data.append(channelNumber);

    if(host.getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR && rawData.str != nullptr)
    {
        char* ptr = rawData.str;
        quint16 strLength = strlen(ptr);

        data.append(strLength >> 8);
        data.append(strLength);

        data.append(deviceName.toUtf8());

        for (quint16 i = 0; i < strLength; i++)
            data.append(ptr[i]);
    }
    else
    {
        data.append(Raw::size >> 8);
        data.append(Raw::size);

        data.append(deviceName.toUtf8());

        for (quint16 i = 0; i < Raw::size; i++)
            data.append(rawData.array[i]);
    }

    return host.writeToServer(data);
//        delete[] rawData.str;
}

QByteArrayList IOTV_SC::response_Device_List(const QByteArray &data)
{
    QByteArrayList result;
    uint8_t countDivece = data.at(1);
    QByteArray buff = data.mid(2);

    for(uint8_t i = 0; i < countDivece; i++)
    {
        uint8_t nameLength = buff.at(0) >> 3;
        quint16 dataLength = buff.at(1 + nameLength + 1) << 8 | buff.at(1 + nameLength + 2);
        uint8_t readChannelCount = buff.at(1 + nameLength + 3) >> 4;
        uint8_t writeChannelCount = buff.at(1 + nameLength + 3) & 0x0f;
        uint packageLength = 5 + dataLength + readChannelCount + writeChannelCount;

        QByteArray package = buff.mid(0, packageLength);
        result.append(package);

        buff.remove(0, packageLength);
    }
    return result;
}

void IOTV_SC::serverResponse_STATE(Base_Host &host, const QByteArray &data)
{
    bool state = data.at(1) & DEVICE_STATE_RESPONSE_BIT;
    host.setOnline(state);
}

void IOTV_SC::serverResponse_READ(Base_Host &host, const QByteArray &data)
{
    uint8_t nameLength = data.at(0) >> 3;
    uint8_t channelNumber = data.at(1) & 0x0f;
    quint16 dataLength = (data.at(2) << 8) | data.at(3);

    QByteArray buf = data.mid(4 + nameLength);
    Raw::RAW rawData;

    if(host.getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR)
    {
        char *ptr = new char[dataLength + 1];

        for (quint16 i = 0; i < dataLength; i++)
            ptr[i] = buf.at(i);
        ptr[dataLength] = '\0';

        if(host.getReadChannelData(channelNumber).str != nullptr && strcmp(ptr, host.getReadChannelData(channelNumber).str) == 0)
        {
            rawData.str = host.getReadChannelData(channelNumber).str;
            delete[] ptr;
        }
        else
        {
            delete[] host.getReadChannelData(channelNumber).str;
            rawData.str = ptr;
        }
    }
    else
    {
        for (uint8_t i = 0; i < dataLength; i++)
            rawData.array[i] = buf.at(i);
    }

    host.setReadChannelData(channelNumber, rawData);
}

void IOTV_SC::serverResponse_WRITE(Base_Host &host, const QByteArray &data)
{
    Q_UNUSED(host);
    Q_UNUSED(data);
//    uint8_t channelNumber = data.at(1) & 0x0f;
//    host.eraseExpectedResponseWrite(channelNumber);
}

void IOTV_SC::responceToClient_Device_One(Base_Host &host, QByteArray &data)
{
    data.clear();
    QString name = host.getName();
    uint8_t nameLength = (name.length()) << 3;
    data.append(nameLength);
    data.append(host.getName().toUtf8());
    data.append(host.getId());
    data.append(host.getDescription().size() >> 8);
    data.append(host.getDescription().size());
    data.append((host.readChannelLength() << 4) | host.writeChannelLength() );
    data.append(host.getDescription().toUtf8());

    for (uint8_t i = 0; i <  host.readChannelLength(); i++ )
        data.append(Raw::toUInt8(host.getReadChannelDataType(i)));
    for (uint8_t i = 0; i <  host.writeChannelLength(); i++ )
        data.append(Raw::toUInt8(host.getWriteChannelDataType(i)));
}

void IOTV_SC::responceToClient_State(const Base_Host &host, QByteArray &data)
{
    data.clear();

    uint8_t state;
    if(host.isOnline())
        state = 1;
    else
        state = 0;

    state <<= 5;

    data.append( (host.getName().length() << 3) | QUERY_RESPONSE_BIT);
    data.append(0x10 | state);
    data.append(host.getName().toUtf8());
}

void IOTV_SC::responceToClient_Read(const Base_Host &host, QByteArray &data)
{
    uint8_t nameLength = data.at(0) >> 3;
    uint8_t channelNumber = data.at(1) & 0x0F;
    QString deviceName = data.mid(2, nameLength);

    Raw::RAW raw = host.getReadChannelData(channelNumber);

    data.clear();

    data.append((nameLength << 3) | RESPONCE_READ);
    data.append(channelNumber);

    if(host.getReadChannelDataType(channelNumber) == Raw::DATA_TYPE::CHAR_PTR && host.getReadChannelData(channelNumber).str != nullptr)
    {
        size_t strLength = strlen(raw.str);

        data.append(strLength >> 8);
        data.append(strLength);
        data.append(deviceName.toUtf8());

        for (uint8_t i = 0; i < strLength; i++)
            data.append(raw.str[i]);
    }
    else
    {
        data.append(Raw::size >> 8);
        data.append(Raw::size);

        data.append(deviceName.toUtf8());

        for (uint8_t i = 0; i < Raw::size; i++)
            data.append(raw.array[i]);
    }
}

void IOTV_SC::responceToClient_Write(QByteArray &data)
{
    uint8_t nameLength = data.at(0) >> 3;
    uint8_t channelNumber = data.at(1) & 0x0F;
    QByteArray deviceName = data.mid(4, nameLength);

    data.clear();

    data.append((nameLength << 3) | RESPONCE_WRITE);
    data.append(channelNumber);
    data.append(deviceName);
}

IOTV_SC::Response_Type IOTV_SC::checkResponsetData(const QByteArray &data)
{
    if(data.length() < 2)
        return Response_Type::RESPONSE_ERROR;

    uint8_t firstByte = data.at(0);
    uint8_t secondByte = data.at(1);
    uint8_t nameLength = data.at(0) >> 3;

    //Если не установлен бит ответа
    if( !(firstByte & QUERY_RESPONSE_BIT))
        return Response_Type::RESPONSE_ERROR;

    //DEVISE_LIST
    if(firstByte & DEVICE_BIT)
        return Response_Type::RESPONSE_DEVICE_LIST;
    //DEVICE_STATE
    else if( !(firstByte & READ_WRITE_BIT) && (secondByte & DEVICE_STATE_QUERY_BIT))
    {
        if(data.length() >= (2 + nameLength))
            return Response_Type::RESPONSE_STATE;
    }
    //DEVICE_READ
    else if( (firstByte & READ_WRITE_BIT) && !(secondByte & DEVICE_STATE_QUERY_BIT))
    {
        if(data.length() >= 4)
            return Response_Type::RESPONSE_READ;
    }
    //DEVICE WRITE
    else if( !(firstByte & READ_WRITE_BIT) && !(secondByte & DEVICE_STATE_QUERY_BIT))
        return Response_Type::RESPONSE_WRITE;

    return Response_Type::RESPONSE_ERROR;
}

IOTV_SC::Query_Type IOTV_SC::checkQueryData(const QByteArray &data)
{
    if(data.length() == 0)
        return Query_Type::QUERY_ERROR;

    uint8_t firstByte = data.at(0);

    //Если установлен бит ответа
    if(firstByte & QUERY_RESPONSE_BIT)
        return Query_Type::QUERY_ERROR;

    //DEVISE_LIST
    if(firstByte & DEVICE_BIT)
    {
        if(firstByte == DEVICE_BIT)
            return Query_Type::QUERY_DEVICE_LIST;
        else
            return Query_Type::QUERY_ERROR;
    }

    uint8_t secondByte = data.at(1);

    //DEVICE_STATE
    if( !(firstByte & (READ_WRITE_BIT)) && (secondByte & DEVICE_STATE_QUERY_BIT) )
        return Query_Type::QUERY_STATE;
    //DEVICE_READ
    else if( (firstByte & READ_WRITE_BIT) && ( !(secondByte & DEVICE_STATE_QUERY_BIT) ) )
        return Query_Type::QUERY_READ;
    //DEVICE_WRITE
    else if( !(firstByte & READ_WRITE_BIT) && ( !(secondByte & DEVICE_STATE_QUERY_BIT) ) )
    {
        if(data.length() > 4)
            return Query_Type::QUERY_WRITE;
    }

    return Query_Type::QUERY_ERROR;
}

uint8_t IOTV_SC::channelNumber(char byte)
{
    return byte & 0x0F;
}

//!!! не проверяется длина данных
bool IOTV_SC::responseName(const QByteArray &data, QString &returnName)
{
    uint8_t nameLength = 0;

    Response_Type dataType = checkResponsetData(data);
    if(dataType == Response_Type::RESPONSE_DEVICE_LIST || dataType == Response_Type::RESPONSE_ERROR)
        return false;
    else if(dataType == Response_Type::RESPONSE_READ)
    {
        nameLength = data.at(0) >> 3;
        returnName = data.mid(4, nameLength);
        return true;
    }
    else if(dataType == Response_Type::RESPONSE_STATE || dataType == Response_Type::RESPONSE_WRITE)
    {
        nameLength = data.at(0) >> 3;
        returnName = data.mid(2, nameLength);
        return true;
    }

    return false;
}

bool IOTV_SC::queryName(const QByteArray &data, QString &returnName)
{
    uint8_t nameLength = 0;
    Query_Type dataType = checkQueryData(data);

    if(dataType == Query_Type::QUERY_DEVICE_LIST || dataType == Query_Type::QUERY_ERROR)
        return false;
    else if(dataType == Query_Type::QUERY_READ || dataType == Query_Type::QUERY_STATE)
    {
        nameLength = data.at(0) >> 3;
        returnName = data.mid(2, nameLength);
        return true;
    }
    else if(dataType == Query_Type::QUERY_WRITE)
    {
        nameLength = data.at(0) >> 3;
        returnName = data.mid(4, nameLength);
        return true;
    }

    return false;
}

std::pair<bool, int> IOTV_SC::accumResponcePacket(const QByteArray &data)
{
    Response_Type dataType = checkResponsetData(data);
    uint32_t dataSize = static_cast<uint32_t>(data.size());

    if(dataType == Response_Type::RESPONSE_ERROR)
        return {false, 0};
    else if(dataSize >= 2 && dataType == Response_Type::RESPONSE_DEVICE_LIST)
    {
        uint8_t countDivece = data.at(1);
        if(countDivece == 0)
            return {true, 2};

        if(dataSize < 3)
            return {false, 0};

        QByteArray buffer = data.mid(2);
        uint64_t bufSize = buffer.size();
        uint32_t packetSize = 2;

        for(uint8_t i = 0; i < countDivece; i++)
        {
            uint8_t nameLength = buffer.at(0) >> 3;
            int prefixSize = 1 + nameLength; //длина имени устройства + само имя
            if(bufSize < static_cast<uint32_t>(4 + prefixSize))
                return {false, 0};

            quint16 msb = buffer[prefixSize + 1] << 8;
            quint16 lsb = buffer[prefixSize + 2];
            quint16 descriptionLength = msb | lsb;
            uint8_t readChannelCount = buffer[prefixSize + 3] >> 4;
            uint8_t writeChannelCount = buffer[prefixSize + 3] & 0x0F;
            uint64_t subPacketSize = prefixSize + 4 + descriptionLength + readChannelCount + writeChannelCount;

            if(bufSize < subPacketSize)
                return {false, 0};
            else
            {
                packetSize += subPacketSize;
                buffer = buffer.mid(subPacketSize);
                bufSize = buffer.size();
            }
        }
        return {true, packetSize};
    }
    else if(dataSize > 2 && dataType == Response_Type::RESPONSE_STATE)
    {
        uint8_t nameLength = data.at(0) >> 3;
        uint32_t packetSize = 2 + nameLength;

        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize > 4 && dataType == Response_Type::RESPONSE_READ)
    {
        uint8_t nameLength = data.at(0) >> 3;
        quint16 msb = data.at(2) << 8;
        quint16 lsb = data.at(3);
        quint16 dataLength = msb | lsb;
        uint32_t packetSize = 4 + nameLength + dataLength;

        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize > 2 && dataType == Response_Type::RESPONSE_WRITE)
    {
        uint8_t nameLength = data.at(0) >> 3;
        uint32_t packetSize = 2 + nameLength;

        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize > 256)
        return {false, 0};

    return {true, 0};
}

std::pair<bool, int> IOTV_SC::accumQueryPacket(const QByteArray &data)
{
    Query_Type dataType = checkQueryData(data);
    uint32_t dataSize = static_cast<uint32_t>(data.size());

    if(dataType == Query_Type::QUERY_ERROR)
        return {false, 0};
    else if(dataSize == 1 && dataType == Query_Type::QUERY_DEVICE_LIST)
        return {true, 1};
    else if(dataSize >= 3 && dataType == Query_Type::QUERY_STATE)
    {
        uint8_t nameLength = data.at(0) >> 3;
        uint32_t packetSize = 2 + nameLength;

        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize >= 3 && dataType == Query_Type::QUERY_READ)
    {
        uint8_t nameLength = data.at(0) >> 3;
        uint32_t packetSize = 2 + nameLength;

        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize >= 5 && dataType == Query_Type::QUERY_WRITE)
    {
        uint8_t nameLength = data.at(0) >> 3;
        quint16 msb = data.at(2) << 8;
        quint16 lsb = data.at(3);
        quint16 dataLength = msb | lsb;
        uint32_t packetSize = 4 + nameLength + dataLength;

        if(dataSize >= packetSize)
            return {true, packetSize};
    }
    else if(dataSize > 256)
        return {false, 0};

    return {false, 0};
}
