#pragma once

#include "driver/ledc.h"

typedef enum {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	LIGTHBLUE,
	MAGENTA,
	WHITE,
	NONE
} Led_RGB_color_t;

typedef struct {
	ledc_channel_config_t red;
	ledc_channel_config_t green;
	ledc_channel_config_t blue;
} led_rgb_t;

void Led_RGB_Task(void *pvParameters);

void Led_RGB_shutdown(const led_rgb_t *led, int16_t time);
void Led_RGB_setColor(const led_rgb_t *led, Led_RGB_color_t color, uint16_t duty, int16_t time, ledc_fade_mode_t mode);

// Поочерёдная смена цвета на всех светодиодах. Один цвет загорелся - погас, следующий цвет загорелся погас.
void Led_RGB_script_1_init();
void Led_RGB_script_1();

// Поочерёдная смена цвета на всех светодиодах. Один цвет загорелся, начинает выключаться одновременно с включением второго цвета
void Led_RGB_script_2_init();
void Led_RGB_script_2();

// Поочерёдная смена цвета на всех светодиодах. В отличии от первого режима сведотиоды гаснут не до конца
void Led_RGB_script_3_init();
void Led_RGB_script_3();

// Поочерёдная загораются/гаснут все сведодиоты. Смена цвета на каждом круге
void Led_RGB_script_4_init();
void Led_RGB_script_4();


//void Led_RGB_fade_start(const led_rgb_t *led, );
