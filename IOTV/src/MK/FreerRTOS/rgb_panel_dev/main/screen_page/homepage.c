/*
 * homepage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "lvgl.h"

#include "Global_def.h"
#include "I2C/DS3231.h"
#include "homepage.h"
#include "screendefault.h"
#include <stdio.h>
#include <stdlib.h>

extern uint8_t glob_currentPage;

extern struct DateTime glob_date_time;

extern void menuPageInit(void);

#define	STYLE_TIME 			0
#define	STYLE_DATE			1
#define	STYLE_TEMPERATURE	2
#define	STYLE_HUMIDITY		3
#define	STYLE_PRESSURE		4

static lv_style_t style_arr[5];

static void drawLabel(lv_obj_t *lbl, lv_style_t *style, const char* str, lv_align_t align, lv_coord_t offset_x, lv_coord_t offset_y);
static void drawTime(const struct tm *timeinfo);
static void drawDate(const struct tm *timeinfo);
static void drawTemperature();
static void drawHumidity();
static void drawPressure();

static void drawLabel(lv_obj_t *lbl, lv_style_t *style, const char* str, lv_align_t align, lv_coord_t offset_x, lv_coord_t offset_y)
{
	lv_label_set_text_fmt(lbl, "%s", str);
	lv_obj_align(lbl, align, offset_x, offset_y);

	lv_obj_add_style(lbl, style, 0);
}

static void drawTime(const struct tm *timeinfo)
{
	++glob_date_time.seconds;
	lv_obj_t *time = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 0);
//	lv_label_set_text_fmt(time, "%.02d:%.02d:%.02d", glob_date_time.hour, glob_date_time.minutes, glob_date_time.seconds);
	lv_label_set_text_fmt(time, "%.02d:%.02d:%.02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

static void drawDate(const struct tm *timeinfo)
{
	lv_obj_t *date = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 1);
//	lv_label_set_text_fmt(date, "%02d.%.02d.%.04d", glob_date_time.date, glob_date_time.month, glob_date_time.year + 1900);
	lv_label_set_text_fmt(date, "%02d.%.02d.%.04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

static void drawTemperature()
{
	lv_obj_t *t = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 2);
	lv_label_set_text_fmt(t, "+25.65 C°");
	lv_obj_set_style_text_color(t, lv_color_make(0xff, 0, 0), 0);
}

static void drawHumidity()
{
	lv_obj_t *h = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 3);
	lv_label_set_text_fmt(h, "53 %%");
}

static void drawPressure()
{
	lv_obj_t *p = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 4);
	lv_label_set_text_fmt(p, "743 mm.");
}

static void event_handler(lv_event_t * e)
{
	lv_obj_remove_event_cb(e->user_data, event_handler);
	printf("event heandler home page\n");
	menuPageInit();
}

void homePageInit(void)
{
	glob_currentPage = PAGE_NONE;

	lv_obj_t *glob_main_widget = lv_obj_get_child(lv_scr_act(), 1);
	lv_obj_remove_event_cb(glob_main_widget, event_handler);
	lv_obj_clean(glob_main_widget);

	static bool init_style = false;
	if (!init_style)
	{
		for (int i = STYLE_TIME; i <= STYLE_PRESSURE; ++i)
		{
			lv_style_init(&style_arr[i]);
			lv_style_set_text_color(&style_arr[i], lv_color_white());
			lv_style_set_text_font(&style_arr[i], &lv_font_montserrat_48);
		}
	}
	init_style = true;

	lv_obj_t *time_lbl = lv_label_create(glob_main_widget);
	drawLabel(time_lbl, &style_arr[STYLE_TIME], "00:00:00", LV_ALIGN_CENTER, 0, 0);

	lv_obj_t *date_lbl = lv_label_create(glob_main_widget);
	drawLabel(date_lbl, &style_arr[STYLE_DATE], "00.00.0000", LV_ALIGN_BOTTOM_MID, 0, -40);

	lv_obj_t *temperature_lbl = lv_label_create(glob_main_widget);
	drawLabel(temperature_lbl, &style_arr[STYLE_TEMPERATURE], "0.0 C°", LV_ALIGN_TOP_LEFT, 40, 40);

	lv_obj_t *humidity_lbl = lv_label_create(glob_main_widget);
	drawLabel(humidity_lbl, &style_arr[STYLE_HUMIDITY], "53 %", LV_ALIGN_TOP_MID, 0, 40);

	lv_obj_t *pressure_lbl = lv_label_create(glob_main_widget);
	drawLabel(pressure_lbl, &style_arr[STYLE_PRESSURE], "743 mm.", LV_ALIGN_TOP_RIGHT, -40, 40);

	lv_obj_add_event_cb(glob_main_widget, event_handler, LV_EVENT_CLICKED, glob_main_widget);

	glob_currentPage = PAGE_HOME;

	drawHomePage();
}

void drawHomePage(void)
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	drawTime(&timeinfo);
	drawDate(&timeinfo);
	drawTemperature();
	drawHumidity();
	drawPressure();
}
