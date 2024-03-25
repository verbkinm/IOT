/*
 * datetime1.h
 *
 *  Created on: 27 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_DATETIME1_DATETIME1_H_
#define MAIN_SCREEN_PAGE_DATETIME1_DATETIME1_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "Global_def.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "screen_page/screendefault.h"
#include "screen_page/datetime2/datetime2.h"
#include "screen_page/page.h"
#include "status_panel/status_panel.h"

void datetime1_page_init(void);
void datetime1_page_deinit(void);

#endif /* MAIN_SCREEN_PAGE_DATETIME1_DATETIME1_H_ */
