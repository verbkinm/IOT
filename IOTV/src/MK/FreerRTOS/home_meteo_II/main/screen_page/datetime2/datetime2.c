/*
 * datetime2.c
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: user
 */

#include "datetime2.h"

#define BOX_SIZE    64

extern lv_font_t ubuntu_mono_48;

static lv_timer_t *timer = NULL;
static lv_obj_t *section_h;
static lv_obj_t *section_m;
static lv_obj_t *section_s;
static lv_obj_t *lbl_date;

lv_obj_t *create_section(lv_obj_t *parent);
void set_data(lv_obj_t *section, uint8_t value);
void set_color(lv_obj_t *obj, uint8_t value);

static void event_handler(lv_event_t * e);
static void timer_handler(lv_timer_t *timer);

lv_obj_t *create_section(lv_obj_t *parent)
{
    lv_obj_t *widget = lv_obj_create(parent);
    lv_obj_set_size(widget, BOX_SIZE * 2.5, BOX_SIZE * 5.5);
    lv_obj_set_style_border_width(widget, 0, 0);
    lv_obj_set_style_bg_opa(widget, 0, 0);
    lv_obj_set_style_pad_hor(widget, 0, 0);
    lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
    lv_obj_add_event_cb(widget, event_handler, LV_EVENT_CLICKED, 0);

    lv_obj_t *arr[8];

    for (int i = 0; i < 8; ++i)
    {
        arr[i] = lv_obj_create(widget);
        lv_obj_set_size(arr[i], BOX_SIZE, BOX_SIZE);
        lv_obj_set_style_pad_hor(arr[i], 0, 0);
        lv_obj_set_style_border_width(arr[i], 0, 0);
        lv_obj_set_scroll_dir(arr[i], LV_DIR_NONE);
        lv_obj_add_event_cb(arr[i], event_handler, LV_EVENT_CLICKED, 0);
    }

    lv_obj_align(arr[0], LV_ALIGN_BOTTOM_RIGHT, 0, 20);
    for (int i = 0; i < 3; ++i)
        lv_obj_align_to(arr[i + 1], arr[i], LV_ALIGN_OUT_TOP_MID, 0, -BOX_SIZE / 2);

    lv_obj_align_to(arr[4], arr[0], LV_ALIGN_OUT_LEFT_MID, -BOX_SIZE / 2, 0);
    for (int i = 4; i < 7; ++i)
        lv_obj_align_to(arr[i + 1], arr[i], LV_ALIGN_OUT_TOP_MID, 0, -BOX_SIZE / 2);

    return widget;
}

void set_data(lv_obj_t *section, uint8_t value)
{
    uint8_t low = value % 10;
    uint8_t high = value / 10;

    for (int i = 0, j = 1; i < 4; ++i, j *=2 )
        set_color(lv_obj_get_child(section, i), low & j);


    for (int i = 4, j = 1; i < 8; ++i, j *=2 )
        set_color(lv_obj_get_child(section, i), high & j);
}

void set_color(lv_obj_t *obj, uint8_t value)
{
    if (value)
        lv_obj_set_style_bg_color(obj, lv_color_make(0, 150, 200), 0);
    else
        lv_obj_set_style_bg_color(obj, lv_color_white(), 0);
}

static void event_handler(lv_event_t *e)
{
	datetime1_page_init();
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

    set_data(section_h, timeinfo.tm_hour);
    set_data(section_m, timeinfo.tm_min);
    set_data(section_s, timeinfo.tm_sec);

    lv_label_set_text_fmt(lbl_date, "%s 0x%02X.0x%02X.0x%03X", weekday_name_full(timeinfo.tm_wday), timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
}

void datetime2_page_init(void)
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = datetime2_page_deinit;
	page->title = page_title(DATETIME2_PAGE_TITLE);
	status_panel_update();

	lv_obj_t *widget = lv_obj_create(page->widget);

	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_add_style(widget, screenStyleDefault(), 0);

	section_h = create_section(widget);
	section_m = create_section(widget);
	section_s = create_section(widget);

	lbl_date = create_lbl_obj(widget, " ", 0, LV_ALIGN_BOTTOM_MID, 0, -10, lv_color_white(), &ubuntu_mono_48);

    lv_obj_align(section_m, LV_ALIGN_CENTER, 0, -32);
    lv_obj_align_to(section_s, section_m, LV_ALIGN_OUT_RIGHT_MID, BOX_SIZE, 0);

    lv_obj_align_to(section_h, section_m, LV_ALIGN_OUT_LEFT_MID, -BOX_SIZE, 0);

	lv_obj_add_event_cb(widget, event_handler, LV_EVENT_CLICKED, 0);

	timer = lv_timer_create(timer_handler, 1000, NULL);
	lv_timer_ready(timer);
}

void datetime2_page_deinit(void)
{
	default_page_deinit();
	lv_timer_del(timer);
}
