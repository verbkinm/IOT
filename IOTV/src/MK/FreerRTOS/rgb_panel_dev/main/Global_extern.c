#include "Global_def.h"
#include "I2C/DS3231.h"
#include "service/weather/weather.h"
#include <lvgl.h>

uint8_t glob_currentPage = PAGE_HOME;

struct DateTime glob_date_time = {
		.seconds = 0,
		.minutes = 0,
		.hour = 0,
		.day = 1,
		.date = 1,
		.month = 1,
		.year = 2000 - 1900,
		.err = false
};

uint32_t glob_status_reg = 0;
uint32_t glob_status_err = 0;

open_meteo_data_t glob_open_meteo = {0};
