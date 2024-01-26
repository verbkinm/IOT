/*
 * factory_reset.c
 *
 *  Created on: 23 янв. 2024 г.
 *      Author: user
 */

#include "update.h"

#define URL_MAX_LENGH	256

static const char *TAG = "UPDATE";

static const char *url_update_info = "https://ota.verbkinm.ru/other/esp32/meteo_home_II/update_info.json";
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

	const esp_app_desc_t *esp_desc = esp_app_get_description();
	if (strcmp(new_version_from_server, esp_desc->version) > 0)
		glob_set_bits_status_reg(STATUS_UPDATE_AVAILABLE);
	else
		glob_clear_bits_status_reg(STATUS_UPDATE_AVAILABLE);

	strcpy(available_version, new_version_from_server);

	cJSON_Delete(root);
	return true;

	bad_end:
	strcpy(available_version, error_version);
	glob_clear_bits_status_reg(STATUS_UPDATE_AVAILABLE);

	cJSON_Delete(root);
	return false;
}

static void chek_update(void)
{
	ESP_LOGI(TAG, "check update");
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

	glob_clear_bits_status_reg(STATUS_UPDATE_CHECK);
}

static void update(void)
{
	esp_err_t ret = ota_firmware(url_update);

	if (ret == ESP_OK)
		glob_set_bits_status_reg(STATUS_UPDATE_DONE);
	else
		glob_set_bits_status_err(STATUS_UPDATE_ERROR);

	glob_clear_bits_status_reg(STATUS_UPDATING);
}

char *update_service_get_available_version(void)
{
	return available_version;
}

void update_service_task(void *pvParameters)
{
	strcpy(available_version, default_version);

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if ( !(glob_get_status_reg() & STATUS_IP_GOT))
			goto for_end;

		if (glob_get_status_reg() & STATUS_UPDATING)
		{
			update();
			break;
		}

		if (glob_get_status_reg() & STATUS_UPDATE_CHECK)
			chek_update();

		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
