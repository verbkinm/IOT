/*
 * date_time.h
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_DATE_TIME_H_
#define MAIN_SCREEN_PAGE_SETTING_DATE_TIME_H_

#include <lvgl.h>
#include "elements.h"

void create_date_time_sub_page(lv_event_t *e);
void create_time_page(void);
void create_date_page(void);
void create_sntp_page(void);

#endif /* MAIN_SCREEN_PAGE_SETTING_DATE_TIME_H_ */
