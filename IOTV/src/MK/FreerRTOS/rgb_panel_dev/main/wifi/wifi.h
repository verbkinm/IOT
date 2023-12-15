/*
 * wifi.h
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: user
 */

#ifndef MAIN_WIFI_WIFI_H_
#define MAIN_WIFI_WIFI_H_

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "nvs_flash.h"
#include "netif/etharp.h"

#include "../Global_def.h"
#include "../json/json_config.h"
#include "../Local_Lib/local_lib.h"

void wifi_init(void);
void read_wifi_conf(void);

#endif /* MAIN_WIFI_WIFI_H_ */
