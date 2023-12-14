/*
 * date_time.c
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: user
 */

#include "date_time.h"
#include "settingpage.h"

extern struct DateTime glob_date_time;
extern lv_obj_t *menu;

extern lv_obj_t *sub_date_time_page;
extern lv_obj_t *sub_sub_time_page;
extern lv_obj_t *sub_sub_date_page;
extern lv_obj_t *sub_sub_sntp_page;

static void save_time(lv_event_t *e);
static void save_date(lv_event_t *e);

static char *time_page_title = "Time";
static char *date_page_title = "Date";

void create_time_page(void)
{
	//	sub_sub_time_page = lv_menu_page_create(menu, time_page_title);
	lv_obj_set_style_pad_hor(sub_sub_time_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_time_page);

	create_spinbox(section, "Hours:\t", glob_date_time.hour, 0, 23);
	create_spinbox(section, "Minutes:", glob_date_time.minutes, 0, 59);
	create_spinbox(section, "Seconds:", glob_date_time.seconds, 0, 59);

	lv_obj_t *obj_btn = NULL;
	create_button(section, "Save", 128, 40, &obj_btn);
	lv_obj_add_event_cb(obj_btn, save_time, LV_EVENT_CLICKED, section);
}

void create_sntp_page(void)
{
	//	sub_sub_date_page = lv_menu_page_create(menu, date_page_title);
	lv_obj_set_style_pad_hor(sub_sub_sntp_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_sntp_page);

	lv_obj_t *switcher;
	create_switch(section, LV_SYMBOL_SETTINGS, "Enable", false, &switcher);

//	lv_obj_add_event_cb(obj_btn, save_date, LV_EVENT_CLICKED, section);
}

void create_date_page(void)
{
	lv_obj_set_style_pad_hor(sub_sub_date_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_date_page);

	create_calendar(section);

	lv_obj_t *obj_btn = NULL;
	create_button(section, "Save", 128, 40, &obj_btn);
	lv_obj_add_event_cb(obj_btn, save_date, LV_EVENT_CLICKED, section);
}

void create_date_time_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	lv_obj_set_style_pad_hor(sub_date_time_page, 20, 0);
	//	lv_obj_t *section = lv_menu_section_create((lv_obj_t *)sub_date_time_page);

	create_time_page();
	create_date_page();
	create_sntp_page();

	lv_obj_t *section = lv_menu_section_create(sub_date_time_page);

	lv_obj_t *cont = create_text(section, LV_SYMBOL_RIGHT, time_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_sub_time_page);

	cont = create_text(section, LV_SYMBOL_RIGHT, date_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_sub_date_page);

	cont = create_text(section, LV_SYMBOL_RIGHT, "SNTP", LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_sub_sntp_page);
}

static void save_time(lv_event_t *e)
{
	lv_obj_t *section = (lv_obj_t *)e->user_data;

	glob_date_time.hour = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 0), 0), 1));
	glob_date_time.minutes = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 1), 0), 1));
	glob_date_time.seconds = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 2), 0), 0));

	//	DS3231_SetDataTime(&glob_date_time);
}

static void save_date(lv_event_t *e)
{
	lv_obj_t *section = (lv_obj_t *)e->user_data;

	lv_calendar_t *calendar = (lv_calendar_t *)lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 0), 0), 0);
	glob_date_time.year = calendar->today.year - 1900;
	glob_date_time.month = calendar->today.month;
	glob_date_time.date = calendar->today.day;

	//	DS3231_SetDataTime(&glob_date_time);
}
