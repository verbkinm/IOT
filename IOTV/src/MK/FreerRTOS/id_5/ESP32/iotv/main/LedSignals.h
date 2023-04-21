#ifndef MAIN_LEDSIGNALS_H_
#define MAIN_LEDSIGNALS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "Global_def.h"

void LedSignals_Task(void *pvParameters);

void LedSignals_setWifiState(bool state);
void LedSignals_setTCPState(bool state);

#endif /* MAIN_LEDSIGNALS_H_ */
