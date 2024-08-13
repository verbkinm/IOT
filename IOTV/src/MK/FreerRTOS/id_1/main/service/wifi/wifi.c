/*
 * wifi.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#include "global/global_def.h"
#include "wifi.h"

#ifdef DEBUG
#include "esp_mac.h"
#endif

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "esp_netif_types.h"
#include "nvs_flash.h"
#include "netif/etharp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/inet.h"

#include "global/registers.h"
#include "Local_Lib/local_lib.h"
#include "nvs/local_nvs.h"
#include "ota/ota.h"

const static char *TAG = "WIFI";
const static char *task_name = "wifi_task";

static esp_netif_t *netif_sta = NULL;
static esp_netif_t *netif_at = NULL;

static wifi_ap_record_t ap_info[AP_INFO_ARR_SIZE] = {};

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void wifi_service_base_init(void);

void wifi_service_only_sta_init(void)
{
	wifi_service_deinit();
	wifi_service_base_init();

	netif_sta = esp_netif_create_default_wifi_sta();
	esp_wifi_set_mode(WIFI_MODE_STA);

	wifi_config_t wifi_config;
	esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
	esp_wifi_start();

	char *sta_ssid = NULL;
	if (nvs_read_wifi_sta_ssid(&sta_ssid) == ESP_OK && sta_ssid != NULL)
	{
		strcpy((char *)wifi_config.sta.ssid, sta_ssid);
		free(sta_ssid);
	}

	char *sta_pwd = NULL;
	if (nvs_read_wifi_sta_pwd(&sta_pwd) == ESP_OK && sta_pwd != NULL)
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
	esp_wifi_set_mode(WIFI_MODE_AP);

	wifi_config_t wifi_config_at = {
			.ap = {
					.ssid = WIFI_SSID_AT,
					.ssid_len = strlen(WIFI_SSID_AT),
					.max_connection = 1,//MAX_STA_CONN,
					.authmode = WIFI_AUTH_OPEN
			},
	};

	esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_at);
	esp_wifi_start();

	esp_netif_ip_info_t ip_info;
	//	esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
	esp_netif_get_ip_info(netif_at, &ip_info);

#ifdef DEBUG
	char ip_addr[16], ip_mask[16], ip_gw[16];
	inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
	inet_ntoa_r(ip_info.netmask, ip_mask, 16);
	inet_ntoa_r(ip_info.gw, ip_gw, 16);
	ESP_LOGI(TAG, "Set up softAP with IP: %s, mask: %s, gw: %s\n", ip_addr, ip_mask, ip_gw);
	ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s'", WIFI_SSID_AT);
	ESP_LOGI(TAG, "wifi_init_sta finished.");
#endif
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

	esp_wifi_set_mode(WIFI_MODE_APSTA);

	wifi_config_t wifi_config_at = {
			.ap = {
					.ssid = WIFI_SSID_AT,
					.ssid_len = strlen(WIFI_SSID_AT),
					.max_connection = 2, //MAX_STA_CONN,
					.authmode = WIFI_AUTH_OPEN
			},
	};

	wifi_config_t wifi_config_sta = {
			.sta = {
					//					.ssid = "",
					//					.password = "",
					.threshold.authmode = WIFI_AUTH_WPA2_PSK,
					.sae_pwe_h2e = WPA3_SAE_PWE_BOTH
			},
	};


	char *sta_ssid = NULL;
	if (nvs_read_wifi_sta_ssid(&sta_ssid) == ESP_OK && sta_ssid != NULL)
	{
		strcpy((char *)(wifi_config_sta.sta.ssid), sta_ssid);
		free(sta_ssid);
	}

	char *sta_pwd = NULL;
	if (nvs_read_wifi_sta_pwd(&sta_pwd) == ESP_OK && sta_pwd != NULL)
	{
		strcpy((char *)wifi_config_sta.sta.password, sta_pwd);
		free(sta_pwd);
	}

	esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_at);
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta);
	esp_wifi_start();

	esp_netif_ip_info_t ip_info;
	//	esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
	esp_netif_get_ip_info(netif_at, &ip_info);

#ifdef DEBUG
	char ip_addr[16], ip_mask[16], ip_gw[16];
	inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
	inet_ntoa_r(ip_info.netmask, ip_mask, 16);
	inet_ntoa_r(ip_info.gw, ip_gw, 16);
	ESP_LOGI(TAG, "Set up softAP with IP: %s, mask: %s, gw: %s\n", ip_addr, ip_mask, ip_gw);
	ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s'", WIFI_SSID_AT);
	ESP_LOGI(TAG, "wifi_init_sta finished.");
#endif
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		printf("%s %s sta start\n", TAG, task_name);
		glob_set_bits_status_reg(STATUS_WIFI_STA_START);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
	{
		printf("%s %s sta stop\n", TAG, task_name);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_START);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
	{
		printf("%s %s wifi sta connected\n", TAG, task_name);
		glob_set_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);

#ifdef DEBUG
		wifi_config_t wifi_config;
		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

		char mac_str[18] = {0};
		char pwd_str[65] = {0};
		char ssid_str[32] = {0};

		sprintf(mac_str, MACSTR, MAC2STR(wifi_config.sta.bssid));
		sprintf(pwd_str, "%s", wifi_config.sta.password);
		sprintf(ssid_str, "%s", wifi_config.sta.ssid);

		printf("%s %s connected to \"%s\" (%s) \"%s\"\n",
				TAG, task_name,
				ssid_str, mac_str, pwd_str);

		wifi_ap_config_t *ap_info = (wifi_ap_config_t *)event_data;
		printf("ssid %s\n", ap_info->ssid);
#endif
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		printf("%s %s sta disconnect\n", TAG, task_name);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
	{
		printf("%s %s scan done\n", TAG, task_name);
		glob_clear_bits_status_reg(STATUS_WIFI_SCANNING);
		glob_set_bits_status_reg(STATUS_WIFI_SCAN_DONE);

		uint16_t number = AP_INFO_ARR_SIZE;
		uint16_t ap_count = 0;

		esp_wifi_scan_get_ap_records(&number, ap_info);
		esp_wifi_scan_get_ap_num(&ap_count);
#ifdef DEBUG
		ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
		for (int i = 0; (i < AP_INFO_ARR_SIZE) && (i < ap_count); i++)
		{
			char ssid[sizeof(ap_info[i].ssid) + 1] = {0};
			memcpy(ssid, ap_info[i].ssid, sizeof(ap_info[i].ssid));

			printf("%s %s SSID \"%s\" (" MACSTR ")\n", TAG, task_name, ssid, MAC2STR(ap_info[i].bssid));
			printf("%s %s RSSI %d\n", TAG, task_name, ap_info[i].rssi);
			//		if (ap_info[i].authmode != WIFI_AUTH_WEP)
			//			print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
			printf("%s %s Channel \t\t%d\n", TAG, task_name, ap_info[i].primary);
		}
#endif
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		glob_set_bits_status_reg(STATUS_IP_GOT);
		printf("%s %s sta got ip\n", TAG, task_name);
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
	{
		printf("%s %s sta lost ip\n", TAG, task_name);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_id == WIFI_EVENT_AP_START)
	{
		printf("AP start\n");
		glob_set_bits_status_reg(STATUS_WIFI_AP_START);
	}
	else if (event_id == WIFI_EVENT_AP_STOP)
	{
		printf("%s %s AP stop\n", TAG, task_name);
		glob_clear_bits_status_reg(STATUS_WIFI_AP_START);
	}
	else if (event_id == WIFI_EVENT_AP_STACONNECTED)
	{
		printf("%s %s APSTA connected\n", TAG, task_name);
#ifdef DEBUG
		wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
#endif
	}
	else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
	{
		printf("%s %s APSTA disconnected\n", TAG, task_name);
#ifdef DEBUG
		wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
		ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
#endif
	}
	else if (event_base == WIFI_EVENT)
		printf("%s %s other event: %s %d\n", TAG, task_name, event_base, (int)event_id);
}

const char *wifi_service_task_name()
{
	return task_name;
}

void wifi_service_scan_wifi(void)
{
	if (!(glob_get_status_reg() & STATUS_WIFI_STA_START) ||
			(glob_get_status_reg() & STATUS_WIFI_SCANNING))
		return;

	printf("%s %s start scan\n", TAG, task_name);

	memset(ap_info, 0, sizeof(ap_info));

	if (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING)
		esp_wifi_disconnect();

	glob_set_bits_status_reg(STATUS_WIFI_SCANNING);
	glob_clear_bits_status_reg(STATUS_WIFI_SCAN_DONE);

	esp_wifi_scan_start(NULL, false);
}

const wifi_ap_record_t *wifi_service_last_scan(void)
{
	return ap_info;
}

void wifi_service_conntect_to_AP(const char *ssid, const char *pwd)
{
	if ( !(glob_get_status_reg() & STATUS_WIFI_STA_START) || ssid == NULL || pwd == NULL)
		return;

	esp_wifi_disconnect();

	wifi_config_t wifi_config;
	esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

	strcpy((char *)(wifi_config.sta.ssid), ssid);
	strcpy((char *)(wifi_config.sta.password), pwd);

	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

	glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);
}

void wifi_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);

	glob_set_bits_service_reg(SERVICE_WIFI_ON);
	printf("%s %s start\n", TAG, task_name);

	{
		// Должен вызываться один раз!!!
		esp_netif_init();

		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		esp_wifi_init(&cfg);

		esp_event_handler_instance_t instance_any_id;
		esp_event_loop_create_default();
		esp_event_handler_instance_register(WIFI_EVENT,
				ESP_EVENT_ANY_ID,
				&event_handler,
				NULL,
				&instance_any_id);
		esp_event_handler_instance_register (IP_EVENT,
				ESP_EVENT_ANY_ID,
				&event_handler,
				NULL,
				&instance_any_id);
	}

	wifi_service_only_sta_init();
	//	wifi_service_only_ap_init();
	//	wifi_service_apsta_init();

	glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if ( !(glob_get_status_reg() & STATUS_WIFI_AUTOCONNECT)
				|| !(glob_get_status_reg() & STATUS_WIFI_STA_START)
				|| (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTING)
				|| (glob_get_status_reg() & STATUS_WIFI_SCANNING)
				|| (glob_get_status_reg() & STATUS_WIFI_STA_CONNECTED))
		{
			goto for_end;
		}

		wifi_config_t wifi_config;
		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

		glob_set_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		printf("%s %s Try connect to ssid = %s, pwd = %s\n", TAG, task_name,
				wifi_config.sta.ssid, wifi_config.sta.password);
		esp_wifi_connect();

		for_end:
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	glob_clear_bits_service_reg(SERVICE_WIFI_ON);
	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
}
