/*
 * datetime1.c
 *
 *  Created on: 27 дек. 2023 г.
 *      Author: user
 */

#include "datetime1.h"

extern lv_font_t ubuntu_mono_48;
extern lv_font_t ubuntu_mono_148;

extern uint8_t glob_currentPage;

static lv_obj_t *lbl_time, *lbl_date;

static void draw_time(const struct tm *timeinfo);
static void draw_date(const struct tm *timeinfo);
static void event_handler(lv_event_t * e);

static void draw_time(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(lbl_time, "%.02d:%.02d:%.02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

static void draw_date(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(lbl_date, "%s %02d.%.02d.%.04d", weekday_name_full(timeinfo->tm_wday), timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

static void event_handler(lv_event_t * e)
{
	printf("event heandler PAGE_DATETIME_1 page\n");
	menuPageInit();
}

void datetime1_page_init(void)
{
	glob_currentPage = PAGE_NONE;

	lv_obj_t *main_widget = lv_obj_get_child(lv_scr_act(), 1);
	lv_obj_clean(main_widget);
	lv_obj_t *widget = lv_obj_create(main_widget);
	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_add_style(widget, screenStyleDefault(), 0);

	lv_obj_add_event_cb(widget, event_handler, LV_EVENT_CLICKED, 0);

	lbl_time = create_lbl_obj(widget, "00:00:00", 0, LV_ALIGN_CENTER, 0, -40, lv_color_white(), &ubuntu_mono_148);
	lbl_date = create_lbl_obj(widget, "Воскресенье 01.06.1986", 0, LV_ALIGN_BOTTOM_MID, 0, -40, lv_color_white(), &ubuntu_mono_48);

	glob_currentPage = PAGE_DATETIME_1;
	draw_datetime1_page();
}

void draw_datetime1_page(void)
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	draw_time(&timeinfo);
	draw_date(&timeinfo);
}
