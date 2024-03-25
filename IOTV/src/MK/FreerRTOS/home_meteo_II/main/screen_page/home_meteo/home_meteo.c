/*
 * home_meteo.c
 *
 *  Created on: 15 февр. 2024 г.
 *      Author: user
 */

#include "home_meteo.h"

extern lv_font_t ubuntu_mono_48;

static lv_obj_t *bar_t, *bar_h, *bar_p;
static lv_timer_t *timer = NULL;

static void timer_handler(lv_timer_t *timer);

static void timer_handler(lv_timer_t *timer)
{
	const struct THP *thp = BME280_service_get_value();

	my_bar_set_value(bar_t, thp->temperature);
	my_bar_set_value(bar_h, thp->humidity);
	my_bar_set_value(bar_p, thp->pressure);
}

void home_meteo_page_init(void)
{
	page_t *page = current_page();
	page->deinit();
	page->deinit = home_meteo_page_deinit;
	page->title = page_title(HOME_METEO_PAGE_TITLE);
	status_panel_update();

	lv_obj_t *widget = lv_obj_create(page->widget);
	lv_obj_set_size(widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(widget, 0, 0);
	lv_obj_add_style(widget, screenStyleDefault(), 0);

	// Влажность
	bar_h = my_bar_create(widget);
	lv_obj_set_size(bar_h, 200, 440);
	my_bar_set_range(bar_h, 0, 100);
	my_bar_set_value(bar_h, 100);
	my_bar_set_fmt_lbl(bar_h, "%.1f %%");
	my_bar_set_font(bar_h, &ubuntu_mono_48);
	lv_obj_align(bar_h, LV_ALIGN_TOP_MID, 0, 0);

    lv_color_t color_h[LV_GRADIENT_MAX_STOPS] = {lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_ORANGE), lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_ORANGE), lv_palette_main(LV_PALETTE_RED)};
    uint8_t frac_h[LV_GRADIENT_MAX_STOPS] = {20, 70, 102, 153, 180, 235};
    my_bar_set_gradient_data(bar_h, color_h, frac_h);

    // Температура
	bar_t = my_bar_create(widget);
	lv_obj_set_size(bar_t, 200, 440);
	my_bar_set_range(bar_t,-40, 85);
	my_bar_set_fmt_lbl(bar_t, "%+.1f°C");
	my_bar_set_font(bar_t, &ubuntu_mono_48);
	lv_obj_align_to(bar_t, bar_h, LV_ALIGN_OUT_LEFT_MID, -70, 0);

    // Давление
	bar_p = my_bar_create(widget);
	lv_obj_set_size(bar_p, 200, 440);
	my_bar_set_range(bar_p, 700, 800);
	my_bar_set_fmt_lbl(bar_p, "%.0f мм");
	my_bar_set_font(bar_p, &ubuntu_mono_48);
	lv_obj_align_to(bar_p, bar_h, LV_ALIGN_OUT_RIGHT_MID, 70, 0);

    uint8_t frac_p[LV_GRADIENT_MAX_STOPS] = {20, 90, 120, 130, 160, 255};
    my_bar_set_gradient_data(bar_p, color_h, frac_p);

	// Таймер
	timer = lv_timer_create(timer_handler, 2000, NULL);
//	lv_timer_ready(timer);
}

void home_meteo_page_deinit(void)
{
	default_page_deinit();
	lv_timer_del(timer);
}
