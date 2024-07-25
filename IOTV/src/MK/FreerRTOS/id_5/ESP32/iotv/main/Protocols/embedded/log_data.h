#ifndef LOG_DATA_H
#define LOG_DATA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct Log_Data
{
    uint64_t startInterval;
    uint64_t endInterval;

    uint32_t interval;
    uint32_t dataSize;

    uint8_t nameSize;
    uint8_t channelNumber;
    uint8_t flags;

    char *name;
    char *data;
};
typedef struct Log_Data log_data_t;

uint64_t logDataCheckSum(const log_data_t *body);
uint64_t logDataSize(const log_data_t *body);
uint64_t logDataToData(const log_data_t *body, char *outData, uint64_t outDataSize);
void clearLogData(log_data_t *body);

log_data_t *logDataCopy(const log_data_t *logData);

#ifdef __cplusplus
}
#endif

#endif // LOG_DATA_H
