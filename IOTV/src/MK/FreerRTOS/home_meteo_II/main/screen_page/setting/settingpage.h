/*
 * settingpage.h
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTINGPAGE_H_
#define MAIN_SCREEN_PAGE_SETTINGPAGE_H_

#include "esp_system.h"
#include "esp_netif.h"
#include "esp_eth.h"

#include <stdint.h>
#include "lvgl.h"

#include "Global_def.h"
#include "elements.h"
#include "esp_wifi.h"
#include "I2C/DS3231.h"
#include "wifi.h"
#include "date_time.h"
#include "weather.h"
#include "display.h"
#include "update.h"
#include "TFT_touchscreen/TFT_touch_screen.h"

void settingPageInit(void);
void setting_page_deinit(void);
void clear_all_sub_page_child(void);

#endif /* MAIN_SCREEN_PAGE_SETTINGPAGE_H_ */
