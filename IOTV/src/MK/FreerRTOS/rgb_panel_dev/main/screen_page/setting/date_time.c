/*
 * date_time.c
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: user
 */

#include "date_time.h"
#include "settingpage.h"

extern uint32_t glob_status_reg;
extern struct DateTime glob_date_time;
extern lv_obj_t *menu;

extern lv_obj_t *sub_date_time_page;
extern lv_obj_t *sub_sub_time_page;
extern lv_obj_t *sub_sub_date_page;
extern lv_obj_t *sub_sub_sntp_page;

static char *time_page_title = "Time";
static char *date_page_title = "Date";
static char *sntp_page_title = "SNTP!";

static void save_time(lv_event_t *e);
static void save_date(lv_event_t *e);
static void sntp_switch_handler(lv_event_t * e);
static void utc_dd_event_handler(lv_event_t * e);
static void url_event_handler(lv_event_t * e);

//struct Date_time_page_obj {
//	lv_obj_t *switcher;
//	lv_obj_t *list;
//	lv_obj_t *sntp_server;
//	lv_timer_t *timer;
//};
//
//static struct Date_time_page_obj *DT_page_obj = NULL;

void create_time_page(void)
{
	lv_obj_set_style_pad_hor(sub_sub_time_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_time_page);

	create_spinbox(section, "Hours:\t", glob_date_time.hour, 0, 23);
	create_spinbox(section, "Minutes:", glob_date_time.minutes, 0, 59);
	create_spinbox(section, "Seconds:", glob_date_time.seconds, 0, 59);

	lv_obj_t *obj_btn = NULL;
	create_button(section, "Save", 128, 40, &obj_btn);
	lv_obj_add_event_cb(obj_btn, save_time, LV_EVENT_CLICKED, section);
}

static void utc_dd_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32] = {0};
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        printf("selected :%s\n", buf);
    }
}

static void url_event_handler(lv_event_t * e)
{
	// клавиатура
	lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
	lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_obj_add_event_cb(kb, delete_obj_handler, LV_EVENT_CANCEL, kb);
	lv_obj_add_event_cb(kb, delete_obj_handler, LV_EVENT_READY, kb);
	lv_obj_add_event_cb(e->current_target, delete_obj_handler, LV_EVENT_DEFOCUSED, kb);

	lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_keyboard_set_textarea(kb, e->current_target);
}

void create_sntp_page(void)
{
	lv_obj_set_style_pad_hor(sub_sub_sntp_page, 20, 0);

	lv_obj_t *section = lv_menu_section_create(sub_sub_sntp_page);

	// Включить/выключить службу sntp
	lv_obj_t *switcher;
	create_switch(section, LV_SYMBOL_SETTINGS, "Enable", (glob_status_reg & STATUS_SNTP_ON), &switcher);
	lv_obj_add_event_cb(switcher, sntp_switch_handler, LV_EVENT_CLICKED, section);

	lv_obj_t *cont = lv_menu_cont_create(section);
	lv_obj_set_scroll_dir(cont, LV_DIR_NONE);
	lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_style_pad_all(cont, 0, 0);

	// Wrap
	lv_obj_t *wrap = lv_obj_create(section);
	lv_obj_set_style_pad_all(wrap, 0, 0);
	lv_obj_set_scrollbar_mode(wrap, LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_scroll_dir(wrap, LV_DIR_NONE);
	lv_obj_set_size(wrap, 520, 100);
	lv_obj_set_style_border_width(wrap, 0, 0);

	// Выпадающий список UTC
	lv_obj_t *dd = lv_dropdown_create(wrap);
	lv_obj_set_size(dd, 128, 40);
	lv_dropdown_set_options(dd,
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
			"UTC-12\n");
	lv_obj_align(dd, LV_ALIGN_TOP_RIGHT, -10, 0);
    lv_obj_add_event_cb(dd, utc_dd_event_handler, LV_EVENT_ALL, NULL);

    // Текст для UTC
	lv_obj_t *lbl_utc = lv_label_create(wrap);
	lv_label_set_text(lbl_utc, "Time zone:");
	lv_obj_align_to(lbl_utc, dd, LV_ALIGN_OUT_LEFT_MID, -290, 0);

	// Поле ввода адреса сервера ntp
	lv_obj_t *text_area = lv_textarea_create(wrap);
	lv_obj_set_size(text_area, 382, 40);
	lv_obj_set_scroll_dir(text_area, LV_DIR_NONE);

	lv_textarea_set_max_length(text_area, 128);
	lv_textarea_set_placeholder_text(text_area, "Example: ntp0.ntp-servers.net");
	lv_obj_align_to(text_area, dd, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 15);
	lv_obj_add_event_cb(text_area, url_event_handler, LV_EVENT_CLICKED, section);

	// Текст для адреса сервера
	lv_obj_t *lbl_url = lv_label_create(wrap);
	lv_label_set_text(lbl_url, "NTP server:");
	lv_obj_align_to(lbl_url, text_area, LV_ALIGN_OUT_LEFT_MID, -33, 0);

	lv_obj_t *btn_save = NULL;
	create_button(section, "Save", 128, 40, &btn_save);
//	lv_obj_add_event_cb(obj_btn, save_time, LV_EVENT_CLICKED, section);
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

static void sntp_switch_handler(lv_event_t * e)
{
	lv_obj_t *switcher = e->target;
	if (lv_obj_has_state(switcher, LV_STATE_CHECKED))
	{
		glob_status_reg |= STATUS_SNTP_ON;
		set_sntp_config_value("on", "1");
	}
	else
	{
		glob_status_reg &= ~STATUS_SNTP_ON;
		set_sntp_config_value("on", "0");
	}
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
