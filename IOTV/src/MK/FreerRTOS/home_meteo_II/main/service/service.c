/*
 * service.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */
#include "service.h"

void start_services(void)
{
	xTaskCreate(wifi_service_task, "wifi_service_task", 4096, 0, 10, 0);
	xTaskCreate(sntp_service_task, "sntp_service_task", 4096, 0, 10, 0);
	xTaskCreate(weather_service_task, "weather_service_task", 8192, 0, 10, 0);
	xTaskCreate(service_display_task, "display_service_task", 4096, 0, 10, 0);

	xTaskCreate(BME280_service_task, "BME280_service_task", 4096, 0, 10, 0);
	xTaskCreate(update_service_task, "update_service_task", 4096, 0, 1, 0);
}
