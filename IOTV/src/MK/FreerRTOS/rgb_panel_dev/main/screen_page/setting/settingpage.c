/*
 * settingpage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "settingpage.h"


extern void menuPageInit(void);

extern uint8_t glob_currentPage;

extern struct DateTime glob_date_time;

static char* TAG = "settings";

static lv_obj_t *menu;
static lv_obj_t *root_page;

static lv_obj_t *sub_date_time_page;
static lv_obj_t *sub_display_page;
lv_obj_t *sub_wifi_page;
static lv_obj_t *sub_about_page;
static lv_obj_t *sub_software_info_page;
static lv_obj_t *sub_legal_info_page;

static char *root_page_title = "Settings";
static char *date_time_page_title = "Date and time";
static char *display_page_title = "Display";
static char *wifi_page_title = "WIFI";

static void back_event_handler(lv_event_t * e);
static void create_date_time_sub_page(lv_event_t *e);
static void create_display_sub_page(lv_event_t *e);

static void create_sub_pages(void);

static void create_date_time_page(lv_obj_t *parent);
static void create_display_page(lv_obj_t *parent);
static void create_wifi_page(lv_obj_t *parent);
static void create_other_pages(void);

static void back_event_handler(lv_event_t * e)
{
	lv_obj_t *obj = lv_event_get_target(e);
	lv_obj_t *_menu = lv_event_get_user_data(e);

	if(lv_menu_back_btn_is_root(_menu, obj))
	{
		((lv_menu_page_t *)root_page)->title = NULL;
		((lv_menu_page_t *)sub_date_time_page)->title = NULL;
		((lv_menu_page_t *)sub_display_page)->title = NULL;
		((lv_menu_page_t *)sub_wifi_page)->title = NULL;

		menuPageInit();
		free_wifi_sub_page();
	}
}

static void save_date_time(lv_event_t *e)
{
	lv_obj_t *section = (lv_obj_t *)e->user_data;

	glob_date_time.hour = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 0), 0), 1));
	glob_date_time.minutes = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 1), 0), 1));
	glob_date_time.seconds = lv_spinbox_get_value(lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 2), 0), 0));
	//
	lv_calendar_t *calendar = (lv_calendar_t *)lv_obj_get_child(lv_obj_get_child(lv_obj_get_child(section, 3), 0), 0);
	//	printf("cal today %d\n", calendar->today.year);
	glob_date_time.year = calendar->today.year - 1900;
	glob_date_time.month = calendar->today.month;
	glob_date_time.date = calendar->today.day;

	//	DS3231_SetDataTime(&glob_date_time);
}

static void debug_lv_obj_t_tree(lv_obj_t *obj, int depth)
{
	for (int i = 0; i < lv_obj_get_child_cnt(obj); ++i)
	{
		for (int j = 0; j < depth; ++j)
			printf("\t");
		printf("depth: %d, current obj addr: %p,", depth, lv_obj_get_child(obj, i));
		printf("children: %d\n", (int)(lv_obj_get_child_cnt(lv_obj_get_child(obj, i))));
		debug_lv_obj_t_tree(lv_obj_get_child(obj, i), depth + 1);
	}
}

static void create_sub_pages(void)
{
	sub_date_time_page = lv_menu_page_create(menu, date_time_page_title);
	sub_display_page = lv_menu_page_create(menu, display_page_title);
	sub_wifi_page = lv_menu_page_create(menu, wifi_page_title);

	//	sub_software_info_page = lv_menu_page_create(menu, NULL);
	//	lv_obj_set_style_pad_hor(sub_software_info_page, 20, 0);
	//	section = lv_menu_section_create(sub_software_info_page);
	//	create_text(section, NULL, "Version 1.0", LV_MENU_ITEM_BUILDER_VARIANT_1);
	//
	//	sub_legal_info_page = lv_menu_page_create(menu, NULL);
	//	lv_obj_set_style_pad_hor(sub_legal_info_page, 20, 0);
	//	section = lv_menu_section_create(sub_legal_info_page);
	//
	//	for(uint32_t i = 0; i < 15; i++)
	//	{
	//		create_text(section, NULL,
	//				"This is a long long long long long long long long long text, if it is long enough it may scroll.",
	//				LV_MENU_ITEM_BUILDER_VARIANT_1);
	//	}
	//
	//	sub_about_page = lv_menu_page_create(menu, NULL);
	//	lv_obj_set_style_pad_hor(sub_about_page, 20, 0);
	//	lv_menu_separator_create(sub_about_page);
	//
	//	section = lv_menu_section_create(sub_about_page);
	//
	//	cont = create_text(section, LV_SYMBOL_RIGHT, "Software information", LV_MENU_ITEM_BUILDER_VARIANT_1);
	//	lv_menu_set_load_page_event(menu, cont, sub_software_info_page);
	//
	//	cont = create_text(section, LV_SYMBOL_RIGHT, "Legal information", LV_MENU_ITEM_BUILDER_VARIANT_1);
	//	lv_menu_set_load_page_event(menu, cont, sub_legal_info_page);
}

static void clear_all_sub_page_child(void)
{
	for (int i = 0; i < lv_obj_get_child_cnt(sub_date_time_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_date_time_page, 0));

	for (int i = 0; i < lv_obj_get_child_cnt(sub_display_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_display_page, 0));

	for (int i = 0; i < lv_obj_get_child_cnt(sub_wifi_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_wifi_page, 0));


	//	for (int i = 0; i < lv_obj_get_child_cnt(sub_about_page); ++i)
	//		lv_obj_del(lv_obj_get_child(sub_about_page, 0));
	//
	//	for (int i = 0; i < lv_obj_get_child_cnt(sub_software_info_page); ++i)
	//		lv_obj_del(lv_obj_get_child(sub_software_info_page, 0));
	//
	//	for (int i = 0; i < lv_obj_get_child_cnt(sub_legal_info_page); ++i)
	//		lv_obj_del(lv_obj_get_child(sub_legal_info_page, 0));
}

static void create_date_time_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	lv_obj_set_style_pad_hor((lv_obj_t *)sub_date_time_page, 20, 0);
	lv_obj_t *section = lv_menu_section_create((lv_obj_t *)sub_date_time_page);
	create_spinbox(section, "Hour:\t", glob_date_time.hour, 0, 23);
	create_spinbox(section, "Minutes:", glob_date_time.minutes, 0, 59);
	create_spinbox(section, "Seconds:", glob_date_time.seconds, 0, 59);
	create_calendar(section);

	lv_obj_t *obj_btn = NULL;
	create_button(section, "Save", 70, 40, &obj_btn);
	lv_obj_add_event_cb(obj_btn, save_date_time, LV_EVENT_CLICKED, section);
}

static void create_display_sub_page(lv_event_t *e)
{
	clear_all_sub_page_child();

	lv_obj_set_style_pad_hor(sub_display_page, 20, 0);
	//    lv_obj_t *separ = lv_menu_separator_create(sub_display_page);
	lv_obj_t *section = lv_menu_section_create(sub_display_page);
	create_slider(section, LV_SYMBOL_SETTINGS, "Brightness", 0, 150, 100);
}

static void create_date_time_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_LIST, "Date and Time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_date_time_page);
	lv_obj_add_event_cb(cont, create_date_time_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_display_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_SETTINGS, "Display", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_display_page);
	lv_obj_add_event_cb(cont, create_display_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_wifi_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_WIFI, "WIFI", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_wifi_page);
	lv_obj_add_event_cb(cont, create_wifi_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_other_pages(void)
{
	create_text(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_obj_t *section = lv_menu_section_create(root_page);
	lv_obj_t *cont = create_text(section, NULL, "About", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, sub_about_page);
}

void settingPageInit(void)
{
	lv_obj_t *scr = lv_obj_create(NULL);

	// Создание объекта Меню
	menu = lv_menu_create(scr);
	lv_obj_set_size(menu, 800, 480);

	// Фон немного "сереем"
	lv_color_t bg_color = lv_obj_get_style_bg_color(menu, 0);
	if(lv_color_brightness(bg_color) > 127)
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 20), 0);
	else
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);

	// Кнопка меню BACK и связывание события нажатия на эту кнопку
	lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_ENABLED);
	lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);

	create_sub_pages();

	lv_obj_t *section;

	//    /*Create a root page*/
	root_page = lv_menu_page_create(menu, root_page_title);
	lv_obj_set_style_pad_hor(root_page, 20, 0);
	section = lv_menu_section_create(root_page);

	create_date_time_page(section);
	create_display_page(section);
	create_wifi_page(section);
	create_other_pages();

	lv_menu_set_sidebar_page(menu, root_page);

	lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_NONE, 0, 0, 1);

	glob_currentPage = PAGE_SETTINGS;

	// открыть первый пункт меню
	//	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);

	printf("heap %u\n", heap_caps_get_free_size(0));
}

void drawSettingPage(void)
{

}

