/*
 * status_panel.h
 *
 *  Created on: 25 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_STATUS_PANEL_STATUS_PANEL_H_
#define MAIN_STATUS_PANEL_STATUS_PANEL_H_

#include "lvgl.h"

#include "screen_page/menupage.h"
#include "Local_Lib/local_lvgl_lib.h"
#include "screen_page/setting/elements.h"
#include "sd/sd_spi.h"

void status_panel_init(void);
void status_panel_update(void);

#endif /* MAIN_STATUS_PANEL_STATUS_PANEL_H_ */
