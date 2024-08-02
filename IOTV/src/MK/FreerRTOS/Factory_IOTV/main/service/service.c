/*
 * service.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */
#include "service.h"

#include "wifi/wifi.h"
//#include "update/update.h"
#include "factory_mode/factory_mode.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void services_start(void)
{
	xTaskCreate(wifi_service_task, wifi_service_task_name(), 4096, 0, 10, 0);
//	xTaskCreate(update_service_task, update_service_task_name(), 4096, 0, 10, 0);
	xTaskCreate(factory_mode_service_task, factory_mode_service_task_name(), 4096, 0, 10, 0);
}
