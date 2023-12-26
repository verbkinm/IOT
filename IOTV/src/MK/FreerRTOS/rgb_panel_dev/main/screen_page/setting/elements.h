/*
 * elements.h
 *
 *  Created on: 5 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_ELEMENTS_H_
#define MAIN_SCREEN_PAGE_SETTING_ELEMENTS_H_

#include "lvgl.h"
#include <time.h>
#include "Global_def.h"
#include "I2C/DS3231.h"


typedef uint8_t lv_menu_builder_variant_t;

enum {
	LV_MENU_ITEM_BUILDER_VAR_1,
	LV_MENU_ITEM_BUILDER_VAR_2
};

struct Time_block {
	lv_obj_t *h;
	lv_obj_t *m;
	lv_obj_t *s;
};
typedef struct Time_block time_block_t;

char *generate_dropdown_number(uint8_t begin, uint8_t end, uint8_t *count);
lv_obj_t *create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);
lv_obj_t *create_spinbox(lv_obj_t *parent, char *text, uint8_t data, uint8_t min, uint8_t max);
lv_obj_t *create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val, lv_obj_t **slider);
lv_obj_t *create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk, lv_obj_t **switch_obj);
lv_obj_t *create_button(lv_obj_t *parent, const char *txt, lv_coord_t width, lv_coord_t height, lv_obj_t **obj_btn);
lv_obj_t *create_button_simply(lv_obj_t *parent, const char *txt, lv_coord_t width, lv_coord_t height);
lv_obj_t *create_list(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_obj_t **obj_list);
lv_obj_t *create_calendar(lv_obj_t *parent);
lv_obj_t *create_msgbox(lv_obj_t *parent, const char *title, const char *txt);
lv_obj_t *create_time_block(lv_obj_t *parent, const char *title, lv_coord_t width, lv_coord_t height, time_block_t **time_block);
lv_obj_t *create_busy_indicator(lv_obj_t *parent, lv_coord_t bg_w, lv_coord_t bg_h, lv_coord_t i_w, lv_coord_t i_h, lv_opa_t opa);
lv_obj_t *create_keyboard(lv_obj_t *parent, lv_align_t align, lv_obj_t *textarea,
		lv_event_cb_t cancel_event_cb,
		lv_event_cb_t ready_event_cb,
		lv_event_cb_t defocused_event_cb);
void clear_busy_indicator(lv_obj_t **indicator);

void delete_obj_handler(lv_event_t * e);
void delete_timer_handler(lv_event_t * e);

void hide_obj_handler(lv_event_t *e);
void show_obj_handler(lv_event_t *e);

#endif /* MAIN_SCREEN_PAGE_SETTING_ELEMENTS_H_ */
