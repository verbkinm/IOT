#include "IOTV_SC.h"

/*
 *  Клиент
 */

IOTV_SC::RESPONSE_PKG *IOTV_SC::Client_RX::accumPacket(QByteArray &data)
{
    if(data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    uint8_t firstByte = static_cast<uint8_t>(data.at(0));

    if(firstByte == IOTV_SC::RESPONSE_DEV_LIST_BYTE)
        return createResponse_DEV_LIST_PKG(data);

    firstByte &= 0x07; // пакет определяется по первым трём битам
    switch (firstByte)
    {
    case IOTV_SC::RESPONSE_READ_BYTE:
        return createResponse_READ_PKG(data);
        break;
    case IOTV_SC::RESPONSE_STATE_FIRST_BYTE: // он же RESPONSE_WRITE_BYTE
    {
        if (data.size() < 2)
            return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

        //у пакетов state и write одинаковые младшие 3 бита, отличие во втором байте в 4 разряде
        if (data.at(1) & IOTV_SC::STATE_BIT_MASK)
            return createResponse_STATE_PKG(data);

        return createResponse_WRITE_PKG(data);
    }
        break;
    }

    return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);
}

IOTV_SC::RESPONSE_PKG *IOTV_SC::Client_RX::createResponse_DEV_LIST_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    if (data.size() < 2)
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    uint8_t devCount = static_cast<uint8_t>(data.at(1));

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

    data = data.mid(data.size() - copyData.size());

    return pkg;
}

IOTV_SC::DEV_PKG IOTV_SC::Client_RX::createResponse_DEV_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return {};

    uint8_t nameLength = static_cast<uint8_t>(data.at(0)) >> 3; //  cast - чтобы не было отрицательного значения

    if (data.size() < (5 + nameLength))
        return {};

    uint8_t id = static_cast<uint8_t>(data.at(1 + nameLength)); //  cast - чтобы не было отрицательного значения

    //если в descriptionLength (data.at(2 + nameLength) << 8) | data.at(3); получается лажа =(, не стал разбираться
    uint8_t MSB = data.at(2 + nameLength) << 8;
    uint8_t LSB = data.at(3 + nameLength);
    uint16_t descriptionLength = (MSB << 8) | LSB;
    uint8_t readChannelCount = static_cast<uint8_t>(data.at(4 + nameLength)) >> 4;
    uint8_t writeChannelCount = static_cast<uint8_t>(data.at(4 + nameLength)) & 0x0F;

    if ( data.size() < (5 + nameLength + descriptionLength + readChannelCount + writeChannelCount))
        return {};

    DEV_PKG pkg;
    pkg.id = id;
    pkg.error = false;
    pkg.name = data.mid(1, nameLength);
    pkg.description = data.mid(5 + nameLength, descriptionLength);

    for (uint8_t i = 0; i < readChannelCount; i++)
        pkg.readChannel.push_back(static_cast<Raw::DATA_TYPE>(data.at(5 + nameLength + descriptionLength + i)));

    for (uint8_t i = 0; i < writeChannelCount; i++)
        pkg.writeChannel.push_back(static_cast<Raw::DATA_TYPE>(data.at(5 + nameLength + descriptionLength + readChannelCount + i)));

    data = data.mid(nameLength + descriptionLength + readChannelCount + writeChannelCount + 5);

    return pkg;
}

IOTV_SC::RESPONSE_PKG *IOTV_SC::Client_RX::createResponse_STATE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    uint8_t nameLength = static_cast<uint8_t>(data.at(0)) >> 3; //  cast - чтобы не было отрицательного значения dataLength

    if (data.size() < (2 + nameLength))
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    RESPONSE_STATE_PKG *pkg = new RESPONSE_STATE_PKG;

    pkg->state = static_cast<uint8_t>(data.at(1)) & IOTV_SC::STATE_BIT_MASK;
    pkg->name = data.mid(2, nameLength);

    data = data.mid(2 + nameLength);

    return pkg;
}

IOTV_SC::RESPONSE_PKG *IOTV_SC::Client_RX::createResponse_READ_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    if (data.size() < 4)
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    uint8_t nameLength = static_cast<uint8_t>(data.at(0)) >> 3; //  cast - чтобы не было отрицательного значения dataLength
    uint8_t channelNumber = static_cast<uint8_t>(data.at(1)) & 0x0F;
    uint16_t dataLength = (static_cast<uint8_t>(data.at(2)) << 4) | static_cast<uint8_t>(data.at(3));

    if (data.size() < (4 + nameLength + dataLength))
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    RESPONSE_READ_PKG *pkg = new RESPONSE_READ_PKG;
    pkg->name = data.mid(4, nameLength);
    pkg->channelNumber = channelNumber;
    pkg->data = data.mid(4 + nameLength);

    data = data.mid(4 + nameLength + dataLength);

    return pkg;
}

IOTV_SC::RESPONSE_PKG *IOTV_SC::Client_RX::createResponse_WRITE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new RESPONSE_PKG(Response_Type::RESPONSE_ERROR);

    if (data.size() < 2)
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    uint8_t nameLength = static_cast<uint8_t>(data.at(0)) >> 3; //  cast - чтобы не было отрицательного значения dataLength
    uint8_t channelNumber = static_cast<uint8_t>(data.at(1)) & 0x0F;

    if (data.size() < (2 + nameLength))
        return new RESPONSE_PKG(Response_Type::RESPONSE_INCOMPLETE);

    RESPONSE_WRITE_PKG *pkg = new RESPONSE_WRITE_PKG;
    pkg->name = data.mid(2, nameLength);
    pkg->channelNumber = channelNumber;

    data = data.mid(2 + nameLength);

    return pkg;
}

QByteArray IOTV_SC::Client_TX::query_Device_List()
{
    QByteArray data;
    data.push_back(IOTV_SC::QUERY_DEV_LIST_BYTE);
    return data;
}

QByteArray IOTV_SC::Client_TX::query_STATE(const QString &deviceName)
{
    QByteArray data;

    uint8_t length = static_cast<uint8_t>(deviceName.length()) << 3;
    data.push_back(length | IOTV_SC::QUERY_STATE_FIRST_BYTE);
    data.push_back(IOTV_SC::QUERY_STATE_SECOND_BYTE);
    // length >> 3 - пресекаем размер больше допустимого
    data.append(deviceName.mid(0, (length >> 3)).toLocal8Bit());

    return data;
}

QByteArray IOTV_SC::Client_TX::query_READ(const QString &deviceName, uint8_t channelNumber)
{
    QByteArray data;

    uint8_t length = static_cast<uint8_t>(deviceName.length()) << 3;
    data.push_back(length | IOTV_SC::QUERY_READ_BYTE);
    data.push_back(0x0F & channelNumber);
    // length >> 3 - пресекаем размер больше допустимого
    data.append(deviceName.mid(0, (length >> 3)).toLocal8Bit());

    return data;
}

QByteArray IOTV_SC::Client_TX::query_WRITE(const QString &deviceName, uint8_t channelNumber, const QByteArray &rawData)
{
    QByteArray data;

    uint8_t length = static_cast<uint8_t>(deviceName.length()) << 3;

    data.push_back(length | IOTV_SC::QUERY_WRITE_BYTE);
    data.push_back(0x0F & channelNumber);

    data.push_back(static_cast<uint8_t>(rawData.size()) >> 8);
    data.push_back(static_cast<uint8_t>(rawData.size()));
    // length >> 3 - пресекаем размер больше допустимого
    data.append(deviceName.mid(0, (length >> 3)).toLocal8Bit());
    data.append(rawData);

    return data;
}

/*
 *  Сервер
 */

IOTV_SC::Server_RX::QUERY_PKG *IOTV_SC::Server_RX::accumPacket(QByteArray &data)
{
    if(data.isEmpty())
        return new QUERY_PKG(Query_Type::QUERY_ERROR);

    uint8_t firstByte = data.at(0);

    if(firstByte == IOTV_SC::QUERY_DEV_LIST_BYTE)
        return createQuery_DEV_LIST_PKG(data);

    firstByte &= 0x07; // пакет определяется по первым трём битам
    switch (firstByte)
    {
    case IOTV_SC::QUERY_READ_BYTE:
        return createQuery_READ_PKG(data);
        break;
    case IOTV_SC::QUERY_WRITE_BYTE: // он же QUERY_STATE_FIRST_BYTE
    {
        if (data.size() < 2)
            return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

        //у пакетов state и write одинаковые младшие 3 бита, отличие во втором байте в 4 разряде
        if (data.at(1) & IOTV_SC::QUERY_STATE_SECOND_BYTE)
            return createQuery_STATE_PKG(data);

        return createQuery_WRITE_PKG(data);
    }
        break;
    }

    return new QUERY_PKG(Query_Type::QUERY_ERROR);
}

IOTV_SC::Server_RX::QUERY_PKG *IOTV_SC::Server_RX::createQuery_DEV_LIST_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new QUERY_PKG(Query_Type::QUERY_ERROR);

    QUERY_DEV_LIST_PKG *pkg = new QUERY_DEV_LIST_PKG;

    data = data.mid(1);

    return pkg;
}

IOTV_SC::Server_RX::QUERY_PKG *IOTV_SC::Server_RX::createQuery_READ_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new QUERY_PKG(Query_Type::QUERY_ERROR);

    if (data.size() < 2)
        return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

    uint8_t nameLength = static_cast<uint8_t>(data.at(0)) >> 3;

    if (data.size() < (2 + nameLength))
        return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

    QUERY_READ_PKG *pkg = new QUERY_READ_PKG;
    pkg->channelNumber = data.at(1) & 0x0F;
    pkg->name = data.mid(2, nameLength);

    data = data.mid(2 + nameLength);

    return pkg;
}

IOTV_SC::Server_RX::QUERY_PKG *IOTV_SC::Server_RX::createQuery_STATE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new QUERY_PKG(Query_Type::QUERY_ERROR);

    if (data.size() < 2)
        return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

    uint8_t nameLength = data.at(0) >> 3;

    if (data.size() < (2 + nameLength))
        return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

    QUERY_STATE_PKG *pkg = new QUERY_STATE_PKG;
    pkg->name = data.mid(2, nameLength);
    pkg->state = static_cast<uint8_t>(data.at(1)) & IOTV_SC::STATE_BIT_MASK;

    data = data.mid(2 + nameLength);

    return pkg;
}

IOTV_SC::Server_RX::QUERY_PKG *IOTV_SC::Server_RX::createQuery_WRITE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new QUERY_PKG(Query_Type::QUERY_ERROR);

    if (data.size() < 4)
        return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

    uint8_t nameLength = static_cast<uint8_t>(data.at(0)) >> 3;

    uint8_t MSB = data.at(2) << 8;
    uint8_t LSB = data.at(3);
    uint16_t dataLength = (MSB << 8) | LSB;

    if (data.size() < (4 + nameLength + dataLength))
        return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

    QUERY_WRITE_PKG *pkg = new QUERY_WRITE_PKG;
    pkg->name = data.mid(4, nameLength);
    pkg->channelNumber = static_cast<uint8_t>(data.at(1)) & 0x0F;
    pkg->data = data.mid(4 + nameLength, dataLength);

    data = data.mid(4 + nameLength + dataLength);

    return pkg;
}

QByteArray IOTV_SC::Server_TX::response_DEV_LIST(const RESPONSE_DEV_LIST_PKG &pkg)
{
    QByteArray data;

    data.push_back(IOTV_SC::RESPONSE_DEV_LIST_BYTE);
    data.push_back(pkg.devs.size());

    for (uint8_t i = 0; i < pkg.devs.size(); i++)
    {
        uint8_t nameLength = pkg.devs.at(i).name.size();

        uint8_t MSB = static_cast<uint8_t>(pkg.devs.at(i).description.size()) >> 8;
        uint8_t LSB = static_cast<uint8_t>(pkg.devs.at(i).description.size());
        uint16_t descriptionLength = (MSB << 8) | LSB;

        uint8_t readChannelSize = 0x0F & pkg.devs.at(i).readChannel.size();
        uint8_t writeChannelSize = 0x0F & pkg.devs.at(i).writeChannel.size();

        data.push_back(nameLength << 3);
        data.append(pkg.devs.at(i).name.mid(0, nameLength).toLocal8Bit());
        data.push_back(pkg.devs.at(i).id);
        data.push_back(MSB);
        data.push_back(LSB);
        data.push_back((readChannelSize << 4) | writeChannelSize);
        data.append(pkg.devs.at(i).description.mid(0, descriptionLength).toLocal8Bit());

        for (uint8_t readChannels = 0; readChannels < readChannelSize; readChannels++)
            data.push_back(static_cast<uint8_t>(pkg.devs.at(i).readChannel.at(readChannels)));

        for (uint8_t writeChannels = 0; writeChannels < writeChannelSize; writeChannels++)
            data.push_back(static_cast<uint8_t>(pkg.devs.at(i).writeChannel.at(writeChannels)));
    }

    return data;
}

QByteArray IOTV_SC::Server_TX::response_STATE(const RESPONSE_STATE_PKG &pkg)
{
    QByteArray data;

    uint8_t length = static_cast<uint8_t>(pkg.name.length()) << 3;

    data.push_back(length | IOTV_SC::RESPONSE_STATE_FIRST_BYTE);
    data.push_back((static_cast<uint8_t>(pkg.state) << 5) | IOTV_SC::RESPONSE_STATE_SECOND_BYTE);
    // length >> 3 - пресекаем размер больше допустимого
    data.append(pkg.name.mid(0, (length >> 3)).toLocal8Bit());

    return data;
}

QByteArray IOTV_SC::Server_TX::response_READ(const RESPONSE_READ_PKG &pkg)
{
    QByteArray data;

    // пресекаем размер больше допустимого
    uint8_t length = static_cast<uint8_t>(pkg.name.length()) & 0x1F;

    data.push_back((length << 3) | IOTV_SC::RESPONSE_READ_BYTE);
    data.push_back(0x0F & pkg.channelNumber);
    data.push_back(static_cast<uint8_t>(pkg.data.size()) << 8);
    data.push_back(static_cast<uint8_t>(pkg.data.size()));
    data.append(pkg.name.mid(0, length).toLocal8Bit());
    data.append(pkg.data);

    return data;
}

QByteArray IOTV_SC::Server_TX::response_WRITE(const RESPONSE_WRITE_PKG &pkg)
{
    QByteArray data;
    // пресекаем размер больше допустимого
    uint8_t length = static_cast<uint8_t>(pkg.name.length()) & 0x1F;

    data.push_back((length << 3) | IOTV_SC::RESPONSE_WRITE_BYTE);
    data.push_back(0x0F & pkg.channelNumber);
    data.append(pkg.name.mid(0, length).toLocal8Bit());

    return data;
}
