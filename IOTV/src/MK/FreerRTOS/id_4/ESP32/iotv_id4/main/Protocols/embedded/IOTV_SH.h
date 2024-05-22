#ifndef IOTVP_SH
#define IOTVP_SH

#include "header.h"
#include "iotv_server_embedded.h"
#include "log_data.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Ответы на запросы. Ответы сразу в сырые данные.
uint64_t responseIdentificationData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, uint8_t ident_flags);
uint64_t responsePingData(char *outData, uint64_t dataSize);
uint64_t responseReadData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot, const struct Header *head, uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, uint8_t rw_flags, uint8_t header_flags);
uint64_t responseWriteData(char *outData, uint64_t dataSize, struct IOTV_Server_embedded *iot, const struct Header *head, uint8_t rw_flags, uint8_t header_flags);
uint64_t responseStateData(char *outData, uint64_t dataSize, const struct IOTV_Server_embedded *iot);
uint64_t responseTech(char *outData, uint64_t outDataSize, const char *inData, uint64_t inDataSize, const struct Header *head);
uint64_t responseLogData(const char *rawData, uint64_t rawDataSize, char *outData, uint64_t outDataSize, const struct Log_Data *pkg, uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj);

// Формируем запросы сразу в сыром виде в outData
uint64_t queryIdentificationData(char *outData, uint64_t dataSize);
uint64_t queryPingData(char *outData, uint64_t dataSize);
uint64_t queryWriteData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, const char *dataToWrite, uint32_t dataWriteSize);
uint64_t queryReadData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, uint8_t flags);
uint64_t queryStateData(char *outData, uint64_t outDataSize, const char *name);
uint64_t queryTech(char *outData, uint64_t dataSize, const char *inData, uint64_t inDataSize, uint8_t type);
uint64_t queryLogData(char *outData, uint64_t outDataSize, const char *name, uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, uint8_t flags);

struct IOTV_Server_embedded *createIotFromHeaderIdentification(const struct Header *header);

#ifdef __cplusplus
}
#endif

#endif //IOTVP_SH
