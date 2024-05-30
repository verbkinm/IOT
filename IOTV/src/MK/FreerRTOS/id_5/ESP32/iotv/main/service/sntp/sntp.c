/*
 * sntp.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#include "sntp.h"

const char *TAG = "SNTP";

const char *sntp_utc = "UTC-5";
const char *sntp_server_url = "pool.ntp.org";

void time_sync_notification_cb(struct timeval *tv);

void time_sync_notification_cb(struct timeval *tv)
{
	glob_set_bits_status_reg(STATUS_TIME_SYNC);

	time_t now;
	struct tm timeinfo;
	char strftime_buf[64];

	time(&now);
	setenv("TZ", sntp_utc, 1);
	tzset();
	localtime_r(&now, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(TAG, "The current date/time in %s is: %s\n", sntp_utc, strftime_buf);
}

void sntp_obtain_time(void)
{
	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(sntp_server_url);
	config.sync_cb = time_sync_notification_cb;
	esp_netif_sntp_init(&config);

	int retry = 0;
	const int retry_count = 15;
	while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count);

	esp_netif_sntp_deinit();
}

void sntp_service_task(void *pvParameters)
{
	vTaskDelay(DELAYED_LAUNCH / portTICK_PERIOD_MS);

	time_t now;
	struct tm timeinfo;

	while(true)
	{
		if (glob_get_status_err())
			break;

		if (glob_get_update_reg() & UPDATE_NOW)
			break;

		if ( !(glob_get_status_reg() & STATUS_IP_GOT) )
			goto for_end;

		time(&now);
		localtime_r(&now, &timeinfo);
		sntp_obtain_time();
		time(&now);

		// Set timezone
		setenv("TZ", sntp_utc, 1);
		tzset();
		localtime_r(&now, &timeinfo);

		if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH)
		{
			struct timeval outdelta;
			while (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS)
			{
				adjtime(NULL, &outdelta);
				vTaskDelay(2000 / portTICK_PERIOD_MS);
			}
		}
		vTaskDelay(1000 * (1 * 60) / portTICK_PERIOD_MS); // раз в 1 минуту

		for_end:
			vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

