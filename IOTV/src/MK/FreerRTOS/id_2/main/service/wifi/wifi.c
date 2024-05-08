/*
 * wifi.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#include "wifi.h"
#include "deep_sleep/deep_sleep.h"

static int counter = 0;
esp_netif_t *sta_netif;

static void wifi_init(void);

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

static void wifi_init(void)
{
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	sta_netif = esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&event_handler,
			NULL,
			&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
			ESP_EVENT_ANY_ID,
			&event_handler,
			NULL,
			&instance_any_id));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	esp_wifi_start();

	wifi_config_t wifi_config;
	esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

	// tmp
	strcpy((char *)wifi_config.sta.ssid, WIFI_SSID);
	strcpy((char *)wifi_config.sta.password, WIFI_PASSWORD);
	// tmp

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		printf("wifi sta start\n");
		glob_set_bits_status_reg(STATUS_WIFI_STA_START);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
	{
		printf("wifi sta stop\n");
		glob_clear_bits_status_reg(STATUS_WIFI_STA_START);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
	{
		printf("wifi sta connected\n");
		glob_set_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);

		wifi_config_t wifi_config;
		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

		char mac_str[18] = {0};
		char pwd_str[65] = {0};
		char ssid_str[32] = {0};

		sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
				wifi_config.sta.bssid[0], wifi_config.sta.bssid[1], wifi_config.sta.bssid[2],
				wifi_config.sta.bssid[3], wifi_config.sta.bssid[4], wifi_config.sta.bssid[5]);
		sprintf(pwd_str, "%s", wifi_config.sta.password);
		sprintf(ssid_str, "%s", wifi_config.sta.ssid);

		wifi_ap_config_t *ap_info = (wifi_ap_config_t *)event_data;
		printf("ap_info->ssid %s\n", ap_info->ssid);

//		set_wifi_config_value("ssid", ssid_str);
//		set_wifi_config_value("bssid", mac_str);
//		bool res = set_wifi_config_value("pwd", pwd_str);
//		set_wifi_config_value("auto", "1");
		glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);

//		printf("write pwd %d, pwd = %s\n", (int)res, pwd_str);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		printf("wifi sta disconnect\n");
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_IP_GOT);

		//!!! счетчик кол-ва попыток подключения. При привышении deep_sleep
		printf("counter connect = %d\n", ++counter);
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		glob_set_bits_status_reg(STATUS_IP_GOT);
		printf("wifi sta got ip\n");
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
	{
		printf("wifi sta ip lost\n");
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_base == WIFI_EVENT)
		printf("wifi other event: %s\n", event_base);

	fflush(stdout);
}

void wifi_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);
	wifi_init();

	glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if (counter > 3)
			deep_sleep();

		if ( !(glob_get_status_reg() & STATUS_WIFI_AUTOCONNECT))
			goto for_end;

		if ( !(glob_get_status_reg() & STATUS_WIFI_STA_START))
			goto for_end;

		if (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING)
			goto for_end;

		if ( !(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED) )
		{
			wifi_config_t wifi_config;
			esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

			glob_set_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
			esp_wifi_connect();
		}

		for_end:
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
