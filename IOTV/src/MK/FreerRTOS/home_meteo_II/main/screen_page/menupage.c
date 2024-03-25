/*
 * menupage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */


#include "menupage.h"


//extern lv_font_t ubuntu_mono_26;
extern lv_font_t ubuntu_mono_14;

extern void homePageInit(void);
extern void settingPageInit(void);

static void btn_main_handler(lv_event_t *e);
static void btn_datetime1_handler(lv_event_t *e);
static void btn_datetime2_handler(lv_event_t *e);
static void btn_setting_handler(lv_event_t *e);
static void btn_meteochart_handler(lv_event_t *e);
static void btn_home_meteo_handler(lv_event_t *e);

static void btn_main_handler(lv_event_t *e)
{
	homePageInit();
}

static void btn_datetime1_handler(lv_event_t *e)
{
	datetime1_page_init();
}

static void btn_datetime2_handler(lv_event_t *e)
{
	datetime2_page_init();
}

static void btn_setting_handler(lv_event_t *e)
{
	settingPageInit();
}

static void btn_meteochart_handler(lv_event_t *e)
{
	meteo_chart_page_init();
}

static void btn_home_meteo_handler(lv_event_t *e)
{
	home_meteo_page_init();
}

void menuPageInit(void)
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = menu_page_deinit;
	page->title = page_title(MENU_PAGE_TITLE);
	status_panel_update();

	lv_obj_t *widget = lv_obj_create(page->widget);

	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_add_style(widget, screenStyleDefault(), 0);

	const int SIZE = 100;
	const int COUNT = 6;
	const int COLUMN = 3;
	int ROW = COUNT / COLUMN;
	if (COUNT % COLUMN > 0)
		++ROW;
	const int PAD = 50;

	lv_obj_t *btns[COUNT];

	void (*cb[])(lv_event_t *e) = {btn_main_handler, btn_datetime1_handler,
			btn_datetime2_handler, btn_setting_handler,
			btn_meteochart_handler, btn_home_meteo_handler};


	lv_coord_t x = (LCD_H_RES - (SIZE * COLUMN + PAD * (COLUMN - 1))) / 2;
	lv_coord_t y = (LCD_V_RES - LCD_PANEL_STATUS_H - (SIZE * ROW + PAD * (ROW - 1))) / 2;

	for (uint8_t i = 0; i < COUNT; ++i, x+= SIZE + PAD)
	{
		if (i == COLUMN)
		{
			x = (LCD_H_RES - (SIZE * COLUMN + PAD * (COLUMN - 1))) / 2;
			y = y + SIZE + PAD;
		}

		char *buf_title = calloc(1, strlen(page_title(i + 2)) + 1);
		strcpy(buf_title, page_title(i + 2));
		replace_char(buf_title, ' ', '\n');

		btns[i] = create_button_simply(widget, buf_title, SIZE, SIZE, &ubuntu_mono_14);
		free(buf_title);

		lv_obj_set_pos(btns[i], x, y);
		lv_obj_add_event_cb(btns[i], cb[i], LV_EVENT_CLICKED, NULL);
	}
}

void menu_page_deinit(void)
{
	default_page_deinit();
}
