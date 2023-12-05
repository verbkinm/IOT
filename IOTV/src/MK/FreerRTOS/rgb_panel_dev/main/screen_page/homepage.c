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

static lv_obj_t *time_lbl = NULL;
static lv_obj_t *date_lbl;
static lv_obj_t *temperature_lbl;
static lv_obj_t *humidity_lbl;
static lv_obj_t *pressure_lbl;

static void drawLabel(lv_obj_t *lbl, lv_style_t *style, const lv_font_t *font, const char* str, lv_align_t align, lv_coord_t offset_x, lv_coord_t offset_y);
static void drawTime();
static void drawDate();
static void drawTemperature();
static void drawHumidity();
static void drawPressure();

static void drawLabel(lv_obj_t *lbl, lv_style_t *style, const lv_font_t *font, const char* str, lv_align_t align, lv_coord_t offset_x, lv_coord_t offset_y)
{
    lv_label_set_text_fmt(lbl, "%s", str);
    lv_obj_align(lbl, align, offset_x, offset_y);

    lv_style_init(style);
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_font(style, font);
    lv_obj_add_style(lbl, style, 0);
}

static void drawTime()
{
	++glob_date_time.seconds;
    lv_label_set_text_fmt(time_lbl, "%.02d:%.02d:%.02d", glob_date_time.hour, glob_date_time.minutes, glob_date_time.seconds);
}

static void drawDate()
{
    lv_label_set_text_fmt(date_lbl, "%02d.%.02d.%.04d", glob_date_time.date, glob_date_time.month, glob_date_time.year + 1900);
}

static void drawTemperature()
{
    lv_label_set_text_fmt(temperature_lbl, "+25.65 C°");
    lv_obj_set_style_text_color(temperature_lbl, lv_color_make(0xff, 0, 0), 0);
}

static void drawHumidity()
{
    lv_label_set_text_fmt(humidity_lbl, "53 %%");
}

static void drawPressure()
{
    lv_label_set_text_fmt(pressure_lbl, "743 mm.");
}

static void event_handler(lv_event_t * e)
{
    menuPageInit();
}

void homePageInit(void)
{
    lv_obj_t *scr = lv_obj_create(NULL);

    static lv_style_t time_style, date_style, temperature_style, humidity_style, pressure_style;
    time_lbl = lv_label_create(scr);
    drawLabel(time_lbl, &time_style, &lv_font_montserrat_48, "00:00:00", LV_ALIGN_CENTER, 0, 0);

    date_lbl = lv_label_create(scr);
    drawLabel(date_lbl, &date_style, &lv_font_montserrat_48, "00.00.0000", LV_ALIGN_BOTTOM_MID, 0, -40);

    temperature_lbl = lv_label_create(scr);
    drawLabel(temperature_lbl, &temperature_style, &lv_font_montserrat_48, "0.0 C°", LV_ALIGN_TOP_LEFT, 40, 40);

    humidity_lbl = lv_label_create(scr);
    drawLabel(humidity_lbl, &humidity_style, &lv_font_montserrat_48, "53 %°", LV_ALIGN_TOP_MID, 0, 40);

    pressure_lbl = lv_label_create(scr);
    drawLabel(pressure_lbl, &pressure_style, &lv_font_montserrat_48, "743 mm.", LV_ALIGN_TOP_RIGHT, -40, 40);

    lv_obj_add_style(scr, screenStyleDefault(), 0);

    lv_obj_add_event_cb(scr, event_handler, LV_EVENT_CLICKED, NULL);

//    lv_scr_load(scr);
    lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_NONE, 0, 0, 1);
    glob_currentPage = PAGE_HOME;
    drawHomePage();
}

void drawHomePage(void)
{
    drawTime();
    drawDate();
    drawTemperature();
    drawHumidity();
    drawPressure();
}
