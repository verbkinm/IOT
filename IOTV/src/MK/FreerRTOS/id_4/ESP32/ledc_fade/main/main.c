#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "nvs_flash.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "iotv.h"
#include "Net/Tcp.h"
#include "Net/WIFI.h"
#include "Led_RGB.h"
#include "df_player.h"

extern QueueHandle_t xQueueInData, xQueueOutData, xQueueDF;

static const char *TAG = "main";

void app_main(void)
{
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
	wifi_init_sta();

	// Инициализациия глобальных очередей
	xQueueInData = xQueueCreate(100, sizeof(struct DataPkg));
	xQueueOutData = xQueueCreate(100, sizeof(struct DataPkg));
	xQueueDF = xQueueCreate(5, sizeof(int8_t));

	if (xQueueInData == NULL  || xQueueOutData == NULL || xQueueDF == NULL)// || xQueueLedSignals == NULL)
	{
		ESP_LOGE(TAG, "Queue was not created and must not be used");
		esp_restart();
	}

	xTaskCreate(Led_RGB_Task, "Led_RGB_Task", 2048, NULL, Led_RGB_PRIORITY, NULL);

	xTaskCreate(iotvTask, "iotvTask", 4096, NULL, IOTV_PRIORITY, NULL);
	xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET, NET_PRIORITY, NULL);

//	vTaskDelay(1000 / portTICK_PERIOD_MS);
	xTaskCreate(DF_Task, "DF_Task", 2048, NULL, DF_PLAYER_PRIORITY, NULL);
}
