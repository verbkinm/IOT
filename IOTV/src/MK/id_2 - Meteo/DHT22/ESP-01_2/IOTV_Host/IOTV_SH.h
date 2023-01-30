#ifndef IOTVP_SH
#define IOTVP_SH

#include <assert.h>

#include "creatorpkgs.h"
#include "iotv_server_embedded.h"

// Ответы на запросы. Ответы сразу в сырые данные.
uint64_t responseIdentificationData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot);
uint64_t responsePingData(char* outData, uint64_t dataSize);
uint64_t responseReadData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, const struct Header *head);
uint64_t responseWriteData(char* outData, uint64_t dataSize, struct IOTV_Server_embedded *iot, const struct Header *head);
uint64_t responseStateData(char* outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot);

// Формируем запросы сразу в сыром виде в outData
uint64_t queryIdentificationData(char* outData, uint64_t dataSize);
uint64_t queryPingData(char* outData, uint64_t dataSize);
uint64_t queryWriteData(char* outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, const char *dataToWrite, uint32_t dataWriteSize);
uint64_t queryReadData(char* outData, uint64_t outDataSize, const char *name, uint8_t channelNumber);
uint64_t queryStateData(char* outData, uint64_t outDataSize, const char *name);

struct IOTV_Server_embedded *createIotFromHeaderIdentification(const struct Header *header);

#endif //IOTVP_SH
