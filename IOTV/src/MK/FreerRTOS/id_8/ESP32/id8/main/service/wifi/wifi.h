/*
 * wifi.h
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_WIFI_WIFI_H_
#define MAIN_SERVICE_WIFI_WIFI_H_

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_netif_types.h"
#include "nvs_flash.h"
#include "netif/etharp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"

#include "Global_def.h"
#include "json/json_config.h"
#include "Local_Lib/local_lib.h"

void wifi_service_task(void *pvParameters);
const char *wifi_service_task_name(void);

#endif /* MAIN_SERVICE_WIFI_WIFI_H_ */
