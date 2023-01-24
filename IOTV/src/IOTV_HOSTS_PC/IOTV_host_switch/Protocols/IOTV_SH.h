#ifndef IOTVP_SH
#define IOTVP_SH

#include "embedded/creatorpkgs.h"
#include "iot_server.h"

enum DATA_TYPE
{
    DATA_TYPE_INT_8 = 0,
    DATA_TYPE_INT_16,
    DATA_TYPE_INT_32,
    DATA_TYPE_INT_64,

    DATA_TYPE_FLOAT_32,
    DATA_TYPE_DOUBLE_64, // на МК double 32-битный может быть

    DATA_TYPE_BOOL,
    DATA_TYPE_STRING,

    DATA_TYPE_RAW,
    DATA_TYPE_NONE
};

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot);
uint64_t responsePingData(char* outData, uint64_t dataSize);
uint64_t responseReadData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot, const Header *head);
uint64_t responseWriteData(char* outData, uint64_t dataSize, IOTV_Server *iot, const Header *head);

#endif //IOTVP_SH
