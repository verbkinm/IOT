/*
 * sntp.h
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_SNTP_SNTP_H_
#define MAIN_SERVICE_SNTP_SNTP_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"

#include "../../Global_def.h"

void sntp_service_task(void *pvParameters);

#endif /* MAIN_SERVICE_SNTP_SNTP_H_ */
