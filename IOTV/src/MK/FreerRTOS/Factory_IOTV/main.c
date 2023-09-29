/* Captive Portal Example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
 */

#include "esp_log.h"
//
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#include "wifi/wifi.h"
#include "web/web.h"

static const char *TAG = "main";

void app_main(void)
{
	/*
        Turn of warnings from HTTP server as redirecting traffic will yield
        lots of invalid requests
	 */
	esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
	esp_log_level_set("httpd_txrx", ESP_LOG_ERROR);
	esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

	// Create default event loop needed by the  main app
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Initialize NVS needed by Wi-Fi
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ESP_ERROR_CHECK(nvs_flash_init());
	}

	wifi_init();
	start_webserver();

	// Start the DNS server that will redirect all queries to the softAP IP
	//    start_dns_server();
}
