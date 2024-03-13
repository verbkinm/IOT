/*
 * datetime1.c
 *
 *  Created on: 27 дек. 2023 г.
 *      Author: user
 */

#include "datetime1.h"

extern lv_font_t ubuntu_mono_48;
extern lv_font_t ubuntu_mono_148;

static lv_obj_t *lbl_time, *lbl_date;
static lv_timer_t *timer = NULL;

static void draw_time(const struct tm *timeinfo);
static void draw_date(const struct tm *timeinfo);
static void event_handler(lv_event_t * e);

static void timer_handler(lv_timer_t *timer);

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
	datetime2_page_init();
}

static void timer_handler(lv_timer_t *timer)
{
	if (glob_get_status_err())
	{
		lv_timer_pause(timer);
		return;
	}

	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	draw_time(&timeinfo);
	draw_date(&timeinfo);
}

void datetime1_page_init(void)
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = datetime1_page_deinit;
	page->title = page_title(DATETIME1_PAGE_TITLE);
	status_panel_update();

	lv_obj_t *widget = lv_obj_create(page->widget);
	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_add_style(widget, screenStyleDefault(), 0);

	lv_obj_add_event_cb(widget, event_handler, LV_EVENT_CLICKED, 0);

	lbl_time = create_lbl_obj(widget, "00:00:00", 0, LV_ALIGN_CENTER, 0, -40, lv_color_white(), &ubuntu_mono_148);
	lbl_date = create_lbl_obj(widget, "Воскресенье 01.06.1986", 0, LV_ALIGN_BOTTOM_MID, 0, -40, lv_color_white(), &ubuntu_mono_48);

	timer = lv_timer_create(timer_handler, 1000, NULL);
	lv_timer_ready(timer);
}

void datetime1_page_deinit(void)
{
	default_page_deinit();
	lv_timer_del(timer);
}
