/*
 * wifi.c
 *
 *  Created on: 13 дек. 2023 г.
 *      Author: user
 */


#include "wifi.h"

extern uint32_t glob_status_reg;

esp_netif_t *sta_netif;

//static TimerHandle_t reconnect_timer;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
//static void timer_toggle_callback( TimerHandle_t xTimer);
//static void vTaskCode(void *pvParameters);

void wifi_init(void)
{

	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	ESP_ERROR_CHECK(nvs_flash_erase());
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
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

	//	wifi_config_t wifi_config_sta = {
	//			.sta = {
	//					.ssid = "123",
	//					.password = "",
	//					.threshold.authmode = WIFI_AUTH_WPA2_PSK,
	//					.sae_pwe_h2e = WPA3_SAE_PWE_BOTH
	//			},
	//	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	read_wifi_conf();

//	xTaskCreate(vTaskCode, "task loop", 4096, 0, 10, 0);
	//	reconnect_timer = xTimerCreate("timer", 7000 / portTICK_PERIOD_MS, pdTRUE, 0, timer_toggle_callback);
	//	xTimerStart(reconnect_timer, 2000 / portTICK_PERIOD_MS);
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		printf("wifi sta start\n");
		glob_status_reg |= STATUS_WIFI_STA_START;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTING;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTED;
		glob_status_reg &= ~STATUS_WIFI_SCANNING;
		glob_status_reg &= ~STATUS_WIFI_SCAN_DONE;
		glob_status_reg &= ~STATUS_IP_GOT;
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
	{
		printf("wifi sta stop\n");
		glob_status_reg &= ~STATUS_WIFI_STA_START;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTING;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTED;
		glob_status_reg &= ~STATUS_WIFI_SCANNING;
		glob_status_reg &= ~STATUS_WIFI_SCAN_DONE;
		glob_status_reg &= ~STATUS_IP_GOT;
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
	{
		printf("wifi sta connected\n");
		glob_status_reg |= STATUS_WIFI_STA_CONNECTED;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTING;

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

		set_wifi_config_value("ssid", ssid_str);
		set_wifi_config_value("bssid", mac_str);
		bool res = set_wifi_config_value("pwd", pwd_str);
		set_wifi_config_value("auto", "1");
		glob_status_reg |= STATUS_WIFI_AUTOCONNECT;

		printf("write pwd %d, pwd = %s\n", (int)res, pwd_str);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		printf("wifi sta disconnect\n");
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTED;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTING;
		//		glob_status_reg &= ~STATUS_WIFI_SCANNING;
		//		glob_status_reg &= ~STATUS_WIFI_SCAN_DONE;
		glob_status_reg &= ~STATUS_IP_GOT;
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
	{
		glob_status_reg &= ~STATUS_WIFI_SCANNING;
		glob_status_reg |= STATUS_WIFI_SCAN_DONE;
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		//		etharp_find_addr(netif, ipaddr, eth_ret, ip_ret)

		glob_status_reg |= STATUS_IP_GOT;
		printf("wifi sta got ip\n");
		//		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		//		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

		//		esp_netif_t *esp_netif = esp_netif_get_default_netif();
		//		esp_netif_ip_info_t ip_info;
		//		esp_netif_get_ip_info(esp_netif, &ip_info);
		//
		//		ESP_LOGI(TAG, "ip:" IPSTR, IP2STR(&event->ip_info.ip));
		//		ESP_LOGI(TAG, "mask:" IPSTR, IP2STR(&event->ip_info.netmask));
		//		ESP_LOGI(TAG, "gw:" IPSTR, IP2STR(&event->ip_info.gw));
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
	{
		printf("wifi sta ip lost\n");
		glob_status_reg &= ~STATUS_IP_GOT;
	}
	else if (event_base == WIFI_EVENT)
		printf("wifi other event: %s\n", event_base);
}

void read_wifi_conf()
{
	char *on = NULL;
	char *auto_on = NULL;
	char *ssid = NULL;
	//	char *bssid = NULL;
	char *pwd = NULL;

	//	uint8_t bssid_arr[6];

	wifi_config_t wifi_config;
	esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

	// Автоматическое включение wifi
	if (get_wifi_config_value("on", &on))
	{
		if (strcmp("1", on) == 0)
			esp_wifi_start();

		printf("read wifi conf. on = %s\n", on);
		free(on);
	}

	// SSID точки доступа
	if (get_wifi_config_value("ssid", &ssid))
	{
		printf("read wifi conf. ssid = %s\n", ssid);
		strcpy((char *)wifi_config.sta.ssid, ssid);

		free(ssid);
	}

	//	// BSSID точки доступа
	//	if (get_wifi_config_value("bssid", &bssid))
	//	{
	//		printf("read wifi conf. bssid = %s\n", bssid);
	//
	//		strmac_to_arr(bssid, bssid_arr);
	//		memcpy(wifi_config.sta.bssid, bssid_arr, sizeof(bssid_arr));
	//		wifi_config.sta.bssid_set = true;
	//
	//		free(bssid);
	//	}

	// Пароль точки доступа
	if (get_wifi_config_value("pwd", &pwd))
	{
		printf("read wifi conf. pwd = %s\n", pwd);
		strcpy((char *)wifi_config.sta.password, pwd);

		free(pwd);
	}

	// Автоматическое подключение
	if (get_wifi_config_value("auto", &auto_on))
	{
		if (strcmp("1", auto_on) == 0)
		{
			glob_status_reg |= STATUS_WIFI_AUTOCONNECT;
		}
		else
			glob_status_reg &= ~STATUS_WIFI_AUTOCONNECT;


		printf("read wifi conf. auto = %s\n", auto_on);

		free(auto_on);
	}

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
}

//static void timer_toggle_callback(TimerHandle_t xTimer)
//{
//	if ( !(glob_status_reg & STATUS_WIFI_AUTOCONNECT))
//	{
//		//		xTimerStop(reconnect_timer, 0);
//		//		xTimerDelete(reconnect_timer, 0);
//		return;
//	}
//
//	if ( !(glob_status_reg & STATUS_WIFI_STA_START))
//		return;
//
//	if (glob_status_reg & STATUS_WIFI_STA_CONNECTING)
//		return;
//
//	if ( !(glob_status_reg & STATUS_WIFI_STA_CONNECTED) )
//	{
//		//		esp_wifi_clear_ap_list();
//		//		esp_wifi_disconnect();
//		//		esp_wifi_stop();
//		//
//		//		esp_wifi_start();
//
//		wifi_config_t wifi_config;
//		esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
//
//		printf("ssid: ");
//		printf("%s", wifi_config.sta.ssid);
//
//		printf("\nbssid: ");
//		printf("%02x:%02x:%02x:%02x:%02x:%02x",
//				wifi_config.sta.bssid[0], wifi_config.sta.bssid[1], wifi_config.sta.bssid[2],
//				wifi_config.sta.bssid[3], wifi_config.sta.bssid[4], wifi_config.sta.bssid[5]);
//
//		printf("\npwd: ");
//		printf("%s", wifi_config.sta.password);
//		printf("\n");
//
//		esp_wifi_connect();
//		glob_status_reg |= STATUS_WIFI_STA_CONNECTING;
//		//		xTimerStop(reconnect_timer, 0);
//	}
//}

//static void vTaskCode(void *pvParameters)
//{
//	for( ;; )
//	{
//		if ( !(glob_status_reg & STATUS_WIFI_AUTOCONNECT))
//		{
//			//		xTimerStop(reconnect_timer, 0);
//			//		xTimerDelete(reconnect_timer, 0);
//			goto for_end;
//		}
//
//		if ( !(glob_status_reg & STATUS_WIFI_STA_START))
//			goto for_end;
//
//		if (glob_status_reg & STATUS_WIFI_STA_CONNECTING)
//			goto for_end;
//
//		if ( !(glob_status_reg & STATUS_WIFI_STA_CONNECTED) )
//		{
//			//		esp_wifi_clear_ap_list();
//			//		esp_wifi_disconnect();
//			//		esp_wifi_stop();
//			//
//			//		esp_wifi_start();
//
//			wifi_config_t wifi_config;
//			esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
//
//			printf("ssid: ");
//			printf("%s", wifi_config.sta.ssid);
//
//			printf("\nbssid: ");
//			printf("%02x:%02x:%02x:%02x:%02x:%02x",
//					wifi_config.sta.bssid[0], wifi_config.sta.bssid[1], wifi_config.sta.bssid[2],
//					wifi_config.sta.bssid[3], wifi_config.sta.bssid[4], wifi_config.sta.bssid[5]);
//
//			printf("\npwd: ");
//			printf("%s", wifi_config.sta.password);
//			printf("\n");
//
//			esp_wifi_connect();
//			glob_status_reg |= STATUS_WIFI_STA_CONNECTING;
//			//		xTimerStop(reconnect_timer, 0);
//		}
//
//		for_end:
//		vTaskDelay(5000 / portTICK_PERIOD_MS);
//	}
//}
