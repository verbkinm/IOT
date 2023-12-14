/*
 * wifi.h
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_WIFI_WIFI_H_
#define MAIN_SERVICE_WIFI_WIFI_H_

#include "esp_wifi.h"
#include "freertos/task.h"

#include "../../Global_def.h"

void wifi_service_task(void *pvParameters);

#endif /* MAIN_SERVICE_WIFI_WIFI_H_ */
