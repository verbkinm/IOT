/*
 * update.h
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_UPDATE_H_
#define MAIN_SCREEN_PAGE_SETTING_UPDATE_H_

#include <lvgl.h>
#include "Local_Lib/local_lib.h"
#include "screen_page/setting/settingpage.h"
#include "screen_page/setting/elements.h"
#include "screen_page/full_screen_page/full_screen_page.h"
#include "service/update/update.h"

void create_update_sub_page(lv_event_t *e);
void free_update_sub_page(void);

#endif /* MAIN_SCREEN_PAGE_SETTING_UPDATE_H_ */
