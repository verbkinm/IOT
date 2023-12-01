/*
 * TFT_touch_screen.h
 *
 *  Created on: 23 нояб. 2023 г.
 *      Author: user
 */

#ifndef MAIN_TFT_TOUCH_SCREEN_H_
#define MAIN_TFT_TOUCH_SCREEN_H_

#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lcd_touch_gt911.h"

void TFT_init(void);
void draw_page(void);

#endif /* MAIN_TFT_TOUCH_SCREEN_H_ */
