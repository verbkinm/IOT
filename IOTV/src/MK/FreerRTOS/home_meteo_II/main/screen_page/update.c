/*
 * updatepage.c
 *
 *  Created on: 24 янв. 2024 г.
 *      Author: user
 */

#include "update.h"

extern lv_font_t ubuntu_mono_48;

lv_obj_t *update_page_init(void)
{
	lv_obj_t *main_widget = lv_obj_create(lv_scr_act());

	lv_obj_set_size(main_widget, LCD_H_RES, LCD_V_RES);
	lv_obj_set_style_radius(main_widget, 0, 0);
	lv_obj_t *img = lv_img_create(main_widget);
	lv_obj_set_size(img, 256, 256);
	lv_obj_center(img);
	lv_img_set_src(img, UPDATE_IMG);

	lv_obj_t *lbl = lv_label_create(main_widget);
	lv_label_set_text(lbl, "Обновление...");
	lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -10);
	lv_obj_set_style_text_font(lbl, &ubuntu_mono_48, 0);

	return main_widget;
}
