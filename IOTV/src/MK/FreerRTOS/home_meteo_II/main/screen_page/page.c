/*
 * page.c
 *
 *  Created on: 29 янв. 2024 г.
 *      Author: user
 */

#include "page.h"

static page_t glob_current_page;
static char *page_title_str[] = {" ", "Меню", "Главный экран", "Цифровые часы", "Бинарные часы", "Настройки", "График погоды", "Погода в доме"};

char* page_title(uint8_t num)
{
	if (num >= END_PAGE_TITLE)
		return page_title_str[0];

	return page_title_str[num];
}

page_t *current_page(void)
{
	return &glob_current_page;
}

void default_page_deinit(void)
{
	page_t *page = current_page();
	lv_obj_clean(page->widget);
}
