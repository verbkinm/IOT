/*
 * factory_reset.c
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#include "factory_reset.h"

#define RESET_FACTORY_COUNTER 	10

static const char *TAG = "factory_reset";

static void gpio_reset_init();

static void gpio_reset_init()
{
	gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
	ESP_LOGI(TAG, "GPIO_NUM_0 for input");
}

void factory_reset_service_task(void *pvParameters)
{
	gpio_reset_init();

	uint8_t counter = 0;

	for( ;; )
	{
		ESP_LOGI(TAG, "GPIO_NUM_0 = %d", gpio_get_level(GPIO_NUM_0));
		ESP_LOGI(TAG, "RESET_FACTORY_COUNTER = %d", counter);
		if (gpio_get_level(GPIO_NUM_0) == 1)
			counter = 0;
		else
			++counter;

		if (counter > RESET_FACTORY_COUNTER)
			ESP_LOGE(TAG, "FACTORY RESET!!!");
//		if (glob_get_status_err())
//			break;
//
//
//		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
