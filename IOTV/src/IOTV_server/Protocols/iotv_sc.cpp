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

void IOTV_SC::response_Device_List(/*IOT_Server *iotServer, */const QByteArray &data)
{

}

void IOTV_SC::response_Device_State(const QByteArray &data)
{

}

void IOTV_SC::response_READ(const QByteArray &data)
{

}

void IOTV_SC::response_WRITE(const QByteArray &data)
{

}

QByteArrayList IOTV_SC::splitQueryData(QByteArray &data)
{

}

QByteArrayList IOTV_SC::splitResponseData(QByteArray &data)
{
    QByteArrayList result;

    while (data.length())
    {
        QByteArray byteArray;
        uint8_t nameLength = data.at(0) >> 3;
        IOTV_SC::Response_Type dataType = checkResponsetData(data);

        if(dataType == Response_Type::RESPONSE_DEVICE_LIST)
        {

        }
        else if(dataType == Response_Type::RESPONSE_STATE)
        {
            byteArray.append(data.mid(0, 2 + nameLength));
            result.append(byteArray);
            data.remove(0, 2 + nameLength);
        }
        else if(dataType == Response_Type::RESPONSE_READ)
        {

        }
        else if(dataType == Response_Type::RESPONSE_READ)
        {

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
