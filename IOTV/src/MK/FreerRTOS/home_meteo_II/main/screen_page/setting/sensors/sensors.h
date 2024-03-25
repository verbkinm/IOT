/*
 * sensors.h
 *
 *  Created on: 9 февр. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_SENSORS_SENSORS_H_
#define MAIN_SCREEN_PAGE_SETTING_SENSORS_SENSORS_H_

#include <lvgl.h>
#include "screen_page/setting/settingpage.h"
#include "screen_page/setting/elements.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "service/bme280/bme280.h"

void create_sensors_sub_page(lv_event_t *e);
void free_sensors_sub_page(void);

#endif /* MAIN_SCREEN_PAGE_SETTING_SENSORS_SENSORS_H_ */
