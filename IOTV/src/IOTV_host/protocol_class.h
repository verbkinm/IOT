#ifndef PROTOCOL_CLASS_H
#define PROTOCOL_CLASS_H

#include <stdint.h>
#include <vector>

#include "iot_server.h"

class Protocol_class
{
public:
    enum class query_type
    {
        QUERY_WAY,
        QUERY_READ,
        QUERY_WRITE,
        ERROR
    };

    static query_type checkQueryData(const std::vector<uint8_t> &data);

    static void response_WAY(const IOT_Server &iotHost, std::vector<uint8_t> &data);
    static void response_READ(const IOT_Server &iotHost, std::vector<uint8_t> &data);
    static void response_WRITE(IOT_Server &iotHost, std::vector<uint8_t> &data);
};

#endif // PROTOCOL_CLASS_H
