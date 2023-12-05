#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "TFT_touch_screen.h"
#include "I2C/i2c.h"

//static const char *TAG = "example";


void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    wifi_sta_config_t sta_cfg;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_stop());

	i2c_init();
	TFT_init(); // Перед этой функцией обязательно выполнить i2c_init();

	while (1)
	{
		// raise the task priority of LVGL and/or reduce the handler period can improve the performance
		vTaskDelay(1 / portTICK_PERIOD_MS);

		// The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
		lv_timer_handler();
	}
}
