/*
 * factory_reset.h
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_FACTORY_RESET_FACTORY_RESET_H_
#define MAIN_SERVICE_FACTORY_RESET_FACTORY_RESET_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

void factory_reset_service_task(void *pvParameters);

#endif /* MAIN_SERVICE_FACTORY_RESET_FACTORY_RESET_H_ */
