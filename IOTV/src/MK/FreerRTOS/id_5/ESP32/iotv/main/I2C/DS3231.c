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
	struct DateTime dt = {.err = false};

	for (uint8_t reg = DS3231_REG_SEC; reg <= DS3231_REG_YEAR; ++reg)
	{
		if (i2c_master_write_read_device(I2C_MASTER_NUM, DS3231_ADDR, &reg, 1, ((uint8_t *)&dt + reg), 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
		{
			ESP_LOGE(TAG, "read error");
			dt.err = true;
		}
	}

	for (int i = DS3231_REG_SEC; i <= DS3231_REG_YEAR; ++i)
		((uint8_t *)&dt)[i] = bcdToDec(((uint8_t *)&dt)[i]);

	return dt;
}

void DS3231_SetDataTime(const struct DateTime *dt)
{
	if (dt == NULL || dt->err == true)
		return;

	struct DateTime copy_dt;

	for (int i = DS3231_REG_SEC; i <= DS3231_REG_YEAR; ++i)
		((uint8_t *)&copy_dt)[i] = decToBcd(((uint8_t *)dt)[i]);

	uint8_t *data_write = (uint8_t *)&copy_dt;

	for (uint8_t reg = DS3231_REG_SEC; reg <= DS3231_REG_YEAR; ++reg)
	{
		uint8_t data[2] = {reg, data_write[reg]};
		if (i2c_master_write_to_device(I2C_MASTER_NUM, DS3231_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) != ESP_OK)
			ESP_LOGE(TAG, "write error");
	}
}

void DS3231_SetDataTime_tm(const struct tm *t)
{
	if (t == NULL)
		return;

	struct DateTime dt = {
			.seconds = t->tm_sec,
			.minutes = t->tm_min,
			.hour = t->tm_hour,
			.day = t->tm_wday,
			.date = t->tm_mday,
			.month = t->tm_mon,
			.year = (t->tm_year + 1900) - 2000,
			.err = false
	};

	DS3231_SetDataTime(&dt);
}

void DS3231_set_system_time(void)
{
	struct DateTime dt = DS3231_DataTime();

	if (dt.err)
		return;

	time_t now;
	time(&now);

	struct tm timeinfo = *localtime(&now);

	timeinfo.tm_sec = dt.seconds;
	timeinfo.tm_min = dt.minutes;
	timeinfo.tm_hour = dt.hour;
	timeinfo.tm_mday = dt.date;
	timeinfo.tm_mon = dt.month;
	timeinfo.tm_year = dt.year + 2000 - 1900;
	timeinfo.tm_wday = dt.day;

	now = mktime(&timeinfo);
	struct timeval tv = {.tv_sec = now};
	settimeofday(&tv, NULL);
}
