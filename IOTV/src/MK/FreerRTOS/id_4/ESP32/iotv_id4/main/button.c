/*
 * button.c
 *
 *  Created on: 24 июл. 2023 г.
 *      Author: user
 */

#include "button.h"

#define BUTTON_SIZE	3

extern QueueHandle_t xQueueDF;
extern struct IOTV_Server_embedded iot;

static const char *TAG = "button";

static bool *isPlay;
static int8_t *curr_mode,  *Led_RGB_scriptNumber;

static gpio_num_t buttons[BUTTON_SIZE] = {BUTTON_PLAY_STOP, BUTTON_PLAY_MODE, BUTTON_LED_MODE};

static void defaultButtonConfig(gpio_num_t gpio_num);
static bool isButtonPressed(gpio_num_t gpio_num);
static void waitButtonRelease(gpio_num_t gpio_num);

void Button_Task(void *pvParameters)
{
	while(iot.state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	isPlay = (bool *)iot.readChannel[CH_PLAY].data;
	curr_mode = (int8_t *)iot.readChannel[CH_PLAY_MODE].data;
	Led_RGB_scriptNumber = (int8_t *)iot.readChannel[CH_LED_MODE].data;

	ESP_LOGW(TAG, "button task created");

	for (int8_t i = 0; i < BUTTON_SIZE; ++i)
		defaultButtonConfig(buttons[i]);

	int8_t cmd;
	while (true)
	{
		if (isButtonPressed(buttons[0]))
		{
			*isPlay = !*isPlay;
			cmd = DF_CMD_PS;
			xQueueSend(xQueueDF, (void *)&cmd, portMAX_DELAY);

			waitButtonRelease(buttons[0]);
		}
		if (isButtonPressed(buttons[1]))
		{
			cmd = DF_CMD_MODE;
			*curr_mode = inc(*curr_mode, DF_MODE_1, DF_MODE_3);
			xQueueSend(xQueueDF, (void *)&cmd, portMAX_DELAY);

			waitButtonRelease(buttons[1]);
		}
		if (isButtonPressed(buttons[2]))
		{
			*Led_RGB_scriptNumber = inc(*Led_RGB_scriptNumber, LED_MODE_MIN, LED_MODE_MAX);

			waitButtonRelease(buttons[2]);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

static void defaultButtonConfig(gpio_num_t gpio_num)
{
	gpio_reset_pin(gpio_num);
	gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
	gpio_set_pull_mode(gpio_num, GPIO_PULLDOWN_ONLY);
}

static bool isButtonPressed(gpio_num_t gpio_num)
{
	if (gpio_get_level(gpio_num))
	{
		vTaskDelay(100 / portTICK_PERIOD_MS);
		if (gpio_get_level(gpio_num))
			return true;
	}

	return false;
}

static void waitButtonRelease(gpio_num_t gpio_num)
{
	while(isButtonPressed(gpio_num))
		vTaskDelay(100 / portTICK_PERIOD_MS);
}
