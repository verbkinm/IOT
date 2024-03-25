/*
 * startscreen.c
 *
 *  Created on: 30 янв. 2024 г.
 *      Author: user
 */

#include "startscreen.h"

extern lv_font_t ubuntu_mono_48;
extern lv_font_t ubuntu_mono_64;

static void anim_x_cb(void *var, int32_t v);
static void create_anim(lv_obj_t *obj, lv_coord_t start, lv_coord_t end, uint32_t duration, uint32_t delay);

static void create_anim(lv_obj_t *obj, lv_coord_t start, lv_coord_t end, uint32_t duration, uint32_t delay)
{
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);
	lv_anim_set_values(&a, start, end);
	lv_anim_set_time(&a, duration);
	lv_anim_set_delay(&a, delay);
	lv_anim_set_exec_cb(&a, anim_x_cb);
	lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
	lv_anim_start(&a);
}

static void anim_x_cb(void *var, int32_t v)
{
	lv_obj_set_x(var, v);
}

void startscreen_init(void)
{
	const esp_app_desc_t *esp_desc = esp_app_get_description();
	char *version = calloc(1, 64);

	strcpy(version, "Версия: ");
	strcat(version, esp_desc->version);

	// Основной виджет
	lv_obj_t *main_widget = lv_obj_create(lv_scr_act());
	lv_obj_set_size(main_widget, LCD_H_RES, LCD_V_RES);
	lv_obj_set_scroll_dir(main_widget, LV_DIR_NONE);
	lv_obj_add_style(main_widget, screenStyleDefault(), 0);

	lv_obj_t *lbl_1 = create_lbl_obj(main_widget, "Метеостанция", NULL, LV_ALIGN_TOP_MID, 0, 50, lv_color_white(), &ubuntu_mono_64);
	lv_obj_t *lbl_2 = create_lbl_obj(main_widget, "Домашняя", NULL, LV_ALIGN_TOP_MID, 0, 150, lv_color_white(), &ubuntu_mono_64);
	lv_obj_t *lbl_3 = create_lbl_obj(main_widget, "II", NULL, LV_ALIGN_TOP_MID, 0, 250, lv_color_white(), &ubuntu_mono_64);
	lv_obj_t *lbl_4 = create_lbl_obj(main_widget, version, NULL, LV_ALIGN_TOP_MID, 0, 370, lv_color_white(), &ubuntu_mono_48);
	free(version);

	create_anim(lbl_1, -1000, lv_obj_get_x(lbl_1), 1000, 1000);
	create_anim(lbl_2, -1000, lv_obj_get_x(lbl_2), 1000, 2000);
	create_anim(lbl_3, -1000, lv_obj_get_x(lbl_3), 1000, 3000);

	create_anim(lbl_4, -1000, lv_obj_get_x(lbl_4), 1000, 4000);
}
