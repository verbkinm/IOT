#ifndef IOTVP_SH
#define IOTVP_SH

#include "embedded/creatorpkgs.h"
#include "iot_server.h"

uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot);
uint64_t responsePingData(char* outData, uint64_t dataSize);
uint64_t responseReadData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot, Header *head);
uint64_t responseWriteData(char* outData, uint64_t dataSize, IOTV_Server *iot, const Header *head);

#endif //IOTVP_SH
