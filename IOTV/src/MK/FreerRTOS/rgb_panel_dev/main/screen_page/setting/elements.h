/*
 * elements.h
 *
 *  Created on: 5 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_ELEMENTS_H_
#define MAIN_SCREEN_PAGE_SETTING_ELEMENTS_H_

#include "lvgl.h"
#include "Global_def.h"
#include "I2C/DS3231.h"
#include "esp_wifi.h"

typedef uint8_t lv_menu_builder_variant_t;

enum {
	LV_MENU_ITEM_BUILDER_VARIANT_1,
	LV_MENU_ITEM_BUILDER_VARIANT_2
};

lv_obj_t *create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);
lv_obj_t *create_spinbox(lv_obj_t *parent, char *text, uint8_t data, uint8_t min, uint8_t max);
lv_obj_t *create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
lv_obj_t *create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk, lv_obj_t **switch_obj);
lv_obj_t *create_button(lv_obj_t *parent, const char *txt, lv_coord_t width, lv_coord_t height, lv_obj_t **obj_btn);
lv_obj_t *create_list(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_obj_t **obj_list);
lv_obj_t *create_calendar(lv_obj_t *parent);

#endif /* MAIN_SCREEN_PAGE_SETTING_ELEMENTS_H_ */
