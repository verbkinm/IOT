/*
 * menupage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */


#include "menupage.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/event_groups.h"

extern lv_font_t ubuntu_mono_14;
//extern lv_font_t monsterat_bold_14;

extern void homePageInit(void);
extern void settingPageInit(void);

extern uint8_t glob_currentPage;

static lv_obj_t *btnHome;
static lv_obj_t *btnSettings;

static void event_handler(lv_event_t * e)
{
	printf("event heandler menu page\n");

    if (e->current_target == btnHome)
    {
        homePageInit();
    }
    else if (e->current_target == btnSettings)
    {
    	settingPageInit();
    	glob_currentPage = PAGE_SETTINGS;
    }

    lv_obj_remove_event_cb(e->user_data, event_handler);

//    lv_event_code_t code = lv_event_get_code(e);

//    if(code == LV_EVENT_CLICKED) {
//        LV_LOG_USER("Clicked");
//    }
//    else if(code == LV_EVENT_VALUE_CHANGED) {
//        LV_LOG_USER("Toggled");
//    }
}

void menuPageInit(void)
{
    uint8_t pad = 20;

	glob_currentPage = PAGE_NONE;

    lv_obj_t *glob_main_widget = lv_obj_get_child(lv_scr_act(), 1);
    lv_obj_clean(glob_main_widget);

    btnHome = lv_btn_create(glob_main_widget);
    lv_obj_set_size(btnHome, 128, 128);
    lv_obj_align(btnHome, LV_ALIGN_CENTER, -128 - pad, 0);
    lv_obj_add_event_cb(btnHome, event_handler, LV_EVENT_CLICKED, glob_main_widget);

    lv_obj_t *label = lv_label_create(btnHome);
    lv_label_set_text(label, "Главная");
    lv_obj_set_style_text_font(label, &ubuntu_mono_14, 0);
    lv_obj_center(label);

    btnSettings = lv_btn_create(glob_main_widget);
    lv_obj_set_size(btnSettings, 128, 128);
    lv_obj_align(btnSettings, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btnSettings, event_handler, LV_EVENT_CLICKED, glob_main_widget);

    label = lv_label_create(btnSettings);
    lv_label_set_text(label, "Настройки");
    lv_obj_set_style_text_font(label, &ubuntu_mono_14, 0);
    lv_obj_center(label);

    glob_currentPage = PAGE_MENU;
    drawMenuPage();
}

void drawMenuPage(void)
{

}
