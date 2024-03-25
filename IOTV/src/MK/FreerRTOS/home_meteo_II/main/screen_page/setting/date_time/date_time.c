/*
 * date_time.c
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: user
 */

#include "date_time.h"

extern lv_obj_t *menu;

extern lv_obj_t *sub_date_time_page;
extern lv_obj_t *sub_sub_time_page;
extern lv_obj_t *sub_sub_date_page;
extern lv_obj_t *sub_sub_sntp_page;

extern char *sntp_utc;
extern char *sntp_server_url;

static char *time_page_title = "Время";
static char *date_page_title = "Дата";
static char *sntp_page_title = "SNTP";

static void save_time(lv_event_t *e);
static void save_date(lv_event_t *e);
static void sntp_switch_handler(lv_event_t * e);
static void utc_dd_event_handler(lv_event_t * e);
static void sntp_save_event_handler(lv_event_t * e);

struct Date_time_page_obj
{
	lv_obj_t *switcher;
	lv_obj_t *list;
	lv_obj_t *sntp_server_url;
	lv_obj_t *btn_save;
	lv_obj_t *keyboard;
};

static struct Date_time_page_obj *dt_page_obj = NULL;

void create_time_page(void)
{
	lv_obj_set_style_pad_hor(sub_sub_time_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_time_page);

	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	lv_obj_t *obj, *btn_minus, *btn_plus;;
	create_spinbox(section, "Часы:", timeinfo.tm_hour, 0, 23, 2, 0, &obj, &btn_minus, &btn_plus, 50, 45);
	create_spinbox(section, "Минуты:", timeinfo.tm_min, 0, 59, 2, 0, &obj, &btn_minus, &btn_plus, 50, 45);
	create_spinbox(section, "Секунды:", timeinfo.tm_sec, 0, 59, 2, 0, &obj, &btn_minus, &btn_plus, 50, 45);

	lv_obj_t *obj_btn = NULL;
	create_button(section, SAVE_STR, 128, 40, &obj_btn);
	lv_obj_add_event_cb(obj_btn, save_time, LV_EVENT_CLICKED, section);
}

static void utc_dd_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	if(code == LV_EVENT_VALUE_CHANGED)
	{
		char buf[32] = {0};
		lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
	}
}

static void sntp_save_event_handler(lv_event_t * e)
{
	if (lv_obj_has_state(dt_page_obj->switcher, LV_STATE_CHECKED))
		set_sntp_config_value("on", "1");
	else
		set_sntp_config_value("on", "0");

	char buf[8] = {0};
	lv_dropdown_get_selected_str(dt_page_obj->list, buf, sizeof(buf) - 1);
	set_sntp_config_value("utc", buf);

	set_sntp_config_value("url", lv_textarea_get_text(dt_page_obj->sntp_server_url));
}

void create_sntp_page(void)
{
	lv_obj_set_style_pad_hor(sub_sub_sntp_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_sntp_page);
	lv_obj_set_scroll_dir(section, LV_DIR_NONE);
	lv_obj_set_scrollbar_mode(section, LV_SCROLLBAR_MODE_OFF);

	// Включить/выключить службу sntp
	create_switch(section, LV_SYMBOL_SETTINGS, "Включить", (glob_get_status_reg() & STATUS_SNTP_ON), &dt_page_obj->switcher);
	lv_obj_add_event_cb(dt_page_obj->switcher, sntp_switch_handler, LV_EVENT_CLICKED, section);
	//
	//	lv_obj_t *cont = lv_menu_cont_create(section);
	//	lv_obj_set_style_pad_all(cont, 0, 0);

	// Wrap
	lv_obj_t *wrap = lv_obj_create(section);
	lv_obj_set_style_pad_all(wrap, 0, 0);
	lv_obj_set_size(wrap, 520, 100);
	lv_obj_set_style_border_width(wrap, 0, 0);

	// Выпадающий список UTC
	dt_page_obj->list = lv_dropdown_create(wrap);
	lv_obj_set_size(dt_page_obj->list, 128, 40);
	lv_dropdown_set_options(dt_page_obj->list,
			"UTC-0\n"
			"UTC-1\n"
			"UTC-2\n"
			"UTC-3\n"
			"UTC-4\n"
			"UTC-5\n"
			"UTC-6\n"
			"UTC-7\n"
			"UTC-8\n"
			"UTC-9\n"
			"UTC-10\n"
			"UTC-11\n"
			"UTC-12");
	lv_obj_align(dt_page_obj->list, LV_ALIGN_TOP_RIGHT, -10, 0);
	lv_obj_add_event_cb(dt_page_obj->list, utc_dd_event_handler, LV_EVENT_ALL, NULL);

	int val = 0;
	if (strlen(sntp_utc) > 3)
		sscanf(&sntp_utc[4], "%d", &val);
	lv_dropdown_set_selected(dt_page_obj->list, val);

	// Текст для UTC
	lv_obj_t *lbl_utc = lv_label_create(wrap);
	lv_label_set_text(lbl_utc, "Часовой пояс:");
	lv_obj_align_to(lbl_utc, dt_page_obj->list, LV_ALIGN_OUT_LEFT_MID, -260, 0);

	// Поле ввода адреса сервера ntp
	dt_page_obj->sntp_server_url = lv_textarea_create(wrap);
	lv_obj_set_size(dt_page_obj->sntp_server_url, 382, 40);
	lv_obj_set_scroll_dir(dt_page_obj->sntp_server_url, LV_DIR_NONE);
	lv_textarea_set_max_length(dt_page_obj->sntp_server_url, 128);
	lv_textarea_set_placeholder_text(dt_page_obj->sntp_server_url, "Пример: ntp0.ntp-servers.net");
	lv_obj_align_to(dt_page_obj->sntp_server_url, dt_page_obj->list, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 15);
	lv_textarea_set_text(dt_page_obj->sntp_server_url, sntp_server_url);

	// Текст для адреса сервера
	lv_obj_t *lbl_url = lv_label_create(wrap);
	lv_label_set_text(lbl_url, "NTP сервер:");
	lv_obj_align_to(lbl_url, dt_page_obj->sntp_server_url, LV_ALIGN_OUT_LEFT_MID, -28, 0);

	// Кнопка сохранить
	create_button(section, SAVE_STR, 128, 40, &dt_page_obj->btn_save);
	lv_obj_add_event_cb(dt_page_obj->btn_save, sntp_save_event_handler, LV_EVENT_CLICKED, 0);

	// клавиатура
	dt_page_obj->keyboard = create_keyboard(lv_scr_act(), LV_ALIGN_BOTTOM_MID, dt_page_obj->sntp_server_url,
			hide_obj_handler, hide_obj_handler, hide_obj_handler);

	lv_obj_add_flag(dt_page_obj->keyboard, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_event_cb(dt_page_obj->sntp_server_url, show_obj_handler, LV_EVENT_CLICKED, dt_page_obj->keyboard);
}

void create_date_page(void)
{
	lv_obj_set_style_pad_hor(sub_sub_date_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_date_page);

	create_calendar(section);

	lv_obj_t *obj_btn = NULL;
	create_button(section, SAVE_STR, 128, 40, &obj_btn);
	lv_obj_add_event_cb(obj_btn, save_date, LV_EVENT_CLICKED, section);
}

void create_date_time_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	dt_page_obj = calloc(1, sizeof(struct Date_time_page_obj));

	lv_obj_set_style_pad_hor(sub_date_time_page, 20, 0);

	create_time_page();
	create_date_page();
	create_sntp_page();

	lv_obj_t *section = lv_menu_section_create(sub_date_time_page);

	lv_obj_t *cont = create_text(section, LV_SYMBOL_RIGHT, time_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_sub_time_page);

	cont = create_text(section, LV_SYMBOL_RIGHT, date_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_sub_date_page);

	cont = create_text(section, LV_SYMBOL_RIGHT, sntp_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_sub_sntp_page);
}

void free_date_time_sub_page(void)
{
	if (dt_page_obj != NULL)
	{
		lv_obj_del(dt_page_obj->keyboard);
		free(dt_page_obj);
		dt_page_obj = NULL;
	}
}

static void sntp_switch_handler(lv_event_t * e)
{
	lv_obj_t *switcher = e->target;
	if (lv_obj_has_state(switcher, LV_STATE_CHECKED))
		glob_set_bits_status_reg(STATUS_SNTP_ON);
	else
		glob_clear_bits_status_reg(STATUS_SNTP_ON);
}

static void save_time(lv_event_t *e)
{
	if (glob_get_status_reg() & STATUS_SNTP_ON)
	{
		create_msgbox(NULL, ATTENTION_STR, "Невозможно установить время, если включена служба SNTP!");
		return;
	}

	lv_obj_t *section = (lv_obj_t *)e->user_data;

	time_t now;
	time(&now);

	struct tm timeinfo = *localtime(&now);

	timeinfo.tm_hour = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 0), 0), 1));
	timeinfo.tm_min = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 1), 0), 1));
	timeinfo.tm_sec = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 2), 0), 1));

	now = mktime(&timeinfo);
	struct timeval tv = {.tv_sec = now};
	settimeofday(&tv, NULL);

	DS3231_SetDataTime_tm(&timeinfo);
//	glob_set_bits_status_reg(STATUS_TIME_SYNC);
}

static void save_date(lv_event_t *e)
{
	if (glob_get_status_reg() & STATUS_SNTP_ON)
	{
		create_msgbox(NULL, ATTENTION_STR, "Невозможно установить дату, если включена служба SNTP!");
		return;
	}

	time_t now;
	time(&now);

	struct tm timeinfo = *localtime(&now);

	lv_obj_t *section = (lv_obj_t *)e->user_data;

	lv_calendar_t *calendar = (lv_calendar_t *)lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 0), 0), 0);
	timeinfo.tm_year = calendar->today.year - 1900;
	timeinfo.tm_mon = calendar->today.month - 1;
	timeinfo.tm_mday = calendar->today.day;

	now = mktime(&timeinfo);
	struct timeval tv = {.tv_sec = now};
	settimeofday(&tv, NULL);

	DS3231_SetDataTime_tm(&timeinfo);
//	glob_set_bits_status_reg(STATUS_TIME_SYNC);
}
