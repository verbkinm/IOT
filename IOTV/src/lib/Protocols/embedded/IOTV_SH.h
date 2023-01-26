#ifndef IOTVP_SH
#define IOTVP_SH

#include "creatorpkgs.h"
#include "iotv_server.h"

// Ответы на запросы. Ответы сразу в сырые данные.
uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot);
uint64_t responsePingData(char* outData, uint64_t dataSize);
uint64_t responseReadData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot, const struct Header *head);
uint64_t responseWriteData(char* outData, uint64_t dataSize, struct IOTV_Server *iot, const struct Header *head);
uint64_t responseStateData(char* outData, uint64_t dataSize, const struct IOTV_Server *iot, const struct Header *head);

#endif //IOTVP_SH
