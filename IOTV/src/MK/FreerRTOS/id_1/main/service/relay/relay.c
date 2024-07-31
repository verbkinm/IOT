/*
 * relay.c
 *
 *  Created on: 31 июл. 2024 г.
 *      Author: user
 */

#include "relay.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"
#include "iotv.h"

const static char *TAG = "RELAY";
const static char *task_name = "relay_task";

static const int RELAY_COUNT = 3;
static gpio_num_t relay[] = {RELE_0_PIN, RELE_1_PIN, RELE_2_PIN};

static void relay_init(void);

static void relay_init()
{
	for (int i = 0; i < RELAY_COUNT; i++)
	{
		gpio_reset_pin(relay[i]);
		gpio_set_direction(relay[i], GPIO_MODE_INPUT_OUTPUT);
		gpio_set_pull_mode(relay[i], GPIO_PULLUP_ONLY);
		gpio_set_level(relay[i], 0);
	}
}
const char *relay_service_task_name()
{
	return task_name;
}

void relay_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);
	printf("%s %s start\n", TAG, task_name);
	relay_init();

	iotv_obj_t *iot = iotv_get();

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		for (int i = 0; i < RELAY_COUNT; i++)
		{
			bool relayState = gpio_get_level(relay[i]);
			bool iotDataState = *(bool *)iot->readChannel[i].data;

			if (relayState != iotDataState)
			{
				printf("%s relay %d turn to %d\n", TAG, i, iotDataState);
				gpio_set_level(relay[i], iotDataState);
			}
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
	printf("%s %s stop\n", TAG, task_name);
}
