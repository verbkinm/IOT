#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Очереди для взаимосвязи TCP и IOTV
QueueHandle_t xQueueInData, xQueueOutData;//, xQueueLedSignals;

// Очередь для DF плеера
QueueHandle_t xQueueDF;

// Очередь для LEDC
QueueHandle_t xQueueLEDC;

uint64_t realBufSize = 0;
uint64_t expextedDataSize = 0;
