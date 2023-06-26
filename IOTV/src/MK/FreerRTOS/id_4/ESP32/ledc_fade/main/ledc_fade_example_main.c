#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "Led_RGB.h"

extern uint8_t Led_RGB_scriptNumber;
TaskHandle_t led_rgb_task = NULL;

void app_main(void)
{
	xTaskCreate(Led_RGB_Task, "Led_RGB_Task", 4096, NULL, 1, &led_rgb_task);


//	ledc_timer_config_t t0 = {
//			.duty_resolution = LEDC_TIMER_13_BIT,
//			.freq_hz = 1000,
//			.speed_mode = LEDC_HIGH_SPEED_MODE,
//			.timer_num = LEDC_TIMER_0,
//			.clk_cfg = LEDC_AUTO_CLK,
//	};
//	ledc_timer_config_t t1 = {
//			.duty_resolution = LEDC_TIMER_13_BIT,
//			.freq_hz = 1000,
//			.speed_mode = LEDC_LOW_SPEED_MODE,
//			.timer_num = LEDC_TIMER_0,
//			.clk_cfg = LEDC_AUTO_CLK,
//	};
//	ledc_timer_config(&t0);
//	ledc_timer_config(&t1);
//
//	ledc_channel_config_t channel1 = {
//			.gpio_num = GPIO_NUM_15,
//			.channel = LEDC_CHANNEL_0,
//			.duty = 0,
//			.speed_mode = LEDC_HIGH_SPEED_MODE,
//			.hpoint = 0,
//			.timer_sel = LEDC_TIMER_0,
//			.flags.output_invert = 0
//	};
//	ledc_channel_config_t channel2 = {
//			.gpio_num = GPIO_NUM_5,
//			.channel = LEDC_CHANNEL_0,
//			.duty = 0,
//			.speed_mode = LEDC_LOW_SPEED_MODE,
//			.hpoint = 0,
//			.timer_sel = LEDC_TIMER_0,
//			.flags.output_invert = 0
//	};
//
//	ledc_channel_config(&channel1);
//	ledc_channel_config(&channel2);
//
//	ledc_fade_func_install(0);

	while (1)
	{
		vTaskDelay(10000 / portTICK_PERIOD_MS);
//		if (++Led_RGB_scriptNumber > 6)
//			Led_RGB_scriptNumber = 1;
	}
}
