/*
 * menupage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */


#include "menupage.h"

extern void homePageInit(void);
extern void settingPageInit(void);
extern uint8_t glob_currentPage;

static lv_obj_t *btnHome;
static lv_obj_t *btnSettings;

static void event_handler(lv_event_t * e)
{
    if (e->current_target == btnHome)
    {
        homePageInit();
        glob_currentPage = PAGE_HOME;
    }
    else if (e->current_target == btnSettings)
    {
    	settingPageInit();
    	glob_currentPage = PAGE_SETTINGS;
    }

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
//    LV_IMG_DECLARE(menu_home);

    lv_obj_t *scr = lv_obj_create(NULL);

    btnHome = lv_btn_create(scr);
    lv_obj_set_size(btnHome, 128, 128);
    lv_obj_align(btnHome, LV_ALIGN_CENTER, -128 - pad, 0);
    lv_obj_add_event_cb(btnHome, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(btnHome);
    lv_label_set_text(label, "Home");
    lv_obj_center(label);

    btnSettings = lv_btn_create(scr);
    lv_obj_set_size(btnSettings, 128, 128);
    lv_obj_align(btnSettings, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btnSettings, event_handler, LV_EVENT_CLICKED, NULL);


    label = lv_label_create(btnSettings);
    lv_label_set_text(label, "Setting");
    lv_obj_center(label);

    lv_obj_add_style(scr, screenStyleDefault(), 0);
    lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_NONE, 0, 0, 1);

    glob_currentPage = PAGE_MENU;
    drawMenuPage();
}

void drawMenuPage(void)
{

}
