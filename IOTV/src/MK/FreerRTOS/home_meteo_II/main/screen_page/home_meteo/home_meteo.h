/*
 * home_meteo.h
 *
 *  Created on: 15 февр. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_HOME_METEO_HOME_METEO_H_
#define MAIN_SCREEN_PAGE_HOME_METEO_HOME_METEO_H_

#include "lvgl.h"
#include "Global_def.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "screen_page/screendefault.h"
#include "screen_page/page.h"
#include "status_panel/status_panel.h"
#include "Local_Lib/widget/bar.h"
#include "service/bme280/bme280.h"

void home_meteo_page_init(void);
void home_meteo_page_deinit(void);

#endif /* MAIN_SCREEN_PAGE_HOME_METEO_HOME_METEO_H_ */
