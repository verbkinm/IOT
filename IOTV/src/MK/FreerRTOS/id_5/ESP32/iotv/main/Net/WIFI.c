#include "WIFI.h"

extern uint64_t realBufSize, expextedDataSize;

static const char *TAG = "WIFI";
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		esp_wifi_connect();
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		realBufSize = 0;
		expextedDataSize = 0;

		if (true) //!!!
		{
			esp_wifi_connect();
			//            s_retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		}
		else
			xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);

		ESP_LOGI(TAG, "disconnect from AP");
		LedSignals_setWifiState(false);
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		LedSignals_setWifiState(true);
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

void wifi_init_sta(void)
{
	s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&event_handler,
			NULL,
			&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
			IP_EVENT_STA_GOT_IP,
			&event_handler,
			NULL,
			&instance_got_ip));

	wifi_config_t wifi_config = {
			.sta = {
					.ssid = CONFIG_EXAMPLE_WIFI_SSID,
					.password = CONFIG_EXAMPLE_WIFI_PASSWORD,
					.threshold.authmode = WIFI_AUTH_WPA2_PSK,
					.sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
			},
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	ESP_ERROR_CHECK(esp_wifi_start() );

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
	 * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
			WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
			pdFALSE,
			pdFALSE,
			portMAX_DELAY);

	/* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
	 * happened. */
	if (bits & WIFI_CONNECTED_BIT)
		ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
	else if (bits & WIFI_FAIL_BIT)
		ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
	else
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
}
