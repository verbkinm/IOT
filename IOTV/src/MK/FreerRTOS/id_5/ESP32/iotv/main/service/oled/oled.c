/*
 * oled.c
 *
 *  Created on: 7 мар. 2024 г.
 *      Author: user
 */

#include "oled.h"

extern lv_font_t ubuntu_mono_10;
extern lv_font_t ubuntu_mono_14;
extern lv_font_t ubuntu_mono_24;

static const char *TAG = "oled";


static lv_disp_t *glob_disp = NULL;

static lv_obj_t *time_obj = NULL;
static lv_obj_t *label_time = NULL;
static lv_obj_t *date_obj = NULL;
static lv_obj_t *label_date = NULL;

static lv_obj_t *label_temperature = NULL;
static lv_obj_t *temperature_block = NULL;

static lv_obj_t *label_humidity = NULL;
static lv_obj_t *humidity_block = NULL;

static lv_obj_t *label_pressure = NULL;
static lv_obj_t *pressure_block = NULL;

static lv_obj_t *canvas_wifi = NULL;
static lv_obj_t *canvas_tcp = NULL;
static lv_obj_t *canvas_rele = NULL;

static void init_draw_time();
static void init_draw_date();

static void init_draw_temperature();
static void init_draw_humidity();
static void init_draw_pressure();

static void init_draw_wifi();
static void init_draw_tcp();
static void init_draw_relestate();

static void draw_time(const struct tm *timeinfo);
static void draw_date(const struct tm *timeinfo);
static void draw_temperature(const struct THP *thp);
static void draw_humidity(const struct THP *thp);
static void draw_pressure(const struct THP *thp);

static void draw_status_wifi(bool isConnected);
static void draw_status_TCP(bool isConnected);
static void draw_status_rele(bool isReleOn);
static void draw_status_on_canva(lv_obj_t *canvas, uint8_t (*arr)[10][10]);

static void anim_x_cb(void * var, int32_t v);

static void OLED_init(void);
static void OLED_boot_screen(void);
static void OLED_init_draw_element(void);

static void init_draw_time()
{
	time_obj = lv_obj_create(lv_scr_act());
	lv_obj_set_size(time_obj, 86, 30);
	lv_obj_set_pos(time_obj, 42, 12);
	lv_obj_set_scrollbar_mode(time_obj , LV_SCROLLBAR_MODE_OFF);

	label_time = lv_label_create(time_obj);
	lv_obj_align(label_time, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style_time;
	lv_style_init(&style_time);

	lv_style_set_text_font(&style_time, &ubuntu_mono_24);
	lv_obj_add_style(label_time, &style_time, 0);
}

static void init_draw_date()
{
	date_obj = lv_obj_create(lv_scr_act());
	lv_obj_set_size(date_obj, 86, 20);
	lv_obj_set_pos(date_obj, 42, 41);
	lv_obj_set_scrollbar_mode(date_obj , LV_SCROLLBAR_MODE_OFF);

	label_date = lv_label_create(date_obj);
	lv_obj_align(label_date, LV_ALIGN_CENTER, 0, 0);

	static lv_style_t style_date;
	lv_style_init(&style_date);

	lv_style_set_text_font(&style_date, &ubuntu_mono_10);
	lv_style_set_text_letter_space(&style_date, 2);
	lv_obj_add_style(label_date, &style_date, 0);
}

static void init_draw_temperature()
{
	temperature_block = lv_obj_create(lv_scr_act());
	lv_obj_set_size(temperature_block, 43, 22);
	lv_obj_set_pos(temperature_block, 0, 0);
	lv_obj_set_style_border_width(temperature_block, 1, 0);
	lv_obj_set_style_border_color(temperature_block, lv_color_black(), 0);
	lv_obj_set_style_radius(temperature_block , 0, 0);
	lv_obj_set_scrollbar_mode(temperature_block , LV_SCROLLBAR_MODE_OFF);

	label_temperature = lv_label_create(temperature_block);
	lv_obj_set_style_text_font(label_temperature, &ubuntu_mono_14, 0);
	lv_obj_align(label_temperature, LV_ALIGN_CENTER, 0, 0);
}

static void init_draw_humidity()
{
	humidity_block = lv_obj_create(lv_scr_act());
	lv_obj_set_size(humidity_block, 43, 22);
	lv_obj_set_pos(humidity_block, 0, 21);
	lv_obj_set_style_border_width(humidity_block, 1, 0);
	lv_obj_set_style_border_color(humidity_block, lv_color_black(), 0);
	lv_obj_set_style_radius(humidity_block , 0, 0);
	lv_obj_set_scrollbar_mode(humidity_block , LV_SCROLLBAR_MODE_OFF);

	label_humidity = lv_label_create(humidity_block);
	lv_obj_set_style_text_font(label_humidity, &ubuntu_mono_14, 0);
	lv_obj_align(label_humidity, LV_ALIGN_CENTER, 0, 0);
}

static void init_draw_pressure()
{
	pressure_block = lv_obj_create(lv_scr_act());
	lv_obj_set_size(pressure_block, 43, 22);
	lv_obj_set_pos(pressure_block, 0, 42);
	lv_obj_set_style_border_width(pressure_block, 1, 0);
	lv_obj_set_style_border_color(pressure_block, lv_color_black(), 0);
	lv_obj_set_style_radius(pressure_block , 0, 0);
	lv_obj_set_scrollbar_mode(pressure_block , LV_SCROLLBAR_MODE_OFF);

	label_pressure = lv_label_create(pressure_block);
	lv_obj_set_style_text_font(label_pressure, &ubuntu_mono_14, 0);
	lv_obj_align(label_pressure, LV_ALIGN_CENTER, 0, 0);
}

static void init_draw_wifi()
{
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(10, 10)];

	canvas_wifi = lv_canvas_create(lv_scr_act());
	lv_obj_set_size(canvas_wifi, 10, 10);
	lv_obj_set_pos(canvas_wifi, 117, 0);
	lv_canvas_set_buffer(canvas_wifi, cbuf, 10, 10, LV_IMG_CF_INDEXED_1BIT);
	lv_canvas_set_palette(canvas_wifi, 0, lv_color_black());
}

static void init_draw_tcp()
{
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(10, 10)];

	canvas_tcp = lv_canvas_create(lv_scr_act());
	lv_obj_set_size(canvas_tcp, 10, 10);
	lv_obj_set_pos(canvas_tcp, 102, 0);
	lv_canvas_set_buffer(canvas_tcp, cbuf, 10, 10, LV_IMG_CF_INDEXED_1BIT);
	lv_canvas_set_palette(canvas_tcp, 0, lv_color_black());
}

static void init_draw_relestate()
{
	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(10, 10)];

	canvas_rele = lv_canvas_create(lv_scr_act());
	lv_obj_set_size(canvas_rele, 10, 10);
	lv_obj_set_pos(canvas_rele, 87, 0);
	lv_canvas_set_buffer(canvas_rele, cbuf, 10, 10, LV_IMG_CF_INDEXED_1BIT);
	lv_canvas_set_palette(canvas_rele, 0, lv_color_black());
}

static void draw_time(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(label_time, "%.02d:%.02d", timeinfo->tm_hour, timeinfo->tm_min);
}

static void draw_date(const struct tm *timeinfo)
{
	lv_label_set_text_fmt(label_date, "%02d.%.02d.%.04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

static void draw_temperature(const struct THP *thp)
{
	int t = thp->temperature + 0.5;

	if (thp->err) // ошибка
		lv_label_set_text_fmt(label_temperature, "err");
	else
		lv_label_set_text_fmt(label_temperature, "%+d°", t);
}

static void draw_humidity(const struct THP *thp)
{
	int h = thp->humidity + 0.5;

	if (thp->err || h > 99)
		lv_label_set_text_fmt(label_humidity, "err");
	else
		lv_label_set_text_fmt(label_humidity, "%d%%", h);
}

static void draw_pressure(const struct THP *thp)
{
	int p = thp->pressure;

	if (thp->err)
		lv_label_set_text(label_pressure, "err");
	else
		lv_label_set_text_fmt(label_pressure, "%d", p);
}

static void draw_status_wifi(bool isConnected)
{
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

	uint8_t (*arr)[10][10] = isConnected ? &wifi_connected : &wifi_disconnected;
	draw_status_on_canva(canvas_wifi, arr);
}

static void draw_status_TCP(bool isConnected)
{
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

	uint8_t (*arr)[10][10] = isConnected ? &tcp_connected : &tcp_disconnected;
	draw_status_on_canva(canvas_tcp, arr);
}

static void draw_status_rele(bool isReleOn)
{
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
	draw_status_on_canva(canvas_rele, arr);
}

static void draw_status_on_canva(lv_obj_t *canvas, uint8_t (*arr)[10][10])
{
	for(uint8_t y = 0; y < 10; ++y)
	{
		for(uint8_t x = 0; x < 10; ++x)
			lv_canvas_set_px(canvas, x, y, (*arr)[y][x] ? lv_color_black() : lv_color_white());
	}
}

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
	lv_disp_t * disp = (lv_disp_t *)user_ctx;
	lvgl_port_flush_ready(disp);
	return false;
}

static void OLED_init(void)
{
	ESP_LOGI(TAG, "Install panel IO");

	esp_lcd_panel_io_handle_t io_handle = NULL;
	esp_lcd_panel_io_i2c_config_t io_config = {
			.dev_addr = OLED_ADDR,
			.control_phase_bytes = 1,               // According to SSD1306 datasheet
			.lcd_cmd_bits = OLED_LCD_CMD_BITS,   // According to SSD1306 datasheet
			.lcd_param_bits = OLED_LCD_CMD_BITS, // According to SSD1306 datasheet
			.dc_bit_offset = 6,
	};
	ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)0, &io_config, &io_handle));

	ESP_LOGI(TAG, "Install SSD1306 panel driver");
	esp_lcd_panel_handle_t panel_handle = NULL;
	esp_lcd_panel_dev_config_t panel_config = {
			.bits_per_pixel = 1,
			.reset_gpio_num = OLED_PIN_NUM_RST,
	};
	ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
	ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
	ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
	ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

	ESP_LOGI(TAG, "Initialize LVGL");
	const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
	lvgl_port_init(&lvgl_cfg);

	const lvgl_port_display_cfg_t disp_cfg = {
			.io_handle = io_handle,
			.panel_handle = panel_handle,
			.buffer_size = OLED_LCD_H_RES * OLED_LCD_V_RES,
			.double_buffer = true,
			.hres = OLED_LCD_H_RES,
			.vres = OLED_LCD_V_RES,
			.monochrome = true,
			.rotation = {
					.swap_xy = false,
					.mirror_x = false,
					.mirror_y = false,
			}
	};
	lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);
	/* Register done callback for IO */
	const esp_lcd_panel_io_callbacks_t cbs = {
			.on_color_trans_done = notify_lvgl_flush_ready,
	};
	ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, disp));

	/* Rotation of the screen */
	lv_disp_set_rotation(disp, readDisplayOrientationFromNVS());
	glob_disp = disp;
}

static void anim_x_cb(void * var, int32_t v)
{
	lv_obj_set_x(var, v);
}

static void OLED_init_draw_element(void)
{
	lv_obj_t *scr = lv_scr_act();
	init_draw_time(scr);
	init_draw_date(scr);

	init_draw_temperature();
	init_draw_humidity();
	init_draw_pressure();

	init_draw_wifi(scr);
	init_draw_tcp(scr);
	init_draw_relestate(scr);
}

static void OLED_boot_screen(void)
{
	lv_obj_t *scr = lv_scr_act();
	lv_obj_clean(scr);

	lv_obj_t *label1 = lv_label_create(scr);
	lv_obj_t *label2 = lv_label_create(scr);

	lv_label_set_text_fmt(label1, "IOTV ID5");
	lv_label_set_text_fmt(label2, "версия: %s", esp_app_get_description()->version);

	lv_style_t style1, style2;
	lv_style_init(&style1);
	lv_style_init(&style2);

	lv_style_set_text_font(&style1, &ubuntu_mono_24);
	lv_obj_add_style(label1, &style1, 0);
	lv_obj_align(label1, LV_ALIGN_CENTER, 0, -10);

	lv_style_set_text_font(&style2, &ubuntu_mono_14);
	lv_obj_add_style(label2, &style2, 0);
	lv_obj_align(label2, LV_ALIGN_CENTER, 0, +10);

	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, label1);
	lv_anim_set_time(&a, 2000);
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
	lv_anim_set_time(&b, 2000);
	lv_anim_set_repeat_count(&b, 0/*LV_ANIM_REPEAT_INFINITE*/);
	lv_anim_set_path_cb(&b, lv_anim_path_bounce);

	lv_anim_set_exec_cb(&b, anim_x_cb);
	lv_anim_set_values(&b, 128, 0);
	lv_anim_start(&b);
	//
	vTaskDelay(OLED_BOOT_SCREEN_TIME / portTICK_PERIOD_MS);

	lv_obj_clean(scr);
}

void OLED_Task(void *pvParameters)
{
	struct IOTV_Server_embedded *iot = iotv_get();

	while(iot->state == 0)
		vTaskDelay(10 / portTICK_PERIOD_MS);

	ESP_LOGW(TAG, "OLED task created");

	OLED_init();
	OLED_boot_screen();
	OLED_init_draw_element(); // после boot_screen

	const struct THP *thp = BME280_service_get_value();

	while(true)
	{
		time_t now;
		struct tm timeinfo;
		time(&now);
		localtime_r(&now, &timeinfo);

		draw_time(&timeinfo);
		draw_date(&timeinfo);

		draw_temperature(thp);
		draw_humidity(thp);
		draw_pressure(thp);

		draw_status_wifi(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED);
		draw_status_TCP(glob_get_status_reg() & STATUS_TCP_CONNECTED);
		draw_status_rele(glob_get_status_reg() & STATUS_RELE);

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
