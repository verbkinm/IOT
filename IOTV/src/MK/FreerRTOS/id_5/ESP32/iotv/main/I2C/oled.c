/*
 * oled.c
 *
 *  Created on: 5 мая 2023 г.
 *      Author: user
 */
#include "oled.h"

extern QueueHandle_t xQueueLedSignals;

static const char *TAG = "OLED";
static bool isWifiConnect, isTCP_Connect;
static lv_disp_t *disp = NULL;

static bool OLED_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
static void OLED_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void OLED_lvgl_set_px_cb(lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
static void OLED_lvgl_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area);
static void OLED_increase_lvgl_tick(void *arg);

void modf_two_part(float val, int *int_part, int *fract_part);

static void draw_time(const struct DateTime *dt, lv_obj_t *scr);
static void draw_date(const struct DateTime *dt, lv_obj_t *scr);
static void draw_temperature(const struct THP *thp, lv_obj_t *scr);
static void draw_humidity(const struct THP *thp, lv_obj_t *scr);
static void draw_pressure(const struct THP *thp, lv_obj_t *scr);

static void draw_status_wifi(lv_obj_t *scr);
static void draw_status_TCP(lv_obj_t *scr);
static void draw_status_rele(bool isReleOn, lv_obj_t *scr);
static void draw_status_on_canva(lv_obj_t *canvas, uint8_t (*arr)[10][10]);

static void anim_x_cb(void * var, int32_t v);

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

static void draw_time(const struct DateTime *dt, lv_obj_t *scr)
{
	// Время
	lv_obj_t *label_time = lv_label_create(scr);
	//	lv_label_set_long_mode(label_time, LV_LABEL_LONG_DOT); /* Circular scroll */
	if (dt->err || dt->hour > 23 || dt->minutes > 59)
		lv_label_set_text_fmt(label_time, "   -- : --");
	else
		lv_label_set_text_fmt(label_time, "%02d : %02d", dt->hour, dt->minutes);
	lv_obj_set_pos(label_time, 47, 15);

	static lv_style_t style_time;
	lv_style_init(&style_time);

	lv_style_set_text_font(&style_time, &lv_font_montserrat_24);
	lv_obj_add_style(label_time, &style_time, 0);
}

static void draw_date(const struct DateTime *dt, lv_obj_t *scr)
{
	// Дата
	lv_obj_t *label_date = lv_label_create(scr);
	if (dt->err || dt->date > 31 || dt->month > 12)
		lv_label_set_text_fmt(label_date, " -- . -- . ----");
	else
		lv_label_set_text_fmt(label_date, "%02d.%02d.20%02d", dt->date, dt->month, dt->year);
	lv_obj_set_pos(label_date, 49, 45);

	static lv_style_t style_date;
	lv_style_init(&style_date);
	lv_style_set_text_letter_space(&style_date, 2);

	lv_style_set_text_font(&style_date, &lv_font_montserrat_10);
	lv_obj_add_style(label_date, &style_date, 0);
}

static void draw_temperature(const struct THP *thp, lv_obj_t *scr)
{
	lv_obj_t *parent = lv_obj_create(scr);
	lv_obj_set_size(parent, 43, 22);
	lv_obj_set_pos(parent, 0, 0);
	lv_obj_set_style_border_width(parent, 1, 0);
	lv_obj_set_style_border_color(parent, lv_color_black(), 0);
	lv_obj_set_style_radius(parent , 0, 0);
	lv_obj_set_scrollbar_mode(parent , LV_SCROLLBAR_MODE_OFF);

	lv_obj_t *label_temp = lv_label_create(parent);
	//	lv_label_set_long_mode(label_temp, LV_LABEL_LONG_DOT); /* Circular scroll */

	int part, fract;
	modf_two_part(thp->temperature, &part, &fract);

	char buff[5];
	itoa(part, buff, 10);
	buff[4] = 0;

	//		lv_label_set_text_fmt(label_temp, "%d.%.01d", part, fract);
	if (strlen(buff) > 3 || thp->err)
		lv_label_set_text_fmt(label_temp, "err");
	else
	{
		if (part > 0)
		{
			memmove(&buff[1], buff, sizeof(buff) - 1);
			buff[0] = '+';
		}
		lv_label_set_text_fmt(label_temp, "%s", buff);
	}

	lv_obj_align(label_temp, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_text_letter_space(&style, 1);

	lv_style_set_text_font(&style, &lv_font_montserrat_14);
	lv_obj_add_style(label_temp, &style, 0);

	// Символ градуса
	lv_obj_t *my_Cir = lv_obj_create(scr);
	lv_obj_set_scrollbar_mode(my_Cir , LV_SCROLLBAR_MODE_OFF);
	lv_obj_set_size(my_Cir, 3, 3);
	lv_obj_set_style_border_width(my_Cir , 1, 0);
	lv_obj_set_style_border_color(my_Cir, lv_color_black(), 0);
	lv_obj_set_style_radius(my_Cir, LV_RADIUS_CIRCLE, 0);

	int cirX = -3;
	// Расположение символа градуса относительно длины строки
	if (thp->err != true)
	{
		if (strlen(buff) == 1)
			cirX = 29;
		else if (strlen(buff) == 2)
			cirX = 32;
		else if (strlen(buff) == 3)
			cirX = 35;
	}
	lv_obj_set_pos(my_Cir, cirX, 4);
}

static void draw_humidity(const struct THP *thp, lv_obj_t *scr)
{
	lv_obj_t *parent = lv_obj_create(scr);
	lv_obj_set_size(parent, 43, 22);
	lv_obj_set_pos(parent, 0, 21);
	lv_obj_set_style_border_width(parent, 1, 0);
	lv_obj_set_style_border_color(parent, lv_color_black(), 0);
	lv_obj_set_style_radius(parent , 0, 0);
	lv_obj_set_scrollbar_mode(parent , LV_SCROLLBAR_MODE_OFF);

	lv_obj_t *label_hum = lv_label_create(parent);

	int part, fract;
	modf_two_part(thp->humidity, &part, &fract);

	char buff[5];
	itoa(part, buff, 10);
	buff[4] = 0;

	//		lv_label_set_text_fmt(label_temp, "%d.%.01d", part, fract);
	if (strlen(buff) > 3 || thp->err)
		lv_label_set_text_fmt(label_hum, "err");
	else
		lv_label_set_text_fmt(label_hum, "%s%%", buff);

	lv_obj_align(label_hum, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_text_letter_space(&style, 1);

	lv_style_set_text_font(&style, &lv_font_montserrat_14);
	lv_obj_add_style(label_hum, &style, 0);
}

static void draw_pressure(const struct THP *thp, lv_obj_t *scr)
{
	lv_obj_t *parent = lv_obj_create(scr);
	lv_obj_set_size(parent, 43, 22);
	lv_obj_set_pos(parent, 0, 42);
	lv_obj_set_style_border_width(parent, 1, 0);
	lv_obj_set_style_border_color(parent, lv_color_black(), 0);
	lv_obj_set_style_radius(parent , 0, 0);
	lv_obj_set_scrollbar_mode(parent , LV_SCROLLBAR_MODE_OFF);

	lv_obj_t *label_hum = lv_label_create(parent);

	int part, fract;
	double convert = thp->pressure * 7.50062 * 0.001;
	modf_two_part(convert, &part, &fract);

	char buff[5];
	itoa(part, buff, 10);
	buff[4] = 0;

	//		lv_label_set_text_fmt(label_temp, "%d.%.01d", part, fract);
	if (strlen(buff) > 3 || thp->err)
		lv_label_set_text_fmt(label_hum, "err");
	else
		lv_label_set_text_fmt(label_hum, "%s", buff);

	lv_obj_align(label_hum, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_text_letter_space(&style, 1);

	lv_style_set_text_font(&style, &lv_font_montserrat_14);
	lv_obj_add_style(label_hum, &style, 0);
}

static void draw_status_wifi(lv_obj_t *scr)
{
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(10, 10)];

	lv_obj_t *canvas = lv_canvas_create(scr);
	lv_obj_set_size(canvas, 10, 10);
	lv_obj_set_pos(canvas, 117, 0);
	lv_canvas_set_buffer(canvas, cbuf, 10, 10, LV_IMG_CF_INDEXED_1BIT);
	lv_canvas_set_palette(canvas, 0, lv_color_black());

	lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);

	uint8_t wifi_connected[10][10] = {
			{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
			{1, 0, 0, 1, 1, 1, 1, 0, 0, 1},
			{0, 0, 1, 0, 0, 0, 0, 1, 0, 0},
			{0, 1, 0, 0, 1, 1, 0, 0, 1, 0},
			{0, 0, 0, 1, 0, 0, 1, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
	};

	uint8_t wifi_disconnected[10][10] = {
			{1, 0, 0, 1, 1, 1, 1, 0, 0, 0},
			{1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
			{0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
			{1, 0, 1, 1, 1, 1, 1, 0, 0, 1},
			{0, 0, 1, 1, 1, 0, 0, 1, 0, 0},
			{0, 1, 0, 0, 1, 1, 0, 0, 1, 0},
			{0, 0, 0, 1, 0, 1, 1, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 1, 1, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 1, 1}
	};

	uint8_t (*arr)[10][10] = isWifiConnect ? &wifi_connected : &wifi_disconnected;
	draw_status_on_canva(canvas, arr);
}

static void draw_status_TCP(lv_obj_t *scr)
{
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(10, 10)];

	lv_obj_t *canvas = lv_canvas_create(scr);
	lv_obj_set_size(canvas, 10, 10);
	lv_obj_set_pos(canvas, 102, 0);
	lv_canvas_set_buffer(canvas, cbuf, 10, 10, LV_IMG_CF_INDEXED_1BIT);
	lv_canvas_set_palette(canvas, 0, lv_color_black());

	lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);

	uint8_t tcp_connected[10][10] = {
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 1, 1, 0, 1, 1, 0, 1, 1, 1},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
			{0, 0, 0, 1, 1, 1, 1, 0, 0, 0}
	};

	uint8_t tcp_disconnected[10][10] = {
			{1, 1, 0, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 0, 1, 1, 1, 1, 1, 1},
			{1, 0, 1, 1, 0, 1, 0, 1, 1, 1},
			{0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 1, 1, 0},
			{0, 0, 0, 1, 1, 1, 1, 0, 1, 1},
			{0, 0, 0, 1, 1, 1, 1, 0, 0, 1}
	};

	uint8_t (*arr)[10][10] = isTCP_Connect ? &tcp_connected : &tcp_disconnected;
	draw_status_on_canva(canvas, arr);
}

static void draw_status_rele(bool isReleOn, lv_obj_t *scr)
{
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(10, 10)];

	lv_obj_t *canvas = lv_canvas_create(scr);
	lv_obj_set_size(canvas, 10, 10);
	lv_obj_set_pos(canvas, 87, 0);
	lv_canvas_set_buffer(canvas, cbuf, 10, 10, LV_IMG_CF_INDEXED_1BIT);
	lv_canvas_set_palette(canvas, 0, lv_color_black());

	lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);

	uint8_t rele_on[10][10] = {
			{0, 0, 0, 1, 0, 0, 1, 0, 0, 0},
			{0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
			{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
			{1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
			{0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
			{0, 1, 1, 1, 0, 0, 1, 1, 1, 0},
			{1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
			{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
			{0, 1, 0, 0, 1, 1, 0, 0, 1, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
	};

	uint8_t rele_off[10][10] = {
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
			{0, 0, 1, 0, 0, 0, 0, 1, 0, 0},
			{0, 1, 0, 1, 1, 1, 1, 0, 1, 0},
			{0, 1, 0, 0, 1, 1, 0, 0, 1, 0},
			{0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
			{0, 0, 0, 1, 0, 0, 1, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
			{0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
	};

	uint8_t (*arr)[10][10] = isReleOn ? &rele_on : &rele_off;
	draw_status_on_canva(canvas, arr);
}

static void draw_status_on_canva(lv_obj_t *canvas, uint8_t (*arr)[10][10])
{
	for(uint8_t y = 0; y < 10; y++)
	{
		for(uint8_t x = 0; x < 10; x++)
		{
			if ((*arr)[y][x])
				lv_canvas_set_px(canvas, x, y, lv_color_black());
		}
	}
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
	lv_disp_set_default(disp);


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

static void anim_x_cb(void * var, int32_t v)
{
	lv_obj_set_x(var, v);
	//	lv_obj_set_style_opa(var, v, 0);
}

void OLED_boot_screen(void)
{
	lv_obj_t *scr = lv_disp_get_scr_act(disp);
	lv_obj_clean(scr);

	lv_obj_t *label1 = lv_label_create(scr);
	lv_obj_t *label2 = lv_label_create(scr);

	lv_label_set_text_fmt(label1, "IOTV ID5");
	lv_label_set_text_fmt(label2, "version: %s", esp_app_get_description()->version);

	lv_style_t style1, style2;
	lv_style_init(&style1);
	lv_style_init(&style2);

	lv_style_set_text_font(&style1, &lv_font_montserrat_24);
	lv_obj_add_style(label1, &style1, 0);
	lv_obj_align(label1, LV_ALIGN_CENTER, 0, -10);

	lv_style_set_text_font(&style2, &lv_font_montserrat_14);
	lv_obj_add_style(label2, &style2, 0);
	lv_obj_align(label2, LV_ALIGN_CENTER, 0, +10);

	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, label1);
	lv_anim_set_time(&a, 1000);
	//	lv_anim_set_playback_delay(&a, 1500);
	//	lv_anim_set_playback_time(&a, 500);
	//	lv_anim_set_repeat_delay(&a, 500);
	lv_anim_set_repeat_count(&a, 0/*LV_ANIM_REPEAT_INFINITE*/);
	lv_anim_set_path_cb(&a, lv_anim_path_bounce);

	lv_anim_set_exec_cb(&a, anim_x_cb);
	lv_anim_set_values(&a, 128, 0);
	lv_anim_start(&a);

	lv_anim_t b;
	lv_anim_init(&b);
	lv_anim_set_var(&b, label2);
	lv_anim_set_time(&b, 1000);
	lv_anim_set_repeat_count(&b, 0/*LV_ANIM_REPEAT_INFINITE*/);
	lv_anim_set_path_cb(&b, lv_anim_path_bounce);

	lv_anim_set_exec_cb(&b, anim_x_cb);
	lv_anim_set_values(&b, 128, 0);
	lv_anim_start(&b);

	for (int i = 0; i < OLED_BOOT_SCREEN_TIME / 10; ++i)
	{
		vTaskDelay(10 / portTICK_PERIOD_MS);
		lv_timer_handler();
	}
}

void OLED_Draw_Page(const struct THP *thp, const struct DateTime *dt, bool isReleOn)
{
	if (dt == NULL || thp == NULL)
		return;

	lv_obj_t *scr = lv_disp_get_scr_act(disp);
	lv_obj_clean(scr);

	draw_time(dt, scr);
	draw_date(dt, scr);
	draw_temperature(thp, scr);
	draw_humidity(thp, scr);
	draw_pressure(thp, scr);

	draw_status_wifi(scr);
	draw_status_TCP(scr);
	draw_status_rele(isReleOn, scr);

	lv_timer_handler();
}

void OLED_setWIFI_State(bool val)
{
	isWifiConnect = val;
}

void OLED_setTCP_State(bool val)
{
	isTCP_Connect = val;
}
