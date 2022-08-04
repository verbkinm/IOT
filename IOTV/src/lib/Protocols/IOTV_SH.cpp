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

QByteArray IOTV_SH::query_WRITE(uint8_t channelNumber, const Raw &rawData)
{
    QByteArray data;
    data.append((channelNumber << 4) | QUERY_WRITE_BYTE);

    uint16_t size = rawData.size();

    data.push_back(size >> 8);
    data.push_back(size);

    data.append(vecUInt8ToQByteArray(rawData.data()));
//    auto &dataFromRaw = rawData.data();
//    for (uint16_t i = 0; i < size; i++)
//        data.push_back(dataFromRaw[i]);

    return data;
}

QByteArray IOTV_SH::query_PING()
{
    QByteArray data;
    data.push_back(QUERY_PING_BYTE);

    return data;
}

IOTV_SH::RESPONSE_PKG IOTV_SH::createResponse_WAY_PKG(QByteArray &data)
{
    uint16_t descriptionLength = 0;
    uint8_t channelReadLength = 0;
    uint8_t channelWriteLength = 0;
    uint16_t expectedLength = 0;

    if (data.size() >= 5)
    {
        descriptionLength = data.at(2);
        descriptionLength <<= 8;
        descriptionLength |= data.at(3);

        channelReadLength = data.at(4) >> 4;
        channelWriteLength = data.at(4) & 0x0F;

        expectedLength = 5 + descriptionLength + channelReadLength + channelWriteLength;

        if (data.size() < expectedLength)
            return {};
    }
    else
        return {};

    RESPONSE_WAY pkg_data;

    pkg_data.id = data.at(1);
    pkg_data.description = data.mid(5, descriptionLength).data();

    int index = 5 + descriptionLength;
    for (uint8_t i = 0; i < channelReadLength; i++)
        pkg_data.readChannel.push_back(static_cast<Raw::DATA_TYPE>(data.at(index++)));
    pkg_data.readChannel.shrink_to_fit();

    for (uint8_t i = 0; i < channelWriteLength; i++)
        pkg_data.writeChannel.push_back(static_cast<Raw::DATA_TYPE>(data.at(index++)));
    pkg_data.writeChannel.shrink_to_fit();

    data = data.mid(expectedLength);

    return pkg_data;
}

IOTV_SH::RESPONSE_PKG IOTV_SH::createResponse_READ_PKG(QByteArray &data)
{
    if (data.length() < 3)
        return {};

    uint16_t dataLength = (data.at(1) << 8) | static_cast<uint8_t>(data.at(2)); //  cast - чтобы не было отрицательного значения dataLength

    if (data.length() < (3 + dataLength))
        return {};

    RESPONSE_READ pkg_data;

    pkg_data.chanelNumber = data.at(0) >> 4;;
    pkg_data.data = data.mid(3, dataLength);

    data = data.mid(3 + dataLength);

    return pkg_data;
}

IOTV_SH::RESPONSE_PKG IOTV_SH::createResponse_WRITE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return {};

    RESPONSE_WRITE pkg_data;
    pkg_data.chanelNumber = data.at(0) >> 4;

    data = data.mid(1);

    return pkg_data;
}

IOTV_SH::RESPONSE_PKG IOTV_SH::createResponse_PONG_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return {};

    RESPONSE_PONG pkg_data;
    pkg_data.state = true;

    data = data.mid(1);

    return pkg_data;
}

uint8_t IOTV_SH::channelNumber(uint8_t byte)
{
    return (byte >> 4);
}

IOTV_SH::RESPONSE_PKG IOTV_SH::accumPacket(QByteArray &data)
{
    if(data.isEmpty())
        return {};

    uint8_t firstByte = data.at(0);

    if(firstByte == RESPONSE_WAY_BYTE)
        return createResponse_WAY_PKG(data);
    else if(firstByte == RESPONSE_PONG_BYTE)
        return createResponse_PONG_PKG(data);

    firstByte &= 0x0F;
    switch (firstByte)
    {
    case RESPONSE_READ_BYTE:
        return createResponse_READ_PKG(data);
        break;
    case RESPONSE_WRITE_BYTE:
        return createResponse_WRITE_PKG(data);
        break;
    default:
        return {};
    }
}

QByteArray IOTV_SH::vecUInt8ToQByteArray(const std::vector<uint8_t> &vec)
{
    QByteArray arr;

    for (auto el : vec)
        arr.push_back(el);

    return arr;
}

std::vector<uint8_t> IOTV_SH::QByteArrayToVecUInt8(const QByteArray &arr)
{
    std::vector<uint8_t> vec;

    for (auto el : arr)
        vec.push_back(el);

    return vec;
}
