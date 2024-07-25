#ifndef IOTVP_SH
#define IOTVP_SH

#include "header.h"
#include "iotv_server_embedded.h"
#include "iotv_types.h"
#include "log_data.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Ответы на запросы. Ответы сразу в сырые данные.
uint64_t responseIdentificationData(char *outData, uint64_t outDataSize, const iotv_obj_t *iot,
                                    uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, identification_flag_t ident_flags, header_flag_t header_flags);

uint64_t responsePingData(char *outData, uint64_t outDataSize,
                          uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, header_flag_t header_flags);

uint64_t responseReadData(char *outData, uint64_t outDataSize, const iotv_obj_t *iot, const header_t *head,
                          uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, readwrite_flag_t rw_flags, header_flag_t header_flags);

uint64_t responseWriteData(char *outData, uint64_t outDataSize, iotv_obj_t *iot, const header_t *head,
                           uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, readwrite_flag_t rw_flags, header_flag_t header_flags);

uint64_t responseStateData(char *outData, uint64_t outDataSize, const iotv_obj_t *iot,
                           uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, state_flag_t rw_flags, header_flag_t header_flags);

uint64_t responseTech(char *outData, uint64_t outDataSize, const char *rawData, uint64_t rawDataSize, const header_t *head,
                      uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, tech_flag_t flags, header_flag_t header_flags);

uint64_t responseLogData(const char *rawData, uint64_t rawDataSize, char *outData, uint64_t outDataSize, const struct Log_Data *pkg,
                         uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj);

// Формируем запросы сразу в сыром виде в outData
uint64_t queryIdentificationData(char *outData, uint64_t outDataSize,
                                uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, header_flag_t header_flags);

uint64_t queryPingData(char *outData, uint64_t outDataSize,
                       uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, header_flag_t header_flags);

uint64_t queryWriteData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber, const char *dataToWrite, uint32_t dataWriteSize,
                        uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, readwrite_flag_t readwrite_flag, header_flag_t header_flags);

uint64_t queryReadData(char *outData, uint64_t outDataSize, const char *name, uint8_t channelNumber,
                       uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, readwrite_flag_t readwrite_flag, header_flag_t header_flags);

uint64_t queryStateData(char *outData, uint64_t outDataSize, const char *name,
                        uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, state_flag_t state_flag, header_flag_t header_flags);

uint64_t queryTech(char *outData, uint64_t outDataSize, const char *inData, uint64_t inDataSize, tech_type_t type,
                   uint64_t (*writeFunc)(char *, uint64_t, void *), void *obj, tech_flag_t tech_flag, header_flag_t header_flags);

uint64_t queryLogData(char *outData, uint64_t outDataSize, const char *name,
                      uint64_t startInterval, uint64_t endInterval, uint32_t interval, uint8_t channelNumber, log_data_flag_t flags);

iotv_obj_t *createIotFromHeaderIdentification(const header_t *header);

#ifdef __cplusplus
}
#endif

#endif //IOTVP_SH
