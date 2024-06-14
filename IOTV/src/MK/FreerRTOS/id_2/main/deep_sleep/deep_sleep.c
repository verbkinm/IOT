/*
 * deep_sleep.c
 *
 *  Created on: 8 мая 2024 г.
 *      Author: user
 */

#include "deep_sleep.h"

#include <esp_log.h>
#include <esp_sleep.h>
#include <stdio.h>

void deep_sleep(void)
{
	const int wakeup_time_sec = 10;
	printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
	ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000));
	esp_deep_sleep_start();
}
