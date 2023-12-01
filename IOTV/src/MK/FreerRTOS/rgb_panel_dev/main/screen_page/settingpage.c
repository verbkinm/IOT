/*
 * settingpage.c
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#include "settingpage.h"


extern void menuPageInit(void);

extern uint8_t currentPage;


enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};

typedef uint8_t lv_menu_builder_variant_t;

static void back_event_handler(lv_event_t * e);
static lv_obj_t *menu;
static lv_obj_t *root_page;

static lv_obj_t *sub_date_time_page;
static lv_obj_t *sub_display_page;
static lv_obj_t *sub_about_page;
static lv_obj_t *sub_software_info_page;
static lv_obj_t *sub_legal_info_page;

static lv_obj_t *create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);
static lv_obj_t *create_text2(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);
static lv_obj_t *create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
static lv_obj_t *create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk);

static void create_sub_pages(void);
static void create_date_time_page(lv_obj_t *parent);
static void create_display_page(lv_obj_t *parent);
static void create_other_pages(void);

void lv_example_menu_5(void)
{
    lv_obj_t *scr = lv_obj_create(NULL);

    // Создание объекта Меню
    menu = lv_menu_create(scr);
    lv_obj_set_size(menu, 800, 480);

    // Фон немного "сереем"
    lv_color_t bg_color = lv_obj_get_style_bg_color(menu, 0);
    if(lv_color_brightness(bg_color) > 127)
        lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 10), 0);
    else
        lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);

    // Кнопка меню BACK и связывание события нажатия на эту кнопку
    lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_ENABLED);
    lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);

    create_sub_pages();

    lv_obj_t *section;

//    /*Create a root page*/
    root_page = lv_menu_page_create(menu, "Settings");
    lv_obj_set_style_pad_hor(root_page, 20, 0);
    section = lv_menu_section_create(root_page);

    create_date_time_page(section);
    create_display_page(section);
    create_other_pages();

    lv_menu_set_sidebar_page(menu, root_page);

    lv_scr_load(scr);
}

static void back_event_handler(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *_menu = lv_event_get_user_data(e);

    if(lv_menu_back_btn_is_root(_menu, obj))
    	menuPageInit();
}

static lv_obj_t *create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t * obj = lv_menu_cont_create(parent);

    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;

    if(icon) {
        img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }

    if(txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
//        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL)
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    return obj;
}

static lv_obj_t * create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t * sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

    return obj;
}

static lv_obj_t *create_text2(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t *obj = lv_menu_cont_create(parent);
    lv_obj_t *label = NULL;

    if (txt)
    {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        //        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        //        lv_obj_set_flex_grow(label, 1);
    }

    return obj;
}

static void create_sub_pages(void)
{
    lv_obj_t *cont;
    lv_obj_t *section;

    sub_date_time_page = lv_menu_page_create(menu, "Date and time");
    lv_obj_set_style_pad_hor(sub_date_time_page, 20, 0);
    section = lv_menu_section_create(sub_date_time_page);
    create_text2(section, LV_SYMBOL_SETTINGS, "TEST", LV_MENU_ITEM_BUILDER_VARIANT_1);

    sub_display_page = lv_menu_page_create(menu, "Display");
    lv_obj_set_style_pad_hor(sub_display_page, 20, 0);
    //    lv_obj_t *separ = lv_menu_separator_create(sub_display_page);
    section = lv_menu_section_create(sub_display_page);
    create_slider(section, LV_SYMBOL_SETTINGS, "Brightness", 0, 150, 100);

    sub_software_info_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_software_info_page, 20, 0);
    section = lv_menu_section_create(sub_software_info_page);
    create_text(section, NULL, "Version 1.0", LV_MENU_ITEM_BUILDER_VARIANT_1);

    sub_legal_info_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_legal_info_page, 20, 0);
    section = lv_menu_section_create(sub_legal_info_page);

    for(uint32_t i = 0; i < 15; i++)
    {
        create_text(section, NULL,
                    "This is a long long long long long long long long long text, if it is long enough it may scroll.",
                    LV_MENU_ITEM_BUILDER_VARIANT_1);
    }

    sub_about_page = lv_menu_page_create(menu, NULL);
    lv_obj_set_style_pad_hor(sub_about_page, 20, 0);
    lv_menu_separator_create(sub_about_page);

    section = lv_menu_section_create(sub_about_page);

    cont = create_text(section, LV_SYMBOL_RIGHT, "Software information", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_software_info_page);

    cont = create_text(section, LV_SYMBOL_RIGHT, "Legal information", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_legal_info_page);
}

static void create_date_time_page(lv_obj_t *parent)
{
    lv_obj_t *cont = create_text(parent, LV_SYMBOL_LIST, "Date and Time", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_date_time_page);
}

static void create_display_page(lv_obj_t *parent)
{
    lv_obj_t *cont = create_text(parent, LV_SYMBOL_SETTINGS, "Display", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_display_page);
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
    lv_example_menu_5();
    currentPage = PAGE_SETTINGS;
}

void drawSettingPage(void)
{

}

