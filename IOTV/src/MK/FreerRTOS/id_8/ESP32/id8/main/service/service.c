/*
 * service.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */
#include "service.h"

void start_services(void)
{
	xTaskCreate(wifi_service_task, "wifi_service_task", 8192, 0, 10, 0);
	xTaskCreate(tcp_server_task, "tcp_server_task", 8192, 0, 10, 0);
	xTaskCreate(iotv_service_task, "iotv_service_task", 8192, 0, 10, 0);
	xTaskCreate(update_service_task, "update_service_task", 4096, 0, 10, 0);
}
