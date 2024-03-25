/*
 * weather.c
 *
 *  Created on: 19 дек. 2023 г.
 *      Author: user
 */

#include "weather.h"

extern lv_font_t ubuntu_mono_14;
extern lv_obj_t *sub_weather_page;

struct Weather_page_obj {
	lv_obj_t *btn_search;
	lv_obj_t *textarea;
	lv_obj_t *keyboard;
	lv_obj_t *switcher;
	lv_obj_t *cur_data_lbl;
	lv_obj_t *busy_ind;
	lv_timer_t *timer;
};
typedef struct Weather_page_obj weather_page_obj_t;

static char *TAG = "setting_weather";

static weather_page_obj_t *weather_page_obj = NULL;

static uint8_t columns = 5;
static uint8_t selected_row = 0;
static uint8_t cell_w = 200;
static uint8_t first_column_w = 40;
static uint8_t cell_h = 40;
const static char *title[5] = {"Страна", "Населённый_пункт", "Широта", "Долгота", "Другое"};

static void city_search_handler(lv_event_t *e);
static void city_save_handler(lv_event_t *e);
static void switcher_handler(lv_event_t *e);
static void cell_clicked_handler(lv_event_t *e);
static void timer_loop(lv_timer_t *timer);

static bool parse_open_meteo_city(const char *data, open_meteo_city_t **open_meteo_city, uint8_t *ret_counter);
static void create_city_table(const open_meteo_city_t *open_meteo_city, uint8_t size);
static lv_obj_t *create_cell(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, const char *txt);

static void cell_clicked_handler(lv_event_t *e)
{
	lv_obj_t *cell = e->current_target;
	lv_obj_t *table = e->user_data;
	uint8_t click_row = 0;

	for (uint32_t i = 0; i < lv_obj_get_child_cnt(table); ++i)
	{
		if (cell == lv_obj_get_child(table, i))
		{
			click_row = i / 5;
			break;
		}
	}

	if (selected_row != 0)
	{
		for (int i = (columns * selected_row); i < ((columns * selected_row) + columns); ++i)
			lv_obj_set_style_bg_color(lv_obj_get_child(table, i), lv_color_white(), 0);
	}

	for (int i = (columns * click_row); i < ((columns * click_row) + columns); ++i)
		lv_obj_set_style_bg_color(lv_obj_get_child(table, i), lv_color_make(150, 150, 150), 0);

	selected_row = click_row;
}

static lv_obj_t *create_cell(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, const char *txt)
{
	lv_obj_t *cell = lv_obj_create(parent);
	lv_obj_set_size(cell, cell_w, cell_h);
	lv_obj_set_pos(cell, x, y);
	lv_obj_set_style_border_width(cell, 1, 0);
	lv_obj_set_style_radius(cell, 0, 0);
	lv_obj_set_scrollbar_mode(cell, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_event_cb(cell, cell_clicked_handler, LV_EVENT_CLICKED, parent);

	lv_obj_t *lbl = lv_label_create(cell);
	if (txt != NULL)
		lv_label_set_text_fmt(lbl, "%s", txt);
	lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 0);

	return cell;
}

static void create_city_table(const open_meteo_city_t *open_meteo_city, uint8_t size)
{
	uint8_t rows = size;
	selected_row = 0;

	// Основное окно
	lv_obj_t *main_widget = lv_obj_create(lv_obj_get_child(lv_scr_act(), 1));
	lv_obj_set_size(main_widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_style_pad_all(main_widget, 0, 0);
	lv_obj_set_style_radius(main_widget, 0, 0);
	lv_obj_set_scrollbar_mode(main_widget, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(main_widget, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_text_font(main_widget, &ubuntu_mono_14, 0);

	// Виджет для размещения таблицы
	lv_obj_t *widget = lv_obj_create(main_widget);
	lv_obj_set_size(widget, LCD_H_RES, 385);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_set_style_radius(widget, 0, 0);

	// Шапка таблицы
	for (int i = 0; i < columns; ++i)
	{
		lv_obj_t *cell = lv_obj_create(widget);
		lv_obj_set_size(cell, cell_w, cell_h);
		lv_obj_set_pos(cell, cell_w * i + first_column_w, 0);
		lv_obj_set_style_border_width(cell, 1, 0);
		lv_obj_set_style_radius(cell, 0, 0);
		lv_obj_set_style_bg_color(cell, lv_color_make(0, 150, 255), 0);
		lv_obj_set_scrollbar_mode(cell, LV_SCROLLBAR_MODE_OFF);
		lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);

		lv_obj_t *lbl = lv_label_create(cell);
		lv_label_set_text_fmt(lbl, "%s", title[i]);
		lv_obj_center(lbl);
	}

	// Таблица
	for (int j = 0; j < rows; ++j)
	{
		//    lv_obj_set_pos(cell, cell_w * i + first_column_w, cell_h * j);
		create_cell(widget, (cell_w * 0 + first_column_w), (cell_h * (j + 1)), open_meteo_city[j].country);
		create_cell(widget, (cell_w * 1 + first_column_w), (cell_h * (j + 1)), open_meteo_city[j].city_name);

		char buffer[256] = {0};

		snprintf(buffer, sizeof(buffer), "%f", open_meteo_city[j].latitude);
		create_cell(widget, (cell_w * 2 + first_column_w), (cell_h * (j + 1)), buffer);

		snprintf(buffer, sizeof(buffer), "%f", open_meteo_city[j].longitude);
		create_cell(widget, (cell_w * 3 + first_column_w), (cell_h * (j + 1)), buffer);

		create_cell(widget, (cell_w * 4 + first_column_w), (cell_h * (j + 1)), open_meteo_city[j].admin1);
	}

	// ячейка #
	lv_obj_t *cell = lv_obj_create(widget);
	lv_obj_set_size(cell, first_column_w, cell_h);
	lv_obj_set_pos(cell, 0, 0);
	lv_obj_set_style_border_width(cell, 1, 0);
	lv_obj_set_style_radius(cell, 0, 0);
	lv_obj_set_style_bg_color(cell, lv_color_make(0, 150, 255), 0);
	lv_obj_set_scrollbar_mode(cell, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *lbl = lv_label_create(cell);
	lv_label_set_text_fmt(lbl, "%s", "#");
	lv_obj_center(lbl);

	// Столбец с номерами строк
	for (int i = 1; i <= rows; ++i)
	{
		lv_obj_t *cell = lv_obj_create(widget);
		lv_obj_set_size(cell, first_column_w, cell_h);
		lv_obj_set_pos(cell, 0, cell_h * i);
		lv_obj_set_style_border_width(cell, 1, 0);
		lv_obj_set_style_radius(cell, 0, 0);
		lv_obj_set_style_bg_color(cell, lv_color_make(0, 150, 255), 0);
		lv_obj_set_scrollbar_mode(cell, LV_SCROLLBAR_MODE_OFF);
		lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);

		lv_obj_t *lbl = lv_label_create(cell);
		lv_label_set_text_fmt(lbl, "%d", i);
		lv_obj_center(lbl);
	}

	lv_obj_t *btn_cancel = create_button_simply(main_widget, CANCEL_STR, 128, 40, &ubuntu_mono_14);
	lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
	lv_obj_add_event_cb(btn_cancel, delete_obj_handler, LV_EVENT_CLICKED, main_widget);

	lv_obj_t *btn_save = create_button_simply(main_widget, SAVE_STR, 128, 40, &ubuntu_mono_14);
	lv_obj_align_to(btn_save, btn_cancel, LV_ALIGN_OUT_LEFT_MID, -10, 0);
	lv_obj_add_event_cb(btn_save, city_save_handler, LV_EVENT_CLICKED, widget);
}

static bool parse_open_meteo_city(const char *data, open_meteo_city_t **open_meteo_city, uint8_t *ret_counter)
{
	if (data == NULL || open_meteo_city != NULL || ret_counter == NULL)
		false;

	cJSON *monitor = cJSON_Parse(data);
	if (monitor == NULL)
		return false;

	cJSON *results = cJSON_GetObjectItemCaseSensitive(monitor, "results");
	if (results == NULL)
		goto bad_end;

	cJSON *it;
	*ret_counter = 0;
	cJSON_ArrayForEach(it, results)
	{
		++*ret_counter;
	}

	if (*ret_counter == 0)
		goto bad_end;

	*open_meteo_city = calloc(sizeof(open_meteo_city_t), *ret_counter);

	int i = 0;
	cJSON_ArrayForEach(it, results)
	{
		cJSON *obj;
		obj = cJSON_GetObjectItemCaseSensitive(it, "country");
		uint8_t len = strlen(cJSON_GetStringValue(obj));
		(*open_meteo_city)[i].country = calloc(1, len + 1);
		memcpy((*open_meteo_city)[i].country, obj->valuestring, len);

		obj = cJSON_GetObjectItemCaseSensitive(it, "name");
		len = strlen(cJSON_GetStringValue(obj));
		(*open_meteo_city)[i].city_name = calloc(1, len + 1);
		memcpy((*open_meteo_city)[i].city_name, obj->valuestring, len);

		obj = cJSON_GetObjectItemCaseSensitive(it, "admin1");
		if (obj != NULL)
		{
			len = strlen(cJSON_GetStringValue(obj));
			(*open_meteo_city)[i].admin1 = calloc(1, len + 1);
			memcpy((*open_meteo_city)[i].admin1, obj->valuestring, len);
		}
		else
			(*open_meteo_city)[i].admin1 = calloc(1, 1);

		obj = cJSON_GetObjectItemCaseSensitive(it, "latitude");
		(*open_meteo_city)[i].latitude = cJSON_GetNumberValue(obj);

		obj = cJSON_GetObjectItemCaseSensitive(it, "longitude");
		(*open_meteo_city)[i].longitude = cJSON_GetNumberValue(obj);
		++i;
	}

	cJSON_Delete(monitor);
	return true;

	bad_end:
	cJSON_Delete(monitor);
	return false;
}

static void timer_loop(lv_timer_t *timer)
{
	if ( !(glob_get_status_reg() & STATUS_METEO_CITY_SEARCH_DONE) )
		return;

	glob_clear_bits_status_reg(STATUS_METEO_CITY_SEARCH_DONE);
	glob_clear_bits_status_reg(STATUS_METEO_CITY_SEARCH);

	struct stat st;
	stat(METEO_CITY_PATH, &st);
	size_t size = st.st_size;

	if (size <= 32)
	{
		create_msgbox(NULL, ATTENTION_STR, "Данные не найдены!");
		clear_busy_indicator(&weather_page_obj->busy_ind);
		return;
	}

	FILE *file = fopen(METEO_CITY_PATH, "r");
	if (file == NULL)
	{
		ESP_LOGW(TAG, "file %s not open", METEO_CITY_PATH);
		return;
	}

	char *buf = calloc(1, size + 1);
	if (buf == NULL)
	{
		ESP_LOGE(TAG, "malloc error");;
		return;
	}

	fread(buf, size, 1, file);
	fclose(file);

	open_meteo_city_t *open_meteo_city = NULL;
	uint8_t arr_size = 0;

	parse_open_meteo_city(buf, &open_meteo_city, &arr_size);
	free(buf);

	if (arr_size > 0)
		create_city_table(open_meteo_city, arr_size);
	else
		create_msgbox(NULL, ATTENTION_STR, "Данные не найдены!");

	free(open_meteo_city);
	clear_busy_indicator(&weather_page_obj->busy_ind);
}

static void city_search_handler(lv_event_t *e)
{
	if ( !(glob_get_status_reg() & STATUS_IP_GOT) )
	{
		create_msgbox_not_connected();
		return;
	}

	const char *city = lv_textarea_get_text(weather_page_obj->textarea);
	char *tmp_city = calloc(1, strlen(city) + 1);
	strcpy(tmp_city, city);
	service_weather_set_city(tmp_city);
	free(tmp_city);

	weather_page_obj->busy_ind = create_busy_indicator(lv_obj_get_child(lv_scr_act(), 1), LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H, 100, 100, LV_OPA_70);
	glob_set_bits_status_reg(STATUS_METEO_CITY_SEARCH);
}

static void city_save_handler(lv_event_t *e)
{
	if (selected_row == 0)
	{
		create_msgbox(NULL, ATTENTION_STR, "Необходимо выбрать любую строку!");
		return;
	}

	lv_obj_t *table = e->user_data;
	lv_obj_t *name_city_cell = lv_obj_get_child(table, columns * selected_row + 1);
	lv_obj_t *latitude_cell = lv_obj_get_child(table, columns * selected_row + 2);
	lv_obj_t *longitude_cell = lv_obj_get_child(table, columns * selected_row + 3);

	char *city_name = lv_label_get_text(lv_obj_get_child(name_city_cell, 0));
	char *latitude = lv_label_get_text(lv_obj_get_child(latitude_cell, 0));
	char *longitude = lv_label_get_text(lv_obj_get_child(longitude_cell, 0));

	set_meteo_config_value(CITY_STR, city_name);
	service_weather_set_city(city_name);
	set_meteo_config_value("latitude", latitude);
	set_meteo_config_value("longitude", longitude);

	lv_label_set_text_fmt(weather_page_obj->cur_data_lbl, "%s\n%s\n%s",
			city_name == NULL ? CITY_STR : city_name,
					latitude == NULL ? "0" : latitude,
							longitude == NULL ? "0" : longitude);
	lv_obj_del(table->parent);

	glob_set_bits_status_reg(STATUS_METEO_UPDATE_NOW);
}

static void switcher_handler(lv_event_t *e)
{
	if (lv_obj_has_state(weather_page_obj->switcher, LV_STATE_CHECKED))
	{
		if (!set_meteo_config_value("on", "1"))
			printf("on not write\n");

		glob_set_bits_status_reg(STATUS_METEO_ON);
	}
	else
	{
		glob_clear_bits_status_reg(STATUS_METEO_ON);
		if (!set_meteo_config_value("on", "0"))
			printf("on not write\n");
	}
}

void create_weather_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	weather_page_obj = calloc(1, sizeof(weather_page_obj_t));

	lv_obj_set_style_pad_hor(sub_weather_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create(sub_weather_page);
	lv_obj_set_scroll_dir(section, LV_DIR_NONE);
	lv_obj_set_scrollbar_mode(section, LV_SCROLLBAR_MODE_OFF);

	// Вкл./Выкл. службу погоды
	create_switch(section, LV_SYMBOL_SETTINGS, "Включить", (glob_get_status_reg() & STATUS_METEO_ON), &weather_page_obj->switcher);
	lv_obj_add_event_cb(weather_page_obj->switcher, switcher_handler, LV_EVENT_CLICKED, 0);

	// Кнопка Search
	create_button(section, "Поиск", 128, 40, &(weather_page_obj->btn_search));
	lv_obj_add_event_cb(weather_page_obj->btn_search, city_search_handler, LV_EVENT_CLICKED, 0);

	// Поле ввода
	weather_page_obj->textarea = lv_textarea_create(weather_page_obj->btn_search->parent);
	lv_obj_set_size(weather_page_obj->textarea, 340, 40);
	lv_obj_set_scroll_dir(weather_page_obj->textarea, LV_DIR_NONE);
	lv_textarea_set_max_length(weather_page_obj->textarea, 32);
	lv_textarea_set_placeholder_text(weather_page_obj->textarea, "Введите название населённого пункта.");
	lv_obj_align(weather_page_obj->textarea, LV_ALIGN_LEFT_MID, 0, 0);
	//	lv_textarea_set_text(weather_page_obj->textarea, ???);

	// клавиатура
	weather_page_obj->keyboard = create_keyboard(lv_scr_act(), LV_ALIGN_BOTTOM_MID,
			weather_page_obj->textarea,
			hide_obj_handler, hide_obj_handler, hide_obj_handler);

	lv_obj_add_flag(weather_page_obj->keyboard, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_event_cb(weather_page_obj->textarea, show_obj_handler, LV_EVENT_CLICKED, weather_page_obj->keyboard);

	// Текущие настройки
	lv_obj_t *cont = create_text(section, 0, "Населёный пункт:\nширота:\nдолгота:", LV_MENU_ITEM_BUILDER_VAR_1);
	weather_page_obj->cur_data_lbl = lv_label_create(cont);

	char *city = NULL;
	char *latitude = NULL;
	char *longitude = NULL;

	get_meteo_config_value(CITY_STR, &city);
	get_meteo_config_value("latitude", &latitude);
	get_meteo_config_value("longitude", &longitude);

	lv_label_set_text_fmt(weather_page_obj->cur_data_lbl, "%s\n%s\n%s",
			city == NULL ? CITY_STR : city,
					latitude == NULL ? "0" : latitude,
							longitude == NULL ? "0" : longitude);

	free(city);
	free(latitude);
	free(longitude);

	weather_page_obj->timer = lv_timer_create(timer_loop, 1000, 0);
}

void free_weather_sub_page(void)
{
	if (weather_page_obj != NULL)
	{
		lv_obj_del(weather_page_obj->keyboard);
		lv_timer_del(weather_page_obj->timer);
		free(weather_page_obj);
		weather_page_obj = NULL;
	}
}
