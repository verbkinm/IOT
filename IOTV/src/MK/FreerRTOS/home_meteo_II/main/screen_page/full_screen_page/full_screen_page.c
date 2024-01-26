/*
 * full_screen_page.c
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: user
 */

#include "full_screen_page.h"

extern lv_font_t ubuntu_mono_48;

void full_screen_page_init(const char *txt, void *img)
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = full_screen_page_deinit;

	lv_obj_clean(lv_scr_act());
	lv_obj_clean(lv_layer_top());
	lv_obj_t *main_widget = lv_obj_create(lv_scr_act());
	page->widget = main_widget;

	lv_obj_set_size(main_widget, LCD_H_RES, LCD_V_RES);
	lv_obj_set_style_radius(main_widget, 0, 0);
	lv_obj_t *img_sd = lv_img_create(main_widget);
	lv_obj_set_size(img_sd, 256, 256);
	lv_obj_center(img_sd);
	lv_img_set_src(img_sd, img);

	lv_obj_t *lbl = lv_label_create(main_widget);
	lv_label_set_text(lbl, txt);
	lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, -10);
	lv_obj_set_style_text_font(lbl, &ubuntu_mono_48, 0);
}

void full_screen_page_deinit(void)
{
	default_page_deinit();
}
