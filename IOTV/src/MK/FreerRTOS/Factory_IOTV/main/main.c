#include "nvs_flash.h"
#include "esp_ota_ops.h"

#include "service/service.h"

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	services_start();

}
