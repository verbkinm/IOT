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
#define LEDC_TEST_FADE_TIME     500
#define LEDS_LENGTH 3

extern uint16_t LED_maxDuty, LED_minDuty;
extern uint8_t Led_RGB_scriptNumber;

static led_rgb_t leds[LEDS_LENGTH];

static TaskHandle_t led_rgb_script_task = NULL;

static void Led_RGB_startFade(const ledc_channel_config_t *pin, uint16_t duty, uint16_t time, ledc_fade_mode_t mode);
static void Led_RGB_default();

static void Led_RGB_script_4_ext1(led_rgb_t *led);
static void Led_RGB_script_4_ext2(led_rgb_t *led, Led_RGB_color_t color);

void Led_RGB_Task(void *pvParameters)
{
	Led_RGB_default();

	uint8_t lastscriptNumber;// = scriptNumber;
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
		else if (Led_RGB_scriptNumber == 3)
		{
			Led_RGB_script_3_init();
			xTaskCreate(Led_RGB_script_3, "Led_RGB_script_3", 2048, NULL, 1, &led_rgb_script_task);
		}
		else if (Led_RGB_scriptNumber == 4)
		{
			Led_RGB_script_4_init();
			xTaskCreate(Led_RGB_script_4, "Led_RGB_script_3", 2048, NULL, 1, &led_rgb_script_task);
		}

		while (Led_RGB_scriptNumber == lastscriptNumber)
			vTaskDelay(100 / portTICK_PERIOD_MS);

		if (led_rgb_script_task != NULL)
		{
			//			vTaskSuspend(led_rgb_script_task);


			// выключить все светодиоды
			//			Led_RGB_shutdown(&led_1, 0);
			//			Led_RGB_shutdown(&led_2, 0);
			//
			//			ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
			//			ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
			//			ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);
			//
			//			ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
			//			ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
			//			ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);

			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);

			ledc_fade_func_uninstall();

			//			vTaskDelay(100 / portTICK_PERIOD_MS);
			vTaskDelete(led_rgb_script_task);
			led_rgb_script_task = NULL;

//			vTaskDelay(200 / portTICK_PERIOD_MS);
		}
	}
}

static void Led_RGB_default()
{
	ledc_timer_config_t t0 = {
			.duty_resolution = LEDC_TIMER_13_BIT,
			.freq_hz = 1000,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.timer_num = LEDC_TIMER_0,
			.clk_cfg = LEDC_AUTO_CLK,
	};
	ledc_timer_config(&t0);

	ledc_channel_config_t channel = {
			.channel = LEDC_CHANNEL_0,
			.duty = 0,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = LEDC_TIMER_0,
			.flags.output_invert = 0
	};

	for (int i = 0; i < LEDS_LENGTH; ++i)
	{
		leds[i].red = channel;
		leds[i].green = channel;
		leds[i].blue = channel;
	}

	leds[0].red.gpio_num = GPIO_NUM_15;
	leds[0].green.gpio_num = GPIO_NUM_2;
	leds[0].blue.gpio_num = GPIO_NUM_4;

	leds[1].red.gpio_num = GPIO_NUM_16;
	leds[1].green.gpio_num = GPIO_NUM_17;
	leds[1].blue.gpio_num = GPIO_NUM_5;

	leds[2].red.gpio_num = GPIO_NUM_18;
	leds[2].green.gpio_num = GPIO_NUM_19;
	leds[2].blue.gpio_num = GPIO_NUM_21;

	for (int i = 0; i < LEDS_LENGTH; ++i)
	{
		gpio_reset_pin(leds[i].red.gpio_num);
		gpio_reset_pin(leds[i].green.gpio_num);
		gpio_reset_pin(leds[i].blue.gpio_num);

//		gpio_set_level(leds[i].red.gpio_num, 0);
//		gpio_set_level(leds[i].green.gpio_num, 0);
//		gpio_set_level(leds[i].blue.gpio_num, 0);
//
//		gpio_set_direction(leds[i].red.gpio_num, GPIO_MODE_OUTPUT);
//		gpio_set_direction(leds[i].green.gpio_num, GPIO_MODE_OUTPUT);
//		gpio_set_direction(leds[i].blue.gpio_num, GPIO_MODE_OUTPUT);
	}

	//!!!
//	const led_rgb_t *led_all = &leds[0];
//	ledc_channel_config(&led_all->red);
//	ledc_channel_config(&led_all->green);
//	ledc_channel_config(&led_all->blue);

//	ledc_fade_func_install(0);
//	vTaskDelay(20000 / portTICK_PERIOD_MS);
}

void Led_RGB_script_1_init()
{
	Led_RGB_default();
	for (int i = 0; i < LEDS_LENGTH; ++i)
	{
		leds[i].red.channel = LEDC_CHANNEL_0;
		leds[i].green.channel = LEDC_CHANNEL_1;
		leds[i].blue.channel = LEDC_CHANNEL_2;

		leds[i].red.timer_sel = LEDC_TIMER_0;
		leds[i].green.timer_sel = LEDC_TIMER_0;
		leds[i].blue.timer_sel = LEDC_TIMER_0;

		ledc_channel_config(&leds[i].red);
		ledc_channel_config(&leds[i].green);
		ledc_channel_config(&leds[i].blue);
	}

	ledc_fade_func_install(0);
}

void Led_RGB_script_2_init()
{
	Led_RGB_script_1_init();
}

void Led_RGB_script_3_init()
{
	Led_RGB_script_1_init();
}

void Led_RGB_script_4_init()
{
	Led_RGB_default();

	ledc_timer_config_t t0 = {
			.duty_resolution = LEDC_TIMER_13_BIT,
			.freq_hz = 1000,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.timer_num = LEDC_TIMER_0,
			.clk_cfg = LEDC_AUTO_CLK,
	};

	ledc_timer_config_t t1 = t0;
	t1.timer_num = LEDC_TIMER_1;

	ledc_timer_config(&t0);
	ledc_timer_config(&t1);

	ledc_fade_func_install(0);
}

void Led_RGB_script_1()
{
	const led_rgb_t *led_all = &leds[0];
	while (1)
	{
		for (int i = RED; i <= WHITE; ++i)
		{
			Led_RGB_setColor(led_all, i, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
			Led_RGB_shutdown(led_all, LEDC_TEST_FADE_TIME);
		}

		Led_RGB_scriptNumber = 2;
		while (1);
	}
}

void Led_RGB_script_2()
{
	const led_rgb_t *led_all = &leds[0];

	while (1)
	{
		for (int i = RED; i <= WHITE; ++i)
			Led_RGB_setColor(led_all, i, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);

		Led_RGB_startFade(&(led_all->green), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led_all->blue), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);

		Led_RGB_scriptNumber = 3;
		while (1);
	}
}

void Led_RGB_script_3()
{
	const led_rgb_t *led_all = &leds[0];
	while (1)
	{
		for (int i = RED; i <= WHITE; ++i)
		{
			Led_RGB_setColor(led_all, i, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
			vTaskDelay(500 / portTICK_PERIOD_MS);
			Led_RGB_setColor(led_all, i, LED_minDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}

		Led_RGB_scriptNumber = 4;
		while (1);
	}
}

static void Led_RGB_script_4_ext1(led_rgb_t *led)
{
	if (led == NULL)
		return;

	led->red.channel = LEDC_CHANNEL_0;
	led->green.channel = LEDC_CHANNEL_1;
	led->blue.channel = LEDC_CHANNEL_2;

	led->red.timer_sel = LEDC_TIMER_0;
	led->green.timer_sel = LEDC_TIMER_0;
	led->blue.timer_sel = LEDC_TIMER_0;

	ledc_channel_config(&led->red);
	ledc_channel_config(&led->green);
	ledc_channel_config(&led->blue);
}

static void Led_RGB_script_4_ext2(led_rgb_t *led, Led_RGB_color_t color)
{
	if (led == NULL)
		return;

	led->red.channel = LEDC_CHANNEL_3;
	led->green.channel = LEDC_CHANNEL_4;
	led->blue.channel = LEDC_CHANNEL_5;

	led->red.timer_sel = LEDC_TIMER_1;
	led->green.timer_sel = LEDC_TIMER_1;
	led->blue.timer_sel = LEDC_TIMER_1;

	ledc_channel_config(&led->red);
	ledc_channel_config(&led->green);
	ledc_channel_config(&led->blue);

	ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, LED_minDuty, 0);
	ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, LED_minDuty, 0);
	ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, LED_minDuty, 0);
}

void Led_RGB_script_4()
{
	while (1)
	{
		for (int color = RED; color <= WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
			{
				Led_RGB_script_4_ext1(&leds[i]);
				Led_RGB_setColor(&leds[i], color, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_setColor(&leds[i], color, LED_minDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_script_4_ext2(&leds[i], color);
			}
		}

		Led_RGB_scriptNumber = 1;
		while (1);
	}
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
