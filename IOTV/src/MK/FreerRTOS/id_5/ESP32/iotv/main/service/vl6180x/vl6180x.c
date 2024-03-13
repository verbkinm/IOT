/*
 * vl6180x.c
 *
 *  Created on: 9 мар. 2024 г.
 *      Author: user
 */

#include "vl6180x.h"

static const char *TAG = "service vl6180x";

void Vl6180X_Task(void *pvParameters)
{
	ESP_LOGW(TAG, "Vl6180X task created");

	struct IOTV_Server_embedded *iot = iotv_get();

	while(iot->state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	char *releState;
	releState = iot->readChannel[CH_RELAY_STATE].data;

	int16_t *border = (int16_t *)iot->readChannel[CH_BORDER].data;
	int16_t *range = (int16_t *)iot->readChannel[CH_RANGE].data;

	gpio_set_direction(RELE_PIN, GPIO_MODE_INPUT_OUTPUT);

	VL6180X_init();

	while(true)
	{
		*range = VL6180X_simpleRange();
		//		*(double *)iot.readChannel[3].data = VL6180X_simpleALS(VL6180X_ALS_GAIN_5);
		//		printf("ALS: %d\n", VL6180X_simpleALS(VL6180X_ALS_GAIN_5));

		if (*range < *border)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			*range = VL6180X_simpleRange();
			if (*range < *border)
			{
				gpio_set_level(RELE_PIN, !(*releState));
				if (*releState != gpio_get_level(RELE_PIN))
				{
					*releState ^= 1;
					printf("Rele state: %s, StateRange: %d\n", (*releState ? "ON" : "OFF"), *range);
					if (*releState)
						glob_set_bits_status_reg(STATUS_RELE);
					else
						glob_clear_bits_status_reg(STATUS_RELE);
				}
				else
					ESP_LOGE(TAG, "Can't switch relay");

				vTaskDelay(1000 / portTICK_PERIOD_MS);
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
