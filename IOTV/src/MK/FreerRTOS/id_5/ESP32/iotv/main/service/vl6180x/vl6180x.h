/*
 * vl6180x.h
 *
 *  Created on: 9 мар. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_VL6180X_VL6180X_H_
#define MAIN_SERVICE_VL6180X_VL6180X_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdint.h>

#include "iotv.h"

void Vl6180X_Task(void *pvParameters);

#endif /* MAIN_SERVICE_VL6180X_VL6180X_H_ */
