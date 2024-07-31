/*
 * update.c
 *
 *  Created on: 2 мар. 2024 г.
 *      Author: user
 */


#include "update.h"

#define URL_MAX_LENGH	256

static const char *TAG = "UPDATE";
static const char *task_name = "update_service_task";

static const char *url_update_info = "https://ota.verbkinm.ru/iotv/esp32/id1/update_info.json";
static const char *default_version = "0.0";
static const char *error_version = "Ошибка сервера обновлений!";

static char *url_update = NULL;

static char available_version[64];

static void chek_update(void);
static void update(void);
static bool parse_http_response(const char* content);

static bool parse_http_response(const char* content)
{
	cJSON *root = cJSON_Parse(content);
	if (root == NULL)
		return false;

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

	if (strlen(new_url) > URL_MAX_LENGH)
		goto bad_end;

	if (url_update != NULL)
		free(url_update);

	url_update = calloc(1, strlen(new_url) + 1);
	if (url_update == NULL)
		goto bad_end;

	strcpy(url_update, new_url);
	strcpy(available_version, new_version_from_server);

	cJSON_Delete(root);
	return true;

	bad_end:
	strcpy(available_version, error_version);

	cJSON_Delete(root);
	return false;
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
	esp_http_client_read(client, response_buffer, BUFSIZE);

	parse_http_response(response_buffer);

	esp_http_client_cleanup(client);

	//	glob_clear_bits_status_reg(STATUS_UPDATE_CHECK);

	free(response_buffer);

	const esp_app_desc_t *esp_app = esp_app_get_description();

	printf("available_version - %s\n", available_version);
	if (strcmp(available_version, esp_app->version) > 0)
	{
		printf("new version available - %s\n", available_version);
		update();
	}
}

static void update(void)
{
	const esp_partition_t* partition = esp_ota_get_boot_partition();

	printf("current partition label: %s\n", partition->label);

	if (strcmp(partition->label, "factory") != 0)
	{
		if (backtofactory() == ESP_OK)
			esp_restart();
	}

	esp_err_t ret = ota_firmware(url_update);

	if (ret == ESP_OK)
	{
		printf("reboting...");
		esp_restart();
	}
}

const char *update_service_task_name()
{
	return task_name;
}

void update_service_task(void *pvParameters)
{
	printf("%s %s start\n", TAG, task_name);

	strcpy(available_version, default_version);

	const uint8_t COUNTER_UPDATE = 60;
	uint8_t counter = COUNTER_UPDATE;

	for( ;; )
	{
		if ( !(glob_get_status_reg() & STATUS_IP_GOT))
			goto for_end;

		if (counter++ < COUNTER_UPDATE)
			goto for_end;

		counter = 0;

		chek_update();

		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
}
