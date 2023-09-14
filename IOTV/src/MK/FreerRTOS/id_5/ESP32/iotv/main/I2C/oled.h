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
#include "esp_app_desc.h"
#include "lvgl.h"
#include "esp_log.h"
#include "esp_err.h"
#include "math.h"
#include "driver/i2c.h"
#include "esp_lvgl_port.h"

#include "Global_def.h"
#include "DS3231.h"
#include "BME280.h"

void OLED_init2(uint8_t displayOrientation);
void OLED_boot_screen(void);
void OLED_init_draw_element(void);
void OLED_Draw_Page(const struct THP *thp, const struct DateTime *dt, uint8_t status);

#endif /* MAIN_I2C_OLED_H_ */
