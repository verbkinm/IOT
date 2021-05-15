#include "iotv_sc.h"

void IOTV_SC::query_Device_List(QByteArray &data)
{
    data.clear();
    data.append(0x01);
}

bool IOTV_SC::query_Device_State(QByteArray &data, const QString &deviceName)
{
    data.clear();

    if(deviceName.length() > 31)
        return false;

    data.append(deviceName.length() << 3);
    data.append(0x10);
    data.append(deviceName);

    return true;
}

bool IOTV_SC::query_READ(QByteArray &data, const QString &deviceName, uint8_t channelNumber)
{
    data.clear();

    if(deviceName.length() > 31 || channelNumber > 0x0f)
        return false;

    data.append( (deviceName.length() << 3) | 0x02);
    data.append(channelNumber);
    data.append(deviceName);

    return true;
}

bool IOTV_SC::query_WRITE(QByteArray &data, const QString &deviceName, uint8_t channelNumber, Raw::RAW writeData)
{
    data.clear();

    if(deviceName.length() > 31 || channelNumber > 0x0f)
        return false;

    data.append(deviceName.length() << 3);
    data.append(channelNumber);

    data.append(char(0x00));
    data.append(0x08);

    data.append(deviceName);

    for (uint16_t i = 0; i < sizeof (Raw); i++)
        data.append(writeData.array[i]);

    return true;
}

QByteArrayList IOTV_SC::response_Device_List(const QByteArray &data)
{
    QByteArrayList result;
    uint8_t countDivece = data.at(1);
    QByteArray buff = data.mid(2);

    for(uint8_t i = 0; i < countDivece; i++)
    {
        uint8_t nameLength = buff.at(0) >> 3;
        uint16_t dataLength = buff.at(1 + nameLength + 1) << 8 | buff.at(1 + nameLength + 2);
        uint8_t readChannelCount = buff.at(1 + nameLength + 3) >> 4;
        uint8_t writeChannelCount = buff.at(1 + nameLength + 3) & 0x0f;
        uint packageLength = 5 + dataLength + readChannelCount + writeChannelCount;

        QByteArray package = buff.mid(0, packageLength);
        result.append(package);

        buff.remove(0, packageLength);
    }
    return result;
}

void IOTV_SC::response_Device_State(Base_Host &iotHost, const QByteArray &data)
{
    bool state = data.at(1) & DEVICE_STATE_RESPONSE_BIT;
    iotHost.setState(state);
}

void IOTV_SC::response_READ(Base_Host &iotHost, const QByteArray &data)
{
    uint8_t nameLength = data.at(0) >> 3;
    uint8_t channelNumber = data.at(1) & 0x0f;
    uint16_t dataLength = (data.at(2) << 8) | data.at(3);

    QByteArray buf = data.mid(4 + nameLength);
    Raw::RAW rawData;
    for (uint8_t i = 0; i < dataLength; i++)
        rawData.array[i] = buf.at(i);

    iotHost.setReadChannelData(channelNumber, rawData);
    iotHost.eraseExpectedResponseRead(channelNumber);
}

void IOTV_SC::response_WRITE(Base_Host &iotHost, const QByteArray &data)
{
    uint8_t channelNumber = data.at(1) & 0x0f;
    iotHost.eraseExpectedResponseWrite(channelNumber);
}

QByteArrayList IOTV_SC::splitQueryData(QByteArray &data)
{
    QByteArrayList result;

    while (data.length())
    {
        IOTV_SC::Query_Type dataType = checkQueryData(data);

        if(dataType == Query_Type::QUERY_DEVICE_LIST)
        {
            QByteArray byte;
            byte.append(data.at(0));
            result.append(byte);
            data.remove(0, 1);
        }
        else if(dataType == Query_Type::QUERY_STATE)
        {
            uint8_t nameLength = data.at(0) >> 3;
            result.append(data.mid(0, 2 + nameLength));
            data.remove(0, 2 + nameLength);
        }
        else if(dataType == Query_Type::QUERY_READ)
        {
            uint8_t nameLength = data.at(0) >> 3;
            result.append(data.mid(0, 2 + nameLength));
            data.remove(0, 2 + nameLength);
        }
        else if(dataType == Query_Type::QUERY_WRITE)
        {
            uint8_t nameLength = data.at(0) >> 3;
            uint16_t dataLength = data.at(2) << 8 | data.at(3);
            result.append(data.mid(0, 4 + nameLength + dataLength));
            data.remove(0, 4 + nameLength + dataLength);
        }
    }
    return result;
}

QByteArrayList IOTV_SC::splitResponseData(QByteArray &data) // !!!
{
    QByteArrayList result;

    while (data.length())
    {
        uint8_t nameLength = data.at(0) >> 3;
        IOTV_SC::Response_Type dataType = checkResponsetData(data);

        if(dataType == Response_Type::RESPONSE_DEVICE_LIST)
        {
            uint8_t countDivece = data.at(1);
            QByteArray buff = data.mid(2);
            uint packetSize = 2;

            for(uint8_t i = 0; i < countDivece; i++)
            {
                uint8_t nameLength = buff.at(0) >> 3;
                uint16_t dataLength = buff.at(1 + nameLength + 1) << 8 | buff.at(1 + nameLength + 2);
                uint8_t readChannelCount = buff.at(1 + nameLength + 3) >> 4;
                uint8_t writeChannelCount = buff.at(1 + nameLength + 3) & 0x0f;

                uint packageLength = 5 + dataLength + readChannelCount + writeChannelCount;
                buff.remove(0, packageLength);
                packetSize += packageLength;
            }
        }
        else if(dataType == Response_Type::RESPONSE_STATE)
        {
            result.append(data.mid(0, 2 + nameLength));
            data.remove(0, 2 + nameLength);
        }
        else if(dataType == Response_Type::RESPONSE_READ)
        {
            uint16_t dataLength = data.at(2) << 8 | data.at(3);
            result.append(data.mid(0, 4 + nameLength + dataLength));
            data.remove(0, 4 + nameLength + dataLength);
        }
        else if(dataType == Response_Type::RESPONSE_WRITE)
        {
            result.append(data.mid(0, 2 + nameLength));
            data.remove(0, 2 + nameLength);
        }
    }
    return result;
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
    {
        if(data.length() < 2)
            return Response_Type::RESPONSE_ERROR;

        return Response_Type::RESPONSE_DEVICE_LIST;
    }
    //DEVICE_STATE
    else if( !(firstByte & READ_WRITE_BIT) && (secondByte & DEVICE_STATE_QUERY_BIT))
    {
        if(data.length() >= 2 + nameLength)
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
    else if(dataType == Response_Type::RESPONSE_STATE || dataType == Response_Type::RESPONSE_STATE)
    {
        nameLength = data.at(0) >> 3;
        returnName = data.mid(2, nameLength);
        return true;
    }

    return false;
}
