/*
 * settingpage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "settingpage.h"


extern lv_font_t ubuntu_mono_14;

lv_obj_t *menu;
static lv_obj_t *root_page;

lv_obj_t *sub_date_time_page;
lv_obj_t *sub_sub_time_page;
lv_obj_t *sub_sub_date_page;
lv_obj_t *sub_sub_sntp_page;

lv_obj_t *sub_display_page;
lv_obj_t *sub_wifi_page;
lv_obj_t *sub_weather_page;
lv_obj_t *sub_update_page;
lv_obj_t *sub_sensors_page;

static const char *root_page_title = "Настройки";
static const char *date_time_page_title = "Дата/Время";
static const char *display_page_title = "Дисплей";
static const char *sntp_page_title = "SNTP";
static const char *wifi_page_title = "WIFI";
static const char *time_page_title = "Время";
static const char *date_page_title = "Дата";
static const char *weather_page_title = "Погода";
static const char *update_page_title = "Обновление";
static const char *sensors_page_title = "Датчики";

static void create_sub_pages(void);

static void create_date_time_page(lv_obj_t *parent);
static void create_display_page(lv_obj_t *parent);
static void create_wifi_page(lv_obj_t *parent);
static void create_weather_page(lv_obj_t *parent);
static void create_update_page(lv_obj_t *parent);
static void create_sensors_page(lv_obj_t *parent);

static void create_sub_pages(void)
{
	sub_date_time_page = lv_menu_page_create(menu, (char *)date_time_page_title);
	sub_sub_time_page = lv_menu_page_create(menu, (char *)time_page_title);
	sub_sub_date_page = lv_menu_page_create(menu, (char *)date_page_title);
	sub_sub_sntp_page = lv_menu_page_create(menu, (char *)sntp_page_title);

	sub_display_page = lv_menu_page_create(menu, (char *)display_page_title);
	sub_wifi_page = lv_menu_page_create(menu, (char *)wifi_page_title);
	sub_weather_page = lv_menu_page_create(menu, (char *)weather_page_title);
	sub_update_page = lv_menu_page_create(menu, (char *)update_page_title);
	sub_sensors_page = lv_menu_page_create(menu, (char *)sensors_page_title);
}

void clear_all_sub_page_child(void)
{
	lv_obj_clean(sub_date_time_page);
	lv_obj_clean(sub_sub_time_page);
	lv_obj_clean(sub_sub_date_page);
	lv_obj_clean(sub_sub_sntp_page);
	free_date_time_sub_page();

	lv_obj_clean(sub_display_page);
	free_display_sub_page();

	lv_obj_clean(sub_wifi_page);
	free_wifi_sub_page();

	lv_obj_clean(sub_weather_page);
	free_weather_sub_page();

	lv_obj_clean(sub_update_page);
	free_update_sub_page();

	lv_obj_clean(sub_sensors_page);
	free_sensors_sub_page();
}

static void create_date_time_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_LIST, date_time_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_date_time_page);
	lv_obj_add_event_cb(cont, create_date_time_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_display_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_SETTINGS, display_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_display_page);
	lv_obj_add_event_cb(cont, create_display_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_wifi_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_WIFI, wifi_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_wifi_page);
	lv_obj_add_event_cb(cont, create_wifi_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_weather_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_IMAGE, weather_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_weather_page);
	lv_obj_add_event_cb(cont, create_weather_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_update_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_DOWNLOAD, update_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_update_page);
	lv_obj_add_event_cb(cont, create_update_sub_page, LV_EVENT_CLICKED, cont);
}

static void create_sensors_page(lv_obj_t *parent)
{
	lv_obj_t *cont = create_text(parent, LV_SYMBOL_SETTINGS, sensors_page_title, LV_MENU_ITEM_BUILDER_VAR_1);
	lv_menu_set_load_page_event(menu, cont, sub_sensors_page);
	lv_obj_add_event_cb(cont, create_sensors_sub_page, LV_EVENT_CLICKED, cont);
}

void settingPageInit(void)
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = setting_page_deinit;
	page->title = page_title(SETTING_PAGE_TITLE);
	status_panel_update();

	lv_obj_t *widget = lv_obj_create(page->widget);
	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_add_style(widget, screenStyleDefault(), 0);

	// Создание объекта Меню
	menu = lv_menu_create(widget);
	lv_obj_set_style_text_font(menu, &ubuntu_mono_14, 0);
	lv_obj_set_size(menu, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);

	// Фон немного "сереем"
	lv_color_t bg_color = lv_obj_get_style_bg_color(menu, 0);
	if(lv_color_brightness(bg_color) > 127)
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 20), 0);
	else
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);

	create_sub_pages();

	lv_obj_t *section;
	//    /*Create a root page*/
	root_page = lv_menu_page_create(menu, (char *)root_page_title);
	lv_obj_set_style_pad_hor(root_page, 20, 0);
	section = lv_menu_section_create(root_page);

	create_date_time_page(section);
	create_display_page(section);
	create_wifi_page(section);
	create_weather_page(section);
	create_update_page(section);
	create_sensors_page(section);

	lv_menu_set_sidebar_page(menu, root_page);
}

void setting_page_deinit(void)
{
	clear_all_sub_page_child();

	((lv_menu_page_t *)root_page)->title = NULL;

	((lv_menu_page_t *)sub_date_time_page)->title = NULL;
	((lv_menu_page_t *)sub_sub_time_page)->title = NULL;
	((lv_menu_page_t *)sub_sub_date_page)->title = NULL;
	((lv_menu_page_t *)sub_sub_sntp_page)->title = NULL;

	((lv_menu_page_t *)sub_display_page)->title = NULL;
	((lv_menu_page_t *)sub_wifi_page)->title = NULL;
	((lv_menu_page_t *)sub_weather_page)->title = NULL;
	((lv_menu_page_t *)sub_update_page)->title = NULL;
	((lv_menu_page_t *)sub_sensors_page)->title = NULL;

	default_page_deinit();
}
