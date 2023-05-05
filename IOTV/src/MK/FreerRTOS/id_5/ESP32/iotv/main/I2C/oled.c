/*
 * oled.c
 *
 *  Created on: 5 мая 2023 г.
 *      Author: user
 */
#include "oled.h"

extern QueueHandle_t xQueueLedSignals;

static const char *TAG = "OLED";
static uint8_t current_page = 0;

static lv_disp_t *disp = NULL;

static bool OLED_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
static void OLED_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void OLED_lvgl_set_px_cb(lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
static void OLED_lvgl_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area);
static void OLED_increase_lvgl_tick(void *arg);

void modf_two_part(float val, int *int_part, int *fract_part);

static void draw_page_0(void);
static void draw_page_1(float t);
static void draw_page_2(float h);
static void draw_page_3(int d);

static bool OLED_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
	lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
	lv_disp_flush_ready(disp_driver);
	return false;
}

static void OLED_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
	esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
	int offsetx1 = area->x1;
	int offsetx2 = area->x2;
	int offsety1 = area->y1;
	int offsety2 = area->y2;
	// copy a buffer's content to a specific area of the display
	esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

static void OLED_lvgl_set_px_cb(lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
		lv_color_t color, lv_opa_t opa)
{
	uint16_t byte_index = x + (( y >> 3 ) * buf_w);
	uint8_t  bit_index  = y & 0x7;

	if ((color.full == 0) && (LV_OPA_TRANSP != opa)) {
		buf[byte_index] |= (1 << bit_index);
	} else {
		buf[byte_index] &= ~(1 << bit_index);
	}
}

static void OLED_lvgl_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
	area->y1 = area->y1 & (~0x7);
	area->y2 = area->y2 | 0x7;
}

static void OLED_increase_lvgl_tick(void *arg)
{
	/* Tell LVGL how many milliseconds has elapsed */
	lv_tick_inc(OLED_LVGL_TICK_PERIOD_MS);
}

void modf_two_part(float val, int *int_part, int *fract_part)
{
	if (int_part == NULL || fract_part == NULL)
		return;

    double int_part_double;

    *fract_part = modf(val, &int_part_double) * 100;
    *int_part = int_part_double;
}


static void draw_page_0(void)
{
	lv_obj_t *scr = lv_disp_get_scr_act(disp);
	lv_obj_clean(scr);

	//	lv_disp_load_scr(scr);
	//	screen_clear(disp);

	//	lv_obj_t *my_rect = lv_obj_create(scr);
	//	lv_obj_set_size(my_rect , 128, 64);
	//	lv_obj_set_pos(my_rect , 0, 0);
	//	lv_obj_set_style_bg_color(my_rect , lv_color_white(), 0);

	// Время
	lv_obj_t *label_time = lv_label_create(scr);
	lv_label_set_long_mode(label_time, LV_LABEL_LONG_DOT); /* Circular scroll */
	lv_label_set_text_fmt(label_time, "%d %c %d", 23, ':', 34);
	lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);
	//	lv_obj_set_pos(label, 0, 10);

	static lv_style_t style_time;
	lv_style_init(&style_time);

	lv_style_set_text_font(&style_time, &lv_font_montserrat_30);
	lv_obj_add_style(label_time, &style_time, 0);

	// Дата
	lv_obj_t *label_date = lv_label_create(scr);
	lv_label_set_long_mode(label_date, LV_LABEL_LONG_DOT); /* Circular scroll */
	lv_label_set_text_fmt(label_date, "%.02d . %.02d . %d", 1, 3, 2023);
	lv_obj_align(label_date, LV_ALIGN_BOTTOM_MID, 0, 0);
	//	lv_obj_set_pos(label, 0, 10);

	static lv_style_t style_date;
	lv_style_init(&style_date);

	lv_style_set_text_font(&style_date, &lv_font_montserrat_12);
	lv_obj_add_style(label_date, &style_date, 0);


}

static void draw_page_1(float t)
{
	lv_obj_t *scr = lv_disp_get_scr_act(disp);
	lv_obj_clean(scr);
	//	lv_disp_load_scr(scr);

	//	lv_obj_t *my_rect = lv_obj_create(scr);
	//	lv_obj_set_size(my_rect , 128, 64);
	//	lv_obj_set_pos(my_rect , 0, 0);
	//	lv_obj_set_style_bg_color(my_rect , lv_color_white(), 0);

	// Температура
	lv_obj_t *label_time = lv_label_create(scr);
	lv_label_set_long_mode(label_time, LV_LABEL_LONG_DOT); /* Circular scroll */

    int part, fract;
    modf_two_part(t, &part, &fract);

	lv_label_set_text_fmt(label_time, "%d.%.02d  C", part, fract);
	lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style_time;
	lv_style_init(&style_time);

	lv_style_set_text_font(&style_time, &lv_font_montserrat_22);
	lv_obj_add_style(label_time, &style_time, 0);

	// Символ градуса
	lv_obj_t *my_Cir = lv_obj_create(scr);
	lv_obj_set_scrollbar_mode(my_Cir , LV_SCROLLBAR_MODE_ON);
	lv_obj_set_size(my_Cir, 7, 7);
	lv_obj_set_pos(my_Cir, 84, 20);
	lv_obj_set_style_border_width(my_Cir , 1, 0);
	lv_obj_set_style_border_color(my_Cir, lv_color_black(), 0);
	//	lv_obj_set_style_bg_color(my_Cir , lv_palette_lighten(LV_PALETTE_YELLOW, 255), 0);
	lv_obj_set_style_radius(my_Cir , LV_RADIUS_CIRCLE, 0);
}

static void draw_page_2(float h)
{
	lv_obj_t *scr = lv_disp_get_scr_act(disp);
	lv_obj_clean(scr);
	//	lv_disp_load_scr(scr);

	// Влажность
	lv_obj_t *label_time = lv_label_create(scr);
	lv_label_set_long_mode(label_time, LV_LABEL_LONG_DOT); /* Circular scroll */

    int part, fract;
    modf_two_part(h, &part, &fract);

	lv_label_set_text_fmt(label_time, "%d.%.02d %%", part, fract);
	lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style_time;
	lv_style_init(&style_time);

	lv_style_set_text_font(&style_time, &lv_font_montserrat_22);
	lv_obj_add_style(label_time, &style_time, 0);
}

static void draw_page_3(int d)
{
	lv_obj_t *scr = lv_disp_get_scr_act(disp);
	lv_obj_clean(scr);
	taskYIELD();

	// Дистанция
	lv_obj_t *label_time = lv_label_create(scr);
	lv_label_set_long_mode(label_time, LV_LABEL_LONG_DOT); /* Circular scroll */

	lv_label_set_text_fmt(label_time, "%d mm", d);
	lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style_time;
	lv_style_init(&style_time);

	lv_style_set_text_font(&style_time, &lv_font_montserrat_22);
	lv_obj_add_style(label_time, &style_time, 0);
}


void OLED_init(void)
{
	static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
	static lv_disp_drv_t disp_drv;      // contains callback functions

	ESP_LOGI(TAG, "Install panel IO");
	esp_lcd_panel_io_handle_t io_handle = NULL;
	esp_lcd_panel_io_i2c_config_t io_config = {
			.dev_addr = OLED_ADDR,
			.control_phase_bytes = 1,               // According to SSD1306 datasheet
			.dc_bit_offset = 6,                     // According to SSD1306 datasheet
			.lcd_cmd_bits = OLED_LCD_CMD_BITS,   // According to SSD1306 datasheet
			.lcd_param_bits = OLED_LCD_CMD_BITS, // According to SSD1306 datasheet
			.on_color_trans_done = OLED_notify_lvgl_flush_ready,
			.user_ctx = &disp_drv,
	};
	esp_err_t res = esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)0, &io_config, &io_handle);

	ESP_LOGI(TAG, "Install SSD1306 panel driver");
	esp_lcd_panel_handle_t panel_handle = NULL;
	esp_lcd_panel_dev_config_t panel_config = {
			.bits_per_pixel = 1,
			.reset_gpio_num = OLED_PIN_NUM_RST,
	};
	res |= esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle);

	res |= esp_lcd_panel_reset(panel_handle);
	res |= esp_lcd_panel_init(panel_handle);
	res |= esp_lcd_panel_disp_on_off(panel_handle, true);

	ESP_LOGI(TAG, "Initialize LVGL library");
	lv_init();
	// alloc draw buffers used by LVGL
	// it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
	lv_color_t *buf1 = malloc(OLED_LCD_H_RES * 20 * sizeof(lv_color_t));
	assert(buf1);
	lv_color_t *buf2 = malloc(OLED_LCD_H_RES * 20 * sizeof(lv_color_t));
	assert(buf2);
	// initialize LVGL draw buffers
	lv_disp_draw_buf_init(&disp_buf, buf1, buf2, OLED_LCD_H_RES * 20);

	ESP_LOGI(TAG, "Register display driver to LVGL");
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = OLED_LCD_H_RES;
	disp_drv.ver_res = OLED_LCD_V_RES;
	disp_drv.flush_cb = OLED_lvgl_flush_cb;
	disp_drv.draw_buf = &disp_buf;
	disp_drv.user_data = panel_handle;
	disp_drv.rounder_cb = OLED_lvgl_rounder;
	disp_drv.set_px_cb = OLED_lvgl_set_px_cb;
	disp = lv_disp_drv_register(&disp_drv);
	//	lv_disp_set_default(disp);


	ESP_LOGI(TAG, "Install LVGL tick timer");
	// Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
	const esp_timer_create_args_t lvgl_tick_timer_args = {
			.callback = &OLED_increase_lvgl_tick,
			.name = "lvgl_tick"
	};
	esp_timer_handle_t lvgl_tick_timer = NULL;
	res |= esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
	res |= esp_timer_start_periodic(lvgl_tick_timer, OLED_LVGL_TICK_PERIOD_MS * 1000);

	if (res == ESP_FAIL)
	{
		struct LedSignalPkg pkg = {TAG, OLED_INIT_FAIL};
		xQueueSend(xQueueLedSignals, (void *)&pkg, 10 / portTICK_PERIOD_MS);
	}

}

void OLED_Draw_Page(float t, float h, float p)
{
	switch (current_page)
	{
	case 0:
		draw_page_0();
		break;
	case 1:
		draw_page_1(t);
		break;
	case 2:
		draw_page_2(h);
		break;
	default:
		break;
	}

	if (++current_page > 2)
		current_page = 0;

	lv_timer_handler();
	taskYIELD();
}

void OLED_Draw_Distance(int d)
{
	draw_page_3(d);
	lv_timer_handler();
	taskYIELD();
}
