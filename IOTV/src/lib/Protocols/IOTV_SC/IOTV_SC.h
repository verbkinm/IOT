#pragma once

#include "IOTV_SC/Client.h"
#include "IOTV_SC/Server.h"

namespace IOTV_SC
{
    enum : uint8_t
    {
        QUERY_DEV_LIST_BYTE = 0x01,
        QUERY_STATE_FIRST_BYTE = 0x00,
        QUERY_STATE_SECOND_BYTE = 0x10,
        QUERY_READ_BYTE = 0x02,
        QUERY_WRITE_BYTE = 0x00
    };

    enum : uint8_t
    {
        RESPONSE_DEV_LIST_BYTE = 0x05,
        RESPONSE_STATE_FIRST_BYTE = 0x04,
        RESPONSE_STATE_SECOND_BYTE = 0x10,
        RESPONSE_READ_BYTE = 0x06,
        RESPONSE_WRITE_BYTE = 0x04,

        STATE_BIT_MASK = 0x20
    };

    class Client_RX;
    class Client_TX;
    class Server_RX;
}
