/*
 * DS3231.c
 *
 *  Created on: 10 мая 2023 г.
 *      Author: user
 */

#include "DS3231.h"

static const char *TAG = "DS3132";

struct DateTime DS3231_DataTime(void)
{
	struct DateTime dt;
	uint8_t *data_read = (uint8_t *)&dt;

	for (uint8_t reg = DS3231_REG_SEC; reg <= DS3231_REG_YEAR; ++reg)
	{
		if (i2c_master_write_read_device(I2C_MASTER_NUM, DS3231_ADDR, &reg, 1, (data_read + reg), 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
		{
//			ESP_LOGE(TAG, "read error");
			dt.err = true;
		}
	}

	for (int i = DS3231_REG_SEC; i <= DS3231_REG_YEAR; ++i)
		data_read[i] = bcdToDec(data_read[i]);

	return dt;
}

void DS3231_SetDataTime(const struct DateTime *dt)
{
	if (dt == NULL)
		return;

	struct DateTime copy_dt;

	for (int i = DS3231_REG_SEC; i <= DS3231_REG_YEAR; ++i)
		((uint8_t *)&copy_dt)[i] = decToBcd(((uint8_t *)dt)[i]);

	uint8_t *data_write = (uint8_t *)&copy_dt;

	for (uint8_t reg = DS3231_REG_SEC; reg <= DS3231_REG_YEAR; ++reg)
	{
		uint8_t data[2] = {reg, data_write[reg]};
		if (i2c_master_write_to_device(I2C_MASTER_NUM, DS3231_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
		{
			;
//			ESP_LOGE(TAG, "write error");
		}
	}
}
