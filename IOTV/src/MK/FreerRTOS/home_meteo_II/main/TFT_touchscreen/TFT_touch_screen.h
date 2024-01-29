/*
 * TFT_touch_screen.h
 *
 *  Created on: 23 нояб. 2023 г.
 *      Author: user
 */

#ifndef MAIN_TFT_TOUCH_SCREEN_H_
#define MAIN_TFT_TOUCH_SCREEN_H_

#include "lvgl.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_log.h"
#include "esp_lcd_touch_gt911.h"
#include "driver/ledc.h"

#include "Global_def.h"
#include "Local_Lib/local_lib.h"

void TFT_init(void);
void rotate_display(lv_disp_rot_t rotation);
lv_disp_rot_t get_rotate_display(void);
void set_display_brightness(uint8_t value);

#endif /* MAIN_TFT_TOUCH_SCREEN_H_ */
