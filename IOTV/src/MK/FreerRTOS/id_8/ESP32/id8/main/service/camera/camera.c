/*
 * camera.c
 *
 *  Created on: 17 янв. 2024 г.
 *      Author: user
 */

#include "../camera/camera.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#define CAM_PIN_PWDN 	32
#define CAM_PIN_RESET 	-1 //software reset will be performed
#define CAM_PIN_XCLK	0
#define CAM_PIN_SIOD 	26
#define CAM_PIN_SIOC 	27

#define CAM_PIN_D7 		35
#define CAM_PIN_D6 		34
#define CAM_PIN_D5 		39
#define CAM_PIN_D4 		36
#define CAM_PIN_D3 		21
#define CAM_PIN_D2 		19
#define CAM_PIN_D1 		18
#define CAM_PIN_D0 		5
#define CAM_PIN_VSYNC 	25
#define CAM_PIN_HREF 	23
#define CAM_PIN_PCLK 	22

static const char *TAG = "camera";

#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config = {
		.pin_pwdn = CAM_PIN_PWDN,
		.pin_reset = CAM_PIN_RESET,
		.pin_xclk = CAM_PIN_XCLK,
		.pin_sccb_sda = CAM_PIN_SIOD,
		.pin_sccb_scl = CAM_PIN_SIOC,

		.pin_d7 = CAM_PIN_D7,
		.pin_d6 = CAM_PIN_D6,
		.pin_d5 = CAM_PIN_D5,
		.pin_d4 = CAM_PIN_D4,
		.pin_d3 = CAM_PIN_D3,
		.pin_d2 = CAM_PIN_D2,
		.pin_d1 = CAM_PIN_D1,
		.pin_d0 = CAM_PIN_D0,
		.pin_vsync = CAM_PIN_VSYNC,
		.pin_href = CAM_PIN_HREF,
		.pin_pclk = CAM_PIN_PCLK,

		//XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
		.xclk_freq_hz = 20000000,
		.ledc_timer = LEDC_TIMER_0,
		.ledc_channel = LEDC_CHANNEL_0,

		.pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
		.frame_size = FRAMESIZE_SVGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

		.jpeg_quality = 15, //0-63, for OV series camera sensors, lower number means higher quality
		.fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
		.grab_mode = CAMERA_GRAB_LATEST,
};

esp_err_t camera_init(void)
{
	ESP_LOGI(TAG, "camera_init start");

	esp_err_t err = esp_camera_init(&camera_config);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "camera_init failed");
		return err;
	}

	ESP_LOGI(TAG, "camera_init complete");
	return ESP_OK;
}
#endif

void camera_start(void)
{
	glob_set_bits_status_reg(STATUS_CAMERA_START);
}

void camera_stop(void)
{
	glob_clear_bits_status_reg(STATUS_CAMERA_START);
}


