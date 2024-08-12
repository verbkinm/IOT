/*
 * ota.c
 *
 *  Created on: 28 сент. 2023 г.
 *      Author: user
 */

#include "ota.h"

#include "global/global_def.h"

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"

static const char *TAG = "ota";

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
//			.event_handler = _http_event_handler,
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
