/*
 * elements.c
 *
 *  Created on: 5 дек. 2023 г.
 *      Author: user
 */

#include "elements.h"

char *generate_dropdown_number(uint8_t begin, uint8_t end, uint8_t *count)
{
	if (count == NULL)
		return NULL;

	*count = end - begin + 1;
	if (*count == 0)
		goto bad_end;

	char *result = calloc(*count + 1, 5);
	if (result == NULL)
		goto bad_end;

	char buf[5];
	for(uint8_t i = 0; i < (*count - 1); ++i)
	{
		snprintf(buf, sizeof(buf), "%.02d\n", i);
		strcat(result, buf);
	}
	snprintf(buf, sizeof(buf), "%.02d", *count - 1);
	strcat(result, buf);

	return result;

	bad_end:
	count = 0;
	return 0;
}

lv_obj_t *create_text(lv_obj_t * parent, const char * icon, const char *txt, lv_menu_builder_variant_t builder_variant)
{
	lv_obj_t *obj = lv_menu_cont_create(parent);

	lv_obj_t *img = NULL;
	lv_obj_t *label = NULL;

	if (icon)
	{
		img = lv_img_create(obj);
		lv_img_set_src(img, icon);
	}

	if (txt)
	{
		label = lv_label_create(obj);
		lv_label_set_text(label, txt);
		//        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_flex_grow(label, 1);
	}

	if (builder_variant == LV_MENU_ITEM_BUILDER_VAR_2 && icon && txt) {
		lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
		lv_obj_swap(img, label);
	}

	return obj;
}

lv_obj_t *create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val, lv_obj_t **slider)
{
	lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VAR_2);

	*slider = lv_slider_create(obj);
	lv_obj_set_flex_grow(*slider, 1);
	lv_slider_set_range(*slider, min, max);
	lv_slider_set_value(*slider, val, LV_ANIM_OFF);

	if(icon == NULL)
		lv_obj_add_flag(*slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

	return obj;
}

lv_obj_t *create_switch(lv_obj_t *parent, const char * icon, const char * txt, bool chk, lv_obj_t **switch_obj)
{
	lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VAR_1);

	*switch_obj = lv_switch_create(obj);
	lv_obj_add_flag(*switch_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_add_state(*switch_obj, chk ? LV_STATE_CHECKED : 0);

	return obj;
}

lv_obj_t *create_time_block(lv_obj_t *parent, const char *title, lv_coord_t width, lv_coord_t height, time_block_t **time_block)
{
	lv_obj_t *obj = lv_menu_cont_create(parent);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

	lv_obj_t *wrap = lv_obj_create(obj);
	lv_obj_set_style_pad_all(wrap, 0, 0);
	lv_obj_set_scroll_dir(wrap, LV_DIR_NONE);
	lv_obj_set_size(wrap, width, height);
	lv_obj_set_style_border_width(wrap, 0, 0);

	lv_obj_t* lbl = lv_label_create(wrap);
	lv_label_set_text(lbl, title);
	lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);

	*time_block = calloc(1, sizeof(time_block_t));

	char *buf = NULL;
	uint8_t counter;
	buf = generate_dropdown_number(0, 59, &counter);
	//	printf("counter: %d\n", (int)counter);
	//	printf("%s", buf);

	// Секунды
	(*time_block)->s = lv_dropdown_create(wrap);
	lv_dropdown_set_options((*time_block)->s, buf);
	lv_obj_set_size((*time_block)->s, 60, 40);
	lv_obj_align((*time_block)->s, LV_ALIGN_RIGHT_MID, -10, 0);

	lv_obj_t *lbl_2dot = lv_label_create(wrap);
	lv_label_set_text(lbl_2dot, ":");
	lv_obj_align_to(lbl_2dot, (*time_block)->s, LV_ALIGN_OUT_LEFT_MID, -10, 0);

	// Минуты
	(*time_block)->m = lv_dropdown_create(wrap);
	lv_dropdown_set_options((*time_block)->m, buf);
	lv_obj_set_size((*time_block)->m, 60, 40);
	lv_obj_align_to((*time_block)->m, lbl_2dot, LV_ALIGN_OUT_LEFT_MID, -10, 0);

	free(buf);
	buf = generate_dropdown_number(0, 23, &counter);

	lbl_2dot = lv_label_create(wrap);
	lv_label_set_text(lbl_2dot, ":");
	lv_obj_align_to(lbl_2dot, (*time_block)->m, LV_ALIGN_OUT_LEFT_MID, -10, 0);

	// Часы
	(*time_block)->h = lv_dropdown_create(wrap);
	lv_dropdown_set_options((*time_block)->h, buf);
	lv_obj_set_size((*time_block)->h, 60, 40);
	lv_obj_align_to((*time_block)->h, lbl_2dot, LV_ALIGN_OUT_LEFT_MID, -10, 0);

	free(buf);

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
	lv_obj_set_size(wrap, 495, height + 5);
	lv_obj_set_style_pad_all(wrap, 0, 0);

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

lv_obj_t *create_button_simply(lv_obj_t *parent, const char *txt, lv_coord_t width, lv_coord_t height,  lv_font_t *font)
{
	lv_obj_t *btn = lv_btn_create(parent);
	lv_obj_set_size(btn, width, height);

	if (txt != NULL)
	{
		lv_obj_t *lbl = lv_label_create(btn);
		lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
		lv_obj_set_width(lbl, width);
		lv_obj_set_style_text_font(lbl, font, 0);
		lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
		lv_label_set_text(lbl, txt);
		lv_obj_center(lbl);
	}

	return btn;
}

void lv_spinbox_increment_event_cb(lv_event_t * e)
{
	if(e->code == LV_EVENT_SHORT_CLICKED || e->code  == LV_EVENT_LONG_PRESSED_REPEAT)
		lv_spinbox_increment(e->user_data);
}

void lv_spinbox_decrement_event_cb(lv_event_t * e)
{
	if(e->code == LV_EVENT_SHORT_CLICKED || e->code == LV_EVENT_LONG_PRESSED_REPEAT)
		lv_spinbox_decrement(e->user_data);
}

lv_obj_t *create_spinbox(lv_obj_t *parent, char *text, int16_t data, int16_t min, int16_t max,
		uint8_t digit_count, uint8_t separator_position,
		lv_obj_t **spin_box, lv_obj_t **btn_minus, lv_obj_t **btn_plus,
		lv_coord_t w, lv_coord_t h)
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
	lv_obj_align(lbl, LV_ALIGN_LEFT_MID, -10, 0);

	*spin_box = lv_spinbox_create(wrap);
	lv_spinbox_set_rollover(*spin_box, true);
	lv_obj_set_size(*spin_box, w, h);
	lv_spinbox_set_digit_format(*spin_box, digit_count, separator_position);
	lv_spinbox_set_range(*spin_box, min, max);
	lv_spinbox_set_value(*spin_box, data);
	lv_obj_center(*spin_box);

	*btn_plus = lv_btn_create(wrap);
	lv_obj_set_size(*btn_plus, h, h);
	lv_obj_align_to(*btn_plus, *spin_box, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_obj_set_style_bg_img_src(*btn_plus, LV_SYMBOL_PLUS, 0);
	lv_obj_add_event_cb(*btn_plus, lv_spinbox_increment_event_cb, LV_EVENT_ALL,  *spin_box);

	*btn_minus = lv_btn_create(wrap);
	lv_obj_set_size(*btn_minus, h, h);
	lv_obj_align_to(*btn_minus, *spin_box, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_set_style_bg_img_src(*btn_minus, LV_SYMBOL_MINUS, 0);
	lv_obj_add_event_cb(*btn_minus, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, *spin_box);

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
	lv_obj_set_size(wrap, 495, 290);
	lv_obj_set_style_border_color(wrap, lv_color_white(), 0);
	lv_obj_set_scrollbar_mode(wrap, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t  *calendar = lv_calendar_create(wrap);

	lv_obj_set_size(calendar, 285, 285);
	lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(calendar, calendar_event_handler, LV_EVENT_ALL, NULL);

	lv_calendar_header_dropdown_create(calendar);

	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	lv_calendar_set_today_date(calendar, timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
	lv_calendar_set_showed_date(calendar, timeinfo.tm_year + 1900, timeinfo.tm_mon + 1);
	static const char *days_str[] = {"пн", "вт", "ср", "чт", "пт", "сб", "вс"};
	lv_calendar_set_day_names(calendar, &days_str[0]);
	//	LV_CALENDAR_WEEK_STARTS_MONDAY
	return obj;
}
