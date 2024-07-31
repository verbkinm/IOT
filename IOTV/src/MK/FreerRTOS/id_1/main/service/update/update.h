/*
 * factory_reset.h
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_UPDATE_UPDATE_H_
#define MAIN_SERVICE_UPDATE_UPDATE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"
#include "cJSON.h"
#include "esp_app_desc.h"
#include "ota/ota.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"

void update_service_task(void *pvParameters);
const char *update_service_task_name(void);
char *update_service_get_available_version(void);

#endif /* MAIN_SERVICE_UPDATE_UPDATE_H_ */
