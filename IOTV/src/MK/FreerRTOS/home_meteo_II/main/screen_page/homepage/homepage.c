/*
 * homepage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "homepage.h"

#define BLOCK1_COUNTER_SEC	5
#define BLOCK2_COUNTER_SEC	60

static uint8_t counter_thp;

extern lv_font_t ubuntu_mono_14;
extern lv_font_t ubuntu_mono_26;
extern lv_font_t ubuntu_mono_48;
extern lv_font_t ubuntu_mono_128;

//static const char *TAG = "homepage";

static lv_obj_t *block_0, *block_1, *block_2;

//Блок 0 - дата и время
lv_obj_t *time_lbl, *date_lbl;

//Блок 1 - локальная температура влажность и давление
lv_obj_t *temperature1_lbl, *humidity1_lbl, *pressure1_lbl, *pressure1_lbl_prefix;

//Блок 2 - погода
lv_obj_t *city_lbl, *last_update_lbl, *cloud_codver_img, *precipitations_img, *wind_direction_img,
*cloud_cover_lbl, *wind_speed_lbl, *wind_direction_lbl, *temperature2_lbl, *humidity2_lbl, *pressure2_lbl,
*apparent_temperature_lbl, *precipitation_lbl, *rain_lbl, *wind_gusts_lbl, *showers_lbl, *snow_lbl;

static lv_timer_t *timer = NULL;

static void drawTime(const struct tm *timeinfo);
static void drawDate(const struct tm *timeinfo);
static bool draw_meteo_data(void);
static void draw_cloud_cover(void);
static void draw_precipitations(void);
static void draw_wind_direction(void);

static lv_obj_t *create_block(lv_obj_t *parent, lv_align_t align, lv_coord_t w, lv_coord_t h);
static void init_block_0(lv_obj_t *parent);
static void init_block_1(lv_obj_t *parent);
static void init_block_2(lv_obj_t *parent);

static void event_handler_block_0(lv_event_t *e); // время/дата
static void event_handler_block_1(lv_event_t *e); // график прогноза
static void event_handler_block_2(lv_event_t *e); // локальные измерения

static void timer_handler(lv_timer_t *timer);

static lv_obj_t *create_block(lv_obj_t *parent, lv_align_t align, lv_coord_t w, lv_coord_t h);

static void drawTime(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(time_lbl, "%.02d:%.02d", timeinfo->tm_hour, timeinfo->tm_min);
}

static void drawDate(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(date_lbl, "%s. %02d.%.02d.%.04d", weekday_name_short(timeinfo->tm_wday), timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

static bool draw_meteo_data(void)
{
	if ( !(glob_get_status_reg() & STATUS_METEO_ON))
		return false;

	char *city = NULL;
	get_meteo_config_value(CITY_STR, &city);
	if (city != NULL)
	{
		lv_label_set_text(city_lbl, city);
		free(city);
	}

	const open_meteo_data_t *open_meteo = service_weather_get_current_meteo_data();
	if (open_meteo == NULL)
		return false;

	lv_label_set_text_fmt(wind_speed_lbl, "%.02f м/с", open_meteo->wind_speed);

	if (open_meteo->temperature > 0)
		lv_label_set_text_fmt(temperature2_lbl, "+%.02f°C", open_meteo->temperature);
	else
		lv_label_set_text_fmt(temperature2_lbl, "%.02f°C", open_meteo->temperature);

	lv_label_set_text_fmt(humidity2_lbl, "%d%%", open_meteo->relative_humidity);
	lv_label_set_text_fmt(pressure2_lbl, "%d мм.рт.cт.", (int)open_meteo->surface_pressure);

	if (open_meteo->apparent_temperature > 0)
		lv_label_set_text_fmt(apparent_temperature_lbl, "Температура\nощущается: +%.02f°C", open_meteo->apparent_temperature);
	else
		lv_label_set_text_fmt(apparent_temperature_lbl, "Температура\nощущается: %.02f°C", open_meteo->apparent_temperature);

	lv_label_set_text_fmt(precipitation_lbl, "Осадки: %.02f мм.", open_meteo->precipitation);
	lv_label_set_text_fmt(rain_lbl, "Дождь: %.02f мм.", open_meteo->rain);
	lv_label_set_text_fmt(wind_gusts_lbl, "Порывы\nветра: %.02f м/c", open_meteo->wind_gusts);
	lv_label_set_text_fmt(showers_lbl, "Ливни: %.02f мм.", open_meteo->showers);
	lv_label_set_text_fmt(snow_lbl, "Снег: %.02f cм.", open_meteo->snowfall);
	lv_label_set_text_fmt(cloud_cover_lbl, "Облачность: %d%%", open_meteo->cloud_cover);
	lv_label_set_text_fmt(wind_direction_lbl, "Ветер: %d°", open_meteo->wind_direction);

	struct tm timeinfo;
	localtime_r(&open_meteo->time, &timeinfo);
	lv_label_set_text_fmt(last_update_lbl, "Данные на:\n"
			"%.02d.%.02d.%.02d\n"
			"%.02d:%.02d",
			timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
			timeinfo.tm_hour, timeinfo.tm_min);

	draw_cloud_cover();
	draw_precipitations();
	draw_wind_direction();

	return true;
}

static void draw_cloud_cover(void)
{
	const open_meteo_data_t *open_meteo = service_weather_get_current_meteo_data();
	if (open_meteo == NULL)
		return;

	if (open_meteo->cloud_cover <= 25)
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_1);
	else if (open_meteo->cloud_cover <= 50)
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_2);
	else if (open_meteo->cloud_cover <= 75)
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_3);
	else
		lv_img_set_src(cloud_codver_img, CLOUD_COVER_4);
}

static void draw_precipitations(void)
{
	const open_meteo_data_t *open_meteo = service_weather_get_current_meteo_data();
	if (open_meteo == NULL)
		return;

	if ( (open_meteo->rain > 0 || open_meteo->showers > 0) && open_meteo->snowfall > 0)
	{
		lv_img_set_src(precipitations_img, SNOW_AND_RAIN);
		return;
	}

	if (open_meteo->rain > 0)
	{
		if (open_meteo->rain <= 5)
			lv_img_set_src(precipitations_img, RAIN_1);
		else if (open_meteo->rain <= 20)
			lv_img_set_src(precipitations_img, RAIN_2);
		else if (open_meteo->rain <= 40)
			lv_img_set_src(precipitations_img, RAIN_3);
		else
			lv_img_set_src(precipitations_img, RAIN_4);
	}

	if (open_meteo->snowfall >= 0.20)
		lv_img_set_src(precipitations_img, SNOW_4);
	else if (open_meteo->snowfall >= 0.15)
		lv_img_set_src(precipitations_img, SNOW_3);
	else if (open_meteo->snowfall >= 0.10)
		lv_img_set_src(precipitations_img, SNOW_2);
	else if (open_meteo->snowfall > 0)
		lv_img_set_src(precipitations_img, SNOW_1);
	else
		lv_img_set_src(precipitations_img, PRECIPITATION0);
}

static void draw_wind_direction(void)
{
	const open_meteo_data_t *open_meteo = service_weather_get_current_meteo_data();
	if (open_meteo == NULL)
		return;

	if (open_meteo->wind_direction >= 338  || (open_meteo->wind_direction <= 22) )
		lv_img_set_src(wind_direction_img, WIND_0);
	else if (open_meteo->wind_direction >= 23 && open_meteo->wind_direction <= 67)
		lv_img_set_src(wind_direction_img, WIND_45);
	else if (open_meteo->wind_direction >= 68 && open_meteo->wind_direction <= 112)
		lv_img_set_src(wind_direction_img, WIND_90);
	else if (open_meteo->wind_direction >= 113 && open_meteo->wind_direction <= 167)
		lv_img_set_src(wind_direction_img, WIND_135);
	else if (open_meteo->wind_direction >= 168 && open_meteo->wind_direction <= 202)
		lv_img_set_src(wind_direction_img, WIND_180);
	else if (open_meteo->wind_direction >= 203 && open_meteo->wind_direction <= 247)
		lv_img_set_src(wind_direction_img, WIND_225);
	else if (open_meteo->wind_direction >= 248 && open_meteo->wind_direction <= 292)
		lv_img_set_src(wind_direction_img, WIND_270);
	else if (open_meteo->wind_direction >= 293 && open_meteo->wind_direction <= 337)
		lv_img_set_src(wind_direction_img, WIND_315);
	else
		lv_img_set_src(wind_direction_img, WIND_NO);
}

static void event_handler_block_0(lv_event_t * e)
{
	datetime1_page_init();
}

static void event_handler_block_1(lv_event_t * e)
{
	home_meteo_page_init();
}

static void event_handler_block_2(lv_event_t * e)
{
	meteo_chart_page_init();
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

	drawTime(&timeinfo);
	drawDate(&timeinfo);

	if (++counter_thp > BLOCK1_COUNTER_SEC) // раз в 5 секунд
	{
		counter_thp = 0;

		const struct THP *thp = BME280_service_get_value();
		lv_label_set_text_fmt(temperature1_lbl, "%+.2f°C", thp->temperature);
		lv_label_set_text_fmt(humidity1_lbl, "%.2f %%", thp->humidity);
		lv_label_set_text_fmt(pressure1_lbl, "%.0f", thp->pressure);
		lv_obj_align_to(pressure1_lbl_prefix, pressure1_lbl, LV_ALIGN_OUT_RIGHT_MID, 25, 7);
	}

//	static uint8_t meteo_thp = BLOCK2_COUNTER_SEC;
//	if (++meteo_thp > BLOCK2_COUNTER_SEC) // раз в 1 минуту
//	{
//		meteo_thp = 0;
		draw_meteo_data();
//		if (!draw_meteo_data())
//			meteo_thp = BLOCK2_COUNTER_SEC;
//	}
}

// Блок время/дата
static void init_block_0(lv_obj_t *parent)
{
	block_0 = create_block(parent, LV_ALIGN_BOTTOM_LEFT, LCD_H_RES / 2, (LCD_V_RES - LCD_PANEL_STATUS_H) / 2);

	time_lbl = create_lbl_obj(block_0, "00:00", block_0, LV_ALIGN_DEFAULT, 0, 0, lv_color_white(), &ubuntu_mono_128);
	lv_obj_align(time_lbl, LV_ALIGN_CENTER, 0, -30);
	date_lbl = create_lbl_obj(block_0, "пн. 00.00.0000", time_lbl, LV_ALIGN_DEFAULT, 0, 0, lv_color_white(), &ubuntu_mono_48);
	lv_obj_align(date_lbl, LV_ALIGN_BOTTOM_MID, 0, -15);

	lv_obj_add_event_cb(block_0, event_handler_block_0, LV_EVENT_CLICKED, 0);
}

// Блок температура/влажность/давление локальная
static void init_block_1(lv_obj_t *parent)
{
	counter_thp = BLOCK1_COUNTER_SEC;

	// Блок
	block_1 = create_block(parent, LV_ALIGN_TOP_LEFT, LCD_H_RES / 2, (LCD_V_RES - LCD_PANEL_STATUS_H) / 2);

	lv_obj_t *temperature_img = create_img_obj(block_1, TEMPERATURE, block_1, LV_ALIGN_TOP_LEFT, 64, 64, 30, 10);
	temperature1_lbl = create_lbl_obj(block_1, "0.0°C", temperature_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5, lv_color_white(), &ubuntu_mono_48);

	lv_obj_t *humidity_img = create_img_obj(block_1, HUMIDITY, temperature_img, LV_ALIGN_OUT_BOTTOM_RIGHT, 64, 64, 0, 5);
	humidity1_lbl = create_lbl_obj(block_1, "0 %%", humidity_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5, lv_color_white(), &ubuntu_mono_48);

	lv_obj_t *pressure_img = create_img_obj(block_1, PRESSURE, humidity_img, LV_ALIGN_OUT_BOTTOM_RIGHT, 64, 64, 0, 5);
	pressure1_lbl = create_lbl_obj(block_1, "0", pressure_img, LV_ALIGN_OUT_RIGHT_TOP, 40, 5, lv_color_white(), &ubuntu_mono_48);
	pressure1_lbl_prefix = create_lbl_obj(block_1, HG_STR, pressure1_lbl, LV_ALIGN_OUT_RIGHT_MID, 20, 7, lv_color_white(), &ubuntu_mono_26);

	lv_obj_add_event_cb(block_1, event_handler_block_1, LV_EVENT_CLICKED, 0);
}

// Блок метео данные
static void init_block_2(lv_obj_t *parent)
{
	// Блок
	block_2 = create_block(parent, LV_ALIGN_TOP_RIGHT, LCD_H_RES / 2, LCD_V_RES - LCD_PANEL_STATUS_H);

	// Город
	city_lbl = lv_label_create(block_2);
	char *city_name = NULL;
	if (get_meteo_config_value(CITY_STR, &city_name))
	{
		if (city_name != NULL)
		{
			lv_label_set_text_fmt(city_lbl, "%s", city_name);
			free(city_name);
		}
	}
	else
		lv_label_set_text(city_lbl, "-");

	lv_obj_align(city_lbl, LV_ALIGN_TOP_RIGHT, -5, 0);
	lv_obj_set_style_text_color(city_lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(city_lbl, &ubuntu_mono_26, 0);

	last_update_lbl = create_lbl_obj(block_2, "Данные на:\n--.--.----\n--:--", block_2, LV_ALIGN_TOP_RIGHT, -40, 40, lv_color_white(), &ubuntu_mono_14);

	cloud_codver_img = create_img_obj(block_2, CLOUD_COVER_1, block_2, LV_ALIGN_TOP_LEFT, 128, 128, 10, 10);
	precipitations_img = create_img_obj(block_2, PRECIPITATION0, cloud_codver_img, LV_ALIGN_OUT_BOTTOM_MID, 128, 64, 0, 0);

	wind_direction_img = create_img_obj(block_2, WIND_NO, cloud_codver_img, LV_ALIGN_OUT_RIGHT_MID, 128, 128, 20, 20);
	lv_obj_t *wind_img = create_img_obj(block_2, WIND, wind_direction_img, LV_ALIGN_OUT_BOTTOM_LEFT, 32, 32, 20, 20);
	wind_speed_lbl = create_lbl_obj(block_2, "- м/с", wind_img, LV_ALIGN_OUT_RIGHT_MID, 20, -6, lv_color_white(), &ubuntu_mono_26);

	lv_obj_t *temperature_img = create_img_obj(block_2, TEMPERATURE32, precipitations_img, LV_ALIGN_OUT_BOTTOM_MID, 32, 32, 0, 30);
	temperature2_lbl = create_lbl_obj(block_2, "-°C", temperature_img, LV_ALIGN_OUT_RIGHT_MID, 20, -7, lv_color_white(), &ubuntu_mono_26);

	lv_obj_t *humidity_img = create_img_obj(block_2, HUMIDITY32, temperature_img, LV_ALIGN_CENTER, 32, 32, 200, 0);
	humidity2_lbl = create_lbl_obj(block_2, "-%", humidity_img, LV_ALIGN_OUT_RIGHT_MID, 20, -7, lv_color_white(), &ubuntu_mono_26);

	lv_obj_t *pressure_img = create_img_obj(block_2, PRESSURE32, temperature_img, LV_ALIGN_OUT_BOTTOM_MID, 32, 32, 0, 10);
	pressure2_lbl = create_lbl_obj(block_2, "- мм.рт.cт.", pressure_img, LV_ALIGN_OUT_RIGHT_MID, 20, -6, lv_color_white(), &ubuntu_mono_26);

	apparent_temperature_lbl = create_lbl_obj(block_2, "Температура\nощущается: -°C", pressure_img, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20, lv_color_white(), &ubuntu_mono_14);
	precipitation_lbl = create_lbl_obj(block_2, "Осадки: - мм.", apparent_temperature_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	rain_lbl = create_lbl_obj(block_2, "Дождь: 0 мм.", precipitation_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	cloud_cover_lbl = create_lbl_obj(block_2, "Облачность: -%", rain_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	wind_gusts_lbl = create_lbl_obj(block_2, "Порывы\nветра: - м/с", apparent_temperature_lbl, LV_ALIGN_BOTTOM_RIGHT, 70, 0, lv_color_white(), &ubuntu_mono_14);
	showers_lbl = create_lbl_obj(block_2, "Ливни: - мм.", wind_gusts_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	snow_lbl = create_lbl_obj(block_2, "Снег: - см.", showers_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);
	wind_direction_lbl = create_lbl_obj(block_2, "Ветер: -°", snow_lbl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5, lv_color_white(), &ubuntu_mono_14);

	lv_obj_add_event_cb(block_2, event_handler_block_2, LV_EVENT_CLICKED, 0);
}

void homePageInit()
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = home_page_deinit;
	page->title = page_title(MAIN_PAGE_TITLE);
	status_panel_update();

	init_block_0(page->widget);
	init_block_1(page->widget);
	init_block_2(page->widget);

	timer = lv_timer_create(timer_handler, 1000, NULL);
	lv_timer_ready(timer);
}

void home_page_deinit(void)
{
	default_page_deinit();
	lv_timer_del(timer);
}
