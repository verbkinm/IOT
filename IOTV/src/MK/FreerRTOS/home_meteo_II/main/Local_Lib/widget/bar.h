/*
 * bar.h
 *
 *  Created on: 15 февр. 2024 г.
 *      Author: user
 */

#ifndef MAIN_LOCAL_LIB_WIDGET_BAR_H_
#define MAIN_LOCAL_LIB_WIDGET_BAR_H_

#ifndef MY_BAR_H
#define MY_BAR_H

#include "lvgl.h"
#include "stdint.h"
#include "stdbool.h"
#include "Local_Lib/local_lib.h"

lv_obj_t *my_bar_create(lv_obj_t *parent);

void my_bar_set_range(lv_obj_t *my_bar, float min, float max);
void my_bar_get_range(lv_obj_t *my_bar, float *min, float *max);

void my_bar_set_value(lv_obj_t *my_bar, float value);

void my_bar_set_gradient_data(lv_obj_t *my_bar, lv_color_t color[LV_GRADIENT_MAX_STOPS], uint8_t frac[LV_GRADIENT_MAX_STOPS]);
void my_bar_set_font(lv_obj_t *my_bar, const lv_font_t *font);
void my_bar_set_fmt_lbl(lv_obj_t *my_bar, const char *fmt);

#endif // MY_BAR_H


#endif /* MAIN_LOCAL_LIB_WIDGET_BAR_H_ */
