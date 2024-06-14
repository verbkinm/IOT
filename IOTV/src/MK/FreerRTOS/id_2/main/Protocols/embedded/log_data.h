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
    int32_t dataSize;

    uint8_t nameSize;
    uint8_t channelNumber;
    uint8_t flags;

    char *name;
    char *data;
};

uint64_t logDataCheckSum(const struct Log_Data *body);
uint64_t logDataSize(const struct Log_Data *body);
uint64_t logDataToData(const struct Log_Data *body, char *outData, uint64_t outDataSize);
void clearLogData(struct Log_Data *body);

#ifdef __cplusplus
}
#endif

#endif // LOG_DATA_H
