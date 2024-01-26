/*
 * wifi_page.h
 *
 *  Created on: 7 дек. 2023 г.
 *      Author: user
 */
#include <lvgl.h>
#include "esp_mac.h"

#include "json/json_config.h"
#include "screen_page/setting/elements.h"
#include "lwip/netif.h"
#include "screen_page/setting/settingpage.h"

void create_wifi_sub_page(lv_event_t *e);
void free_wifi_sub_page(void);
