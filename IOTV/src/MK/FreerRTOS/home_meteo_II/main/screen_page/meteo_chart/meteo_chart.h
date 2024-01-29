/*
 * meteo_chart.h
 *
 *  Created on: 9 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_METEO_CHART_METEO_CHART_H_
#define MAIN_SCREEN_PAGE_METEO_CHART_METEO_CHART_H_

#include "lvgl.h"
#include "Global_def.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "Local_Lib/local_lib.h"
#include "status_panel/status_panel.h"
#include "screen_page/menupage.h"
#include "screen_page/menupage.h"
#include "screen_page/menupage.h"
#include "service/weather/weather.h"

void meteo_chart_page_init(void);
void meteo_chart_page_deinit(void);

#endif /* MAIN_SCREEN_PAGE_METEO_CHART_METEO_CHART_H_ */
