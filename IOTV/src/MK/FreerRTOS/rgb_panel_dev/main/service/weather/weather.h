/*
 * weather.h
 *
 *  Created on: 18 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_WEATHER_WEATHER_H_
#define MAIN_SERVICE_WEATHER_WEATHER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"
#include "esp_log.h"

#include "../../Global_def.h"
#include "../../json/json_config.h"
#include "../../Local_Lib/local_lib.h"

void weather_service_task(void *pvParameters);

#endif /* MAIN_SERVICE_WEATHER_WEATHER_H_ */
