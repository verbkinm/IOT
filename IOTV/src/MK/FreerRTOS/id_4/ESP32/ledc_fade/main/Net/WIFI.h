#ifndef MAIN_NET_WIFI_H_
#define MAIN_NET_WIFI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "Global_def.h"

void wifi_init_sta(void);

#endif /* MAIN_NET_WIFI_H_ */
