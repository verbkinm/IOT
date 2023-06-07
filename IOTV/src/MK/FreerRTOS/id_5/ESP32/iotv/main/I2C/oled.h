/*
 * oled.h
 *
 *  Created on: 5 мая 2023 г.
 *      Author: user
 */

#ifndef MAIN_I2C_OLED_H_
#define MAIN_I2C_OLED_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "lvgl.h"
#include "esp_log.h"
#include "esp_err.h"
#include "math.h"

#include "Global_def.h"
#include "DS3231.h"

void OLED_init(void);
void OLED_Draw_Page(float t, float h, float p, const struct DateTime *dt);
void OLED_Draw_Distance(int d);

#endif /* MAIN_I2C_OLED_H_ */
