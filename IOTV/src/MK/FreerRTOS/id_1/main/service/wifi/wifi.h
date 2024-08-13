/*
 * wifi.h
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_WIFI_WIFI_H_
#define MAIN_SERVICE_WIFI_WIFI_H_

#include "esp_wifi_types.h"

void wifi_service_task(void *pvParameters);
const char *wifi_service_task_name(void);

void wifi_service_apsta_init(void);
void wifi_service_deinit(void);

void wifi_service_only_sta_init(void);
void wifi_service_only_ap_init(void);

void wifi_service_scan_wifi(void);
const wifi_ap_record_t *wifi_service_last_scan(void);

void wifi_service_conntect_to_AP(const char *ssid, const char *pwd);

#endif /* MAIN_SERVICE_WIFI_WIFI_H_ */
