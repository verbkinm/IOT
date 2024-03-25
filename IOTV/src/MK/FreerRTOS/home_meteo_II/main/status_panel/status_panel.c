/*
 * status_panel.c
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: user
 */

#include "status_panel.h"

extern lv_font_t ubuntu_mono_14;

static lv_obj_t *icon_wifi;
static lv_obj_t *icon_sd;
static lv_obj_t *lbl_mem;
static lv_obj_t *heap_lbl;
static lv_obj_t *menu_btn;
static lv_obj_t *title_lbl;

static void event_handler(lv_event_t *e);

static void event_handler(lv_event_t *e)
{
	menuPageInit();
}

void status_panel_init(void)
{
	// Статус панель
	lv_obj_t *status_panel = lv_obj_create(lv_scr_act());
	lv_obj_set_size(status_panel, LCD_H_RES, LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(status_panel, LV_DIR_NONE);
	lv_obj_set_style_pad_all(status_panel, 0, 0);
	lv_obj_add_style(status_panel, screenStyleDefault(), 0);

	// Свобондо/занято памяти на sd
	lbl_mem  = create_lbl_obj(status_panel, NULL, NULL, LV_ALIGN_RIGHT_MID, -5, 0, lv_color_white(), &ubuntu_mono_14);

	// Статус SD
	icon_sd = lv_img_create(status_panel);
	lv_obj_set_size(icon_sd, 24, 24);
	lv_obj_align_to(icon_sd, lbl_mem, LV_ALIGN_OUT_LEFT_MID, -5, 0);

	// Статус wifi
	icon_wifi = lv_img_create(status_panel);
	lv_obj_set_size(icon_wifi, 24, 24);
	lv_obj_align_to(icon_wifi, icon_sd, LV_ALIGN_OUT_LEFT_MID, -5, 0);

	// Название страницы
	title_lbl = create_lbl_obj(status_panel, current_page()->title, NULL, LV_ALIGN_CENTER, 0, 0, lv_color_white(), &ubuntu_mono_14);

	// Кнопка вызова страницы меню
	menu_btn = create_button_simply(status_panel, LV_SYMBOL_LIST, 64, LCD_PANEL_STATUS_H - 8, &ubuntu_mono_14);
	lv_obj_align(menu_btn, LV_ALIGN_LEFT_MID, 5, 0);

	lv_obj_add_event_cb(menu_btn, event_handler, LV_EVENT_CLICKED, 0);

	// Статус кучи
	heap_lbl = create_lbl_obj(status_panel, NULL, menu_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0, lv_color_white(), &ubuntu_mono_14);
}

void status_panel_update(void)
{
	lv_img_set_src(icon_sd, SD_ON);

	if (glob_get_status_reg() & STATUS_WIFI_STA_START)
	{
		if (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING)
			lv_img_set_src(icon_wifi, WIFI_CONNECTING);
		else
		{
			if (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED)
				lv_img_set_src(icon_wifi, WIFI_CONNECTED);
			else
				lv_img_set_src(icon_wifi, WIFI_ENABLE);
		}
	}
	else
		lv_img_set_src(icon_wifi, WIFI_DISABLE);

	lv_label_set_text_fmt(title_lbl, "%s", current_page()->title);

	uint32_t total, available;
	sd_spi_space_info(&total, &available);

	lv_label_set_text_fmt(lbl_mem, "(%d%%)", (total > 0) ? (int)(100 - (float)available / total * 100) : 0);

	lv_obj_align_to(icon_sd, lbl_mem, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_align_to(icon_wifi, icon_sd, LV_ALIGN_OUT_LEFT_MID, -5, 0);

	lv_label_set_text_fmt(heap_lbl, "%u", heap_caps_get_free_size(0));
}
