/*
 * homepage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "homepage.h"

extern lv_font_t ubuntu_mono_14;
extern lv_font_t ubuntu_mono_26;
extern lv_font_t ubuntu_mono_32;
extern lv_font_t ubuntu_mono_48;
extern lv_font_t ubuntu_mono_64;
extern lv_font_t ubuntu_mono_128;

extern uint8_t glob_currentPage;
extern uint32_t glob_status_reg;
extern open_meteo_data_t glob_open_meteo;

extern void menuPageInit(void);

static lv_obj_t *block_0, *block_1, *block_2;

//Блок 0
lv_obj_t *time_lbl, *date_lbl;
//Блок 1
lv_obj_t *temperature1_lbl, *humidity_lbl, *pressure_lbl;
//Блок 2
lv_obj_t *city_lbl, *last_update_lbl, *cloud_codver_img, *precipitations_img, *wind_direction_img,
*cloud_cover_lbl, *wind_speed_lbl, *wind_direction_lbl, *temperature2_lbl, *humidity_lbl, *pressure_lbl,
*apparent_temperature_lbl, *precipitation_lbl, *rain_lbl, *wind_gusts_lbl, *showers_lbl, *snow_lbl;

static void drawTime(const struct tm *timeinfo);
static void drawDate(const struct tm *timeinfo);
static void drawTemperature(void);
static void drawHumidity(void);
static void drawPressure(void);
static void draw_meteo_data(void);
static void draw_cloud_cover(void);
static void draw_precipitations(void);
static void draw_wind_direction(void);

static lv_obj_t *create_block(lv_obj_t *parent, lv_align_t align, lv_coord_t w, lv_coord_t h);
static void init_block_0(lv_obj_t *parent);
static void init_block_1(lv_obj_t *parent);
static void init_block_2(lv_obj_t *parent);

static lv_obj_t *create_block(lv_obj_t *parent, lv_align_t align, lv_coord_t w, lv_coord_t h);

static void drawTime(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(time_lbl, "%.02d:%.02d", timeinfo->tm_hour, timeinfo->tm_min);
}

static void drawDate(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(date_lbl, "%s. %02d.%.02d.%.04d", weekday_name_short(timeinfo->tm_wday), timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

static void drawTemperature(void)
{
	//	lv_obj_t *t = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 2);
	//	lv_label_set_text_fmt(t, "+25.65 C°");
	//	lv_obj_set_style_text_color(t, lv_color_make(0xff, 0, 0), 0);
}

static void drawHumidity(void)
{
	//	lv_obj_t *h = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 3);
	//	lv_label_set_text_fmt(h, "53 %%");
}

static void drawPressure(void)
{
	//	lv_obj_t *p = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 1), 4);
	//	lv_label_set_text_fmt(p, "743 мм.");
}

static void draw_meteo_data(void)
{
	if ( !(glob_status_reg & STATUS_METEO_ON))
		return;

	if (glob_open_meteo.city_name != NULL)
		lv_label_set_text(city_lbl, glob_open_meteo.city_name);

	lv_label_set_text_fmt(wind_speed_lbl, "%.02f м/с", glob_open_meteo.wind_speed);

	if (glob_open_meteo.temperature > 0)
		lv_label_set_text_fmt(temperature2_lbl, "+%.02f°C", glob_open_meteo.temperature);
	else
		lv_label_set_text_fmt(temperature2_lbl, "%.02f°C", glob_open_meteo.temperature);

	lv_label_set_text_fmt(humidity_lbl, "%d%%", glob_open_meteo.relative_humidity);
	lv_label_set_text_fmt(pressure_lbl, "%d мм.рт.cт.", (int)glob_open_meteo.surface_pressure);

	if (glob_open_meteo.apparent_temperature > 0)
		lv_label_set_text_fmt(apparent_temperature_lbl, "Температура\nощущается: +%.02f°C", glob_open_meteo.apparent_temperature);
	else
		lv_label_set_text_fmt(apparent_temperature_lbl, "Температура\nощущается: %.02f°C", glob_open_meteo.apparent_temperature);

	lv_label_set_text_fmt(precipitation_lbl, "Осадки: %.02f мм.", glob_open_meteo.precipitation);
	lv_label_set_text_fmt(rain_lbl, "Дождь: %.02f мм.", glob_open_meteo.rain);
	lv_label_set_text_fmt(wind_gusts_lbl, "Порывы\nветра: %.02f м/c", glob_open_meteo.wind_gusts);
	lv_label_set_text_fmt(showers_lbl, "Ливни: %.02f мм.", glob_open_meteo.showers);
	lv_label_set_text_fmt(snow_lbl, "Снег: %.02f cм.", glob_open_meteo.snowfall);
	lv_label_set_text_fmt(cloud_cover_lbl, "Облачность: %d%%", glob_open_meteo.cloud_cover);
	lv_label_set_text_fmt(wind_direction_lbl, "Ветер: %d°", glob_open_meteo.wind_direction);

	struct tm timeinfo;
	localtime_r(&glob_open_meteo.time, &timeinfo);
	lv_label_set_text_fmt(last_update_lbl, "Обновлено:\n"
			"%.02d.%.02d.%.02d\n"
			"%.02d:%.02d",
			timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
			timeinfo.tm_hour, timeinfo.tm_min);

	draw_cloud_cover();
	draw_precipitations();
	draw_wind_direction();
}

static void draw_cloud_cover(void)
{
	if (glob_open_meteo.cloud_cover <= 25)
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_1);
	else if (glob_open_meteo.cloud_cover <= 50)
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_2);
	else if (glob_open_meteo.cloud_cover <= 75)
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_3);
	else
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_4);
}

static void draw_precipitations(void)
{
	if ( (glob_open_meteo.rain > 0 || glob_open_meteo.showers > 0) && glob_open_meteo.snowfall > 0)
	{
		lv_img_set_src(precipitations_img, SNOW_AND_RAIN);
		return;
	}

	if (glob_open_meteo.rain > 0)
	{
		if (glob_open_meteo.rain <= 5)
			lv_img_set_src(precipitations_img, RAIN_1);
		else if (glob_open_meteo.rain <= 20)
			lv_img_set_src(precipitations_img, RAIN_2);
		else if (glob_open_meteo.rain <= 40)
			lv_img_set_src(precipitations_img, RAIN_3);
		else
			lv_img_set_src(precipitations_img, RAIN_4);
	}

	if (glob_open_meteo.snowfall >= 0.20)
		lv_img_set_src(precipitations_img, SNOW_4);
	else if (glob_open_meteo.snowfall >= 0.15)
		lv_img_set_src(precipitations_img, SNOW_3);
	else if (glob_open_meteo.snowfall >= 0.10)
		lv_img_set_src(precipitations_img, SNOW_2);
	else if (glob_open_meteo.snowfall > 0)
		lv_img_set_src(precipitations_img, SNOW_1);
	else
		lv_img_set_src(precipitations_img, PRECIPITATION0);
}

static void draw_wind_direction(void)
{
	if (glob_open_meteo.wind_direction >= 338  || (glob_open_meteo.wind_direction <= 22) )
		lv_img_set_src(wind_direction_img, WIND_0);
	else if (glob_open_meteo.wind_direction >= 23 && glob_open_meteo.wind_direction <= 67)
		lv_img_set_src(wind_direction_img, WIND_45);
	else if (glob_open_meteo.wind_direction >= 68 && glob_open_meteo.wind_direction <= 112)
		lv_img_set_src(wind_direction_img, WIND_90);
	else if (glob_open_meteo.wind_direction >= 113 && glob_open_meteo.wind_direction <= 167)
		lv_img_set_src(wind_direction_img, WIND_135);
	else if (glob_open_meteo.wind_direction >= 168 && glob_open_meteo.wind_direction <= 202)
		lv_img_set_src(wind_direction_img, WIND_180);
	else if (glob_open_meteo.wind_direction >= 203 && glob_open_meteo.wind_direction <= 247)
		lv_img_set_src(wind_direction_img, WIND_225);
	else if (glob_open_meteo.wind_direction >= 248 && glob_open_meteo.wind_direction <= 292)
		lv_img_set_src(wind_direction_img, WIND_270);
	else if (glob_open_meteo.wind_direction >= 293 && glob_open_meteo.wind_direction <= 337)
		lv_img_set_src(wind_direction_img, WIND_315);
	else
		lv_img_set_src(wind_direction_img, WIND_NO);
}

static void event_handler(lv_event_t * e)
{
	//	lv_obj_remove_event_cb(e->user_data, event_handler);
	printf("event heandler home page\n");
	menuPageInit();
}

static void block0_handler(lv_event_t * e)
{
	//	lv_obj_remove_event_cb(e->user_data, event_handler);
	printf("event heandler home page\n");
	datetime1_page_init();
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

	time_lbl = create_lbl_obj(block_0, "00:00", block_0, LV_ALIGN_DEFAULT, 0, 0, lv_color_white(), &ubuntu_mono_128);
	lv_obj_align(time_lbl, LV_ALIGN_CENTER, 0, -30);
	date_lbl = create_lbl_obj(block_0, "пн. 00.00.0000", time_lbl, LV_ALIGN_DEFAULT, 0, 0, lv_color_white(), &ubuntu_mono_48);
	lv_obj_align(date_lbl, LV_ALIGN_BOTTOM_MID, 0, -15);

	lv_obj_add_event_cb(block_0, block0_handler, LV_EVENT_CLICKED, 0);
}

// Блок температура/влажность/давление локальная
static void init_block_1(lv_obj_t *parent)
{
	// Блок
	block_1 = create_block(parent, LV_ALIGN_TOP_LEFT, LCD_H_RES / 2, (LCD_V_RES - LCD_PANEL_STATUS_H) / 2);

	lv_obj_t *temperature_img = create_img_obj(block_1, TEMPERATURE, block_1, LV_ALIGN_TOP_LEFT, 64, 64, 30, 10);
	temperature1_lbl = create_lbl_obj(block_1, "+25.5°C", temperature_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5, lv_color_white(), &ubuntu_mono_48);

	lv_obj_t *humidity_img = create_img_obj(block_1, HUMIDITY, temperature_img, LV_ALIGN_OUT_BOTTOM_RIGHT, 64, 64, 0, 5);
	humidity_lbl = create_lbl_obj(block_1, "55 %", humidity_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5, lv_color_white(), &ubuntu_mono_48);

	lv_obj_t *pressure_img = create_img_obj(block_1, PRESSURE, humidity_img, LV_ALIGN_OUT_BOTTOM_RIGHT, 64, 64, 0, 5);
	pressure_lbl = create_lbl_obj(block_1, "700", pressure_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5, lv_color_white(), &ubuntu_mono_48);
	create_lbl_obj(block_1, "мм.рт.ст.", pressure_lbl, LV_ALIGN_BOTTOM_RIGHT, 20, -20, lv_color_white(), &ubuntu_mono_26);

	lv_obj_add_event_cb(block_1, event_handler, LV_EVENT_CLICKED, 0);
}

// Блок температура/влажность/давление локальная
static void init_block_2(lv_obj_t *parent)
{
	// Блок
	block_2 = create_block(parent, LV_ALIGN_TOP_RIGHT, LCD_H_RES / 2, LCD_V_RES - LCD_PANEL_STATUS_H);

	// Город
	city_lbl = lv_label_create(block_2);
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

	lv_obj_align(city_lbl, LV_ALIGN_TOP_RIGHT, -5, 0);
	lv_obj_set_style_text_color(city_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(city_lbl, &ubuntu_mono_26, 0);

	last_update_lbl = create_lbl_obj(block_2, "Обновлено:\n--.--.----\n--:--", block_2, LV_ALIGN_TOP_RIGHT, -40, 40, lv_color_white(), &ubuntu_mono_14);

	cloud_codver_img = create_img_obj(block_2, CLOUD_COVER_1, block_2, LV_ALIGN_TOP_LEFT, 128, 128, 10, 10);
	precipitations_img = create_img_obj(block_2, PRECIPITATION0, cloud_codver_img, LV_ALIGN_OUT_BOTTOM_MID, 128, 64, 0, 0);

	wind_direction_img = create_img_obj(block_2, WIND_NO, cloud_codver_img, LV_ALIGN_OUT_RIGHT_MID, 128, 128, 20, 20);
	lv_obj_t *wind_img = create_img_obj(block_2, WIND, wind_direction_img, LV_ALIGN_OUT_BOTTOM_LEFT, 32, 32, 20, 20);
	wind_speed_lbl = create_lbl_obj(block_2, "- м/с", wind_img, LV_ALIGN_OUT_RIGHT_MID, 20, -6, lv_color_white(), &ubuntu_mono_26);

	lv_obj_t *temperature_img = create_img_obj(block_2, TEMPERATURE32, precipitations_img, LV_ALIGN_OUT_BOTTOM_MID, 32, 32, 0, 30);
	temperature2_lbl = create_lbl_obj(block_2, "-°C", temperature_img, LV_ALIGN_OUT_RIGHT_MID, 20, -7, lv_color_white(), &ubuntu_mono_26);

	lv_obj_t *humidity_img = create_img_obj(block_2, HUMIDITY32, temperature_img, LV_ALIGN_CENTER, 32, 32, 200, 0);
	humidity_lbl = create_lbl_obj(block_2, "-%", humidity_img, LV_ALIGN_OUT_RIGHT_MID, 20, -7, lv_color_white(), &ubuntu_mono_26);

	lv_obj_t *pressure_img = create_img_obj(block_2, PRESSURE32, temperature_img, LV_ALIGN_OUT_BOTTOM_MID, 32, 32, 0, 10);
	pressure_lbl = create_lbl_obj(block_2, "- мм.рт.cт.", pressure_img, LV_ALIGN_OUT_RIGHT_MID, 20, -6, lv_color_white(), &ubuntu_mono_26);

	apparent_temperature_lbl = create_lbl_obj(block_2, "Температура\nощущается: -°C", pressure_img, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20, lv_color_white(), &ubuntu_mono_14);
	precipitation_lbl = create_lbl_obj(block_2, "Осадки: - мм.", apparent_temperature_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	rain_lbl = create_lbl_obj(block_2, "Дождь: 0 мм.", precipitation_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	cloud_cover_lbl = create_lbl_obj(block_2, "Облачность: -%", rain_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	wind_gusts_lbl = create_lbl_obj(block_2, "Порывы\nветра: - м/с", apparent_temperature_lbl, LV_ALIGN_BOTTOM_RIGHT, 70, 0, lv_color_white(), &ubuntu_mono_14);
	showers_lbl = create_lbl_obj(block_2, "Ливни: - мм.", wind_gusts_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	snow_lbl = create_lbl_obj(block_2, "Снег: - см.", showers_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	wind_direction_lbl = create_lbl_obj(block_2, "Ветер: -°", snow_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);

	lv_obj_add_event_cb(block_2, event_handler, LV_EVENT_CLICKED, 0);
}

void homePageInit(void)
{
	glob_currentPage = PAGE_NONE;

	lv_obj_t *main_widget = lv_obj_get_child(lv_scr_act(), 1);
//	lv_obj_remove_event_cb(main_widget, event_handler);
	lv_obj_clean(main_widget);

	init_block_0(main_widget);
	init_block_1(main_widget);
	init_block_2(main_widget);

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
	draw_meteo_data();
}
