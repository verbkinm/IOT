/*
 * update.c
 *
 *  Created on: 2 мар. 2024 г.
 *      Author: user
 */


#include "global/global_def.h"
#include "update.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"
#include "cJSON.h"
#include "esp_app_desc.h"
#include "ota/ota.h"

#include "global/registers.h"
#include "Local_Lib/local_lib.h"
#include "nvs/local_nvs.h"

#define URL_MAX_LENGH	256

static const char *TAG = "UPDATE";
static const char *task_name = "update_service_task";

static const char *url_update_info = "https://ota.verbkinm.ru/iotv/esp32/id1/update_info.json";
static const char *default_version = "0.0";
static const char *error_version = "Ошибка обновления";

static char available_version[64];

static void chek_update(void);
static esp_err_t parse_http_response(const char* content);

static esp_err_t parse_http_response(const char* content)
{
	cJSON *root = cJSON_Parse(content);
	if (root == NULL)
		return ESP_FAIL;

	cJSON *firmware = cJSON_GetObjectItemCaseSensitive(root, "firmware");
	if (firmware == NULL)
		goto bad_end;

	cJSON *version = cJSON_GetObjectItemCaseSensitive(firmware, "version");
	if (version == NULL)
		goto bad_end;

	char *new_version_from_server = cJSON_GetStringValue(version);
	if (new_version_from_server == NULL || strlen(new_version_from_server) == 0)
		goto bad_end;

	cJSON *url = cJSON_GetObjectItemCaseSensitive(firmware, "url");
	char *new_url = cJSON_GetStringValue(url);
	if (new_url == NULL || strlen(new_url) == 0)
		goto bad_end;

	if (strlen(new_url) >= URL_MAX_LENGH)
		goto bad_end;

	if (nvs_write_update_url(new_url) != ESP_OK)
		goto bad_end;

	strcpy(available_version, new_version_from_server);

	cJSON_Delete(root);
	return ESP_OK;

	bad_end:
	strcpy(available_version, error_version);

	cJSON_Delete(root);
	return ESP_FAIL;
}

static void chek_update(void)
{
	printf("%s check update\n", TAG);
	esp_http_client_config_t config = {
			.url = url_update_info,
			.crt_bundle_attach = esp_crt_bundle_attach,
			//			.event_handler = http_event_handler,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_open(client, 0);
	esp_http_client_fetch_headers(client);

	char *response_buffer = calloc(1, BUFSIZE);
	esp_http_client_read(client, response_buffer, BUFSIZE - 1);

	esp_err_t err = parse_http_response(response_buffer);

	esp_http_client_cleanup(client);
	free(response_buffer);

	if (err != ESP_OK)
	{
		printf("%s %s error parse update\n", TAG, task_name);
		return;
	}

	const esp_app_desc_t *esp_app = esp_app_get_description();

	printf("%s %s available_version - %s\n", TAG, task_name, available_version);
	if (strcmp(available_version, esp_app->version) > 0)
	{
		printf("%s %s new version available - %s\n", TAG, task_name,available_version);

		if (nvs_write_update_flag(NVS_VALUE_UPDATE_YES) == ESP_OK)
		{
			if (update_service_backtofactory() == ESP_OK)
				esp_restart();
		}
		else
			nvs_write_update_flag(NVS_VALUE_UPDATE_NO);
	}
}

esp_err_t update_service_backtofactory(void)
{
	const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

	if (esp_ota_set_boot_partition(part) != ESP_OK)
	{
		printf("%s Failed to set boot partition\n", TAG);
		return ESP_FAIL;
	}

	return ESP_OK;
}

const char *update_service_task_name()
{
	return task_name;
}

void update_service_task(void *pvParameters)
{
	glob_set_bits_service_reg(SERVICE_UPDATE_ON);
	printf("%s %s start\n", TAG, task_name);

	strcpy(available_version, default_version);

	const uint8_t COUNTER_UPDATE = 60;
	uint8_t counter = COUNTER_UPDATE;

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if ( !(glob_get_status_reg() & STATUS_IP_GOT))
			goto for_end;

		if (counter++ < COUNTER_UPDATE)
			goto for_end;

		counter = 0;

		chek_update();

		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	glob_clear_bits_service_reg(SERVICE_UPDATE_ON);
	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
}
