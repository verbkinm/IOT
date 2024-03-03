/*
 * wifi.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#include "wifi.h"

const static char *TAG = WIFI_STR;
const static char *task_name = "wifi_service_task";

static void wifi_init(void);
static void read_wifi_conf(void);

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void check_wifi_conf_file(void);

const char *wifi_service_task_name(void)
{
	return task_name;
}

static void wifi_init(void)
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
	esp_netif_create_default_wifi_sta();

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

	check_wifi_conf_file();
	read_wifi_conf();
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		printf("%s wifi sta start\n", TAG);
		glob_set_bits_status_reg(STATUS_WIFI_STA_START);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
	{
		printf("%s wifi sta stop\n", TAG);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_START);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
		glob_clear_bits_status_reg(STATUS_CAMERA_START);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
	{
		printf("%s wifi sta connected\n", TAG);
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

		set_wifi_config_value(SSID_STR, ssid_str);
		set_wifi_config_value(BSSID_STR, mac_str);
		bool res = set_wifi_config_value(PWD_STR, pwd_str);
		set_wifi_config_value(AUTO_STR, ONE_STR);
		glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);

		printf("%s write pwd %d, pwd = %s\n", TAG, (int)res, pwd_str);
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		printf("%s wifi sta disconnect\n", TAG);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTED);
		glob_clear_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		//		glob_status_reg &= ~STATUS_WIFI_SCANNING;
		//		glob_status_reg &= ~STATUS_WIFI_SCAN_DONE;
		glob_clear_bits_status_reg(STATUS_IP_GOT);
		glob_clear_bits_status_reg(STATUS_CAMERA_START);
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		glob_set_bits_status_reg(STATUS_IP_GOT);
		printf("%s wifi sta got ip\n", TAG);
		fflush(stdout);
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
	{
		printf("%s wifi sta ip lost\n", TAG);
		glob_clear_bits_status_reg(STATUS_IP_GOT);
		glob_clear_bits_status_reg(STATUS_CAMERA_START);
	}
	else if (event_base == WIFI_EVENT)
		printf("%s wifi other event: %s\n", TAG, event_base);
}

static void check_wifi_conf_file(void)
{
	cJSON *root = cJSON_CreateObject();

	cJSON *wifi = cJSON_CreateObject();
	cJSON_AddItemToObjectCS(root, WIFI_STR, wifi);

	cJSON *on_obj = cJSON_CreateString(ZERO_STR);
	cJSON_AddItemToObject(wifi, ON_STR, on_obj);

	cJSON *auto_con_obj = cJSON_CreateString(ZERO_STR);
	cJSON_AddItemToObjectCS(wifi, AUTO_STR, auto_con_obj);

	cJSON *ssid_obj = cJSON_CreateString("");
	cJSON_AddItemToObjectCS(wifi, SSID_STR, ssid_obj);

	cJSON *pwd_obj = cJSON_CreateString("");
	cJSON_AddItemToObjectCS(wifi, PWD_STR, pwd_obj);

	get_wifi_config_value(ON_STR, &on_obj->valuestring);
	get_wifi_config_value(AUTO_STR, &auto_con_obj->valuestring);
	get_wifi_config_value(SSID_STR, &ssid_obj->valuestring);
	get_wifi_config_value(PWD_STR, &pwd_obj->valuestring);

    FILE *file = fopen(WIFI_CONF_PATH, "w");
    if (file == NULL)
        printf("%s can't write \"%s\" file!\n", TAG, WIFI_CONF_PATH);
    else
    {
    	fprintf(file, "%s", cJSON_Print(root));
    	fclose(file);
    }

	cJSON_Delete(root);
}

static void read_wifi_conf(void)
{
	char *obj = NULL;

	wifi_config_t wifi_config;
	esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

	// Автоматическое включение wifi
	if (get_wifi_config_value(ON_STR, &obj))
	{
		if (strcmp(ONE_STR, obj) == 0)
			esp_wifi_start();
		free(obj);
	}

	// SSID точки доступа
	if (get_wifi_config_value(SSID_STR, &obj))
	{
		strcpy((char *)wifi_config.sta.ssid, obj);
		free(obj);
	}

	// Пароль точки доступа
	if (get_wifi_config_value(PWD_STR, &obj))
	{
		strcpy((char *)wifi_config.sta.password, obj);
		free(obj);
	}

	// Автоматическое подключение
	if (get_wifi_config_value(AUTO_STR, &obj))
	{
		if (strcmp(ONE_STR, obj) == 0)
			glob_set_bits_status_reg(STATUS_WIFI_AUTOCONNECT);
		else
			glob_clear_bits_status_reg(STATUS_WIFI_AUTOCONNECT);
		free(obj);
	}

	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
//	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
}

void wifi_service_task(void *pvParameters)
{
	wifi_init();

	printf("%s %s start\n", TAG, task_name);

	for( ;; )
	{
//		if (glob_get_status_err())
//			break;

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

			esp_wifi_connect();
			glob_set_bits_status_reg(STATUS_WIFI_STA_CONNECTING);
		}

		for_end:
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	printf("%s %s stop\n", TAG, task_name);

	vTaskDelete(NULL);
}
