/*
 * display.h
 *
 *  Created on: 25 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_DISPLAY_DISPLAY_H_
#define MAIN_SERVICE_DISPLAY_DISPLAY_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "json/json_config.h"
#include "TFT_touchscreen/TFT_touch_screen.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "time.h"

struct Service_display{
	uint8_t brightnes;
	uint8_t brightness_day;
	uint8_t brightness_night;

	uint8_t day_begin_h;
	uint8_t day_begin_m;
	uint8_t day_begin_s;

	uint8_t night_begin_h;
	uint8_t night_begin_m;
	uint8_t night_begin_s;
};
typedef struct Service_display service_display_t;

void service_display_task(void *pvParameters);
void service_display_read_conf(void);

#endif /* MAIN_SERVICE_DISPLAY_DISPLAY_H_ */
