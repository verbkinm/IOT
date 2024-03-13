/*
 * oled.h
 *
 *  Created on: 7 мар. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SERVICE_OLED_OLED_H_
#define MAIN_SERVICE_OLED_OLED_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include <stdio.h>
#include <time.h>
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
#include "I2C/DS3231.h"
#include "service/bme280/bme280.h"
#include "iotv.h"

void OLED_Task(void *pvParameters);
void OLED_set_disp_rotation(lv_disp_rot_t rotation);

#endif /* MAIN_SERVICE_OLED_OLED_H_ */
