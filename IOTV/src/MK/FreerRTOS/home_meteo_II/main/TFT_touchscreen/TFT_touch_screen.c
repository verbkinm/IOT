/*
 * TFT_touch_screen.c
 *
 *  Created on: 23 нояб. 2023 г.
 *      Author: user
 */

#include "TFT_touch_screen.h"

static const char *TAG = "TFT_touch_screen";

static esp_lcd_touch_handle_t tp;
static uint16_t touch_x[1];
static uint16_t touch_y[1];
static uint16_t touch_strength[1];

static lv_disp_t *disp;

static bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data);
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void increase_lvgl_tick(void *arg);
//static void increase_lvgl_handler_tick(void *arg);
static void TFT_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
static void TFT_rgb_panel_init(void);
static void TFT_touch_panel_init(void);

static bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data)
{
	BaseType_t high_task_awoken = pdFALSE;
	return high_task_awoken == pdTRUE;
}

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
	esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
	int offsetx1 = area->x1;
	int offsetx2 = area->x2;
	int offsety1 = area->y1;
	int offsety2 = area->y2;
	// pass the draw buffer to the driver
	esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
	lv_disp_flush_ready(drv);
}

IRAM_ATTR static void increase_lvgl_tick(void *arg)
{
	lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static void TFT_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
	uint8_t touch_cnt = 1;

	esp_lcd_touch_read_data(tp);
	bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, touch_x, touch_y, touch_strength, &touch_cnt, 1);
	if(touchpad_pressed)
	{
		data->point.x = touch_x[0] * 1.666;
		data->point.y = touch_y[0] * 1.777;
		data->state = LV_INDEV_STATE_PRESSED;
		//		printf("my_input_read coord - x: %d, y: %d\n", (uint)touch_x[0], (uint)touch_y[0]);
		//		printf("offset coord - x: %d, y: %d\n", (uint)(touch_x[0] * 1.666), (uint)(touch_y[0] * 1.777));
	}
	else
		data->state = LV_INDEV_STATE_RELEASED;
}

static void TFT_rgb_panel_init(void)
{
	static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
	static lv_disp_drv_t disp_drv;      // contains callback functions

	ESP_LOGI(TAG, "Turn off LCD backlight");
	gpio_config_t bk_gpio_config = {
			.mode = GPIO_MODE_OUTPUT,
			.pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
	};
	ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

	ESP_LOGI(TAG, "Install RGB LCD panel driver");
	esp_lcd_panel_handle_t panel_handle = NULL;
	esp_lcd_rgb_panel_config_t panel_config = {
			.data_width = 16, // RGB565 in parallel mode, thus 16bit in width
			.psram_trans_align = 64,
			.num_fbs = LCD_NUM_FB,
			.clk_src = LCD_CLK_SRC_DEFAULT,
			.disp_gpio_num = PIN_NUM_DISP_EN,
			.pclk_gpio_num = PIN_NUM_PCLK,
			.vsync_gpio_num = PIN_NUM_VSYNC,
			.hsync_gpio_num = PIN_NUM_HSYNC,
			.de_gpio_num = PIN_NUM_DE,
			.data_gpio_nums = {
					PIN_NUM_DATA0,
					PIN_NUM_DATA1,
					PIN_NUM_DATA2,
					PIN_NUM_DATA3,
					PIN_NUM_DATA4,
					PIN_NUM_DATA5,
					PIN_NUM_DATA6,
					PIN_NUM_DATA7,
					PIN_NUM_DATA8,
					PIN_NUM_DATA9,
					PIN_NUM_DATA10,
					PIN_NUM_DATA11,
					PIN_NUM_DATA12,
					PIN_NUM_DATA13,
					PIN_NUM_DATA14,
					PIN_NUM_DATA15,
			},
			.timings = {
					.pclk_hz = LCD_PIXEL_CLOCK_HZ,
					.h_res = LCD_H_RES,
					.v_res = LCD_V_RES,
					// The following parameters should refer to LCD spec
					.hsync_back_porch = 40,
					.hsync_front_porch = 20,
					.hsync_pulse_width = 1,
					.vsync_back_porch = 8,
					.vsync_front_porch = 4,
					.vsync_pulse_width = 1,
					.flags.pclk_active_neg = true,
					//					.bounce_buffer_size_px = 20,
			},
			.flags.fb_in_psram = true, // allocate frame buffer in PSRAM
	};
	ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

	ESP_LOGI(TAG, "Register event callbacks");
	esp_lcd_rgb_panel_event_callbacks_t cbs = {
			.on_vsync = on_vsync_event,
	};
	ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, &disp_drv));

	ESP_LOGI(TAG, "Initialize RGB LCD panel");
	ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
	ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

	ESP_LOGI(TAG, "Turn on LCD backlight");
	gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);

	ESP_LOGI(TAG, "Initialize LVGL library");
	lv_init();
	void *buf1 = NULL;
	void *buf2 = NULL;
	ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
	buf1 = heap_caps_malloc(LCD_H_RES * 1000 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	assert(buf1);
	//	buf2 = heap_caps_malloc(LCD_H_RES * 1000 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	//	assert(buf2);
	// initialize LVGL draw buffers
	lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * 1000);

	ESP_LOGI(TAG, "Register display driver to LVGL");
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = LCD_H_RES;
	disp_drv.ver_res = LCD_V_RES;
	disp_drv.flush_cb = lvgl_flush_cb;
	disp_drv.draw_buf = &disp_buf;
	disp_drv.user_data = panel_handle;
	disp_drv.sw_rotate = 1;
	disp = lv_disp_drv_register(&disp_drv);

	ESP_LOGI(TAG, "Install LVGL tick timer");
	const esp_timer_create_args_t lvgl_tick_timer_args = {
			.callback = &increase_lvgl_tick,
	};
	esp_timer_handle_t lvgl_tick_timer = NULL;
	ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

	ledc_timer_config_t ledc_timer = {
			.speed_mode       = LEDC_MODE,
			.timer_num        = LEDC_TIMER,
			.duty_resolution  = LEDC_DUTY_RES,
			.freq_hz          = LEDC_FREQUENCY,
			.clk_cfg          = LEDC_AUTO_CLK
	};
	ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

	ledc_channel_config_t ledc_channel = {
			.speed_mode     = LEDC_MODE,
			.channel        = LEDC_CHANNEL,
			.timer_sel      = LEDC_TIMER,
			.intr_type      = LEDC_INTR_DISABLE,
			.gpio_num       = LEDC_OUTPUT_IO,
			.duty           = 0, // Set duty to 0%
			.hpoint         = 0
	};
	ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
	ledc_fade_func_install(0);

	set_display_brightness(255);
}

static void TFT_touch_panel_init(void)
{
	const esp_lcd_touch_config_t tp_cfg = {
			.x_max = LCD_H_RES,
			.y_max = LCD_V_RES,
			.rst_gpio_num = PIN_TOUCH_RST,
			.int_gpio_num = GPIO_NUM_NC,
			.levels = {
					.reset = 0,
					.interrupt = 0,
			},
			.flags = {
					.swap_xy = 0,
					.mirror_x = 0,
					.mirror_y = 0,
			},
	};

	esp_lcd_panel_io_handle_t io_handle = NULL;
	esp_lcd_panel_io_i2c_config_t io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();

	esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_MASTER_NUM, &io_config, &io_handle);
	esp_lcd_touch_new_i2c_gt911(io_handle, &tp_cfg, &tp);
	assert(tp);

	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = TFT_input_read;
	indev_drv.disp = disp;
	/*Register the driver in LVGL and save the created input device object*/
	lv_indev_drv_register(&indev_drv);
}

// Перед этой функцией обязательно выполнить i2c_init();
void TFT_init(void)
{
	TFT_rgb_panel_init();
	TFT_touch_panel_init();
}

void rotate_display(lv_disp_rot_t rotation)
{
	lv_disp_set_rotation(disp, rotation);
}

lv_disp_rot_t get_rotate_display(void)
{
	return lv_disp_get_rotation(disp);
}

void set_display_brightness(uint8_t value)
{
	value = inRange(value, 2, 255);

	ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value));
	ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}
