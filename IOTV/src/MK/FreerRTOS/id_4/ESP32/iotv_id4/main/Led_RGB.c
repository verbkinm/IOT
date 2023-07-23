/*
 * Led_RGB.cpp
 *
 *  Created on: 21 июн. 2023 г.
 *      Author: verbkinm
 */
#include "Led_RGB.h"

#define LEDC_TEST_FREQ			5000
#define LEDC_TEST_FADE_TIME     5000
#define LEDS_LENGTH 5

extern struct IOTV_Server_embedded iot;

static const char *TAG = "Led_RGB";

static led_rgb_t leds[LEDS_LENGTH];
static TaskHandle_t led_rgb_script_task = NULL;

static int16_t *LED_maxDuty, *LED_minDuty;
static int8_t *Led_RGB_scriptNumber, *Led_Manual;

static void Led_RGB_default();

void Led_RGB_Task(void *pvParameters)
{
	while(iot.state == 0)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	LED_maxDuty = (int16_t *)iot.readChannel[CH_MAX_DUTY].data;
	LED_minDuty = (int16_t *)iot.readChannel[CH_MIN_DUTY].data;
	Led_RGB_scriptNumber = (int8_t *)iot.readChannel[CH_LED_MODE].data;
	Led_Manual = (int8_t *)iot.readChannel[CH_LED_COLOR].data;

	ESP_LOGW(TAG, "Led_RGB task created");

	Led_RGB_default();

	int8_t lastscriptNumber;
	while (1)
	{
//		*Led_RGB_scriptNumber = inRange(*Led_RGB_scriptNumber, -2, 6);

		lastscriptNumber = *Led_RGB_scriptNumber;

		for (int i = 0; i < LEDS_LENGTH; ++i)
			Led_RGB_shutdown(&(leds[i]), 200);

		if (*Led_RGB_scriptNumber == 1)
			xTaskCreate(Led_RGB_script_1, "Led_RGB_script_1", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);
		else if (*Led_RGB_scriptNumber == 2)
			xTaskCreate(Led_RGB_script_2, "Led_RGB_script_2", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);
		else if (*Led_RGB_scriptNumber == 3)
			xTaskCreate(Led_RGB_script_3, "Led_RGB_script_3", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);
		else if (*Led_RGB_scriptNumber == 4)
			xTaskCreate(Led_RGB_script_4, "Led_RGB_script_4", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);
		else if (*Led_RGB_scriptNumber == 5)
			xTaskCreate(Led_RGB_script_5, "Led_RGB_script_5", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);
		else if (*Led_RGB_scriptNumber == 6)
			xTaskCreate(Led_RGB_script_6, "Led_RGB_script_6", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);
		else if (*Led_RGB_scriptNumber == -2)
			xTaskCreate(Led_RGB_script_manual, "Led_RGB_script_manual", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);
		else if (*Led_RGB_scriptNumber == -1)
			xTaskCreate(Led_RGB_script_alert, "Led_RGB_script_alert", 2048, NULL, LOW_PROIRITY, &led_rgb_script_task);

		while (*Led_RGB_scriptNumber == lastscriptNumber)
			vTaskDelay(100 / portTICK_PERIOD_MS);

		if (led_rgb_script_task != NULL)
		{
			vTaskDelete(led_rgb_script_task);
			led_rgb_script_task = NULL;
		}
	}
}

static void Led_RGB_default()
{
	ledc_timer_config_t t0 = {
			.duty_resolution = LEDC_TIMER_13_BIT,
			.freq_hz = 5000,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.timer_num = LEDC_TIMER_0,
			.clk_cfg = LEDC_AUTO_CLK,
	};

	ledc_timer_config_t t1 = {
			.duty_resolution = LEDC_TIMER_13_BIT,
			.freq_hz = 5000,
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.timer_num = LEDC_TIMER_0,
			.clk_cfg = LEDC_AUTO_CLK,
	};

	ledc_timer_config(&t0);
	ledc_timer_config(&t1);

	for (int i = 0; i < LEDS_LENGTH; ++i)
	{
		leds[i].red.duty = 0;
		leds[i].red.hpoint = 0;
		leds[i].red.flags.output_invert = 0;
	}

	leds[0].red.channel = LEDC_CHANNEL_0;
	leds[0].red.speed_mode = t0.speed_mode;
	leds[0].red.timer_sel = t0.timer_num;

	leds[0].green.channel = LEDC_CHANNEL_1;
	leds[0].green.speed_mode = t0.speed_mode;
	leds[0].green.timer_sel = t0.timer_num;

	leds[0].blue.channel = LEDC_CHANNEL_2;
	leds[0].blue.speed_mode = t0.speed_mode;
	leds[0].blue.timer_sel = t0.timer_num;

	leds[1].red.channel = LEDC_CHANNEL_3;
	leds[1].red.speed_mode = t0.speed_mode;
	leds[1].red.timer_sel = t0.timer_num;

	leds[1].green.channel = LEDC_CHANNEL_4;
	leds[1].green.speed_mode = t0.speed_mode;
	leds[1].green.timer_sel = t0.timer_num;

	leds[1].blue.channel = LEDC_CHANNEL_5;
	leds[1].blue.speed_mode = t0.speed_mode;
	leds[1].blue.timer_sel = t0.timer_num;

	leds[2].red.channel = LEDC_CHANNEL_0;
	leds[2].red.speed_mode = t1.speed_mode;
	leds[2].red.timer_sel = t1.timer_num;

	leds[2].green.channel = LEDC_CHANNEL_1;
	leds[2].green.speed_mode = t1.speed_mode;
	leds[2].green.timer_sel = t1.timer_num;

	leds[2].blue.channel = LEDC_CHANNEL_2;
	leds[2].blue.speed_mode = t1.speed_mode;
	leds[2].blue.timer_sel = t1.timer_num;

	leds[3].red.channel = LEDC_CHANNEL_3;
	leds[3].red.speed_mode = t1.speed_mode;
	leds[3].red.timer_sel = t1.timer_num;

	leds[3].green.channel = LEDC_CHANNEL_4;
	leds[3].green.speed_mode = t1.speed_mode;
	leds[3].green.timer_sel = t1.timer_num;

	leds[3].blue.channel = LEDC_CHANNEL_5;
	leds[3].blue.speed_mode = t1.speed_mode;
	leds[3].blue.timer_sel = t1.timer_num;

	leds[4].red.channel = LEDC_CHANNEL_6;
	leds[4].red.speed_mode = t0.speed_mode;
	leds[4].red.timer_sel = t0.timer_num;

	leds[4].green.channel = LEDC_CHANNEL_7;
	leds[4].green.speed_mode = t0.speed_mode;
	leds[4].green.timer_sel = t0.timer_num;

	leds[4].blue.channel = LEDC_CHANNEL_6;
	leds[4].blue.speed_mode = t1.speed_mode;
	leds[4].blue.timer_sel = t1.timer_num;


	leds[0].red.gpio_num = LED_0_RED;
	leds[0].green.gpio_num = LED_0_GREEN;
	leds[0].blue.gpio_num = LED_0_BLUE;

	leds[1].red.gpio_num = LED_1_RED;
	leds[1].green.gpio_num = LED_1_GREEN;
	leds[1].blue.gpio_num = LED_1_BLUE;

	leds[2].red.gpio_num = LED_2_RED;
	leds[2].green.gpio_num = LED_2_GREEN;
	leds[2].blue.gpio_num = LED_2_BLUE;

	leds[3].red.gpio_num = LED_3_RED;
	leds[3].green.gpio_num = LED_3_GREEN;
	leds[3].blue.gpio_num = LED_3_BLUE;

	leds[4].red.gpio_num = LED_4_RED;
	leds[4].green.gpio_num = LED_4_GREEN;
	leds[4].blue.gpio_num = LED_4_BLUE;

	for (int i = 0; i < LEDS_LENGTH; ++i)
	{
		ledc_channel_config(&leds[i].red);
		ledc_channel_config(&leds[i].green);
		ledc_channel_config(&leds[i].blue);
	}

	ledc_fade_func_install(0);
}

void Led_RGB_script_1()
{
	while (1)
	{
		for (int color = LED_RED; color <= LED_WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
				Led_RGB_setColor(&(leds[i]), color, *LED_maxDuty, LEDC_TEST_FADE_TIME * 2, LEDC_FADE_NO_WAIT);

//			vTaskDelay((LEDC_TEST_FADE_TIME + 2000) / portTICK_PERIOD_MS);

			for (int i = 0; i < LEDS_LENGTH; ++i)
				Led_RGB_setColor(&(leds[i]), LED_NONE_COLOR, 0, LEDC_TEST_FADE_TIME, LEDC_FADE_NO_WAIT);

			vTaskDelay((LEDC_TEST_FADE_TIME + 2000) / portTICK_PERIOD_MS);
		}
	}
}

void Led_RGB_script_2()
{
	while (1)
	{
		for (int color = LED_RED; color <= LED_WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
				Led_RGB_setColor(&(leds[i]), color, *LED_maxDuty, LEDC_TEST_FADE_TIME * 2, LEDC_FADE_NO_WAIT);

			vTaskDelay((LEDC_TEST_FADE_TIME * 2 + 2000) / portTICK_PERIOD_MS);
		}
	}
}

void Led_RGB_script_3()
{
	while (1)
	{
		for (int color = LED_RED; color <= LED_WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
				Led_RGB_setColor(&(leds[i]), color, *LED_maxDuty, 10000, LEDC_FADE_NO_WAIT);

			vTaskDelay((10000 + 2000) / portTICK_PERIOD_MS);

			for (int i = 0; i < LEDS_LENGTH; ++i)
				Led_RGB_setColor(&(leds[i]), color, *LED_minDuty, 5000, LEDC_FADE_NO_WAIT);

			vTaskDelay((5000 + 2000) / portTICK_PERIOD_MS);
		}
	}
}

void Led_RGB_script_4()
{
	while (1)
	{
		for (int color = LED_RED; color <= LED_WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
			{
				Led_RGB_setColor(&leds[i], color, *LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_setColor(&leds[i], color, 0, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
			}
		}
	}
}

void Led_RGB_script_5()
{
	while (1)
	{
		for (int color = LED_RED; color <= LED_WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
			{
				Led_RGB_setColor(&leds[i], color, *LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
				Led_RGB_setColor(&leds[i], color, *LED_minDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
			}
		}
	}
}

void Led_RGB_script_6()
{
	while (1)
	{
		for (int color = LED_RED; color <= LED_WHITE; ++color)
		{
			for (int i = 0; i < LEDS_LENGTH; ++i)
				Led_RGB_setColor(&leds[i], color, *LED_maxDuty, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);

			for (int i = 0; i < LEDS_LENGTH; ++i)
				Led_RGB_setColor(&leds[i], color, 0, LEDC_TEST_FADE_TIME, LEDC_FADE_WAIT_DONE);
		}
	}
}

void Led_RGB_script_manual()
{
	while (1)
	{
		while (*Led_Manual == -1)
			vTaskDelay(10 / portTICK_PERIOD_MS);

		*Led_Manual = decToBcd(*Led_Manual);
		uint8_t Led_Number = inRange((*Led_Manual) >> 4, 0, LEDS_LENGTH - 1);
		uint8_t Led_Color = inRange((*Led_Manual) & 0x0F, LED_RED, LED_NONE_COLOR);
		Led_RGB_setColor(&leds[Led_Number], Led_Color, *LED_maxDuty, 100, LEDC_FADE_WAIT_DONE);

		*Led_Manual = -1;
	}
}

void Led_RGB_script_alert()
{
	while (1)
	{
		for (int i = 0; i < LEDS_LENGTH; ++i)
			Led_RGB_setColor(&leds[i], LED_RED, *LED_maxDuty, 100, LEDC_FADE_WAIT_DONE);

		for (int i = 0; i < LEDS_LENGTH; ++i)
			Led_RGB_shutdown(&leds[i], 100);
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
	case LED_RED:
		Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->red), duty, time, mode);
		break;
	case LED_GREEN:
		Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->green), duty, time, mode);
		break;
	case LED_BLUE:
		Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	case LED_YELLOW:
		Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->red), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->green), duty, time, mode);
		break;
	case LED_LIGTHBLUE:
		Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->green), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	case LED_MAGENTA:
		Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);

		Led_RGB_startFade(&(led->red), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	case LED_WHITE:
		Led_RGB_startFade(&(led->red), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->green), duty, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), duty, time, mode);
		break;
	case LED_NONE_COLOR:
	default:
		Led_RGB_startFade(&(led->red), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->green), 0, time, LEDC_FADE_NO_WAIT);
		Led_RGB_startFade(&(led->blue), 0, time, LEDC_FADE_NO_WAIT);
		if (mode == LEDC_FADE_WAIT_DONE)
			vTaskDelay(time / portTICK_PERIOD_MS);
		break;
	}
}
