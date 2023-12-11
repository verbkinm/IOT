#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"

#include "TFT_touch_screen.h"
#include "I2C/i2c.h"
#include "screen_page/setting/elements.h"
#include "sd/sd_spi.h"

#include "json/json_config.h"

extern uint32_t glob_status_reg;

static const char *TAG = "main";

//static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		printf("wifi sta start\n");
		glob_status_reg |= STATUS_WIFI_STA_START;
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
	{
		printf("wifi sta stop\n");
		glob_status_reg &= ~STATUS_WIFI_STA_START;
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
	{
		printf("wifi sta connected\n");
		glob_status_reg |= STATUS_WIFI_STA_CONNECTED;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTING;
		//		lv_obj_t *mbox1 = lv_msgbox_create(NULL, "WIFI", "wifi sta connected!", 0, true);
		//		lv_obj_center(mbox1);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		printf("wifi sta disconnect\n");
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTED;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTING;
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
	{
		glob_status_reg &= ~STATUS_WIFI_SCANNING;
		glob_status_reg |= STATUS_WIFI_SCAN_DONE;
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		glob_status_reg |= STATUS_IP_GOT;
		glob_status_reg &= ~STATUS_WIFI_STA_CONNECTING;
		printf("wifi sta got ip\n");
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		//		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

		//		esp_netif_t *esp_netif = esp_netif_get_default_netif();
		//		esp_netif_ip_info_t ip_info;
		//		esp_netif_get_ip_info(esp_netif, &ip_info);
		//
		//		ESP_LOGI(TAG, "ip:" IPSTR, IP2STR(&event->ip_info.ip));
		//		ESP_LOGI(TAG, "mask:" IPSTR, IP2STR(&event->ip_info.netmask));
		//		ESP_LOGI(TAG, "gw:" IPSTR, IP2STR(&event->ip_info.gw));

		//		printf("ip - %s\n", IP2STR(&ip_info->ip));
		//		printf("mask - %s\n", IPSTR(ip_info->netmask));
		//		printf("gw - %s\n", IPSTR(ip_info->gw));
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
	{
		printf("wifi sta ip lost\n");
		glob_status_reg &= ~STATUS_IP_GOT;
	}
	else if (event_base == WIFI_EVENT)
		printf("wifi other event: %s\n", event_base);
}

void app_main(void)
{
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&event_handler,
			NULL,
			&instance_any_id));
	//	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
	//			ESP_EVENT_ANY_ID,
	//			&event_handler,
	//			NULL,
	//			&instance_got_ip));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_stop());

	sd_spi_init();
	i2c_init();
	TFT_init(); // Перед этой функцией обязательно выполнить i2c_init();

	printf("%s\n", get_wifi_config_value(monitor, key));


	//	esp_netif_t *esp_netif = esp_netif_get_default_netif();
	//	esp_netif_ip_info_t ip_info;
	//	esp_netif_get_ip_info(esp_netif, &ip_info);
	//
	//	ESP_LOGI(TAG, "ip:" IPSTR, IP2STR(&ip_info.ip));
	//	ESP_LOGI(TAG, "mask:" IPSTR, IP2STR(&ip_info.netmask));
	//	ESP_LOGI(TAG, "gw:" IPSTR, IP2STR(&ip_info.gw));


//	FILE *file = fopen("/sdcard/conf.jsn", "w");
//	if (file == NULL)
//		printf("cant create conf.jsn\n");
//
//	cJSON *root = cJSON_CreateObject();
//
//	cJSON *wifi = cJSON_CreateObject();
//	cJSON_AddItemToObjectCS(root, "wifi", wifi);
//
//	cJSON *obj = cJSON_CreateString("0");
//	cJSON_AddItemToObject(wifi, "on", obj);
//
//	obj = cJSON_CreateString("0");
//	cJSON_AddItemToObjectCS(wifi, "auto", obj);
//
//	obj = cJSON_CreateString("00:00:00:01:FF:12");
//	cJSON_AddItemToObjectCS(wifi, "bssid", obj);
//
//	obj = cJSON_CreateString("y3z4pfrm");
//	cJSON_AddItemToObjectCS(wifi, "pwd", obj);
//
//	char *string = cJSON_Print(root);
//	if (string == NULL)
//	{
//		fprintf(file, "Failed to print monitor.\n");
//	}
//
//	fprintf(file, "%s", string);
//
//	fclose(file);
//	cJSON_Delete(root);
//	free(string);
//
//	file = fopen("/sdcard/conf.jsn", "r");
//	if (file == NULL)
//		printf("cant read conf.jsn\n");
//
//	char *buf = calloc(1, BUFSIZE);
//	fread(buf, BUFSIZE, 1, file);
//	printf("file read:\n%s\n", buf);
//
//	free(buf);

	while (1)
	{
		// raise the task priority of LVGL and/or reduce the handler period can improve the performance
		vTaskDelay(10 / portTICK_PERIOD_MS);

		// The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
		lv_timer_handler();
	}
}
