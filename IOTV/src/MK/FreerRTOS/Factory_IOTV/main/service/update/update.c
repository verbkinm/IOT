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
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "ota/ota.h"

#include "global/registers.h"
#include "Local_Lib/local_lib.h"
#include "nvs/local_nvs.h"

static const char *TAG = "UPDATE";
static const char *task_name = "update_service_task";

esp_err_t update_service_backtofactory(void)
{
	const esp_partition_t* esp_ota_get_boot_partition();

	const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);

	if (esp_ota_set_boot_partition(part) != ESP_OK)
	{
		printf("%s Failed to set boot partition\n", TAG);
		return ESP_FAIL;
	}

	return ESP_OK;
}

esp_err_t update_service_firmware(const char *data, size_t dataSize, size_t pkgNumber, size_t pkgTotal)
{
	esp_err_t err;

	const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);

	if (data == NULL || dataSize == 0 || dataSize > BUFSIZE || pkgNumber > pkgTotal || part == NULL)
		return ESP_FAIL;

	if (pkgNumber == 1)
	{
		err = esp_partition_erase_range(part, 0, part->size);
		if (err != ESP_OK)
			return err;
	}

	err = esp_partition_write(part, (pkgNumber - 1) * BUFSIZE, data, dataSize);
	if (err != ESP_OK)
	{
		printf("%s error esp_partition_write. %d\n", TAG, err);
		return err;
	}

	if (pkgNumber == pkgTotal)
	{
		err = esp_ota_set_boot_partition(part);
		if (err != ESP_OK)
		{
			printf("%s error esp_ota_set_boot_partition. %d\n", TAG, err);
			return err;
		}

		printf("%s Write firmware - SUCCESS\n", TAG);
	}

	return ESP_OK;
}

const char *update_service_task_name()
{
	return task_name;
}

void update_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);

	glob_set_bits_service_reg(SERVICE_UPDATE_ON);
	printf("%s %s start\n", TAG, task_name);

//	nvs_write_update_flag(1);

	// При перезагрузке возвращаться в рабочий раздел
	const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
	if (part != NULL)
		esp_ota_set_boot_partition(part);

	for( ;; )
	{
		if (glob_get_status_err())
			break;

		if ( !(glob_get_status_reg() & STATUS_IP_GOT))
			goto for_end;

		uint8_t update_flag = 0;
		if (nvs_read_update_flag(&update_flag) == ESP_OK && update_flag)
		{
			char *url = NULL;
			if (nvs_read_update_url(&url) == ESP_OK && url != NULL)
			{
				if (ota_firmware(url) != ESP_OK)
					update_service_backtofactory();
				else
				{
					nvs_write_update_flag(0);
					esp_restart();
				}
			}
		}

		for_end:
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	glob_clear_bits_service_reg(SERVICE_UPDATE_ON);
	printf("%s %s stop\n", TAG, task_name);
	vTaskDelete(NULL);
}
