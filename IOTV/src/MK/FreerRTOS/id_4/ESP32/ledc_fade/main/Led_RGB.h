#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"

typedef struct {
	ledc_timer_config_t timer;
//	ledc_channel_config_t pins[3];
	ledc_channel_config_t red;
	ledc_channel_config_t green;
	ledc_channel_config_t blue;
} led_rgb_t;

// Поочерёдная смена цвета на всех светодиодах. Один цвет загорелся - погас, следующий цвет загорелся погас.
void Lef_RGB_script_1_init();
void Lef_RGB_script_1();

// Поочерёдная смена цвета на всех светодиодах. Один цвет загорелся, начинает выключаться одновременно с включением второго цвета
void Lef_RGB_script_2();
void Lef_RGB_script_3();


//void Led_RGB_fade_start(const led_rgb_t *led, );
