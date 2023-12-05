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

uint8_t glob_currentPage = PAGE_HOME;
bool glob_wifi_STA_run = false;

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
