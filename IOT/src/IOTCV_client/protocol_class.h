#ifndef PROTOCOL_CLASS_H
#define PROTOCOL_CLASS_H

#include <stdint.h>

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

    static query_type checkQueryData(const char *data);
    static uint8_t getChannelNumber(char data);
};

#endif // PROTOCOL_CLASS_H
