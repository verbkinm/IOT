//#include "freertos/FreeRTOS.h"
//#include "freertos/queue.h"
//#include <stdint.h>
//#include <stdbool.h>
//
//uint64_t realBufSize = 0;
//uint64_t expextedDataSize = 20;
//
///* Статусы реле, соединения TCP и WIFI
//Биты:
//0 - wifi
//1 - tcp
//3 - rele
//*/
//uint8_t glob_status = 0;

#include "Global_def.h"
#include "I2C/DS3231.h"
#include <lvgl.h>

uint8_t glob_currentPage = PAGE_HOME;
bool glob_wifi_STA_run = false;

lv_obj_t *glob_status_panel = NULL;
lv_obj_t *glob_busy_indicator = NULL;
lv_obj_t *glob_msgbox = NULL;

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
