/*
 * menupage.h
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_MENUPAGE_H_
#define MAIN_MENUPAGE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lvgl.h"
#include "screendefault.h"
#include "Global_def.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "screen_page/page.h"
#include "screen_page/datetime1/datetime1.h"
#include "screen_page/datetime2/datetime2.h"
#include "screen_page/meteo_chart/meteo_chart.h"
#include "screen_page/home_meteo/home_meteo.h"
#include "status_panel/status_panel.h"

void menuPageInit(void);
void menu_page_deinit(void);

#endif /* MAIN_MENUPAGE_H_ */
