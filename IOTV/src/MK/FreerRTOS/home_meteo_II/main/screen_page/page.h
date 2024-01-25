/*
 * page.h
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_PAGE_H_
#define MAIN_SCREEN_PAGE_PAGE_H_

#include "lvgl.h"

//const char *page_title[] = {"Главный экран", "Меню", "Настройки"};

typedef struct Page {
	char *title;
	lv_obj_t *widget;
	void (*deinit)(void);
} page_t;

#endif /* MAIN_SCREEN_PAGE_PAGE_H_ */
