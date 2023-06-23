#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdint.h>

QueueHandle_t xQueueInData, xQueueOutData, xQueueLedSignals;

uint64_t realBufSize = 0;
uint64_t expextedDataSize = 20;

uint16_t LED_maxDuty = 8191;
uint8_t Led_RGB_scriptNumber = 0;
