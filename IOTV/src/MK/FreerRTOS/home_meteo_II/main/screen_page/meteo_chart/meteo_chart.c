/*
 * meteo_char.c
 *
 *  Created on: 9 янв. 2024 г.
 *      Author: user
 */

#include "meteo_chart.h"

#define TICK_INTERVAL 	6
#define SERIES_COUNT 	12

extern lv_font_t ubuntu_mono_14;

static const char *TAG = "METEO_CHART";
static lv_obj_t *chart;
static char *words[SERIES_COUNT] = {"Температура", "Температура\nощущается", "Давление", "Осдаки", "Дождь", "Ливни", "Снег", "Скорость\nветра", "Порывы\nветра", "Влажность", "Облачность", "Направление\nветра"};

static void check_meteo_chart_file(void);

static lv_obj_t *create_ser_chkbox(lv_obj_t *parent, lv_chart_series_t *ser, const char *txt);
static lv_obj_t *create_line_values_range(lv_obj_t *parent, float min, float max, const lv_obj_t *align_to, lv_coord_t x_ofs, lv_coord_t y_ofs, const char *postfix,
		const char *fmt0, const char *fmt1, const char *fmt2, const char *fmt3, const char *fmt4);
static void create_lines_value_range(lv_obj_t *parent, const open_meteo_data_t *meteo_min, const open_meteo_data_t *meteo_max);

static void draw_event_cb(lv_event_t *e);
static void show_hide_ser_event_cb(lv_event_t *e);

static void check_meteo_chart_file(void)
{
	cJSON *root = cJSON_CreateObject();

	cJSON *visible = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "visible", visible);

	cJSON *objs[SERIES_COUNT];
	for (int i = 0; i < SERIES_COUNT; ++i)
	{
		objs[i] = cJSON_CreateString("1");
		cJSON_AddItemToObject(visible, words[i], objs[i]);
	}

	for (int i = 0; i < SERIES_COUNT; ++i)
		get_meteo_chart_config_value(words[i], &objs[i]->valuestring);

	FILE *file = fopen(METEO_CHART_PATH, "w");
	if (file == NULL)
		printf("cant write \"%s\" file!\n", METEO_CHART_PATH);
	else
	{
		fprintf(file, "%s", cJSON_Print(root));
		fclose(file);
	}

	cJSON_Delete(root);
}

static void draw_event_cb(lv_event_t * e)
{
	lv_obj_draw_part_dsc_t* dsc = lv_event_get_draw_part_dsc(e); // Get the object of the event object drawing the descriptor pointer
	if (!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL))
		return; // lv_chart_draw_part_tick_label part is not drawn
	if (dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text)
	{
		const open_meteo_data_t *open_meteo = service_weather_get_meteo_data();
		if (open_meteo == NULL || (dsc->value * TICK_INTERVAL) >= OPEN_METEO_WEEK_SIZE)
			return;

		struct tm timeinfo;
		time_t time = open_meteo[dsc->value * TICK_INTERVAL].time;
		localtime_r(&time, &timeinfo);

		//!!! lv_chart.c LV_CHART_LABEL_MAX_TEXT_LENGTH - длина для текста тиков по умолчанию.
		lv_snprintf(dsc->text, 12, "%.02d:%.02d\n%.02d.%.02d",
				timeinfo.tm_hour, timeinfo.tm_min,
				timeinfo.tm_mday, timeinfo.tm_mon + 1);//, timeinfo.tm_year + 1900);
	}
}

static void show_hide_ser_event_cb(lv_event_t *e)
{
	lv_chart_series_t *ser = e->user_data;
	lv_chart_hide_series(chart, ser, !ser->hidden);

	lv_obj_t *chkbox = e->target;
	const char *txt = lv_checkbox_get_text(chkbox);
	set_meteo_chart_config_value(txt, ser->hidden ? "0" : "1");
}

static lv_obj_t *create_ser_chkbox(lv_obj_t *parent, lv_chart_series_t *ser, const char *txt)
{
	lv_obj_t *chkbox = lv_checkbox_create(parent);
	lv_checkbox_set_text(chkbox, txt);
	lv_obj_set_style_text_color(chkbox, ser->color, 0);
	lv_obj_set_style_text_font(chkbox, &ubuntu_mono_14, 0);

	char *visible = NULL;
	if (get_meteo_chart_config_value(txt, &visible))
	{
		if (strcmp(visible, "1") == 0)
			lv_obj_add_state(chkbox, LV_STATE_CHECKED);
		else
		{
			lv_obj_clear_state(chkbox, LV_STATE_CHECKED);
			lv_chart_hide_series(chart, ser, true);
		}

		free(visible);
	}
	else
		lv_obj_add_state(chkbox, LV_STATE_CHECKED);

	lv_obj_add_event_cb(chkbox, show_hide_ser_event_cb, LV_EVENT_CLICKED, ser);

	return chkbox;
}

static lv_obj_t *create_line_values_range(lv_obj_t *parent, float min, float max, const lv_obj_t *align_to_obj, lv_coord_t x_ofs, lv_coord_t y_ofs, const char *postfix,
		const char *fmt0, const char *fmt1, const char *fmt2, const char *fmt3, const char *fmt4)
{
	const int LABEL_COUNT = 6;
	lv_obj_t *lbls[LABEL_COUNT];
	for (int i = 0; i < LABEL_COUNT; ++i)
	{
		lbls[i] = lv_label_create(parent);
		lv_obj_set_style_text_font(lbls[i], &ubuntu_mono_14, 0);
	}

	lv_label_set_text_fmt(lbls[0], fmt0, max);
	lv_label_set_text_fmt(lbls[1], fmt1, AVG(max, AVG(min, max)));
	lv_label_set_text_fmt(lbls[2], fmt2, AVG(min, max));
	lv_label_set_text_fmt(lbls[3], fmt3, AVG(AVG(min, max), min));
	lv_label_set_text_fmt(lbls[4], fmt4, min);
	lv_label_set_text(lbls[5], postfix);

	lv_obj_align_to(lbls[0], align_to_obj, LV_ALIGN_OUT_LEFT_TOP, x_ofs, y_ofs);

	for (int i = 1; i < LABEL_COUNT - 1; ++i)
		lv_obj_align_to(lbls[i], lbls[i - 1], LV_ALIGN_OUT_BOTTOM_MID, 0, 52);

	lv_obj_align_to(lbls[5], lbls[4], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

	return lbls[0];
}

static void create_lines_value_range(lv_obj_t *parent, const open_meteo_data_t *meteo_min, const open_meteo_data_t *meteo_max)
{
	if (meteo_min == NULL || meteo_max == NULL)
		return;

	float min, max;

	min = MIN(meteo_min->temperature, meteo_min->apparent_temperature);
	max = MAX(meteo_max->temperature, meteo_max->apparent_temperature);

	const char *dft_fmt = "%.0f";
	lv_obj_t *obj = create_line_values_range(parent, min, max, chart, -10, 5, "°C", dft_fmt, dft_fmt, dft_fmt, dft_fmt, dft_fmt);
	obj = create_line_values_range(parent, meteo_min->surface_pressure, meteo_max->surface_pressure, obj, -10, 0, "mm.Hg", dft_fmt, dft_fmt, dft_fmt, dft_fmt, dft_fmt);

	min = MIN(MIN(meteo_min->precipitation, meteo_min->rain), meteo_min->showers);
	max = MAX(MAX(meteo_max->precipitation, meteo_max->rain), meteo_max->showers);

	const char *_2_fmt = "%.2f";
	obj = create_line_values_range(parent, min, max, obj, -10, 0, "мм", _2_fmt, _2_fmt, _2_fmt, _2_fmt, _2_fmt);
	obj = create_line_values_range(parent, meteo_min->snowfall, meteo_max->snowfall, obj, -10, 0, "см", _2_fmt, _2_fmt, _2_fmt, _2_fmt, _2_fmt);

	min = MIN(meteo_min->wind_speed, meteo_min->wind_gusts);
	max = MAX(meteo_max->wind_speed, meteo_max->wind_gusts);

	obj = create_line_values_range(parent, min, max, obj, -10, 0, "м/c", dft_fmt, dft_fmt, dft_fmt, dft_fmt, dft_fmt);
	obj = create_line_values_range(parent, 0, 100, obj, -10, 0, "%", dft_fmt, dft_fmt, dft_fmt, dft_fmt, dft_fmt);

	obj = create_line_values_range(parent, 0, 259, obj, -10, 0, "°", dft_fmt, dft_fmt, dft_fmt, dft_fmt, dft_fmt);
}

void meteo_chart_page_init(void)
{
	check_meteo_chart_file();

	page_t *page = current_page();
	page->deinit();
	page->deinit = meteo_chart_page_deinit;
	page->title = page_title(METEO_CHART_PAGE_TITLE);
	status_panel_update();

	lv_obj_t *widget = lv_obj_create(page->widget);
	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_set_style_radius(widget, 0, 0);

	/*Create a chart*/
	chart = lv_chart_create(widget);
	//	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
	lv_chart_set_point_count(chart, OPEN_METEO_WEEK_SIZE);
	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
	lv_chart_set_div_line_count(chart, 5, 12);//OPEN_METEO_WEEK_SIZE / 3 / 4 - 1);
	lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

	lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR); // размер точек
	lv_obj_set_size(chart, 550, 305);
	lv_obj_align(chart, LV_ALIGN_TOP_RIGHT, 0, 0);

	lv_obj_refresh_ext_draw_size(chart);

	lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 12, 5, true, 45);
	lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

	lv_chart_set_zoom_x(chart, 700);

	// Создание chart series
	lv_chart_series_t *sers[12];
	int x = 20, y = 355;
	for (int i = 0; i < SERIES_COUNT; ++i)
	{
		sers[i] = lv_chart_add_series(chart, lv_palette_main(i), LV_CHART_AXIS_PRIMARY_Y);
		lv_obj_t *chkbox = create_ser_chkbox(widget, sers[i], words[i]);

		if (i == 6)
		{
			x = 20;
			y = 395;
		}

		lv_obj_set_pos(chkbox, x, y);
		x+= 130;
	}

	const open_meteo_data_t *open_meteo = service_weather_get_meteo_data();
	if (open_meteo != NULL)
	{
		open_meteo_data_t meteo_min, meteo_max;
		service_weather_get_range(&meteo_min, &meteo_max);

		create_lines_value_range(widget, &meteo_min, &meteo_max);

		for(int i = 0; i < OPEN_METEO_WEEK_SIZE; i++)
		{
			float min, max;

			min = MIN(meteo_min.temperature, meteo_min.apparent_temperature);
			max = MAX(meteo_max.temperature, meteo_max.apparent_temperature);

			lv_chart_set_next_value(chart, sers[METEO_TEMPERATURE], convert_range(open_meteo[i].temperature, min, max, 0, 100));
			lv_chart_set_next_value(chart, sers[METEO_APPARENT_TEMPERATURE], convert_range(open_meteo[i].apparent_temperature, min, max, 0, 100));

			lv_chart_set_next_value(chart, sers[METEO_SURFACE_PRESSURE], convert_range(open_meteo[i].surface_pressure, meteo_min.surface_pressure, meteo_max.surface_pressure, 0, 100));

			min = MIN(MIN(meteo_min.precipitation, meteo_min.rain), meteo_min.showers);
			max = MAX(MAX(meteo_max.precipitation, meteo_max.rain), meteo_max.showers);
			lv_chart_set_next_value(chart, sers[METEO_PRECIPITATION], convert_range(open_meteo[i].precipitation, min, max, 0, 100));
			lv_chart_set_next_value(chart, sers[METEO_RAIN], convert_range(open_meteo[i].rain, min, max, 0, 100));
			lv_chart_set_next_value(chart, sers[METEO_SHOWERS], convert_range(open_meteo[i].showers, min, max, 0, 100));

			lv_chart_set_next_value(chart, sers[METEO_SNOWFALL], convert_range(open_meteo[i].snowfall, meteo_min.snowfall, meteo_max.snowfall, 0, 100));

			min = MIN(meteo_min.wind_speed, meteo_min.wind_gusts);
			max = MAX(meteo_max.wind_speed, meteo_max.wind_gusts);
			lv_chart_set_next_value(chart, sers[METEO_WIND_SPEED], convert_range(open_meteo[i].wind_speed, min, max, 0, 100));
			lv_chart_set_next_value(chart, sers[METEO_WIND_GUSTS], convert_range(open_meteo[i].wind_gusts, min, max, 0, 100));

			lv_chart_set_next_value(chart, sers[METEO_RELATIVE_HUMIDITY], open_meteo[i].relative_humidity);
			lv_chart_set_next_value(chart, sers[METEO_CLOUD_COVER], open_meteo[i].cloud_cover);

			lv_chart_set_next_value(chart, sers[METEO_WIND_DIRECTION], convert_range(open_meteo[i].wind_direction, meteo_min.wind_direction, meteo_max.wind_direction, 0, 100));
		}
		lv_chart_refresh(chart);
	}
	else
		ESP_LOGE(TAG, "open_meteo = NULL");
}

void meteo_chart_page_deinit(void)
{
	default_page_deinit();
}
