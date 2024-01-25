/*
 * update.h
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#ifndef MAIN_SCREEN_PAGE_SETTING_UPDATE_H_
#define MAIN_SCREEN_PAGE_SETTING_UPDATE_H_

#include "esp_app_desc.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"

#include <lvgl.h>
#include "Local_Lib/local_lib.h"
#include "settingpage.h"
#include "elements.h"
#include "ota/ota.h"
#include "screen_page/full_screen_page/full_screen_page.h"

void create_update_sub_page(lv_event_t *e);
void free_update_sub_page(void);

#endif /* MAIN_SCREEN_PAGE_SETTING_UPDATE_H_ */
