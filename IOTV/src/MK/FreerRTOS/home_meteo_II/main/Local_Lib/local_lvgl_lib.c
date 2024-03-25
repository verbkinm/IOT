/*
 * local_lvgl_lib.c
 *
 *  Created on: 27 дек. 2023 г.
 *      Author: user
 */

#include "local_lvgl_lib.h"

extern lv_font_t ubuntu_mono_14;

static uint32_t glob_status_reg = 0;
static uint32_t glob_status_err = 0;
static uint32_t glob_update_reg = 0;

static const char *days_str_short[7] = {"вс", "пн", "вт", "ср", "чт", "пт", "сб"};
static const char *days_str_full[7] = {"Воскресенье", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота"};
static const char *btns_txt[] = {YES_STR, NO_STR, NULL};

static void create_dialog_box_accept_handler(lv_event_t * e);

const char **btns_yes_no_matrix(void)
{
	return btns_txt;
}

const char *weekday_name_short(uint8_t day)
{
	day = inRange(day, 0, 6);
	return days_str_short[day];
}

const char *weekday_name_full(uint8_t day)
{
	day = inRange(day, 0, 6);
	return days_str_full[day];
}

lv_obj_t *create_busy_indicator(lv_obj_t *parent, lv_coord_t bg_w, lv_coord_t bg_h, lv_coord_t i_w, lv_coord_t i_h, lv_opa_t opa)
{
	lv_obj_t *spinner_widget = lv_obj_create(parent);
	lv_obj_set_size(spinner_widget, bg_w, bg_h);
	lv_obj_set_style_bg_opa(spinner_widget, opa, 0);

	lv_obj_t *spinner = lv_spinner_create(spinner_widget, 5000, 90);
	lv_obj_set_size(spinner, i_w, i_h);
	lv_obj_center(spinner);

	return spinner_widget;
}

lv_obj_t *create_keyboard(lv_obj_t *parent, lv_align_t align, lv_obj_t *textarea,
		lv_event_cb_t cancel_event_cb,
		lv_event_cb_t ready_event_cb,
		lv_event_cb_t defocused_event_cb)
{
	// клавиатура
	lv_obj_t *kb = lv_keyboard_create(parent);
	lv_obj_set_style_text_font(kb, &ubuntu_mono_14, 0);

	lv_obj_align(kb, align, 0, 0);
	lv_keyboard_set_textarea(kb, textarea);

	if (cancel_event_cb != NULL)
		lv_obj_add_event_cb(kb, cancel_event_cb, LV_EVENT_CANCEL, kb);
	if (ready_event_cb != NULL)
		lv_obj_add_event_cb(kb, ready_event_cb, LV_EVENT_READY, kb);
	if (cancel_event_cb != NULL)
		lv_obj_add_event_cb(textarea, defocused_event_cb, LV_EVENT_DEFOCUSED, kb);

	return kb;
}

void delete_obj_handler(lv_event_t * e)
{
	lv_obj_del(e->user_data);
}

void delete_timer_handler(lv_event_t *e)
{
	lv_timer_del(e->user_data);
}


void hide_obj_handler(lv_event_t *e)
{
	lv_obj_add_flag(e->user_data, LV_OBJ_FLAG_HIDDEN);
}

void show_obj_handler(lv_event_t *e)
{
	lv_obj_clear_flag(e->user_data, LV_OBJ_FLAG_HIDDEN);
}

lv_obj_t *create_msgbox(lv_obj_t *parent, const char *title, const char *txt)
{
	lv_obj_t *mbox = lv_msgbox_create(parent, title, txt, 0, true);
	lv_obj_set_style_text_font(lv_msgbox_get_title(mbox), &ubuntu_mono_14, 0);
	lv_obj_set_style_text_font(lv_msgbox_get_text(mbox), &ubuntu_mono_14, 0);
	lv_obj_center(mbox);

	return mbox;
}

lv_obj_t *create_msgbox_not_connected(void)
{
	return create_msgbox(NULL, ATTENTION_STR, "Сетевое соединение не установлено!");
}

static void create_dialog_box_accept_handler(lv_event_t * e)
{
	lv_obj_t *dialog_box = lv_event_get_current_target(e);

	if (strcmp(lv_msgbox_get_active_btn_text(dialog_box), YES_STR) == 0 && e->user_data != NULL)
	{
		void (*fn)();
		fn = e->user_data;
		fn();
	}
	else
		lv_msgbox_close(dialog_box);
}

lv_obj_t *create_dialog_box(lv_obj_t *parent, const char *title, const char *txt, const char **btns_txt, void (*accept)())
{
	lv_obj_t *mbox = lv_msgbox_create(parent, title, txt, btns_txt, true);
	lv_obj_set_style_text_font(lv_msgbox_get_title(mbox), &ubuntu_mono_14, 0);
	lv_obj_set_style_text_font(lv_msgbox_get_text(mbox), &ubuntu_mono_14, 0);
	lv_obj_set_style_text_font(lv_msgbox_get_btns(mbox), &ubuntu_mono_14, 0);
	lv_obj_center(mbox);

	if (accept != NULL)
		lv_obj_add_event_cb(mbox, create_dialog_box_accept_handler, LV_EVENT_VALUE_CHANGED, accept);

	return mbox;
}

void clear_busy_indicator(lv_obj_t **indicator)
{
	if (*indicator == NULL)
		return;

	lv_obj_del(*indicator);
	*indicator = NULL;
}

lv_obj_t *create_lbl_obj(lv_obj_t *parent, const char* txt,
		const lv_obj_t *align_to_obj, lv_align_t align,
		lv_coord_t x_ofs, lv_coord_t y_ofs,
		lv_color_t color, lv_font_t *font)
{
	lv_obj_t *obj = lv_label_create(parent);

	if (txt != NULL)
		lv_label_set_text_fmt(obj, "%s", txt);

	if (align_to_obj == NULL)
		lv_obj_align(obj, align, x_ofs, y_ofs);
	else
		lv_obj_align_to(obj, align_to_obj, align, x_ofs, y_ofs);

	lv_obj_set_style_text_color(obj, color, 0);
	lv_obj_set_style_text_font(obj, font, 0);
	lv_obj_set_style_text_align(obj, LV_ALIGN_CENTER, 0);

	return obj;
}

lv_obj_t *create_img_obj(lv_obj_t *parent, const char* icon_path,
		const lv_obj_t *align_to_obj, lv_align_t align,
		lv_coord_t w, lv_coord_t h,
		lv_coord_t x_ofs, lv_coord_t y_ofs)
{
	lv_obj_t *obj = lv_img_create(parent);
	lv_obj_set_size(obj, w, h);
	lv_img_set_src(obj, icon_path);
	lv_obj_align_to(obj, align_to_obj, align, x_ofs, y_ofs);

	return obj;
}

uint32_t glob_get_status_reg(void)
{
	return glob_status_reg;
}

void glob_set_status_reg(uint32_t reg)
{
	glob_status_reg = reg;
}

void glob_set_bits_status_reg(uint32_t bits)
{
	glob_status_reg |= bits;
}

void glob_clear_bits_status_reg(uint32_t bits)
{
	glob_status_reg &= ~bits;
}

uint32_t glob_get_update_reg(void)
{
	return glob_update_reg;
}

void glob_set_update_reg(uint32_t reg)
{
	glob_update_reg = reg;
}

void glob_set_bits_update_reg(uint32_t bits)
{
	glob_update_reg |= bits;
}

void glob_clear_bits_update_reg(uint32_t bits)
{
	glob_update_reg &= ~bits;
}

uint32_t glob_get_status_err(void)
{
	return glob_status_err;
}

void glob_set_status_err(uint32_t reg)
{
	glob_status_err = reg;
}

void glob_set_bits_status_err(uint32_t bits)
{
	glob_status_err |= bits;
}

void glob_clear_bits_status_err(uint32_t bits)
{
	glob_status_err &= ~bits;
}

void debug_lv_obj_t_tree(lv_obj_t *obj, int depth)
{
	for (int i = 0; i < lv_obj_get_child_cnt(obj); ++i)
	{
		for (int j = 0; j < depth; ++j)
			printf("\t");
		printf("depth: %d, current obj addr: %p,", depth, lv_obj_get_child(obj, i));
		printf("children: %d\n", (int)(lv_obj_get_child_cnt(lv_obj_get_child(obj, i))));
		debug_lv_obj_t_tree(lv_obj_get_child(obj, i), depth + 1);
	}
}
