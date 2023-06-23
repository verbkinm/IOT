/*
 * Led_RGB.cpp
 *
 *  Created on: 21 июн. 2023 г.
 *      Author: verbkinm
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Global_def.h"
#include "Led_RGB.h"

#define LEDC_TEST_FREQ			1000
#define LEDC_TEST_FADE_TIME    (2000)

extern uint16_t LED_maxDuty;

extern uint8_t Led_RGB_scriptNumber;
static led_rgb_t led_1, led_2;

static TaskHandle_t led_rgb_script_task = NULL;

static void Led_RGB_startFade(const ledc_channel_config_t *pin, uint16_t duty, uint16_t time, ledc_fade_mode_t mode);

void Led_RGB_Task(void *pvParameters)
{
	uint8_t lastscriptNumber;// = scriptNumber;
	Led_RGB_script_1_init();
	while (1)
	{
		lastscriptNumber = Led_RGB_scriptNumber;

		if (Led_RGB_scriptNumber == 1)
		{
			Led_RGB_script_1_init();
			xTaskCreate(Led_RGB_script_1, "Led_RGB_script_1", 2048, NULL, 1, &led_rgb_script_task);
		}
		else if (Led_RGB_scriptNumber == 2)
		{
			Led_RGB_script_2_init();
			xTaskCreate(Led_RGB_script_2, "Led_RGB_script_2", 2048, NULL, 1, &led_rgb_script_task);
		}

		while (Led_RGB_scriptNumber == lastscriptNumber)
			vTaskDelay(100 / portTICK_PERIOD_MS);

		if (led_rgb_script_task != NULL)
		{
//			vTaskSuspend(led_rgb_script_task);


			// выключить все светодиоды
//			Led_RGB_shutdown(&led_1, 0);
//			Led_RGB_shutdown(&led_2, 0);

			ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
			ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
			ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);

			ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
			ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
			ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

//			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
//			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
//			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);

//			ledc_fade_func_uninstall();

			vTaskDelay(100 / portTICK_PERIOD_MS);
			vTaskDelete(led_rgb_script_task);
			led_rgb_script_task = NULL;
		}
	}
}

void Led_RGB_script_1_init()
{
	ledc_timer_config_t t0 = {
			.duty_resolution = LEDC_TIMER_13_BIT,
			.freq_hz = 1000,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.timer_num = LEDC_TIMER_0,
			.clk_cfg = LEDC_AUTO_CLK,
	};

	//	ledc_timer_config_t t1 = {
	//			.duty_resolution = LEDC_TIMER_13_BIT,
	//			.freq_hz = 1000,
	//			.speed_mode = LEDC_HIGH_SPEED_MODE,
	//			.timer_num = LEDC_TIMER_1,
	//			.clk_cfg = LEDC_AUTO_CLK,
	//	};
	//
	//	ledc_timer_config_t t2 = {
	//			.duty_resolution = LEDC_TIMER_13_BIT,
	//			.freq_hz = 1000,
	//			.speed_mode = LEDC_HIGH_SPEED_MODE,
	//			.timer_num = LEDC_TIMER_2,
	//			.clk_cfg = LEDC_AUTO_CLK,
	//	};

	//	led_1.timer = t;
	//	led_2.timer = t;

	ledc_channel_config_t red = {
			.channel = LEDC_CHANNEL_0,
			.duty = 0,
			.gpio_num = GPIO_NUM_15,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = t0.timer_num,
			.flags.output_invert = 0
	};
	ledc_channel_config_t green = {
			.channel = LEDC_CHANNEL_1,
			.duty = 0,
			.gpio_num = GPIO_NUM_2,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = t0.timer_num,
			.flags.output_invert = 0
	};
	ledc_channel_config_t blue = {
			.channel = LEDC_CHANNEL_2,
			.duty = 0,
			.gpio_num = GPIO_NUM_4,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = t0.timer_num,
			.flags.output_invert = 0
	};

	led_1.red = red;
	led_1.green = green;
	led_1.blue = blue;

	led_2.red = red;
	led_2.red.gpio_num = GPIO_NUM_14;

	led_2.green = green;
	led_2.green.gpio_num = GPIO_NUM_13;

	led_2.blue = blue;
	led_2.blue.gpio_num = GPIO_NUM_12;

	ledc_timer_config(&t0);
	//	ledc_timer_config(&t1);
	//	ledc_timer_config(&t2);

	ledc_channel_config(&led_1.red);
	ledc_channel_config(&led_1.green);
	ledc_channel_config(&led_1.blue);

	ledc_channel_config(&led_2.red);
	ledc_channel_config(&led_2.green);
	ledc_channel_config(&led_2.blue);

	ledc_fade_func_install(0);
}

void Led_RGB_script_2_init()
{
	Led_RGB_script_1_init();
}
void Led_RGB_script_1()
{
	const led_rgb_t *led_all = &led_1;
	while (Led_RGB_scriptNumber == 1)
	{
		Led_RGB_setColor(led_all, RED, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);

		Led_RGB_setColor(led_all, GREEN, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);

		Led_RGB_setColor(led_all, BLUE, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);

		Led_RGB_setColor(led_all, YELLOW, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);

		Led_RGB_setColor(led_all, LIGTHBLUE, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);

		Led_RGB_setColor(led_all, MAGENTA, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);

		Led_RGB_setColor(led_all, WHITE, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);
	}
}

void Led_RGB_script_2()
{
	const led_rgb_t *led_all = &led_1;

	while (Led_RGB_scriptNumber == 2)
	{
		Led_RGB_setColor(led_all, RED, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
		Led_RGB_setColor(led_all, GREEN, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
		Led_RGB_setColor(led_all, BLUE, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
		Led_RGB_setColor(led_all, YELLOW, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
		Led_RGB_setColor(led_all, LIGTHBLUE, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
		Led_RGB_setColor(led_all, MAGENTA, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
		Led_RGB_setColor(led_all, WHITE, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);

		Led_RGB_startFade(&(led_all->green), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led_all->blue), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
	}
}


void Led_RGB_script_3()
{

}

void Led_RGB_startFade(const ledc_channel_config_t *pin, uint16_t duty, uint16_t time, ledc_fade_mode_t mode)
{
	if (pin == NULL)
		return;

	ledc_set_fade_with_time(pin->speed_mode, pin->channel, duty, time);
	ledc_fade_start(pin->speed_mode, pin->channel, mode);
}

void Led_RGB_shutdown(const led_rgb_t *led, int16_t time)
{
	if (led == NULL)
		return;

	Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);
	vTaskDelay(time / portTICK_PERIOD_MS);
}

void Led_RGB_setColor(const led_rgb_t *led, Led_RGB_color_t color, uint16_t duty, int16_t time, ledc_fade_mode_t mode)
{
	if (led == NULL)
		return;

	switch (color)
	{
	case RED:
		Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->red), duty, time, mode);
		break;
	case GREEN:
		Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->green), duty, time, mode);
		break;
	case BLUE:
		Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	case YELLOW:
		Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->red), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->green), duty, time, mode);
		break;
	case LIGTHBLUE:
		Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->green), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	case MAGENTA:
		Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->red), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	case WHITE:
		Led_RGB_startFade(&(led->red), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->green), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	default:
		break;
	}
}

void Led_RGB_unsetColor(const led_rgb_t *led, Led_RGB_color_t color, uint16_t duty, int16_t time, ledc_fade_mode_t mode)
{

}

