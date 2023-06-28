#pragma once

#include "driver/ledc.h"

typedef enum {
	LED_RED,
	LED_GREEN,
	LED_BLUE,
	LED_YELLOW,
	LED_LIGTHBLUE,
	LED_MAGENTA,
	LED_WHITE,
	LED_NONE_COLOR
} Led_RGB_color_t;

typedef struct {
	ledc_channel_config_t red;
	ledc_channel_config_t green;
	ledc_channel_config_t blue;
} led_rgb_t;

void Led_RGB_Task(void *pvParameters);

void Led_RGB_startFade(const ledc_channel_config_t *pin, uint16_t duty, uint16_t time, ledc_fade_mode_t mode);
void Led_RGB_setColor(const led_rgb_t *led, Led_RGB_color_t color, uint16_t duty, int16_t time, ledc_fade_mode_t mode);
void Led_RGB_shutdown(const led_rgb_t *led, int16_t time);

// Поочерёдная смена цвета на всех светодиодах. Один цвет загорелся - погас, следующий цвет загорелся погас.
void Led_RGB_script_1();

// Поочерёдная смена цвета на всех светодиодах. Один цвет "переходит" в другой.
void Led_RGB_script_2();

// Поочерёдная смена цвета на всех светодиодах. В отличии от первого режима сведотиоды гаснут не до конца
void Led_RGB_script_3();

// Поочерёдная загораются/гаснут все сведодиоты. Смена цвета на каждом круге. Фоновый цвет - выкл.
void Led_RGB_script_4();

// Поочерёдная загораются/гаснут все сведодиоты. Смена цвета на каждом круге. Фоновый цвет образуется наполнением текущего сведодиода.
void Led_RGB_script_5();

// Как и 5-й режим, но загораются и гаснут светодиоды на каждый круг.
void Led_RGB_script_6();

// Тревога
void Led_RGB_script_alert();
