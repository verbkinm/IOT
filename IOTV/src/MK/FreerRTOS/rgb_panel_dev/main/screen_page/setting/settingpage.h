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
#include "../../status_panel/status_panel.h"

void settingPageInit(void);
void drawSettingPage(void);

#endif /* MAIN_SCREEN_PAGE_SETTINGPAGE_H_ */
