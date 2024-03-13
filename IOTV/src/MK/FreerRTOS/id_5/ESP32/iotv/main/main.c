#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "service/service.h"
#include "I2C/i2c.h"
#include "iotv.h"
#include "I2C/DS3231.h"

//static const char *TAG = "main";

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	i2c_init();
//	DS3231_set_system_time();

	iotv_init();

	start_services();
}

