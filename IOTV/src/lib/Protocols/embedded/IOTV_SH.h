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

// Формируем запросы сразу в сыром виде в outData
uint64_t queryIdentificationData(char* outData, uint64_t dataSize);
uint64_t queryPingData(char* outData, uint64_t dataSize);
uint64_t queryWriteData(char* outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, const char *dataToWrite, uint32_t dataWriteSize);
uint64_t queryReadData(char* outData, uint64_t outDataSize, const char *name, uint8_t channelNumber);
uint64_t queryStateData(char* outData, uint64_t outDataSize, const char *name);

#endif //IOTVP_SH
