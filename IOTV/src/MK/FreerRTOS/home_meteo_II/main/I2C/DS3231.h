/*
 * DS3231.h
 *
 *  Created on: 10 мая 2023 г.
 *      Author: user
 */

#ifndef MAIN_I2C_DS3231_H_
#define MAIN_I2C_DS3231_H_

#include <sys/time.h>
#include "esp_log.h"
#include "driver/i2c.h"
//#include "time.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"

#define DS3231_REG_SEC	0x00
#define DS3231_REG_YEAR	0x06

struct DateTime {
	uint8_t seconds,
			minutes,
			hour,
			day,
			date,
			month,
			year;
	bool err;
};

struct DateTime DS3231_DataTime(void);
void DS3231_SetDataTime(const struct DateTime *dt);
void DS3231_SetDataTime_tm(const struct tm *t);

void DS3231_set_system_time(void);

#endif /* MAIN_I2C_DS3231_H_ */
