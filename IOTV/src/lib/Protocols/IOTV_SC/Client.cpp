#include "Client.h"


Client_RX::RESPONSE_PKG *Client_RX::accumResponsePacket(QByteArray &data)
{
    if(data.isEmpty())
        return new Client_RX::RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    uint8_t firstByte = data.at(0);

    if(firstByte == RESPONSE_DEV_LIST_BYTE)
        return createResponse_DEV_LIST_PKG(data);

    firstByte &= 0x07;
    switch (firstByte)
    {
    case RESPONSE_STATE_BYTE:
        return createResponse_STATE_PKG(data);
        break;
    case RESPONSE_READ_BYTE:
        return createResponse_READ_PKG(data);
        break;
    case RESPONSE_WRITE_BYTE:
        return createResponse_WRITE_PKG(data);
        break;
    }

    return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);
}

Client_RX::RESPONSE_PKG *Client_RX::createResponse_DEV_LIST_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    if (data.size() < 2)
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    uint8_t devCount = data.at(1);

    QByteArray copyData = data.mid(2);
    std::vector<DEV_PKG> devVec(devCount);

    for (uint8_t i = 0; i < devCount; i++)
    {
        DEV_PKG dev = createResponse_DEV_PKG(copyData);
        if (dev.error)
            return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

        devVec[i] = dev;
    }

    RESPONSE_DEV_LIST_PKG *pkg = new RESPONSE_DEV_LIST_PKG;
    pkg->devs = devVec;

    data = data.mid(data.size() - copyData.size() - 2);

    return pkg;
}

Client_RX::DEV_PKG Client_RX::createResponse_DEV_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return {};

    uint8_t nameLength = data.at(0) >> 3;

    if (data.size() < (5 + nameLength))
        return {};

    uint8_t id = data.at(2);
    uint16_t descriptionLength = (data.at(3) << 8) | data.at(4);
    uint8_t readChannelCount = data.at(5) >> 4;
    uint8_t writeChannelCount = data.at(5) & 0x0F;

    if ( data.size() < (5 + nameLength + descriptionLength + readChannelCount + writeChannelCount))
        return {};

    DEV_PKG pkg;
    pkg.id = id;
    pkg.error = false;
    pkg.name = data.mid(1, nameLength);
    pkg.description = data.mid(6, descriptionLength);

    for (uint8_t i = 0; i < readChannelCount; i++)
        pkg.readChannel.push_back(static_cast<Raw::DATA_TYPE>(data.at(5 + nameLength + descriptionLength + i)));

    for (uint8_t i = 0; i < writeChannelCount; i++)
        pkg.writeChannel.push_back(static_cast<Raw::DATA_TYPE>(data.at(5 + nameLength + descriptionLength + readChannelCount + i)));

    return pkg;
}

Client_RX::RESPONSE_PKG *Client_RX::createResponse_STATE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    uint8_t nameLength = data.at(0) >> 3;

    if (data.size() < (2 + nameLength))
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    RESPONSE_STATE_PKG *pkg = new RESPONSE_STATE_PKG;

    uint8_t stateBit = data.at(1) & 0x20;
    pkg->state = (stateBit == 1) ? true : false;
    pkg->name = data.mid(2, nameLength);

    data = data.mid(2 + nameLength);

    return pkg;
}

Client_RX::RESPONSE_PKG *Client_RX::createResponse_READ_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    if (data.size() < 4)
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    uint8_t nameLength = data.at(0) >> 3;
    uint8_t channelNumber = data.at(1) >> 4;
    uint16_t dataLength = (data.at(2) << 4) | data.at(3);

    if (data.size() < (4 + nameLength + dataLength))
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    RESPONSE_READ_PKG *pkg = new RESPONSE_READ_PKG;
    pkg->name = data.mid(4, nameLength);
    pkg->channelNumber = channelNumber;
    pkg->data = data.mid(4 + nameLength);

    data = data.mid(4 + nameLength + dataLength);

    return pkg;
}

Client_RX::RESPONSE_PKG *Client_RX::createResponse_WRITE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    if (data.size() < 2)
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    uint8_t nameLength = data.at(0) >> 3;
    uint8_t channelNumber = data.at(1) >> 4;

    if (data.size() < (2 + nameLength))
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    RESPONSE_WRITE_PKG *pkg = new RESPONSE_WRITE_PKG;
    pkg->name = data.mid(2, nameLength);
    pkg->channelNumber = channelNumber;

    data = data.mid(2 + nameLength);

    return pkg;
}

QByteArray Client_TX::query_Device_List()
{
    QByteArray data;
    data.push_back(QUERY_DEV_LIST_BYTE);
    return data;
}

QByteArray Client_TX::query_STATE(const QString &deviceName)
{
    QByteArray data;

    uint8_t length = deviceName.length() << 3;
    data.push_back(length | QUERY_STATE_FIRST_BYTE);
    data.push_back(QUERY_STATE_BYTE);
    // length >> 3 - пресекаем размер больше допустимого
    data.append(deviceName.mid(0, (length >> 3)).toLocal8Bit());

    return data;
}

QByteArray Client_TX::query_READ(const QString &deviceName, uint8_t channelNumber)
{
    QByteArray data;

    uint8_t length = deviceName.length() << 3;
    data.push_back(length | QUERY_READ_BYTE);
    data.push_back(0x0F & channelNumber);
    // length >> 3 - пресекаем размер больше допустимого
    data.append(deviceName.mid(0, (length >> 3)).toLocal8Bit());

    return data;
}

QByteArray Client_TX::query_WRITE(const QString &deviceName, uint8_t channelNumber, const QByteArray &rawData)
{
    QByteArray data;

    uint8_t length = deviceName.length() << 3;

    data.push_back(length | QUERY_WRITE_BYTE);
    data.push_back(0x0F & channelNumber);

    data.push_back(rawData.size() >> 8);
    data.push_back(rawData.size());
    // length >> 3 - пресекаем размер больше допустимого
    data.append(deviceName.mid(0, (length >> 3)).toLocal8Bit());
    data.append(rawData);

    return data;
}
