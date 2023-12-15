/*
 * wifi.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#include "wifi.h"

extern uint32_t glob_status_reg;

void wifi_service_task(void *pvParameters)
{
	for( ;; )
	{
		if ( !(glob_status_reg & STATUS_WIFI_AUTOCONNECT))
		{
			//		xTimerStop(reconnect_timer, 0);
			//		xTimerDelete(reconnect_timer, 0);
			goto for_end;
		}

		if ( !(glob_status_reg & STATUS_WIFI_STA_START))
			goto for_end;

		if (glob_status_reg & STATUS_WIFI_STA_CONNECTING)
			goto for_end;

		if ( !(glob_status_reg & STATUS_WIFI_STA_CONNECTED) )
		{
			wifi_config_t wifi_config;
			esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

//			printf("auth mode: %d\n", (int)wifi_config.sta.);
			printf("ssid: ");
			printf("%s", wifi_config.sta.ssid);

			printf("\nbssid: ");
			printf("%02x:%02x:%02x:%02x:%02x:%02x",
					wifi_config.sta.bssid[0], wifi_config.sta.bssid[1], wifi_config.sta.bssid[2],
					wifi_config.sta.bssid[3], wifi_config.sta.bssid[4], wifi_config.sta.bssid[5]);

			printf("\npwd: ");
			printf("%s", wifi_config.sta.password);
			printf("\n");

			esp_wifi_connect();
			glob_status_reg |= STATUS_WIFI_STA_CONNECTING;
			//		xTimerStop(reconnect_timer, 0);
		}

		for_end:
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}
