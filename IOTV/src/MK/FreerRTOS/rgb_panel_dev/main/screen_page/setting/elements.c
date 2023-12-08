/*
 * elements.c
 *
 *  Created on: 5 дек. 2023 г.
 *      Author: user
 */

#include "elements.h"

extern struct DateTime glob_date_time;

lv_obj_t *create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant)
{
	lv_obj_t * obj = lv_menu_cont_create(parent);

	lv_obj_t * img = NULL;
	lv_obj_t * label = NULL;

	if(icon) {
		img = lv_img_create(obj);
		lv_img_set_src(img, icon);
	}

	if(txt) {
		label = lv_label_create(obj);
		lv_label_set_text(label, txt);
		//        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_flex_grow(label, 1);
	}

	if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
		lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
		lv_obj_swap(img, label);
	}

	return obj;
}

lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val)
{
	lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

	lv_obj_t * slider = lv_slider_create(obj);
	lv_obj_set_flex_grow(slider, 1);
	lv_slider_set_range(slider, min, max);
	lv_slider_set_value(slider, val, LV_ANIM_OFF);

	if(icon == NULL)
		lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

	return obj;
}

lv_obj_t *create_switch(lv_obj_t *parent, const char * icon, const char * txt, bool chk, lv_obj_t **switch_obj)
{
	lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

	*switch_obj = lv_switch_create(obj);
	lv_obj_add_flag(*switch_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_add_state(*switch_obj, chk ? LV_STATE_CHECKED : 0);

	return obj;
}

lv_obj_t *create_list(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_obj_t **obj_list)
{
	lv_obj_t *obj = lv_menu_cont_create(parent);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
	lv_obj_t *wrap = lv_obj_create(obj);
	lv_obj_set_style_pad_all(wrap, 0, 0);
	lv_obj_set_scroll_dir(wrap, LV_DIR_NONE);
	lv_obj_set_size(wrap, width, height);
	lv_obj_set_style_border_width(wrap, 0, 0);
	*obj_list = lv_list_create(wrap);
	lv_obj_set_scroll_dir(*obj_list, LV_DIR_VER);
	lv_obj_set_size(*obj_list, width, height);
	lv_obj_set_style_pad_all(*obj_list, 0, 0);

	return obj;
}

lv_obj_t *create_button(lv_obj_t *parent, const char *txt, lv_coord_t width, lv_coord_t height, lv_obj_t **obj_btn)
{
	lv_obj_t *obj = lv_menu_cont_create(parent);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
	lv_obj_t *wrap = lv_obj_create(obj);
	lv_obj_set_size(wrap, 495, 45);

	*obj_btn = lv_btn_create(wrap);
	lv_obj_set_style_border_color(wrap, lv_color_white(), 0);
	lv_obj_set_scrollbar_mode(wrap, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(*obj_btn, width, height);
	lv_obj_align(*obj_btn, LV_ALIGN_RIGHT_MID, 0, 0);

	lv_obj_t *btn_lbl = lv_label_create(*obj_btn);
	lv_label_set_text(btn_lbl, txt);
	lv_obj_align(btn_lbl, LV_ALIGN_CENTER, 0, 0);

	return obj;
}

void lv_spinbox_increment_event_cb(lv_event_t * e)
{
	if(e->code == LV_EVENT_SHORT_CLICKED || e->code  == LV_EVENT_LONG_PRESSED_REPEAT)
	{
		lv_spinbox_t *spinbox = (lv_spinbox_t *)e->user_data;
		if (spinbox->value == spinbox->range_max)
			lv_spinbox_set_value(e->user_data, spinbox->range_min);
		else
			lv_spinbox_increment(e->user_data);
	}
}

void lv_spinbox_decrement_event_cb(lv_event_t * e)
{
	if(e->code == LV_EVENT_SHORT_CLICKED || e->code == LV_EVENT_LONG_PRESSED_REPEAT)
	{
		lv_spinbox_t *spinbox = (lv_spinbox_t *)e->user_data;
		if (spinbox->value == spinbox->range_min)
			lv_spinbox_set_value(e->user_data, spinbox->range_max);
		else
			lv_spinbox_decrement(e->user_data);
	}
}

lv_obj_t *create_spinbox(lv_obj_t *parent, char *text, uint8_t data, uint8_t min, uint8_t max)
{
	lv_obj_t *obj = lv_menu_cont_create(parent);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

	lv_obj_t *wrap = lv_obj_create(obj);
	lv_obj_set_size(wrap, 495, 50);
	lv_obj_set_style_border_color(wrap, lv_color_white(), 0);
	lv_obj_set_scrollbar_mode(wrap, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *lbl = lv_label_create(wrap);
	lv_label_set_text(lbl, text);
	lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);

	lv_obj_t *spin_box = lv_spinbox_create(wrap);
	lv_obj_set_width(spin_box, 50);
	lv_spinbox_set_range(spin_box, min, max);
	lv_spinbox_set_digit_format(spin_box, 2, 0);
	lv_spinbox_set_value(spin_box, data);
	//	lv_spinbox_step_prev(spin_box);
	lv_obj_center(spin_box);
	int32_t h = lv_obj_get_height(spin_box);

	lv_obj_t *btn = lv_btn_create(wrap);
	lv_obj_set_size(btn, h, h);
	lv_obj_align_to(btn, spin_box, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLUS, 0);
	lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb, LV_EVENT_ALL,  spin_box);

	btn = lv_btn_create(wrap);
	lv_obj_set_size(btn, h, h);
	lv_obj_align_to(btn, spin_box, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_MINUS, 0);
	lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, spin_box);

	return obj;
}

void calendar_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *calendar = lv_event_get_current_target(e);

	if(code == LV_EVENT_VALUE_CHANGED)
	{
		lv_calendar_date_t date;
		if(lv_calendar_get_pressed_date(calendar, &date))
		{
			lv_calendar_set_today_date(calendar, date.year, date.month, date.day);
			printf("Clicked date: %04d.%02d.%02d\n", date.year, date.month, date.day);
		}
	}
}

lv_obj_t *create_calendar(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_menu_cont_create(parent);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

	lv_obj_t *wrap = lv_obj_create(obj);
	lv_obj_set_size(wrap, 495, 310);
	lv_obj_set_style_border_color(wrap, lv_color_white(), 0);
	lv_obj_set_scrollbar_mode(wrap, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t  *calendar = lv_calendar_create(wrap);

	lv_obj_set_size(calendar, 300, 300);
	lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(calendar, calendar_event_handler, LV_EVENT_ALL, NULL);

	lv_calendar_header_dropdown_create(calendar);

	lv_calendar_set_today_date(calendar, glob_date_time.year + 1900, glob_date_time.month, glob_date_time.date);
	lv_calendar_set_showed_date(calendar, glob_date_time.year + 1900, glob_date_time.month);

	return obj;
}

void delete_obj_handler(lv_event_t * e)
{
	lv_obj_del(e->user_data);
	e->user_data = NULL;
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

void clear_busy_indicator(lv_obj_t **indicator)
{
	if (*indicator == NULL)
		return;

	lv_obj_clean(*indicator);
	lv_obj_del(*indicator);
	*indicator = NULL;
}
