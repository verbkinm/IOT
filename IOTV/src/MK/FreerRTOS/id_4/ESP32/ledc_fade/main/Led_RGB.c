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

#define LEDC_TEST_FREQ			5000
#define LEDC_TEST_FADE_TIME     2000
#define LEDS_LENGTH 3

extern uint16_t LED_maxDuty, LED_minDuty;
extern uint8_t Led_RGB_scriptNumber;
extern bool demo;

static led_rgb_t leds[LEDS_LENGTH];

static TaskHandle_t led_rgb_script_task = NULL;

static void Led_RGB_default();

static void Led_RGB_script_4_ext1(led_rgb_t *led);
static void Led_RGB_script_4_ext2(led_rgb_t *led, Led_RGB_color_t color);

static void Led_RGB_script_5_ext1(led_rgb_t *led);
static void Led_RGB_script_5_ext2(led_rgb_t *led, Led_RGB_color_t color, uint16_t duty);

static void Led_RGB_script_6_ext1(led_rgb_t *led);
static void Led_RGB_script_6_ext2(led_rgb_t *led, Led_RGB_color_t color, uint16_t duty);

void Led_RGB_Task(void *pvParameters)
{
	Led_RGB_default();

	uint8_t lastscriptNumber;
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
			xTaskCreate(Led_RGB_script_4, "Led_RGB_script_4", 2048, NULL, 1, &led_rgb_script_task);
		}
		else if (Led_RGB_scriptNumber == 5)
		{
			Led_RGB_script_5_init();
			xTaskCreate(Led_RGB_script_5, "Led_RGB_script_5", 2048, NULL, 1, &led_rgb_script_task);
		}
		else if (Led_RGB_scriptNumber == 6)
		{
			Led_RGB_script_6_init();
			xTaskCreate(Led_RGB_script_6, "Led_RGB_script_6", 2048, NULL, 1, &led_rgb_script_task);
		}

		while (Led_RGB_scriptNumber == lastscriptNumber)
			vTaskDelay(100 / portTICK_PERIOD_MS);

		if (led_rgb_script_task != NULL)
		{
			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
			ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0);

			ledc_fade_func_uninstall();

			vTaskDelete(led_rgb_script_task);
			led_rgb_script_task = NULL;
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

	ledc_timer_config_t t1 = {
			.duty_resolution = LEDC_TIMER_13_BIT,
			.freq_hz = 1000,
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.timer_num = LEDC_TIMER_0,
			.clk_cfg = LEDC_AUTO_CLK,
	};

	ledc_timer_config(&t0);
	ledc_timer_config(&t1);

	ledc_channel_config_t channel = {
			.channel = LEDC_CHANNEL_0,
			.duty = 0,
			.speed_mode = t0.speed_mode,
			.hpoint = 0,
			.timer_sel = t0.timer_num,
			.flags.output_invert = 0
	};

	for (int i = 0; i < LEDS_LENGTH; ++i)
	{
		leds[i].red = channel;
		leds[i].green = channel;
		leds[i].blue = channel;
	}

	leds[0].red.gpio_num = LED_0_RED;
	leds[0].green.gpio_num = LED_0_GREEN;
	leds[0].blue.gpio_num = LED_0_BLUE;

	leds[1].red.gpio_num = LED_1_RED;
	leds[1].green.gpio_num = LED_1_GREEN;
	leds[1].blue.gpio_num = LED_1_BLUE;

	leds[2].red.gpio_num = LED_2_RED;
	leds[2].green.gpio_num = LED_2_GREEN;
	leds[2].blue.gpio_num = LED_2_BLUE;

	for (int i = 0; i < LEDS_LENGTH; ++i)
	{
		gpio_reset_pin(leds[i].red.gpio_num);
		gpio_reset_pin(leds[i].green.gpio_num);
		gpio_reset_pin(leds[i].blue.gpio_num);

		gpio_set_level(leds[i].red.gpio_num, 0);
		gpio_set_level(leds[i].green.gpio_num, 0);
		gpio_set_level(leds[i].blue.gpio_num, 0);

		gpio_set_direction(leds[i].red.gpio_num, GPIO_MODE_OUTPUT);
		gpio_set_direction(leds[i].green.gpio_num, GPIO_MODE_OUTPUT);
		gpio_set_direction(leds[i].blue.gpio_num, GPIO_MODE_OUTPUT);
	}

	ledc_fade_func_install(0);
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

void Led_RGB_script_5_init()
{
	Led_RGB_default();
}

void Led_RGB_script_6_init()
{
	Led_RGB_default();
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

		if (demo)
		{
			Led_RGB_scriptNumber = 2;
			while (1);
		}
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

		if (demo)
		{
			Led_RGB_scriptNumber = 3;
			while (1);
		}
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

		if (demo)
		{
			Led_RGB_scriptNumber = 4;
			while (1);
		}
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

	ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, 0, 0);
	ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, 0, 0);
	ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, 0, 0);
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

		if (demo)
		{
			Led_RGB_scriptNumber = 5;
			while (1);
		}
	}
}

static void Led_RGB_script_5_ext1(led_rgb_t *led)
{
	if (led == NULL)
		return;

	led->red.channel = LEDC_CHANNEL_0;
	led->green.channel = LEDC_CHANNEL_1;
	led->blue.channel = LEDC_CHANNEL_2;

	led->red.speed_mode = LEDC_LOW_SPEED_MODE;
	led->green.speed_mode = LEDC_LOW_SPEED_MODE;
	led->blue.speed_mode = LEDC_LOW_SPEED_MODE;

	ledc_channel_config(&led->red);
	ledc_channel_config(&led->green);
	ledc_channel_config(&led->blue);
}

static void Led_RGB_script_5_ext2(led_rgb_t *led, Led_RGB_color_t color, uint16_t duty)
{
	if (led == NULL)
		return;

	static bool flag = false;

	led->red.speed_mode = LEDC_HIGH_SPEED_MODE;
	led->green.speed_mode = LEDC_HIGH_SPEED_MODE;
	led->blue.speed_mode = LEDC_HIGH_SPEED_MODE;

	if (flag)
	{
		led->red.channel = LEDC_CHANNEL_3;
		led->green.channel = LEDC_CHANNEL_4;
		led->blue.channel = LEDC_CHANNEL_5;

		flag = false;
	}
	else
	{
		led->red.channel = LEDC_CHANNEL_0;
		led->green.channel = LEDC_CHANNEL_1;
		led->blue.channel = LEDC_CHANNEL_2;

		flag = true;
	}

	ledc_channel_config(&led->red);
	ledc_channel_config(&led->green);
	ledc_channel_config(&led->blue);

	switch (color)
	{
	case RED:
		ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, duty, 0);
		ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, 0, 0);
		ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, 0, 0);
		break;
	case GREEN:
		ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, 0, 0);
		ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, duty, 0);
		ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, 0, 0);
		break;
	case BLUE:
		ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, 0, 0);
		ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, 0, 0);
		ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, duty, 0);
		break;
	case YELLOW:
		ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, duty, 0);
		ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, duty, 0);
		ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, 0, 0);
		break;
	case LIGTHBLUE:
		ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, 0, 0);
		ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, duty, 0);
		ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, duty, 0);
		break;
	case MAGENTA:
		ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, duty, 0);
		ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, 0, 0);
		ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, duty, 0);
		break;
	case WHITE:
		ledc_set_duty_and_update(led->red.speed_mode, led->red.channel, duty, 0);
		ledc_set_duty_and_update(led->green.speed_mode, led->green.channel, duty, 0);
		ledc_set_duty_and_update(led->blue.speed_mode, led->blue.channel, duty, 0);
		break;
	default:
		break;
	}
}

void Led_RGB_script_5()
{
	while (1)
	{
		for (int color = RED; color <= WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
			{
				Led_RGB_script_5_ext1(&leds[i]);
				Led_RGB_setColor(&leds[i], color, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_setColor(&leds[i], color, LED_minDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_script_5_ext2(&leds[i], color, LED_minDuty);
			}
		}

		if (demo)
		{
			Led_RGB_scriptNumber = 6;
			while (1);
		}
	}
}

static void Led_RGB_script_6_ext1(led_rgb_t *led)
{
	Led_RGB_script_5_ext1(led);
}

static void Led_RGB_script_6_ext2(led_rgb_t *led, Led_RGB_color_t color, uint16_t duty)
{
	Led_RGB_script_5_ext2(led, color, duty);
}

void Led_RGB_script_6()
{
	while (1)
	{
		for (int color = RED; color <= WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
			{
				Led_RGB_script_6_ext1(&leds[i]);
				Led_RGB_setColor(&leds[i], color, LED_minDuty, 0, LEDC_FADE_WAIT_DONE);
				Led_RGB_setColor(&leds[i], color, LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_script_6_ext2(&leds[i], color, LED_maxDuty);
			}
			for (int i = 0; i < LEDS_LENGTH; ++i)
			{
				Led_RGB_script_6_ext1(&leds[i]);
				Led_RGB_setColor(&leds[i], color, LED_maxDuty, 0, LEDC_FADE_WAIT_DONE);
				Led_RGB_setColor(&leds[i], color, LED_minDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_script_6_ext2(&leds[i], color, LED_minDuty);
			}
		}

		if (demo)
		{
			Led_RGB_scriptNumber = 1;
			while (1);
		}
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
