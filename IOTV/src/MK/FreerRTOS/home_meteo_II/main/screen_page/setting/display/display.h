/*
 * display.h
 *
 *  Created on: 25 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_DISPLAY_H_
#define MAIN_SCREEN_PAGE_SETTING_DISPLAY_H_

#include <lvgl.h>
#include "screen_page/setting/settingpage.h"
#include "screen_page/setting/elements.h"
#include "json/json_config.h"
#include "service/display/display.h"

void create_display_sub_page(lv_event_t *e);

void free_display_sub_page(void);

#endif /* MAIN_SCREEN_PAGE_SETTING_DISPLAY_H_ */
