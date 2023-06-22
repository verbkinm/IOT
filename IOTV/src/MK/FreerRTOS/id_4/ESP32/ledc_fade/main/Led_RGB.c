/*
 * Led_RGB.cpp
 *
 *  Created on: 21 июн. 2023 г.
 *      Author: verbkinm
 */
#include "Global_def.h"
#include "Led_RGB.h"

#define LEDC_TEST_FREQ			1000
#define LEDC_TEST_FADE_TIME    (2000)

extern uint16_t LED_maxDuty;

static led_rgb_t led_1, led_2;

static void Led_RGB_startFade(const ledc_channel_config_t *pin, uint16_t duty, uint16_t time, ledc_fade_mode_t mode);

void Lef_RGB_script_1_init()
{
	ledc_timer_config_t t = {
			.duty_resolution = LEDC_TIMER_13_BIT,
			.freq_hz = 1000,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.timer_num = LEDC_TIMER_0,
			.clk_cfg = LEDC_AUTO_CLK,
	};
	led_1.timer = t;
	led_2.timer = t;

	ledc_channel_config_t red = {
			.channel = LEDC_CHANNEL_0,
			.duty = 0,
			.gpio_num = GPIO_NUM_15,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = LEDC_TIMER_0,
			.flags.output_invert = 0
	};
	ledc_channel_config_t green = {
			.channel = LEDC_CHANNEL_1,
			.duty = 0,
			.gpio_num = GPIO_NUM_2,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = LEDC_TIMER_0,
			.flags.output_invert = 0
	};
	ledc_channel_config_t blue = {
			.channel = LEDC_CHANNEL_2,
			.duty = 0,
			.gpio_num = GPIO_NUM_4,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.hpoint = 0,
			.timer_sel = LEDC_TIMER_0,
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

	ledc_timer_config(&led_1.timer);

	ledc_channel_config(&led_1.red);
	ledc_channel_config(&led_1.green);
	ledc_channel_config(&led_1.blue);

	ledc_channel_config(&led_2.red);
	ledc_channel_config(&led_2.green);
	ledc_channel_config(&led_2.blue);

	ledc_fade_func_install(0);
}

void Lef_RGB_script_1()
{
	Led_RGB_startFade(&(led_1.red), LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led_2.red), LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
	Led_RGB_startFade(&(led_1.red), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led_2.red), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);

	Led_RGB_startFade(&(led_1.green), LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led_2.green), LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
	Led_RGB_startFade(&(led_1.green), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led_2.green), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);

	Led_RGB_startFade(&(led_1.blue), LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led_2.blue), LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
	Led_RGB_startFade(&(led_1.blue), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);
	Led_RGB_startFade(&(led_2.blue), 0, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
}

void Lef_RGB_script_2()
{

}


void Lef_RGB_script_3()
{

}

void Led_RGB_startFade(const ledc_channel_config_t *pin, uint16_t duty, uint16_t time, ledc_fade_mode_t mode)
{
	if (pin == NULL)
		return;

	ledc_set_fade_with_time(pin->speed_mode, pin->channel, duty, time);
	ledc_fade_start(pin->speed_mode, pin->channel, mode);
}


