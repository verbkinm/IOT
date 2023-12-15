/*
 * service.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */
#include "service.h"

void rgb_panel_start_services(void)
{
	xTaskCreate(wifi_service_task, "wifi_service_task", 4096, 0, 10, 0);
	xTaskCreate(sntp_service_task, "sntp_service_task", 4096, 0, 10, 0);
}
