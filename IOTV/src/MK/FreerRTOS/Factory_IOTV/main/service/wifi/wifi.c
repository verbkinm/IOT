/*
 * wifi.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#include "global/global_def.h"
#include "wifi.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_netif_types.h"
#include "nvs_flash.h"
#include "netif/etharp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/inet.h"

#include "global/registers.h"
#include "Local_Lib/local_lib.h"
#include "nvs/local_nvs.h"

const static char *TAG = "WIFI";
const static char *task_name = "wifi_task";

static esp_netif_t *netif_sta = NULL;
static esp_netif_t *netif_at = NULL;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void wifi_service_base_init(void);

void wifi_service_only_sta_init(void)
{
	wifi_service_deinit();
	wifi_service_base_init();

	netif_sta = esp_netif_create_default_wifi_sta();
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	wifi_config_t wifi_config;
	esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
	esp_wifi_start();

	char *sta_ssid = NULL;
	if (nvs_read_wifi_sta_ssid(sta_ssid) == ESP_OK && sta_ssid != NULL)
	{
		strcpy((char *)wifi_config.sta.ssid, sta_ssid);
		free(sta_ssid);
	}

	char *sta_pwd = NULL;
	if (nvs_read_wifi_sta_pwd(sta_pwd) == ESP_OK && sta_pwd != NULL)
	{
		strcpy((char *)wifi_config.sta.password, sta_pwd);
		free(sta_pwd);
	}
}

void wifi_service_only_ap_init(void)
{
	wifi_service_deinit();
	wifi_service_base_init();

	netif_at = esp_netif_create_default_wifi_ap();
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

	wifi_config_t wifi_config_at = {
			.ap = {
					.ssid = WIFI_SSID_AT,
					.ssid_len = strlen(WIFI_SSID_AT),
					.max_connection = 1,//MAX_STA_CONN,
					.authmode = WIFI_AUTH_OPEN
			},
	};

	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_at) );
	esp_wifi_start();

	esp_netif_ip_info_t ip_info;
	esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

	char ip_addr[16];
	inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
	ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);
	ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s'", WIFI_SSID_AT);
	ESP_LOGI(TAG, "wifi_init_sta finished.");
}

static void wifi_service_base_init(void)
{
//	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
}

void wifi_service_deinit(void)
{
	esp_wifi_disconnect();
	esp_wifi_stop();
//	esp_wifi_deinit();

	esp_netif_destroy(netif_sta);
	esp_netif_destroy(netif_at);
	netif_sta = NULL;
	netif_at = NULL;
}

void wifi_service_apsta_init(void)
{
	wifi_service_deinit();
	wifi_service_base_init();

	netif_sta = esp_netif_create_default_wifi_sta();
	netif_at = esp_netif_create_default_wifi_ap();

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

	wifi_config_t wifi_config_at = {
			.ap = {
					.ssid = WIFI_SSID_AT,
					.ssid_len = strlen(WIFI_SSID_AT),
					.max_connection = 1, //MAX_STA_CONN,
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


	char *sta_ssid = NULL;
	if (nvs_read_wifi_sta_ssid(sta_ssid) != ESP_OK && sta_ssid != NULL)
	{
		strcpy((char *)wifi_config_sta.sta.ssid, sta_ssid);
		free(sta_ssid);
	}

	char *sta_pwd = NULL;
	if (nvs_read_wifi_sta_pwd(sta_pwd) == ESP_OK && sta_pwd != NULL)
	{
		strcpy((char *)wifi_config_sta.sta.password, sta_pwd);
		free(sta_pwd);
	}


	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_at));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));
	ESP_ERROR_CHECK(esp_wifi_start());

	esp_netif_ip_info_t ip_info;
	esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

	char ip_addr[16];
	inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
	ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);
	ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s'", WIFI_SSID_AT);
	ESP_LOGI(TAG, "wifi_init_sta finished.");
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
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		printf("wifi sta disconnect\n");
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
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
	else if (event_id == WIFI_EVENT_AP_START)
	{
		printf("wifi AP start\n");
		glob_set_bits_status_reg(STATUS_WIFI_AP_START);
	}
	else if (event_id == WIFI_EVENT_AP_STOP)
	{
		printf("wifi AP stop\n");
		glob_clear_bits_status_reg(STATUS_WIFI_AP_START);
	}
	else if (event_id == WIFI_EVENT_AP_STACONNECTED)
	{
		wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
	}
	else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
	{
		wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
	}


	else if (event_base == WIFI_EVENT)
		printf("wifi other event: %s %d\n", event_base, (int)event_id);
}

const char *wifi_service_task_name()
{
	return task_name;
}

void wifi_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);

	glob_set_bits_service_reg(SERVICE_WIFI_ON);
	printf("%s %s start\n", TAG, task_name);

	{
		// Должен вызываться один раз!!!
		ESP_ERROR_CHECK(esp_netif_init());

		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK(esp_wifi_init(&cfg));

		esp_event_handler_instance_t instance_any_id;
		ESP_ERROR_CHECK(esp_event_loop_create_default());
		ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
				ESP_EVENT_ANY_ID,
				&event_handler,
				NULL,
				&instance_any_id));
		ESP_ERROR_CHECK(esp_event_handler_instance_register (IP_EVENT,
				ESP_EVENT_ANY_ID,
				&event_handler,
				NULL,
				&instance_any_id));
	}

//	wifi_service_only_sta_init();
	//	wifi_service_only_ap_init();
		wifi_service_apsta_init();

	glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if ( !(glob_get_status_reg() & STATUS_WIFI_AUTOCONNECT)
				|| !(glob_get_status_reg() & STATUS_WIFI_STA_START)
				|| (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING) )
		{
			goto for_end;
		}

		if ( !(glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED) )
		{
			wifi_config_t wifi_config;
			esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

			glob_set_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
			printf("Try connect to ssid = %s, pwd = %s\n", wifi_config.sta.ssid, wifi_config.sta.password);
			esp_wifi_connect();
		}

		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	glob_clear_bits_service_reg(SERVICE_WIFI_ON);
	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
}
