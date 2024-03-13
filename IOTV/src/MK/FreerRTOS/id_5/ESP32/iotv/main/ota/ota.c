/*
 * ota.c
 *
 *  Created on: 28 сент. 2023 г.
 *      Author: user
 */

#include "ota.h"

static const char *TAG = "ota";

static esp_err_t _http_event_handler(esp_http_client_event_t *evt);

esp_err_t backtofactory(void)
{
	const esp_partition_t* esp_ota_get_boot_partition();

	esp_partition_iterator_t pi = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, "factory") ;
	if (pi == NULL)
	{
		printf("%s Failed to find factory partition\n", TAG);
		return ESP_FAIL;
	}

	const esp_partition_t *factory = esp_partition_get(pi);
	esp_partition_iterator_release(pi);

	if (esp_ota_set_boot_partition(factory) != ESP_OK)
	{
		printf("%s Failed to set boot partition\n", TAG);
		return ESP_FAIL;
	}

	return ESP_OK;
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
//	switch (evt->event_id) {
//	case HTTP_EVENT_ERROR:
//		ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
//		break;
//	case HTTP_EVENT_ON_CONNECTED:
//		ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
//		break;
//	case HTTP_EVENT_HEADER_SENT:
//		ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
//		break;
//	case HTTP_EVENT_ON_HEADER:
//		ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
//		break;
//	case HTTP_EVENT_ON_DATA:
//		ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//		break;
//	case HTTP_EVENT_ON_FINISH:
//		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
//		break;
//	case HTTP_EVENT_DISCONNECTED:
//		ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
//		break;
//	case HTTP_EVENT_REDIRECT:
//		ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
//		break;
//	}
	return ESP_OK;
}

esp_err_t ota_firmware(const char *url)
{
	printf("%s url - %s\n", TAG, url);

	if (url == NULL)
	{
		printf("%s url fail\n", TAG);
		return ESP_FAIL;
	}

	esp_http_client_config_t config = {
			.url = url,
			.crt_bundle_attach = esp_crt_bundle_attach,
			.event_handler = _http_event_handler,
			.keep_alive_enable = true,
	};

	esp_https_ota_config_t ota_config = {
			.http_config = &config,
	};

	printf("%s Attempting to download update from %s\n", TAG, config.url);

	esp_err_t ret = esp_https_ota(&ota_config);
	if (ret == ESP_OK)
	{
		printf("%s OTA Succeed\n", TAG);
		return ESP_OK;
	}

	printf("%s Firmware upgrade failed\n", TAG);
	return ESP_FAIL;
}
