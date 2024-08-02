/*
 * service.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */
#include "service.h"

#include "wifi/wifi.h"
#include "update/update.h"
#include "tcp/tcp.h"
#include "relay/buttons.h"
#include "relay/relay.h"
#include "factory_mode/factory_mode.h"

void start_services(void)
{
	xTaskCreate(tcp_server_task, tcp_server_task_name(), 8192, 0, 10, 0);
	xTaskCreate(wifi_service_task, wifi_service_task_name(), 4096, 0, 10, 0);
	xTaskCreate(update_service_task, update_service_task_name(), 4096, 0, 10, 0);
	xTaskCreate(buttons_service_task, buttons_service_task_name(), 4096, 0, 10, 0);
	xTaskCreate(relay_service_task, relay_service_task_name(), 4096, 0, 10, 0);
	xTaskCreate(factory_mode_service_task, factory_mode_service_task_name(), 4096, 0, 10, 0);
}
