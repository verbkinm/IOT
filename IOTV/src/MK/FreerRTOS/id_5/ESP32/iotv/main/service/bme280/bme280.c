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
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);

	BME280_init();

	struct IOTV_Server_embedded *iot = iotv_get();

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if (glob_get_update_reg() & UPDATE_NOW)
			break;

		thp = BME280_readValues();
		if (!thp.err)
		{
			*(float *)iot->readChannel[CH_TEMP].data = thp.temperature;
			*(float *)iot->readChannel[CH_PRES].data = thp.pressure;
			*(float *)iot->readChannel[CH_HUM].data = thp.humidity;
		}

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

