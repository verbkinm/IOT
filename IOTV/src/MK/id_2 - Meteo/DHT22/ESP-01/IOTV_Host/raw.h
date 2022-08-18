#ifndef RAW_H
#define RAW_H

typedef signed char iot_int8_t;
typedef int iot_int16_t;
typedef long iot_int32_t;
//typedef char int64_t;

typedef unsigned char iot_uint8_t;
typedef unsigned int iot_uint16_t;
typedef unsigned long iot_uint32_t;
//typedef char uint64_t;

class  Raw
{
public:
    enum class DATA_TYPE
    {
        INTEGER_8 = 0x00,
        INTEGER_16 = 0x01,
        INTEGER_32 = 0x02,
        INTEGER_64 = 0x03,

        UNSIGNED_INTEGER_8 = 0x04,
        UNSIGNED_INTEGER_16 = 0x05,
        UNSIGNED_INTEGER_32 = 0x06,
        UNSIGNED_INTEGER_64 = 0x07,

        FLOAT_32 = 0x08,
        DOUBLE_32 = 0x09,
        DOUBLE_64 = 0x0A,

        BOOL_8 = 0x0B,
        STRING = 0x0C,

        RAW = 0x0D,
        NONE = 0x0E
    };

    union RAW
    {
        iot_int8_t i8;          // 1
        iot_int16_t i16;        // 2
        iot_int32_t i32;        // 3
        //int64_t i64;        // 4

        iot_uint8_t ui8;        // 5
        iot_uint16_t ui16;      // 6
        iot_uint32_t ui32;      // 7
        //uint64_t ui64;      // 8

        float f;            // 9
        double d;           // 10 double32
                            // 11 double64
        bool b;             // 12
        char* str;    // 13

        char array[8];      // побайтный доступ к RAW
    };

    static DATA_TYPE toDataType(iot_uint8_t type);
    static iot_uint8_t toUInt8(DATA_TYPE dataType);

    static const iot_uint8_t size = sizeof(RAW);
};

#endif // RAW_H
