/*
 * wifi.h
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_WIFI_WIFI_H_
#define MAIN_SERVICE_WIFI_WIFI_H_

void wifi_service_task(void *pvParameters);
const char *wifi_service_task_name(void);

void wifi_service_apsta_init(void);
void wifi_service_deinit(void);

void wifi_service_only_sta_init(void);
void wifi_service_only_ap_init(void);

#endif /* MAIN_SERVICE_WIFI_WIFI_H_ */
