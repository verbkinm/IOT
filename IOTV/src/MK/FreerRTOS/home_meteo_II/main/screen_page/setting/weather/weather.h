/*
 * weather.h
 *
 *  Created on: 19 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_WEATHER_H_
#define MAIN_SCREEN_PAGE_SETTING_WEATHER_H_

#include <lvgl.h>
#include "screen_page/setting/settingpage.h"
#include "screen_page/setting/elements.h"
#include "service/weather/weather.h"

void create_weather_sub_page(lv_event_t *e);
void free_weather_sub_page(void);

#endif /* MAIN_SCREEN_PAGE_SETTING_WEATHER_H_ */
