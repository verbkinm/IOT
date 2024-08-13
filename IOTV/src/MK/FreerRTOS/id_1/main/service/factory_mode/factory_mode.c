/*
 * factory_mode.c
 *
 *  Created on: 1 авг. 2024 г.
 *      Author: user
 */

#include "global/global_def.h"
#include "factory_mode.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "nvs_flash.h"
#include "netif/etharp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "global/registers.h"
#include "Local_Lib/local_lib.h"
#include "service/wifi/wifi.h"
#include "service/update/update.h"
#include "service/tcp/tcp.h"

const static char *TAG = "FACTORY_MODE";
const static char *task_name = "factory_mode_task";

void factory_mode_service_task(void *pvParameters)
{
//	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);

	glob_set_bits_service_reg(SERVICE_FACTORY_MODE_ON);
	printf("%s %s start\n", TAG, task_name);

	// FACTORY_MODE_PIN init
	gpio_reset_pin(FACTORY_MODE_PIN);
	gpio_set_direction(FACTORY_MODE_PIN, GPIO_MODE_INPUT);
	gpio_set_pull_mode(FACTORY_MODE_PIN, GPIO_PULLDOWN_ONLY);

	for( ;; )
	{
		if (gpio_get_level(FACTORY_MODE_PIN) == 0)
		{
			vTaskDelay(5000 / portTICK_PERIOD_MS);
			if (gpio_get_level(FACTORY_MODE_PIN) == 0)
			{
				printf("%s boot\n", TAG);
				glob_set_status_err(STATUS_ERROR_BREAK_ALL_SERVICE);
				tcp_server_task_close_listen();

				while(glob_get_service_reg() != SERVICE_FACTORY_MODE_ON)
				{
					printf("%s %s wait stop all service - 0x%x\n", TAG, task_name, (int)glob_get_service_reg());
					vTaskDelay(1000 / portTICK_PERIOD_MS);
				}

				if (update_service_backtofactory() == ESP_OK)
					esp_restart();
			}
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	glob_clear_bits_service_reg(SERVICE_FACTORY_MODE_ON);
	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
}

const char *factory_mode_service_task_name(void)
{
	return task_name;
}
