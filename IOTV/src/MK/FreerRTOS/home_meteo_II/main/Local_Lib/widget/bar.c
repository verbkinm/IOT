/*
 * bar.c
 *
 *  Created on: 15 февр. 2024 г.
 *      Author: user
 */


#include "bar.h"

#define MY_BAR_LBL_FMT_SIZE 32

typedef struct My_Bar {
	lv_obj_t *bar;
	lv_obj_t *indic;
	lv_obj_t *lbl;
	lv_timer_t *timer;
	lv_grad_dsc_t *grad;
	lv_style_t *style_indic;
	float val;
	float min;
	float max;
	char lbl_fmt[MY_BAR_LBL_FMT_SIZE];
} my_bar_t;

static const char *LBL_FMT = "%.2f";

static lv_obj_t *create_bar(lv_obj_t *parent, lv_grad_dsc_t *grad, lv_style_t *style_indic);
static lv_obj_t *create_indic_line(lv_obj_t *parent);
static lv_obj_t *create_indic_label(lv_obj_t *parent);

static void set_bar_val(void * bar, int32_t val);
static void set_y_and_lbl(void *obj, int32_t y);

void set_bar_val(void *bar, int32_t val)
{
	lv_bar_set_value((lv_obj_t *)bar, val, LV_ANIM_ON);
}

static void set_y_and_lbl(void *obj, int32_t y)
{
	lv_obj_set_y(obj, y - lv_obj_get_height(obj) / 2);

	my_bar_t *my_bar_widgets = lv_obj_get_parent(obj)->user_data;
	int h = lv_obj_get_height(my_bar_widgets->bar);

	float val = convert_range(y, h, 0, my_bar_widgets->min, my_bar_widgets->max);

	lv_label_set_text_fmt(my_bar_widgets->lbl, my_bar_widgets->lbl_fmt, val);
}

static lv_obj_t *create_bar(lv_obj_t *parent, lv_grad_dsc_t *grad, lv_style_t *style_indic)
{
	// default arrays
	lv_color_t color[LV_GRADIENT_MAX_STOPS] = {lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_ORANGE), lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_LIGHT_GREEN), lv_palette_main(LV_PALETTE_LIGHT_BLUE), lv_palette_main(LV_PALETTE_BLUE)};
	uint8_t frac[LV_GRADIENT_MAX_STOPS] = {70, 100, 130, 140, 170, 200};

	grad->dir = LV_GRAD_DIR_VER;
	grad->stops_count = LV_GRADIENT_MAX_STOPS;

	for (int i = 0; i < LV_GRADIENT_MAX_STOPS; ++i)
	{
		grad->stops[i].color = color[i];
		grad->stops[i].frac  = frac[i];
	}

	lv_style_init(style_indic);
	lv_style_set_bg_grad(style_indic, grad);

	lv_obj_t *bar = lv_bar_create(parent);
	lv_obj_add_style(bar, style_indic, LV_PART_INDICATOR);
	lv_bar_set_value(bar, 0, LV_ANIM_OFF);
//    lv_obj_set_style_shadow_width(bar, 20, 0);
//    lv_obj_set_style_shadow_color(bar, lv_palette_main(LV_PALETTE_BLUE), 0);
	lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);

	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_exec_cb(&a, set_bar_val);
	lv_anim_set_time(&a, 2000);
	lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
	lv_anim_set_var(&a, bar);
	lv_anim_set_values(&a, 0, 100);
	lv_anim_start(&a);

	return bar;
}

static lv_obj_t *create_indic_label(lv_obj_t *parent)
{
	lv_obj_t *lbl = lv_label_create(parent);
	lv_obj_align(lbl, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_label_set_text(lbl, "0");
	lv_obj_set_style_text_color(lbl, lv_color_white(), 0);

	return lbl;
}

static lv_obj_t *create_indic_line(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent);
	lv_obj_set_scroll_dir(obj, LV_DIR_NONE);
	lv_obj_set_style_border_width(obj, 5, 0);
	lv_obj_set_style_border_color(obj, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
	lv_obj_set_style_pad_all(obj, 0, 0);
	lv_obj_set_size(obj, 0, 0);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 0);

	return obj;
}

static void my_bar_resize_handler(lv_event_t * e)
{
	lv_obj_t *parent = e->target;
	my_bar_t *my_bar_widgets = e->user_data;

	lv_coord_t w = lv_obj_get_width(parent);
	lv_coord_t h = lv_obj_get_height(parent);

	lv_obj_set_width(my_bar_widgets->bar, w * 0.3);
	lv_obj_set_height(my_bar_widgets->bar, h * 0.75);

	lv_obj_set_width(my_bar_widgets->indic, w * 0.7);
	lv_obj_set_height(my_bar_widgets->indic, w * 0.1);
}

static void my_bar_widgets_delete(lv_event_t *e)
{
	my_bar_t *my_bar_widgets = (my_bar_t *)e->user_data;
	free(my_bar_widgets->grad);
	free(my_bar_widgets->style_indic);

	free(my_bar_widgets);
}

lv_obj_t *my_bar_create(lv_obj_t *parent)
{
	lv_obj_t *my_bar = lv_obj_create(parent);
	//	lv_obj_set_style_pad_all(my_bar, 0, 0);
	lv_obj_set_scroll_dir(my_bar, LV_DIR_NONE);
	lv_obj_set_style_bg_opa(my_bar, 0, 0);
	lv_obj_set_style_border_width(my_bar, 0, 0);

	my_bar_t *my_bar_widgets = calloc(1, sizeof(my_bar_t));
	lv_obj_set_user_data(my_bar, my_bar_widgets);

	my_bar_widgets->grad = calloc(1, sizeof(lv_grad_dsc_t));
	my_bar_widgets->style_indic = calloc(1, sizeof(lv_style_t));

	my_bar_widgets->bar = create_bar(my_bar, my_bar_widgets->grad, my_bar_widgets->style_indic);
	my_bar_widgets->indic = create_indic_line(my_bar);
	my_bar_widgets->lbl = create_indic_label(my_bar);

	strcpy(my_bar_widgets->lbl_fmt, LBL_FMT);

	lv_obj_add_event_cb(my_bar, my_bar_resize_handler, LV_EVENT_SIZE_CHANGED, my_bar_widgets);
	lv_obj_add_event_cb(my_bar, my_bar_widgets_delete, LV_EVENT_DELETE, my_bar_widgets);

	return my_bar;
}

void my_bar_set_range(lv_obj_t *my_bar, float min, float max)
{
	my_bar_t *my_bar_widgets = my_bar->user_data;
	my_bar_widgets->min = min;
	my_bar_widgets->max = max;
}

void my_bar_set_value(lv_obj_t *my_bar, float value)
{
	my_bar_t *my_bar_widgets = my_bar->user_data;

	float min = my_bar_widgets->min;
	float max = my_bar_widgets->max;

	value = inRange(value, min, max);

	my_bar_widgets->val = value;

	int h = lv_obj_get_height(my_bar_widgets->bar);
	float y = convert_range(value, min, max, h, 0);

	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_exec_cb(&a, set_y_and_lbl);
	lv_anim_set_time(&a, 1000);
	lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
	lv_anim_set_var(&a, my_bar_widgets->indic);
	int pos_y = lv_obj_get_y(my_bar_widgets->indic);
	lv_anim_set_values(&a, pos_y + lv_obj_get_height(my_bar_widgets->indic) / 2, y);
	lv_anim_set_early_apply(&a, true);
	lv_anim_start(&a);
}

void my_bar_get_range(lv_obj_t *my_bar, float *min, float *max)
{
	if (my_bar == NULL || min == NULL || max == NULL)
		return;

	my_bar_t *my_bar_widgets = my_bar->user_data;
	*min = my_bar_widgets->min;
	*max = my_bar_widgets->max;
}

void my_bar_set_gradient_data(lv_obj_t *my_bar, lv_color_t color[LV_GRADIENT_MAX_STOPS], uint8_t frac[LV_GRADIENT_MAX_STOPS])
{
	my_bar_t *my_bar_widgets = my_bar->user_data;

	for (int i = 0; i < LV_GRADIENT_MAX_STOPS; ++i)
	{
		my_bar_widgets->grad->stops[i].color = color[i];
		my_bar_widgets->grad->stops[i].frac  = frac[i];
	}
}

void my_bar_set_font(lv_obj_t *my_bar, const lv_font_t *font)
{
	my_bar_t *my_bar_widgets = my_bar->user_data;
	lv_obj_set_style_text_font(my_bar_widgets->lbl, font, 0);
}

void my_bar_set_fmt_lbl(lv_obj_t *my_bar, const char *fmt)
{
	my_bar_t *my_bar_widgets = my_bar->user_data;
	strncpy(my_bar_widgets->lbl_fmt, fmt, sizeof(my_bar_widgets->lbl_fmt) - 1);
}
