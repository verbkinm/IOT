/*
 * sntp.c
 *
 *  Created on: 14 дек. 2023 г.
 *      Author: user
 */

#include "sntp.h"

#include "../../Global_def.h"

extern uint32_t glob_status_reg;

void time_sync_notification_cb(struct timeval *tv)
{
	printf("Notification of a time synchronization event\n");
}

static void obtain_time(void)
{
	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("ntp5.ntp-servers.net");
	config.sync_cb = time_sync_notification_cb;
	esp_netif_sntp_init(&config);

	// wait for time to be set
	time_t now = 0;
	struct tm timeinfo = { 0 };
	int retry = 0;
	const int retry_count = 15;
	while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
		printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
	}
	time(&now);
	localtime_r(&now, &timeinfo);

	esp_netif_sntp_deinit();
}
void sntp_service_task(void *pvParameters)
{
	time_t now;
	struct tm timeinfo;
	char strftime_buf[64];

	for( ;; )
	{
		if ( !(glob_status_reg & STATUS_SNTP_ON) )
			goto for_end;

		time(&now);
		localtime_r(&now, &timeinfo);
		obtain_time();
		// update 'now' variable with current time
		time(&now);

		// Set timezone to China Standard Time
		setenv("TZ", "UTC-5", 1);
		tzset();
		localtime_r(&now, &timeinfo);
		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		printf("The current date/time in MSK is: %s\n", strftime_buf);

		if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH)
		{
			struct timeval outdelta;
			while (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS) {
				adjtime(NULL, &outdelta);
				printf("Waiting for adjusting time ... outdelta = %jd sec: %li ms: %li us\n",
						(intmax_t)outdelta.tv_sec,
						outdelta.tv_usec/1000,
						outdelta.tv_usec%1000);
				vTaskDelay(3000 / portTICK_PERIOD_MS);
			}
		}

		for_end:
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

