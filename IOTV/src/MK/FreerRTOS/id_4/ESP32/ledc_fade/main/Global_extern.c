#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdint.h>

//QueueHandle_t xQueueInData, xQueueOutData;

//uint64_t realBufSize = 0;
//uint64_t expextedDataSize = 20;

uint16_t LED_maxDuty = 8192;
uint16_t LED_minDuty = 100;
uint8_t Led_RGB_scriptNumber = 1;

bool demo = true;
