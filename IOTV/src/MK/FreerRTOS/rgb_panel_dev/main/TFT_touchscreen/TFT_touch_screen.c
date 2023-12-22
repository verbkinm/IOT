/*
 * TFT_touch_screen.c
 *
 *  Created on: 23 нояб. 2023 г.
 *      Author: user
 */

#include "TFT_touch_screen.h"
#include "Global_def.h"

#include "screen_page/homepage.h"
#include "screen_page/menupage.h"
#include "screen_page/setting/settingpage.h"
#include "screen_page/setting/elements.h"
#include "status_panel/status_panel.h"
#include "screen_page/screendefault.h"

extern uint8_t glob_currentPage;
extern uint32_t glob_status_reg;

extern lv_obj_t *glob_busy_indicator;

static const char *TAG = "TFT_touch_screen";

esp_lcd_touch_handle_t tp;
uint16_t touch_x[1];
uint16_t touch_y[1];
uint16_t touch_strength[1];

lv_disp_t *disp;

static bool on_vsync_event(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data);
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void increase_lvgl_tick(void *arg);
static void TFT_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
static void TFT_rgb_panel_init(void);
static void TFT_touch_panel_init(void);
static void timer_loop(lv_timer_t *timer);

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
	/* Tell LVGL how many milliseconds has elapsed */
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
	buf1 = heap_caps_malloc(LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	assert(buf1);
	//	buf2 = heap_caps_malloc(LCD_H_RES * 100 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	//	assert(buf2);
	// initialize LVGL draw buffers
	lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * 100);

	ESP_LOGI(TAG, "Register display driver to LVGL");
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = LCD_H_RES;
	disp_drv.ver_res = LCD_V_RES;
	disp_drv.flush_cb = lvgl_flush_cb;
	disp_drv.draw_buf = &disp_buf;
	disp_drv.user_data = panel_handle;
	disp = lv_disp_drv_register(&disp_drv);

	ESP_LOGI(TAG, "Install LVGL tick timer");
	// Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
	const esp_timer_create_args_t lvgl_tick_timer_args = {
			.callback = &increase_lvgl_tick,
	};
	esp_timer_handle_t lvgl_tick_timer = NULL;
	ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

	ESP_LOGI(TAG, "Display lvgl_demo_ui");
}

static void TFT_touch_panel_init(void)
{
	const esp_lcd_touch_config_t tp_cfg = {
			.x_max = LCD_H_RES,
			.y_max = LCD_V_RES,
			.rst_gpio_num = GPIO_NUM_38,
			.int_gpio_num = GPIO_NUM_18,
			.levels = {
					.reset = 0,
					.interrupt = 1,
			},
			.flags = {
					.swap_xy = 0,
					.mirror_x = 0,
					.mirror_y = 0,
			},
			//.process_coordinates = prc_coord,
			//			.interrupt_callback = callb_b,
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
	//	indev_drv.read_timer = lv_timer_create(timer_tick, 50,  NULL);
	/*Register the driver in LVGL and save the created input device object*/
	lv_indev_drv_register(&indev_drv);
}

void draw_page(void)
{
	switch (glob_currentPage)
	{
	case PAGE_HOME:
		drawHomePage();
		break;
	case PAGE_MENU:
		drawMenuPage();
		break;
	case PAGE_CHARTS:
		//        drawCharsPage();
		break;
	case PAGE_SETTINGS:
		//        drawSettingPage();
		break;
	case PAGE_NONE:
	default:
		break;
	}
}

static void timer_loop(lv_timer_t *timer)
{
	//	if (!(glob_status_reg & STATUS_WIFI_STA_CONNECTING) &&   // Если не идёт подключение
	//			!(glob_status_reg & STATUS_WIFI_SCANNING))		 // И нет сканирования
	//		clear_busy_indicator(&glob_busy_indicator);

	lv_obj_t *sd_icon = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 0), 0);
	lv_obj_t *wifi_icon = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 0), 1);
	lv_obj_t *heap_lbl = lv_obj_get_child(lv_obj_get_child(lv_scr_act(), 0), 2);

	lv_img_set_src(sd_icon, SD_ON);

	if (glob_status_reg & STATUS_WIFI_STA_START)
	{
		if (glob_status_reg & STATUS_WIFI_STA_CONNECTING)
			lv_img_set_src(wifi_icon, WIFI_CONNECTING);
		else
		{
			if (glob_status_reg & STATUS_WIFI_STA_CONNECTED)
				lv_img_set_src(wifi_icon, WIFI_CONNECTED);
			else
				lv_img_set_src(wifi_icon, WIFI_ENABLE);
		}
	}
	else
		lv_img_set_src(wifi_icon, WIFI_DISABLE);

	lv_label_set_text_fmt(heap_lbl, "%u", heap_caps_get_free_size(0));
}

void TFT_draw_page(void *pvParameters)
{
	lv_timer_create(timer_loop, 1000, 0);

	while (1)
	{
		//		menuPageInit();
		draw_page();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

// Перед этой функцией обязательно выполнить i2c_init();
void TFT_init(void)
{
	TFT_rgb_panel_init();
	TFT_touch_panel_init();

	// Статус панель
	lv_obj_t *status_panel = lv_obj_create(lv_scr_act());
	lv_obj_set_size(status_panel, LCD_H_RES, LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(status_panel, LV_DIR_NONE);
	lv_obj_set_style_pad_all(status_panel, 0, 0);
	lv_obj_set_flex_flow(status_panel, LV_FLEX_FLOW_ROW);
	lv_obj_add_style(status_panel, screenStyleDefault(), 0);

	// Статус SD
	lv_obj_t *icon_sd = lv_img_create(status_panel);
	lv_obj_set_size(icon_sd, 24, 24);

	// Статус wifi
	lv_obj_t *icon_wifi = lv_img_create(status_panel);
	lv_obj_set_size(icon_wifi, 24, 24);

	// Статус кучи
	lv_label_create(status_panel);

	// Основной виджет
	lv_obj_t *main_widget = lv_obj_create(lv_scr_act());
	lv_obj_set_size(main_widget, LCD_H_RES, LCD_V_RES - LCD_PANEL_STATUS_H);
	lv_obj_set_y(main_widget, LCD_PANEL_STATUS_H);
	lv_obj_set_scroll_dir(main_widget, LV_DIR_NONE);
	lv_obj_set_style_pad_all(main_widget, 0, 0);
	lv_obj_add_style(main_widget, screenStyleDefault(), 0);

	homePageInit();
	//	menuPageInit();
	//	settingPageInit();

	xTaskCreate(TFT_draw_page, "TFT_draw_page", 2048, NULL, 10, NULL);
}
