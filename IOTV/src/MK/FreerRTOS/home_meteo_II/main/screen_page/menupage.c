/*
 * menupage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */


#include "menupage.h"

//#include "freertos/event_groups.h"

extern lv_font_t ubuntu_mono_14;
//extern lv_font_t monsterat_bold_14;

extern void homePageInit(void);
extern void settingPageInit(void);

static lv_obj_t *btnHome;
static lv_obj_t *btnSettings;

static void event_handler(lv_event_t *e);

static void event_handler(lv_event_t *e)
{
    if (e->current_target == btnHome)
        homePageInit();
    else if (e->current_target == btnSettings)
    	settingPageInit();
}

void menuPageInit(void)
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = menu_page_deinit;
	page->title = page_title(MENU_PAGE_TITLE);
	status_panel_update();

    uint8_t pad = 20;

    lv_obj_t *widget = lv_obj_create(page->widget);
	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_add_style(widget, screenStyleDefault(), 0);

    btnHome = lv_btn_create(widget);
    lv_obj_set_size(btnHome, 128, 128);
    lv_obj_align(btnHome, LV_ALIGN_CENTER, -128 - pad, 0);
    lv_obj_add_event_cb(btnHome, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(btnHome);
    lv_label_set_text(label, "Главная");
    lv_obj_set_style_text_font(label, &ubuntu_mono_14, 0);
    lv_obj_center(label);

    btnSettings = lv_btn_create(widget);
    lv_obj_set_size(btnSettings, 128, 128);
    lv_obj_align(btnSettings, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btnSettings, event_handler, LV_EVENT_CLICKED, NULL);

    label = lv_label_create(btnSettings);
    lv_label_set_text(label, "Настройки");
    lv_obj_set_style_text_font(label, &ubuntu_mono_14, 0);
    lv_obj_center(label);
}

void menu_page_deinit(void)
{
	default_page_deinit();
}
