/*
 * settingpage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "settingpage.h"


extern void menuPageInit(void);

extern uint8_t glob_currentPage;

lv_obj_t *menu;
static lv_obj_t *root_page;


lv_obj_t *sub_date_time_page;
lv_obj_t *sub_sub_time_page;
lv_obj_t *sub_sub_date_page;
lv_obj_t *sub_sub_sntp_page;

lv_obj_t *sub_display_page;
lv_obj_t *sub_wifi_page;

lv_obj_t *sub_about_page;
//static lv_obj_t *sub_software_info_page;
//static lv_obj_t *sub_legal_info_page;

static char *root_page_title = "Settings";
static char *date_time_page_title = "Date and time";
static char *display_page_title = "Display";
static char *sntp_page_title = "SNTP";
static char *wifi_page_title = "WIFI";
static char *time_page_title = "Time";
static char *date_page_title = "Date";

static void back_event_handler(lv_event_t * e);
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
		((lv_menu_page_t *)sub_sub_time_page)->title = NULL;
		((lv_menu_page_t *)sub_sub_date_page)->title = NULL;
		((lv_menu_page_t *)sub_sub_sntp_page)->title = NULL;

		((lv_menu_page_t *)sub_display_page)->title = NULL;
		((lv_menu_page_t *)sub_wifi_page)->title = NULL;

		menuPageInit();
		free_wifi_sub_page();
	}
}

//static void debug_lv_obj_t_tree(lv_obj_t *obj, int depth)
//{
//	for (int i = 0; i < lv_obj_get_child_cnt(obj); ++i)
//	{
//		for (int j = 0; j < depth; ++j)
//			printf("\t");
//		printf("depth: %d, current obj addr: %p,", depth, lv_obj_get_child(obj, i));
//		printf("children: %d\n", (int)(lv_obj_get_child_cnt(lv_obj_get_child(obj, i))));
//		debug_lv_obj_t_tree(lv_obj_get_child(obj, i), depth + 1);
//	}
//}

static void create_sub_pages(void)
{
	sub_date_time_page = lv_menu_page_create(menu, date_time_page_title);
	sub_sub_time_page = lv_menu_page_create(menu, time_page_title);
	sub_sub_date_page = lv_menu_page_create(menu, date_page_title);
	sub_sub_sntp_page = lv_menu_page_create(menu, sntp_page_title);

	sub_display_page = lv_menu_page_create(menu, display_page_title);
	sub_wifi_page = lv_menu_page_create(menu, wifi_page_title);
}

void clear_all_sub_page_child(void)
{
	for (int i = 0; i < lv_obj_get_child_cnt(sub_date_time_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_date_time_page, 0));

	for (int i = 0; i < lv_obj_get_child_cnt(sub_sub_time_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_sub_time_page, 0));

	for (int i = 0; i < lv_obj_get_child_cnt(sub_sub_date_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_sub_date_page, 0));

	for (int i = 0; i < lv_obj_get_child_cnt(sub_sub_sntp_page); ++i)
		lv_obj_del(lv_obj_get_child(sub_sub_sntp_page, 0));




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
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_LIST, "Date and Time", LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_date_time_page);
	lv_obj_add_event_cb(cont, create_date_time_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_display_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_SETTINGS, "Display", LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_display_page);
	lv_obj_add_event_cb(cont, create_display_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_wifi_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_WIFI, "WIFI", LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_wifi_page);
	lv_obj_add_event_cb(cont, create_wifi_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_other_pages(void)
{
	create_text(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VAR_1);
	lv_obj_t *section = lv_menu_section_create(root_page);
	lv_obj_t *cont = create_text(section, NULL, "About", LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_about_page);
}

void settingPageInit(void)
{
	glob_currentPage = PAGE_NONE;

	lv_obj_t *glob_main_widget = lv_obj_get_child(lv_scr_act(), 1);

	// Создание объекта Меню
	menu = lv_menu_create(glob_main_widget);
	lv_obj_set_size(menu, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);

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

	glob_currentPage = PAGE_SETTINGS;

	// открыть первый пункт меню
	//	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
}

void drawSettingPage(void)
{

}

