/*
 * bme280.c
 *
 *  Created on: 26 янв. 2024 г.
 *      Author: user
 */

#include "bme280.h"

static struct THP thp;

const struct THP *BME280_service_get_value(void)
{
	return &thp;
}

void BME280_service_task(void *pvParameters)
{
	BME280_init();

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if (glob_get_update_reg() & UPDATE_NOW)
			break;

		thp = BME280_readValues();

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

