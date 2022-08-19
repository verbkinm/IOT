#include "Server.h"
#include "IOTV_SC.h"

Server_RX::QUERY_PKG *Server_RX::accumPacket(QByteArray &data)
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

Server_RX::QUERY_PKG *Server_RX::createQuery_DEV_LIST_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new QUERY_PKG(Query_Type::QUERY_ERROR);

    QUERY_DEV_LIST_PKG *pkg = new QUERY_DEV_LIST_PKG;

    data = data.mid(1);

    return pkg;
}

Server_RX::QUERY_PKG *Server_RX::createQuery_READ_PKG(QByteArray &data)
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

Server_RX::QUERY_PKG *Server_RX::createQuery_STATE_PKG(QByteArray &data)
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

Server_RX::QUERY_PKG *Server_RX::createQuery_WRITE_PKG(QByteArray &data)
{
    if (data.isEmpty())
        return new QUERY_PKG(Query_Type::QUERY_ERROR);

    if (data.size() < 4)
        return new QUERY_PKG(Query_Type::QUERY_INCOMPLETE);

    uint8_t nameLength = static_cast<uint8_t>(data.at(0)) >> 3;

    uint8_t MSB = data.at(2) << 8;
    uint8_t LSB = data.at(3 + nameLength);
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
