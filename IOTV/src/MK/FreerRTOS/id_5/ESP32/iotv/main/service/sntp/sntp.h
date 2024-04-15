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
#include "esp_log.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"

void sntp_service_task(void *pvParameters);
void sntp_obtain_time(void);

#endif /* MAIN_SERVICE_SNTP_SNTP_H_ */
