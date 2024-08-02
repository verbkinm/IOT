/* Captive Portal Example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
 */
#include "nvs_flash.h"

#include "service/service.h"
#include "web/web.h"

static const char *TAG = "main";

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	services_start();

	vTaskDelay(1000 / portTICK_PERIOD_MS);
	start_webserver();

	//	 Start the DNS server that will redirect all queries to the softAP IP
//	start_dns_server();
}
