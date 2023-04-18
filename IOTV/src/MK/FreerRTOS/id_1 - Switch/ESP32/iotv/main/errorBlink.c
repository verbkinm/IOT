/*
 * errorBlink.c
 *
 *  Created on: 18 апр. 2023 г.
 *      Author: user
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "errorBlink.h"

static const char *failStr[] = {
		"I2C_INIT_FAIL",
		"I2C_DEINIT_FAIL",
		"I2C_WRITE_FAIL",
		"I2C_READ_FAIL",
		"RESULT__INTERRUPT_STATUS_GPIO_LOOP",
		"RESULT__RANGE_STATUS_LOOP",
		"SYSTEM__FRESH_OUT_OF_RESET_LOOP"
};

static SemaphoreHandle_t xSemaphore = NULL;

void errorLoopBlink(const char* TAG, uint8_t value)
{
	if (xSemaphore == NULL)
		xSemaphore = xSemaphoreCreateMutex();

	if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
	{
		gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT_OUTPUT);

		for (uint8_t counter = 0; counter < value; ++counter)
		{
			gpio_set_level(GPIO_NUM_2, 1);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			gpio_set_level(GPIO_NUM_2, 0);
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
		vTaskDelay(500 / portTICK_PERIOD_MS);

		if (value > 0 && value < SYSTEM__FRESH_OUT_OF_RESET_LOOP) // SYSTEM__FRESH_OUT_OF_RESET_LOOP - крайний элемент
			ESP_LOGE(TAG, "FAIL: %s", failStr[value - 1]);

		xSemaphoreGive( xSemaphore );
	}
}
