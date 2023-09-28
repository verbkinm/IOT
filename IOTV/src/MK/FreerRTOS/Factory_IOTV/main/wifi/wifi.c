/*
 * wifi.c
 *
 *  Created on: 27 сент. 2023 г.
 *      Author: user
 */

#include <sys/param.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"

#include "esp_wifi.h"
#include "esp_netif.h"
#include "lwip/inet.h"

#include "wifi.h"

// Глобальные объекты
//extern bool global_wifi_sta_connect;
extern iotv_wifi_status_t global_wifi_sta_status;

// Приватные объекты
static const char *TAG = "wifi";

// Приватные функцие
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

//Реализация функций
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_id == WIFI_EVENT_AP_STACONNECTED)
	{
		wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
	}
	else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
	{
		wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		;
//		esp_wifi_connect();
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		global_wifi_sta_status = IOTV_WIFI_STA_NULL;
		ESP_LOGI(TAG, "disconnect from AP");

		static uint8_t counter = 0;
		if (++counter > 3)
		{
			counter = 0;
			global_wifi_sta_status = IOTV_WIFI_FAIL_CONNECT;
			return;
		}

		global_wifi_sta_status |= IOTV_WIFI_STA_START_CONNECTING;
		esp_wifi_connect();

		ESP_LOGI(TAG, "retry to connect to the AP");
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		global_wifi_sta_status = IOTV_WIFI_STA_CONNECTED;
	}
}

void wifi_disconnect_sta(void)
{
	esp_wifi_disconnect();

	ESP_LOGI(TAG, "wifi_disconnect_sta finished.");
}

void wifi_sta_connect(const char *ssid, const char *pass)
{
	if (ssid == NULL || pass == NULL)
	{
		ESP_LOGE(TAG, "ssid or pass empty.");
		return;
	}

	wifi_config_t sta_config = {};
	memcpy(sta_config.sta.ssid, ssid, (strlen(ssid) > WIFI_SSID_MAX_LENGH ? WIFI_SSID_MAX_LENGH : strlen(ssid) ));
	memcpy(sta_config.sta.password, pass, (strlen(pass) > WIFI_PASS_MAX_LENGH ? WIFI_PASS_MAX_LENGH : strlen(pass) ));
	sta_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	sta_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

	global_wifi_sta_status |= IOTV_WIFI_STA_START_CONNECTING;
	esp_wifi_connect();
}

void wifi_init(void)
{
	ESP_ERROR_CHECK(esp_netif_init());

	esp_netif_create_default_wifi_ap();
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

	wifi_config_t wifi_config_at = {
			.ap = {
					.ssid = WIFI_SSID,
					.ssid_len = strlen(WIFI_SSID),
					.max_connection = MAX_STA_CONN,
					.authmode = WIFI_AUTH_OPEN
			},
	};

	wifi_config_t wifi_config_sta = {
			.sta = {
					.ssid = "",
					.password = "",
					.threshold.authmode = WIFI_AUTH_WPA2_PSK,
					.sae_pwe_h2e = WPA3_SAE_PWE_BOTH
			},
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_at));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));
	ESP_ERROR_CHECK(esp_wifi_start());

	esp_netif_ip_info_t ip_info;
	esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

	char ip_addr[16];
	inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
	ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);
	ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s'", WIFI_SSID);
	ESP_LOGI(TAG, "wifi_init_sta finished.");
}



