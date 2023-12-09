/*
 * status_panel.c
 *
 *  Created on: 9 дек. 2023 г.
 *      Author: user
 */

#include "status_panel.h"

#include "esp_heap_caps.h"

extern uint32_t glob_status_reg;
extern lv_obj_t *glob_status_panel;

static void creat_panel_status_icon(const char *path)
{
	lv_obj_t *icon = lv_img_create(glob_status_panel);
	lv_obj_set_size(icon, 24, 24);
	lv_img_set_src(icon, path);
}

void draw_sd_icon(void)
{
//	if (glob_status_reg & STATUS_WIFI_STA_CONNECTING)
//		creat_panel_status_icon(WIFI_CONNECTING);
//	else
//	{
//		if (glob_status_reg & STATUS_WIFI_STA_CONNECTED)
//			lv_img_set_src(wifi_icon, WIFI_CONNECTED);
//		else
//			lv_img_set_src(wifi_icon, WIFI_DISCONNECTED);
//	}
//
//	lv_label_set_text_fmt(heap_lbl, "%u", heap_caps_get_free_size(0));
}

void draw_wifi_icon(void)
{

}

lv_obj_t *create_status_panel(lv_obj_t *parent)
{
	glob_status_panel = lv_obj_create(parent);
	lv_obj_set_size(glob_status_panel, LCD_H_RES, LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(glob_status_panel, LV_DIR_NONE);
	lv_obj_set_style_pad_all(glob_status_panel, 0, 0);

	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_black());
	lv_style_set_border_width(&style, 0);
	lv_style_set_radius(&style, 0);

	lv_obj_add_style(glob_status_panel, &style, 0);
	lv_obj_set_flex_flow(glob_status_panel, LV_FLEX_FLOW_ROW);

	//...
	creat_panel_status_icon(SD_OFF);
	creat_panel_status_icon(WIFI_DISCONNECTED);

	lv_obj_t *heap_lbl = lv_label_create(glob_status_panel);
//	lv_obj_set_height(heap_lbl, 24);
//	lv_label_set_text_fmt(heap_lbl, "%u", heap_caps_get_free_size(0));


	return glob_status_panel;
}
