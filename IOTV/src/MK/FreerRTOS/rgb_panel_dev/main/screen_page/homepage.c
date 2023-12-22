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

extern lv_font_t ubuntu_mono_14;
extern lv_font_t ubuntu_mono_26;
extern lv_font_t ubuntu_mono_32;
extern lv_font_t ubuntu_mono_48;
extern lv_font_t ubuntu_mono_64;
extern lv_font_t ubuntu_mono_128;

extern uint8_t glob_currentPage;

extern void menuPageInit(void);

#define	STYLE_TIME 			0
#define	STYLE_DATE			1
#define	STYLE_TEMPERATURE	2
#define	STYLE_HUMIDITY		3
#define	STYLE_PRESSURE		4

static lv_style_t style_arr[5];
static char *days_str[7] = {"вс", "пн", "вт", "ср", "чт", "пт", "сб"};

static lv_obj_t *block_0, *block_1, *block_2, *block_3;

static void drawLabel(lv_obj_t *lbl, lv_style_t *style, const char* str, lv_align_t align, lv_coord_t offset_x, lv_coord_t offset_y);
static void drawTime(const struct tm *timeinfo);
static void drawDate(const struct tm *timeinfo);
static void drawTemperature();
static void drawHumidity();
static void drawPressure();

static void init_block_0(lv_obj_t *parent);
static void init_block_1(lv_obj_t *parent);
static void init_block_2(lv_obj_t *parent);
static void init_block_3(lv_obj_t *parent);

static lv_obj_t *create_block(lv_obj_t *parent, lv_align_t align, lv_coord_t w, lv_coord_t h);

static void drawLabel(lv_obj_t *lbl, lv_style_t *style, const char* str, lv_align_t align, lv_coord_t offset_x, lv_coord_t offset_y)
{
	lv_label_set_text_fmt(lbl, "%s", str);
	lv_obj_align(lbl, align, offset_x, offset_y);

	lv_obj_add_style(lbl, style, 0);
}

static void drawTime(const struct tm *timeinfo)
{
	lv_obj_t *time = lv_obj_get_child(block_0, 0);
	lv_label_set_text_fmt(time, "%.02d:%.02d", timeinfo->tm_hour, timeinfo->tm_min);
}

static void drawDate(const struct tm *timeinfo)
{
	lv_obj_t *date = lv_obj_get_child(block_0, 1);
	lv_label_set_text_fmt(date, "%s. %02d.%.02d.%.04d", days_str[timeinfo->tm_wday], timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

static void drawTemperature()
{
	//	lv_obj_t *t = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 2);
	//	lv_label_set_text_fmt(t, "+25.65 C°");
	//	lv_obj_set_style_text_color(t, lv_color_make(0xff, 0, 0), 0);
}

static void drawHumidity()
{
	//	lv_obj_t *h = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 3);
	//	lv_label_set_text_fmt(h, "53 %%");
}

static void drawPressure()
{
	//	lv_obj_t *p = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 4);
	//	lv_label_set_text_fmt(p, "743 мм.");
}

static void event_handler(lv_event_t * e)
{
	//	lv_obj_remove_event_cb(e->user_data, event_handler);
	printf("event heandler home page\n");
	menuPageInit();
}

static lv_obj_t *create_block(lv_obj_t *parent, lv_align_t align, lv_coord_t w, lv_coord_t h)
{
	lv_obj_t *obj = lv_obj_create(parent);
	lv_obj_set_style_pad_all(obj, 0, 0);
	lv_obj_set_size(obj, w, h);
	lv_obj_set_style_border_width(obj, 1, 0);
	lv_obj_set_style_radius(obj, 0, 0);
	lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
	lv_obj_align(obj, align, 0, 0);
	lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

	return obj;
}

// Блок время/дата
static void init_block_0(lv_obj_t *parent)
{
	block_0 = create_block(parent, LV_ALIGN_BOTTOM_LEFT, LCD_H_RES / 2, (LCD_V_RES - LCD_PANEL_STATUS_H) / 2);

	lv_obj_t *obj = lv_label_create(block_0);
	drawLabel(obj, &style_arr[STYLE_TIME], "00:00", LV_ALIGN_CENTER, 0, -30);

	obj = lv_label_create(block_0);
	drawLabel(obj, &style_arr[STYLE_DATE], "00.00.0000", LV_ALIGN_BOTTOM_MID, 0, -0);

	lv_obj_add_event_cb(block_0, event_handler, LV_EVENT_CLICKED, 0);
}

// Блок температура/влажность/давление локальная
static void init_block_1(lv_obj_t *parent)
{
	// Блок
	block_1 = create_block(parent, LV_ALIGN_TOP_LEFT, LCD_H_RES / 2, (LCD_V_RES - LCD_PANEL_STATUS_H) / 2);

	// Температура картинка
	lv_obj_t *temperature_img = lv_img_create(block_1);
	lv_obj_set_size(temperature_img, 64, 64);
	lv_img_set_src(temperature_img, TEMPERATURE);
	lv_obj_align(temperature_img, LV_ALIGN_TOP_LEFT, 30, 10);

	// Температура текст
	lv_obj_t *temperature_lbl = lv_label_create(block_1);
	lv_label_set_text_fmt(temperature_lbl, "%s", "+25.5°C");
	lv_obj_align_to(temperature_lbl, temperature_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5);
	lv_obj_set_style_text_color(temperature_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(temperature_lbl, &ubuntu_mono_48, 0);

	// Влажность картинка
	lv_obj_t *humidity_img = lv_img_create(block_1);
	lv_obj_set_size(humidity_img, 64, 64);
	lv_img_set_src(humidity_img, HUMIDITY);
	lv_obj_align_to(humidity_img, temperature_img, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);

	// Влажность текст
	lv_obj_t *humidity_lbl = lv_label_create(block_1);
	lv_label_set_text_fmt(humidity_lbl, "%s", "55 %");
	lv_obj_align_to(humidity_lbl, humidity_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5);
	lv_obj_set_style_text_color(humidity_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(humidity_lbl, &ubuntu_mono_48, 0);

	// Давление картинка
	lv_obj_t *pressure_img = lv_img_create(block_1);
	lv_obj_set_size(pressure_img, 64, 64);
	lv_img_set_src(pressure_img, PRESSURE);
	lv_obj_align_to(pressure_img, humidity_img, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);

	// Давление текст
	lv_obj_t *pressure_lbl = lv_label_create(block_1);
	lv_label_set_text_fmt(pressure_lbl, "%s", "700");
	lv_obj_align_to(pressure_lbl, pressure_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5);
	lv_obj_set_style_text_color(pressure_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(pressure_lbl, &ubuntu_mono_48, 0);

	// Давление текст мм.рт.ст.
	lv_obj_t *pressure_lbl_2 = lv_label_create(block_1);
	lv_label_set_text(pressure_lbl_2, "мм.рт.ст.");
	lv_obj_align_to(pressure_lbl_2, pressure_lbl, LV_ALIGN_BOTTOM_RIGHT, 20, -20);
	lv_obj_set_style_text_color(pressure_lbl_2, lv_color_white(), 0);
	lv_obj_set_style_text_font(pressure_lbl_2, &ubuntu_mono_26, 0);

	lv_obj_add_event_cb(block_1, event_handler, LV_EVENT_CLICKED, 0);
}

// Блок температура/влажность/давление локальная
static void init_block_2(lv_obj_t *parent)
{
	// Блок
	block_2 = create_block(parent, LV_ALIGN_TOP_RIGHT, LCD_H_RES / 2, LCD_V_RES - LCD_PANEL_STATUS_H);

	// Город
	lv_obj_t *city_lbl = lv_label_create(block_2);
	char *city_name = NULL;
	if (get_meteo_config_value("city", &city_name))
	{
		if (city_name != NULL)
		{
			lv_label_set_text_fmt(city_lbl, "%s", city_name);
			free(city_name);
		}
	}
	else
		lv_label_set_text_fmt(city_lbl, "%s", "Город");

	lv_obj_align(city_lbl, LV_ALIGN_TOP_RIGHT, -0, 0);
	lv_obj_set_style_text_color(city_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(city_lbl, &ubuntu_mono_26, 0);

	// Дата последнего обновления данных погоды
	lv_obj_t *last_update_lbl = lv_label_create(block_2);
	lv_label_set_text(last_update_lbl, "Обновлено:\n01.01.1970\n00:00");
	lv_obj_align_to(last_update_lbl, city_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
	lv_obj_set_style_text_color(last_update_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(last_update_lbl, &ubuntu_mono_14, 0);

	// Облачность картинка
	lv_obj_t *cloud_codver_img = lv_img_create(block_2);
	lv_obj_set_size(cloud_codver_img, 128, 128);
	lv_img_set_src(cloud_codver_img, CLOUD_COVER_2);
	lv_obj_align(cloud_codver_img, LV_ALIGN_TOP_LEFT, 10, 10);

	// Осадки картинка
	lv_obj_t *precipitations_img = lv_img_create(block_2);
	lv_obj_set_size(precipitations_img, 128, 64);
	lv_img_set_src(precipitations_img, SNOW_AND_RAIN);
	lv_obj_align_to(precipitations_img, cloud_codver_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

	// Направление ветера картинка
	lv_obj_t *wind_direction_img = lv_img_create(block_2);
	lv_obj_set_size(wind_direction_img, 128, 128);
	lv_img_set_src(wind_direction_img, WIND_225);
	lv_obj_align_to(wind_direction_img, cloud_codver_img, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

	// Ветер картинка
	lv_obj_t *wind_img = lv_img_create(block_2);
	lv_obj_set_size(wind_img, 32, 32);
	lv_img_set_src(wind_img, WIND);
	lv_obj_align_to(wind_img, wind_direction_img, LV_ALIGN_OUT_BOTTOM_LEFT, 20, 20);

	// Ветер текст
	lv_obj_t *wind_lbl = lv_label_create(block_2);
	lv_label_set_text(wind_lbl, "3.61 м/с");
	lv_obj_align_to(wind_lbl, wind_img, LV_ALIGN_OUT_RIGHT_MID, 20, -6);
	lv_obj_set_style_text_color(wind_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(wind_lbl, &ubuntu_mono_26, 0);

	// Температура картинка
	lv_obj_t *temperature_img = lv_img_create(block_2);
	lv_obj_set_size(temperature_img, 32, 32);
	lv_img_set_src(temperature_img, TEMPERATURE32);
	lv_obj_align_to(temperature_img, precipitations_img, LV_ALIGN_OUT_BOTTOM_MID, -46, 20);

	// Температура текст
	lv_obj_t *temperature_lbl = lv_label_create(block_2);
	lv_label_set_text(temperature_lbl, "+20.5°C");
	lv_obj_align_to(temperature_lbl, temperature_img, LV_ALIGN_OUT_RIGHT_MID, 20, -7);
	lv_obj_set_style_text_color(temperature_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(temperature_lbl, &ubuntu_mono_26, 0);

	// Влажность картинка
	lv_obj_t *humidity_img = lv_img_create(block_2);
	lv_obj_set_size(humidity_img, 32, 32);
	lv_img_set_src(humidity_img, HUMIDITY32);
	lv_obj_align_to(humidity_img, temperature_img, LV_ALIGN_CENTER, 200, 0);

	// Влажность текст
	lv_obj_t *humidity_lbl = lv_label_create(block_2);
	lv_label_set_text(humidity_lbl, "75%");
	lv_obj_align_to(humidity_lbl, humidity_img, LV_ALIGN_OUT_RIGHT_MID, 20, -7);
	lv_obj_set_style_text_color(humidity_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(humidity_lbl, &ubuntu_mono_26, 0);

	// Давление картинка
	lv_obj_t *pressure_img = lv_img_create(block_2);
	lv_obj_set_size(pressure_img, 32, 32);
	lv_img_set_src(pressure_img, PRESSURE32);
	lv_obj_align_to(pressure_img, temperature_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

	// Давление текст
	lv_obj_t *pressure_lbl = lv_label_create(block_2);
	lv_label_set_text(pressure_lbl, "685 мм.рт.мт.");
	lv_obj_align_to(pressure_lbl, pressure_img, LV_ALIGN_OUT_RIGHT_MID, 20, -6);
	lv_obj_set_style_text_color(pressure_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(pressure_lbl, &ubuntu_mono_26, 0);

	// Температура ощущается текст
	lv_obj_t *apparent_temperature_lbl = lv_label_create(block_2);
	lv_label_set_text(apparent_temperature_lbl, "Температура\nощущается: +20.5°C");
	lv_obj_align_to(apparent_temperature_lbl, pressure_img, LV_ALIGN_OUT_BOTTOM_LEFT, 40, 30);
	lv_obj_set_style_text_color(apparent_temperature_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(apparent_temperature_lbl, &ubuntu_mono_14, 0);

	// Осадки текст
	lv_obj_t *precipitation_lbl = lv_label_create(block_2);
	lv_label_set_text(precipitation_lbl, "Осадки: 0 мм.");
	lv_obj_align_to(precipitation_lbl, apparent_temperature_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
	lv_obj_set_style_text_color(precipitation_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(precipitation_lbl, &ubuntu_mono_14, 0);

	// Дождь текст
	lv_obj_t *rain_lbl = lv_label_create(block_2);
	lv_label_set_text(rain_lbl, "Дождь: 0 мм.");
	lv_obj_align_to(rain_lbl, precipitation_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
	lv_obj_set_style_text_color(rain_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(rain_lbl, &ubuntu_mono_14, 0);

	// Порывы ветра текст
	lv_obj_t *wind_gusts = lv_label_create(block_2);
	lv_label_set_text(wind_gusts, "Порывы\nветра: 0 м/с");
	lv_obj_align_to(wind_gusts, apparent_temperature_lbl, LV_ALIGN_BOTTOM_RIGHT, 50, 0);
	lv_obj_set_style_text_color(wind_gusts, lv_color_white(), 0);
	lv_obj_set_style_text_font(wind_gusts, &ubuntu_mono_14, 0);

	// Ливни текст
	lv_obj_t *showers_lbl = lv_label_create(block_2);
	lv_label_set_text(showers_lbl, "Ливни: 0 мм.");
	lv_obj_align_to(showers_lbl, wind_gusts, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
	lv_obj_set_style_text_color(showers_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(showers_lbl, &ubuntu_mono_14, 0);

	// Снег текст
	lv_obj_t *snow_lbl = lv_label_create(block_2);
	lv_label_set_text(snow_lbl, "Снег: 0 см.");
	lv_obj_align_to(snow_lbl, showers_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
	lv_obj_set_style_text_color(snow_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(snow_lbl, &ubuntu_mono_14, 0);

	lv_obj_add_event_cb(block_2, event_handler, LV_EVENT_CLICKED, 0);
}

static void init_block_3(lv_obj_t *parent)
{

}

void homePageInit(void)
{
	glob_currentPage = PAGE_NONE;

	lv_obj_t *main_widget = lv_obj_get_child(lv_scr_act(), 1);
	lv_obj_remove_event_cb(main_widget, event_handler);
	lv_obj_clean(main_widget);

	static bool init_style = false;
	if (!init_style)
	{
		for (int i = STYLE_TIME; i <= STYLE_PRESSURE; ++i)
		{
			lv_style_init(&style_arr[i]);
			lv_style_set_text_color(&style_arr[i], lv_color_white());
			lv_style_set_text_font(&style_arr[i], &ubuntu_mono_48);
		}
		lv_style_set_text_font(&style_arr[STYLE_TIME], &ubuntu_mono_128);
	}
	init_style = true;

	init_block_0(main_widget);
	init_block_1(main_widget);
	init_block_2(main_widget);

	//	block_2 = create_block(main_widget, LV_ALIGN_TOP_RIGHT, LCD_H_RES / 2, (LCD_V_RES - LCD_PANEL_STATUS_H) / 2);
	//
	//	block_3 = create_block(main_widget, LV_ALIGN_BOTTOM_RIGHT, LCD_H_RES / 2, (LCD_V_RES - LCD_PANEL_STATUS_H) / 2);
	//
	//	lv_obj_add_event_cb(block_0, event_handler, LV_EVENT_CLICKED, 0);
	//	lv_obj_add_event_cb(block_1, event_handler, LV_EVENT_CLICKED, 0);
	//	lv_obj_add_event_cb(block_2, event_handler, LV_EVENT_CLICKED, 0);
	//	lv_obj_add_event_cb(block_3, event_handler, LV_EVENT_CLICKED, 0);
	//	lv_obj_add_event_cb(main_widget, event_handler, LV_EVENT_CLICKED, main_widget);

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
