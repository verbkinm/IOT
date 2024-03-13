/*
 * local_lvgl_lib.h
 *
 *  Created on: 27 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_LOCAL_LIB_LOCAL_LVGL_LIB_H_
#define MAIN_LOCAL_LIB_LOCAL_LVGL_LIB_H_

#include "lvgl.h"
#include "local_lib.h"
#include "Global_def.h"
#include "screen_page/page.h"

lv_obj_t *create_busy_indicator(lv_obj_t *parent, lv_coord_t bg_w, lv_coord_t bg_h, lv_coord_t i_w, lv_coord_t i_h, lv_opa_t opa);
lv_obj_t *create_keyboard(lv_obj_t *parent, lv_align_t align, lv_obj_t *textarea,
		lv_event_cb_t cancel_event_cb,
		lv_event_cb_t ready_event_cb,
		lv_event_cb_t defocused_event_cb);
lv_obj_t *create_msgbox(lv_obj_t *parent, const char *title, const char *txt);
lv_obj_t *create_msgbox_not_connected(void);
lv_obj_t *create_dialog_box(lv_obj_t *parent, const char *title, const char *txt, const char **btns_txt, void (*accept)());

void clear_busy_indicator(lv_obj_t **indicator);

void delete_obj_handler(lv_event_t * e);
void delete_timer_handler(lv_event_t * e);

void hide_obj_handler(lv_event_t *e);
void show_obj_handler(lv_event_t *e);

lv_obj_t *create_lbl_obj(lv_obj_t *parent, const char* txt, const lv_obj_t *align_to_obj, lv_align_t align,
		lv_coord_t x_ofs, lv_coord_t y_ofs, lv_color_t color, lv_font_t *font);
lv_obj_t *create_img_obj(lv_obj_t *parent, const char* icon_path, const lv_obj_t *align_to_obj, lv_align_t align,
		lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs);

const char *weekday_name_short(uint8_t day);
const char *weekday_name_full(uint8_t day);

uint32_t glob_get_status_reg(void);
void glob_set_status_reg(uint32_t reg);
void glob_set_bits_status_reg(uint32_t bits);
void glob_clear_bits_status_reg(uint32_t bits);

uint32_t glob_get_update_reg(void);
void glob_set_update_reg(uint32_t reg);
void glob_set_bits_update_reg(uint32_t bits);
void glob_clear_bits_update_reg(uint32_t bits);

uint32_t glob_get_status_err(void);
void glob_set_status_err(uint32_t reg);
void glob_set_bits_status_err(uint32_t bits);
void glob_clear_bits_status_err(uint32_t bits);

const char **btns_yes_no_matrix(void);

void debug_lv_obj_t_tree(lv_obj_t *obj, int depth);

#endif /* MAIN_LOCAL_LIB_LOCAL_LVGL_LIB_H_ */
