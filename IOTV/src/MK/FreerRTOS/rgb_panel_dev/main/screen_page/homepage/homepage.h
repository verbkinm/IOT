/*
 * homepage.h
 *
 *  Created on: 1 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_HOMEPAGE_H_
#define MAIN_HOMEPAGE_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "Global_def.h"
#include "I2C/DS3231.h"
#include "screen_page/screendefault.h"
#include "datetime1/datetime1.h"
#include "status_panel/status_panel.h"
#include "json/json_config.h"
#include "service/weather/weather.h"
#include "Local_Lib/local_lvgl_lib.h"

void homePageInit(void);
void drawHomePage(void);

#endif /* MAIN_HOMEPAGE_H_ */
