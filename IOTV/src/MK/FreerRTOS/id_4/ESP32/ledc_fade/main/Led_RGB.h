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
//	ledc_timer_config_t timer;
//	ledc_channel_config_t pins[3];
	ledc_channel_config_t red;
	ledc_channel_config_t green;
	ledc_channel_config_t blue;
} led_rgb_t;

void Led_RGB_Task(void *pvParameters);

void Led_RGB_shutdown(const led_rgb_t *led, int16_t time);
void Led_RGB_setColor(const led_rgb_t *led, Led_RGB_color_t color, uint16_t duty, int16_t time, ledc_fade_mode_t mode);
void Led_RGB_unsetColor(const led_rgb_t *led, Led_RGB_color_t color, uint16_t duty, int16_t time, ledc_fade_mode_t mode);

// Поочерёдная смена цвета на всех светодиодах. Один цвет загорелся - погас, следующий цвет загорелся погас.
void Led_RGB_script_1_init();
void Led_RGB_script_1();

// Поочерёдная смена цвета на всех светодиодах. Один цвет загорелся, начинает выключаться одновременно с включением второго цвета
void Led_RGB_script_2_init();
void Led_RGB_script_2();

void Led_RGB_script_3();


//void Led_RGB_fade_start(const led_rgb_t *led, );
