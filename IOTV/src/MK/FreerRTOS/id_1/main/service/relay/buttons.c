/*
 * button.c
 *
 *  Created on: 31 июл. 2024 г.
 *      Author: user
 */

#include "buttons.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"
#include "iotv.h"

const static char *TAG = "BUTTONS";
const static char *task_name = "buttons_task";

static const int BUTTONS_COUNT = 3;
static gpio_num_t buttons[] = {BUTTON_0_PIN, BUTTON_1_PIN, BUTTON_2_PIN};
static bool wait_released[] = {false, false, false};

static void buttons_init(void);

static void buttons_init()
{
	for (int i = 0; i < BUTTONS_COUNT; i++)
	{
		gpio_reset_pin(buttons[i]);
		gpio_set_direction(buttons[i], GPIO_MODE_INPUT);
		gpio_set_pull_mode(buttons[i], GPIO_PULLUP_ONLY);
	}
}

const char *buttons_service_task_name()
{
	return task_name;
}

void buttons_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);
	printf("%s %s start\n", TAG, task_name);
	buttons_init();

	iotv_obj_t *iot = iotv_get();

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		for (int i = 0; i < BUTTONS_COUNT; i++)
		{
			// кнопку нажата и не ожидается её отжатие
			if ((gpio_get_level(buttons[i]) == 0) && (wait_released[i] == false))
			{
				printf("%s button %d pressed\n", TAG, i);
				*(bool *)iot->readChannel[i].data = !(*(bool *)iot->readChannel[i].data);
				wait_released[i] = true;
				vTaskDelay(50 / portTICK_PERIOD_MS);
			}

//			if (gpio_get_level(buttons[i]) == 1) // кнопка не нажата
//				wait_released[i] = false;
		}
		//!!! нужен ли цикл?
		for (int i = 0; i < BUTTONS_COUNT; i++)
		{
			if (gpio_get_level(buttons[i]) == 1) // кнопка не нажата
				wait_released[i] = false;
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
	printf("%s %s stop\n", TAG, task_name);
}
