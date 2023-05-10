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
			ESP_LOGE(TAG, "read error");
		}
	}

	for (int i = DS3231_REG_SEC; i <= DS3231_REG_YEAR; ++i)
		data_read[i] = bcdToDec(data_read[i]);

//	dt.seconds = bcdToDec(dt.seconds);
//	dt.minutes = bcdToDec(dt.minutes);
//	dt.hour = bcdToDec(dt.hour);// + (((dt.hour & 0x20) >> 5) * 10);
//	dt.date = bcdToDec(dt.date);
//	dt.month = bcdToDec(dt.month);
//	dt.year = bcdToDec(dt.year);

	return dt;
}

void DS3231_SetDataTime(const struct DateTime *dt)
{
	if (dt == NULL)
		return ;

	struct DateTime copy_dt;// = *dt;

	for (int i = DS3231_REG_SEC; i <= DS3231_REG_YEAR; ++i)
		((uint8_t *)&copy_dt)[i] = decToBcd(((uint8_t *)dt)[i]);

//	copy_dt.seconds = decToBcd(dt->seconds);
//	copy_dt.minutes = decToBcd(dt->minutes);
//	copy_dt.hour = decToBcd(dt->hour);// + (((dt.hour & 0x20) >> 5) * 10);
//	copy_dt.date = decToBcd(dt->date);
//	copy_dt.month = decToBcd(dt->month);
//	copy_dt.year = decToBcd(dt->year);

	uint8_t *data_write = (uint8_t *)&copy_dt;

	for (uint8_t reg = DS3231_REG_SEC; reg <= DS3231_REG_YEAR; ++reg)
	{
		uint8_t data[2] = {reg, data_write[reg]};
		if (i2c_master_write_to_device(I2C_MASTER_NUM, DS3231_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
		{
			ESP_LOGE(TAG, "write error");
		}
	}
}
