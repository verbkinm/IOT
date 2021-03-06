#ifndef RAW_H
#define RAW_H

typedef signed char int8_t;
typedef int int16_t;
typedef long int32_t;
//typedef char int64_t;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
//typedef char uint64_t;

class  Raw
{
public:
    enum class DATA_TYPE
    {
        INTEGER_8,
        INTEGER_16,
        INTEGER_32,
        //INTEGER_64,

        UNSIGNED_INTEGER_8,
        UNSIGNED_INTEGER_16,
        UNSIGNED_INTEGER_32,
        //UNSIGNED_INTEGER_64,

        FLOAT_32,
        DOUBLE_32,
        //DOUBLE_64,

        BOOL_8,
        CHAR_PTR,

        RAW
    };

    union RAW
    {
        int8_t i8;          // 1
        int16_t i16;        // 2
        int32_t i32;        // 3
        //int64_t i64;        // 4

        uint8_t ui8;        // 5
        uint16_t ui16;      // 6
        uint32_t ui32;      // 7
        //uint64_t ui64;      // 8

        float f;            // 9
        double d;           // 10 double32
                            // 11 double64
        bool b;             // 12
        char* str;    // 13

        char array[8];      // побайтный доступ к RAW
    };

    static DATA_TYPE toDataType(uint8_t type);
    static uint8_t toUInt8(DATA_TYPE dataType);

    static const uint8_t size = sizeof(RAW);
};

#endif // RAW_H
