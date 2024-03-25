/*
 * page.h
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_PAGE_H_
#define MAIN_SCREEN_PAGE_PAGE_H_

#include "lvgl.h"

enum {
	EMPTY_PAGE_TITLE = 0,
	MENU_PAGE_TITLE,
	MAIN_PAGE_TITLE,
	DATETIME1_PAGE_TITLE,
	DATETIME2_PAGE_TITLE,
	SETTING_PAGE_TITLE,
	METEO_CHART_PAGE_TITLE,
	HOME_METEO_PAGE_TITLE,

	END_PAGE_TITLE
};

typedef struct Page {
	char *title;
	lv_obj_t *widget;
	void (*deinit)(void);
} page_t;

char* page_title(uint8_t num);

page_t *current_page(void);
void default_page_deinit(void);

#endif /* MAIN_SCREEN_PAGE_PAGE_H_ */
