#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdint.h>
#include <stdbool.h>

QueueHandle_t xQueueInData, xQueueOutData, xQueueLedSignals;

uint64_t realBufSize = 0;
uint64_t expextedDataSize = 20;
