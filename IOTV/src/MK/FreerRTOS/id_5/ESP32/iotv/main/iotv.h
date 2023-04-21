#ifndef MAIN_IOTV_H_
#define MAIN_IOTV_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include <stdint.h>
#include "nvs.h"

#include "Protocols/embedded/creatorpkgs.h"
#include "Protocols/embedded/IOTV_SH.h"
#include "Protocols/embedded/iotv_server_embedded.h"

#include "I2C/BME280.h"
#include "I2C/VL6180X_Simple.h"

void iotvTask(void *pvParameters);
void Vl6180X_Task(void *pvParameters);
void BME280_Task(void *pvParameters);

#endif /* MAIN_IOTV_H_ */
